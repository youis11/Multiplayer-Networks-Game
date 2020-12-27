#include "ModuleNetworkingServer.h"



//////////////////////////////////////////////////////////////////////
// ModuleNetworkingServer public methods
//////////////////////////////////////////////////////////////////////

void ModuleNetworkingServer::setListenPort(int port)
{
	listenPort = port;
}



//////////////////////////////////////////////////////////////////////
// ModuleNetworking virtual methods
//////////////////////////////////////////////////////////////////////

void ModuleNetworkingServer::onStart()
{
	if (!createSocket()) return;

	// Reuse address
	int enable = 1;
	int res = setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&enable, sizeof(int));
	if (res == SOCKET_ERROR) {
		reportError("ModuleNetworkingServer::start() - setsockopt");
		disconnect();
		return;
	}

	// Create and bind to local address
	if (!bindSocketToPort(listenPort)) {
		return;
	}

	state = ServerState::Listening;

	secondsSinceLastPing = 0.0f;
}

void ModuleNetworkingServer::onGui()
{
	if (ImGui::CollapsingHeader("ModuleNetworkingServer", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("Connection checking info:");
		ImGui::Text(" - Ping interval (s): %f", PING_INTERVAL_SECONDS);
		ImGui::Text(" - Disconnection timeout (s): %f", DISCONNECT_TIMEOUT_SECONDS);

		ImGui::Separator();

		if (state == ServerState::Listening)
		{
			int count = 0;

			for (int i = 0; i < MAX_CLIENTS; ++i)
			{
				if (clientProxies[i].connected)
				{
					ImGui::Text("CLIENT %d", count++);
					ImGui::Text(" - address: %d.%d.%d.%d",
						clientProxies[i].address.sin_addr.S_un.S_un_b.s_b1,
						clientProxies[i].address.sin_addr.S_un.S_un_b.s_b2,
						clientProxies[i].address.sin_addr.S_un.S_un_b.s_b3,
						clientProxies[i].address.sin_addr.S_un.S_un_b.s_b4);
					ImGui::Text(" - port: %d", ntohs(clientProxies[i].address.sin_port));
					ImGui::Text(" - name: %s", clientProxies[i].name.c_str());
					ImGui::Text(" - id: %d", clientProxies[i].clientId);
					if (clientProxies[i].gameObject != nullptr)
					{
						ImGui::Text(" - gameObject net id: %d", clientProxies[i].gameObject->networkId);
					}
					else
					{
						ImGui::Text(" - gameObject net id: (null)");
					}
					
					ImGui::Separator();
				}
			}

			ImGui::Checkbox("Render colliders", &App->modRender->mustRenderColliders);
		}
	}
}

void ModuleNetworkingServer::onPacketReceived(const InputMemoryStream &packet, const sockaddr_in &fromAddress)
{
	if (state == ServerState::Listening)
	{
		uint32 protoId;
		packet >> protoId;
		if (protoId != PROTOCOL_ID) return;

		ClientMessage message;
		packet >> message;

		ClientProxy *proxy = getClientProxy(fromAddress);

		if (message == ClientMessage::Hello)
		{
			if (proxy == nullptr)
			{
				proxy = createClientProxy();

				if (proxy != nullptr)
				{
					std::string playerName;
					uint8 spaceshipType;
					packet >> playerName;
					packet >> spaceshipType;

					proxy->address.sin_family = fromAddress.sin_family;
					proxy->address.sin_addr.S_un.S_addr = fromAddress.sin_addr.S_un.S_addr;
					proxy->address.sin_port = fromAddress.sin_port;
					proxy->connected = true;
					proxy->name = playerName;
					proxy->clientId = nextClientId++;
					//proxy->secondsSinceLastReplication = replicationDeliveryIntervalSeconds;
					if (spaceshipType == 0) player1_joined = true;
					if (spaceshipType == 1) player2_joined = true;

					if (clientProxies[0].connected && clientProxies[1].connected && !game_is_full && player1_joined && player2_joined)
					{
						// Create new network object BALL
						vec2 initialBallPosition = { 0,0 };
						float initialBallAngle = 360.0f * Random.next();
						proxy->gameObject = spawnBall(initialBallPosition, initialBallAngle);
						// Create new network object WALL
						vec2 initialWallPosition = { 0,370 };
						proxy->gameObject = spawnWall(initialWallPosition);
						proxy->gameObject = spawnWall(initialWallPosition * -1);
						// Create new network object GOAL
						vec2 initialGoalPosition = { 500, 0 };
						proxy->gameObject = spawnGoal(initialGoalPosition);
						proxy->gameObject = spawnGoal(initialGoalPosition * -1);

						game_is_full = true;
					}

					// Create new network object SCORE
					proxy->gameObject = spawnScore(spaceshipType, { 0,0 });		

					// Create new network object PLAYER
					vec2 initialPosition = 500.0f * vec2{ Random.next() - 0.5f, Random.next() - 0.5f };
					float initialAngle = 360.0f * Random.next();
					proxy->gameObject = spawnPlayer(spaceshipType, initialPosition, initialAngle);

					
					
				}
				else
				{
					// NOTE(jesus): Server is full...
				}
			}

			if (proxy != nullptr)
			{
				// Send welcome to the new player
				OutputMemoryStream welcomePacket;
				welcomePacket << PROTOCOL_ID;
				welcomePacket << ServerMessage::Welcome;
				welcomePacket << proxy->clientId;
				welcomePacket << proxy->gameObject->networkId;
				sendPacket(welcomePacket, fromAddress);
				proxy->lastPacketReceivedTime = Time.time;

				// Send all network objects to the new player
				uint16 networkGameObjectsCount;
				GameObject *networkGameObjects[MAX_NETWORK_OBJECTS];
				App->modLinkingContext->getNetworkGameObjects(networkGameObjects, &networkGameObjectsCount);
				for (uint16 i = 0; i < networkGameObjectsCount; ++i)
				{
					GameObject *gameObject = networkGameObjects[i];
					
					// TODO(you): World state replication lab session
					if (gameObject != proxy->gameObject)
					{
						proxy->m_replicationManager.create(gameObject->networkId);
					}
				}

				LOG("Message received: hello - from player %s", proxy->name.c_str());
			}
			else
			{
				OutputMemoryStream unwelcomePacket;
				unwelcomePacket << PROTOCOL_ID;
				unwelcomePacket << ServerMessage::Unwelcome;
				sendPacket(unwelcomePacket, fromAddress);

				WLOG("Message received: UNWELCOMED hello - server is full");
			}
		}
		else if (message == ClientMessage::Input)
		{
			// Process the input packet and update the corresponding game object
			if (proxy != nullptr && IsValid(proxy->gameObject))
			{
				// TODO(you): Reliability on top of UDP lab session
				// TODO(marc): send notifications about the last processed input data

				// Read input data
				while (packet.RemainingByteCount() > 0)
				{
					InputPacketData inputData;
					packet >> inputData.sequenceNumber;
					uint32 sequenceNumber = inputData.sequenceNumber;
					packet >> inputData.horizontalAxis;
					packet >> inputData.verticalAxis;
					packet >> inputData.buttonBits;

					if (inputData.sequenceNumber >= proxy->nextExpectedInputSequenceNumber)
					{
						proxy->gamepad.horizontalAxis = inputData.horizontalAxis;
						proxy->gamepad.verticalAxis = inputData.verticalAxis;
						unpackInputControllerButtons(inputData.buttonBits, proxy->gamepad);
						proxy->gameObject->behaviour->onInput(proxy->gamepad);
						proxy->nextExpectedInputSequenceNumber = inputData.sequenceNumber + 1;
					}

					OutputMemoryStream reliabilityPacket;
					reliabilityPacket << PROTOCOL_ID;
					reliabilityPacket << ServerMessage::Reliability;
					reliabilityPacket << sequenceNumber;
					sendPacket(reliabilityPacket, fromAddress);
				}
			}
		}

		// TODO(you): UDP virtual connection lab session
		else if (message == ClientMessage::Ping) {
			proxy->lastPacketReceivedTime = Time.time;
			
		}
	}
}

void ModuleNetworkingServer::onUpdate()
{
	if (state == ServerState::Listening)
	{

		// Handle networked game object destructions
		for (DelayedDestroyEntry &destroyEntry : netGameObjectsToDestroyWithDelay)
		{
			if (destroyEntry.object != nullptr)
			{
				destroyEntry.delaySeconds -= Time.deltaTime;
				if (destroyEntry.delaySeconds <= 0.0f)
				{
					destroyNetworkObject(destroyEntry.object);
					destroyEntry.object = nullptr;
				}
			}
		}
		secondsSinceLastPing += Time.deltaTime;

		for (ClientProxy &clientProxy : clientProxies)
		{
			if (clientProxy.connected)
			{
				// TODO(you): UDP virtual connection lab session
				if (secondsSinceLastPing >= PING_INTERVAL_SECONDS) {
					OutputMemoryStream packet;
					packet << PROTOCOL_ID;
					packet << ServerMessage::Ping;
					sendPacket(packet, clientProxy.address);
				}

				if (Time.time - clientProxy.lastPacketReceivedTime >= DISCONNECT_TIMEOUT_SECONDS)
					onConnectionReset(clientProxy.address);
				
				// Don't let the client proxy point to a destroyed game object
				if (!IsValid(clientProxy.gameObject))
				{
					clientProxy.gameObject = nullptr;
				}

				// TODO(you): World state replication lab session
				if (clientProxy.secondsSinceLastReplication >= REPLICATION_INTERVAL_SECONDS) {
					OutputMemoryStream packet;
					packet << PROTOCOL_ID;
					packet << ServerMessage::Replication;

					clientProxy.m_replicationManager.write(packet);
					sendPacket(packet, clientProxy.address);

					clientProxy.secondsSinceLastReplication = 0.0f;
				}
				else {
					clientProxy.secondsSinceLastReplication += Time.deltaTime;
				}
				
				// TODO(you): Reliability on top of UDP lab session
			}			
		}
		if (secondsSinceLastPing >= PING_INTERVAL_SECONDS)
			secondsSinceLastPing = 0.0f;
	}
}

void ModuleNetworkingServer::onConnectionReset(const sockaddr_in & fromAddress)
{
	// Find the client proxy
	ClientProxy *proxy = getClientProxy(fromAddress);

	if (proxy)
	{
		// Clear the client proxy
		destroyClientProxy(proxy);
		player1_joined = false;
		player2_joined = false;
	}
}

void ModuleNetworkingServer::onDisconnect()
{
	uint16 netGameObjectsCount;
	GameObject *netGameObjects[MAX_NETWORK_OBJECTS];
	App->modLinkingContext->getNetworkGameObjects(netGameObjects, &netGameObjectsCount);
	for (uint32 i = 0; i < netGameObjectsCount; ++i)
	{
		NetworkDestroy(netGameObjects[i]);
	}

	for (ClientProxy &clientProxy : clientProxies)
	{
		destroyClientProxy(&clientProxy);
	}
	
	nextClientId = 0;
	player1_joined = false;
	player2_joined = false;
	state = ServerState::Stopped;
}



//////////////////////////////////////////////////////////////////////
// Client proxies
//////////////////////////////////////////////////////////////////////

ModuleNetworkingServer::ClientProxy * ModuleNetworkingServer::createClientProxy()
{
	// If it does not exist, pick an empty entry
	for (int i = 0; i < MAX_CLIENTS; ++i)
	{
		if (!clientProxies[i].connected)
		{
			return &clientProxies[i];
		}
	}

	return nullptr;
}

ModuleNetworkingServer::ClientProxy * ModuleNetworkingServer::getClientProxy(const sockaddr_in &clientAddress)
{
	// Try to find the client
	for (int i = 0; i < MAX_CLIENTS; ++i)
	{
		if (clientProxies[i].address.sin_addr.S_un.S_addr == clientAddress.sin_addr.S_un.S_addr &&
			clientProxies[i].address.sin_port == clientAddress.sin_port)
		{
			return &clientProxies[i];
		}
	}

	return nullptr;
}

void ModuleNetworkingServer::destroyClientProxy(ClientProxy *clientProxy)
{
	// Destroy the object from all clients
	if (IsValid(clientProxy->gameObject))
	{
		destroyNetworkObject(clientProxy->gameObject);
	}

    *clientProxy = {};
}

void ModuleNetworkingServer::playNetworkAudio(std::string fileName)
{
	for (ClientProxy& clientProxy : clientProxies)
	{
		if (clientProxy.connected)
		{
			clientProxy.m_replicationManager.playAudio(0, fileName);

		}
	}

}


//////////////////////////////////////////////////////////////////////
// Spawning
//////////////////////////////////////////////////////////////////////

GameObject * ModuleNetworkingServer::spawnPlayer(uint8 spaceshipType, vec2 initialPosition, float initialAngle)
{
	// Create a new game object with the player properties
	GameObject *gameObject = NetworkInstantiate();
	gameObject->position = initialPosition;
	gameObject->size = { 30, 120 };
	gameObject->angle = initialAngle;

	// Create sprite
	gameObject->sprite = App->modRender->addSprite(gameObject);
	gameObject->sprite->order = 5;
	gameObject->sprite->texture = App->modResources->spacecraft1;

	// Create collider
	gameObject->collider = App->modCollision->addCollider(ColliderType::Player, gameObject);
	gameObject->collider->isTrigger = true; // NOTE(jesus): This object will receive onCollisionTriggered events

	// Create behaviour
	Spaceship* spaceshipBehaviour = App->modBehaviour->addSpaceship(gameObject, spaceshipType);
	gameObject->behaviour = spaceshipBehaviour;
	gameObject->behaviour->isServer = true;

	return gameObject;
}

GameObject* ModuleNetworkingServer::spawnBall(vec2 initialPosition, float initialAngle)
{
	// Create a new game object with the ball properties
	GameObject* gameObject = NetworkInstantiate();
	gameObject->position = initialPosition;
	gameObject->size = { 30, 30};
	gameObject->angle = initialAngle;

	// Create sprite
	gameObject->sprite = App->modRender->addSprite(gameObject);
	gameObject->sprite->order = 6;
	gameObject->sprite->texture = App->modResources->asteroid1;

	// Create collider
	gameObject->collider = App->modCollision->addCollider(ColliderType::Ball, gameObject);
	gameObject->collider->isTrigger = true; // NOTE(jesus): This object will receive onCollisionTriggered events

	// Create behaviour
	Ball* ballBehaviour = App->modBehaviour->addBall(gameObject);
	gameObject->behaviour = ballBehaviour;
	gameObject->behaviour->isServer = true;

	return gameObject;
}

GameObject* ModuleNetworkingServer::spawnWall(vec2 initialPosition)
{
	// Create a new game object with the ball properties
	GameObject* gameObject = NetworkInstantiate();
	gameObject->position = initialPosition;
	gameObject->size = { 1000, 50 };
	gameObject->angle = 0;

	// Create sprite
	gameObject->sprite = App->modRender->addSprite(gameObject);
	gameObject->sprite->order = 7;
	gameObject->sprite->texture = App->modResources->asteroid2;

	// Create collider
	gameObject->collider = App->modCollision->addCollider(ColliderType::Wall, gameObject);
	gameObject->collider->isTrigger = true; // NOTE(jesus): This object will receive onCollisionTriggered events

	return gameObject;
}

GameObject* ModuleNetworkingServer::spawnGoal(vec2 initialPosition)
{
	// Create a new game object with the ball properties
	GameObject* gameObject = NetworkInstantiate();
	gameObject->position = initialPosition;
	gameObject->size = { 50, 1000 };
	gameObject->angle = 0;

	// Create sprite
	gameObject->sprite = App->modRender->addSprite(gameObject);
	gameObject->sprite->order = 7;
	gameObject->sprite->texture = App->modResources->asteroid2;

	// Create collider
	gameObject->collider = App->modCollision->addCollider(ColliderType::Goal, gameObject);
	gameObject->collider->isTrigger = true; // NOTE(jesus): This object will receive onCollisionTriggered events

	return gameObject;
}

GameObject* ModuleNetworkingServer::spawnScore(uint8 spaceshipType, vec2 initialPosition)
{
	// Create a new game object with the player properties
	GameObject* gameObject = NetworkInstantiate();
	gameObject->position = initialPosition;
	gameObject->size = { 100, 175 };
	gameObject->angle = 0;

	// Create sprite
	gameObject->sprite = App->modRender->addSprite(gameObject);
	gameObject->sprite->order = 5;
	gameObject->sprite->texture = App->modResources->score_0;
	
	// Create behaviour
	Score* scoreBehaviour = App->modBehaviour->addScore(gameObject, spaceshipType);
	gameObject->behaviour = scoreBehaviour;
	gameObject->behaviour->isServer = true;

	return gameObject;
}


//////////////////////////////////////////////////////////////////////
// Update / destruction
//////////////////////////////////////////////////////////////////////

GameObject * ModuleNetworkingServer::instantiateNetworkObject()
{
	// Create an object into the server
	GameObject * gameObject = Instantiate();

	// Register the object into the linking context
	App->modLinkingContext->registerNetworkGameObject(gameObject);

	// Notify all client proxies' replication manager to create the object remotely
	for (int i = 0; i < MAX_CLIENTS; ++i)
	{
		if (clientProxies[i].connected)
		{
			// TODO(you): World state replication lab session
			clientProxies[i].m_replicationManager.create(gameObject->networkId);
		}
	}

	return gameObject;
}

void ModuleNetworkingServer::updateNetworkObject(GameObject * gameObject)
{
	// Notify all client proxies' replication manager to destroy the object remotely
	for (int i = 0; i < MAX_CLIENTS; ++i)
	{
		if (clientProxies[i].connected)
		{
			// TODO(you): World state replication lab session
			clientProxies[i].m_replicationManager.update(gameObject->networkId);
		}
	}
}

void ModuleNetworkingServer::destroyNetworkObject(GameObject * gameObject)
{
	// Notify all client proxies' replication manager to destroy the object remotely
	for (int i = 0; i < MAX_CLIENTS; ++i)
	{
		if (clientProxies[i].connected)
		{
			// TODO(you): World state replication lab session
			clientProxies[i].m_replicationManager.destroy(gameObject->networkId);
		}
	}

	// Assuming the message was received, unregister the network identity
	App->modLinkingContext->unregisterNetworkGameObject(gameObject);

	// Finally, destroy the object from the server
	Destroy(gameObject);
}

void ModuleNetworkingServer::destroyNetworkObject(GameObject * gameObject, float delaySeconds)
{
	uint32 emptyIndex = MAX_GAME_OBJECTS;
	for (uint32 i = 0; i < MAX_GAME_OBJECTS; ++i)
	{
		if (netGameObjectsToDestroyWithDelay[i].object == gameObject)
		{
			float currentDelaySeconds = netGameObjectsToDestroyWithDelay[i].delaySeconds;
			netGameObjectsToDestroyWithDelay[i].delaySeconds = min(currentDelaySeconds, delaySeconds);
			return;
		}
		else if (netGameObjectsToDestroyWithDelay[i].object == nullptr)
		{
			if (emptyIndex == MAX_GAME_OBJECTS)
			{
				emptyIndex = i;
			}
		}
	}

	ASSERT(emptyIndex < MAX_GAME_OBJECTS);

	netGameObjectsToDestroyWithDelay[emptyIndex].object = gameObject;
	netGameObjectsToDestroyWithDelay[emptyIndex].delaySeconds = delaySeconds;
}


//////////////////////////////////////////////////////////////////////
// Global create / update / destruction of network game objects
//////////////////////////////////////////////////////////////////////

GameObject * NetworkInstantiate()
{
	ASSERT(App->modNetServer->isConnected());

	return App->modNetServer->instantiateNetworkObject();
}

void NetworkUpdate(GameObject * gameObject)
{
	ASSERT(App->modNetServer->isConnected());
	ASSERT(gameObject->networkId != 0);

	App->modNetServer->updateNetworkObject(gameObject);
}

void NetworkDestroy(GameObject * gameObject)
{
	NetworkDestroy(gameObject, 0.0f);
}

void NetworkDestroy(GameObject * gameObject, float delaySeconds)
{
	ASSERT(App->modNetServer->isConnected());
	ASSERT(gameObject->networkId != 0);

	App->modNetServer->destroyNetworkObject(gameObject, delaySeconds);
}
