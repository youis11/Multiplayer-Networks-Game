#include "Networks.h"
#include "Behaviours.h"
#include <string>


void Laser::start()
{
	gameObject->networkInterpolationEnabled = false;
	App->modSound->playAudioClip(App->modResources->audioClipLaser);
}

void Laser::update()
{
	secondsSinceCreation += Time.deltaTime;

	const float pixelsPerSecond = 1000.0f;
	gameObject->position += vec2FromDegrees(gameObject->angle) * pixelsPerSecond * Time.deltaTime;

	if (isServer)
	{
		const float lifetimeSeconds = 2.0f;
		if (secondsSinceCreation >= lifetimeSeconds) {
			NetworkDestroy(gameObject);
		}
	}
}

float Laser::GetSecondsLived()
{
	return secondsSinceCreation;
}






void Spaceship::start()
{
	gameObject->tag = (uint32)(Random.next() * UINT_MAX);
	gameObject->angle = 0;
	
	/*if (player_waiting)
		playerNum = PlayerNum::PLAYER2;
	else
	{
		player_waiting = true;
		playerNum = PlayerNum::PLAYER1;
	}*/

	switch (playerNum)
	{
	case Spaceship::PLAYER1:
		gameObject->position = {450,0};
		break;
	case Spaceship::PLAYER2:
		gameObject->position = { -450,0 };
		break;
	case Spaceship::NONE:
		break;
	default:
		break;
	}
	if (isServer)
		NetworkUpdate(gameObject);
	/*lifebar = Instantiate();
	lifebar->sprite = App->modRender->addSprite(lifebar);
	lifebar->sprite->pivot = vec2{ 0.0f, 0.5f };
	lifebar->sprite->order = 5;*/
}

void Spaceship::onInput(const InputController &input)
{
	if (input.verticalAxis != 0.0f)
	{
		float speed = 700.0f;
		if (input.actionDown == ButtonState::Pressed || input.actionUp == ButtonState::Pressed || input.actionRight == ButtonState::Pressed || input.actionLeft == ButtonState::Pressed)
			speed *= 4.0f;

		float max_high = 265;
		float min_high = -265;
		if (gameObject->position.y >= min_high && gameObject->position.y <= max_high)
		{
			gameObject->position.y -= input.verticalAxis * speed * Time.deltaTime;
			if (isServer)
				NetworkUpdate(gameObject);
		}

		if (gameObject->position.y < min_high)
			gameObject->position.y = min_high;
		if (gameObject->position.y > max_high)
			gameObject->position.y = max_high;
	}

	/*if (input.horizontalAxis != 0.0f)
	{
		const float rotateSpeed = 180.0f;
		gameObject->angle += input.horizontalAxis * rotateSpeed * Time.deltaTime;

		if (isServer)
		{
			NetworkUpdate(gameObject);
		}
	}

	if (input.actionDown == ButtonState::Pressed)
	{
		const float advanceSpeed = 200.0f;
		gameObject->position += vec2FromDegrees(gameObject->angle) * advanceSpeed * Time.deltaTime;

		if (isServer)
		{
			NetworkUpdate(gameObject);
		}
	}

	if (input.actionLeft == ButtonState::Press)
	{
		if (isServer)
		{
			GameObject *laser = NetworkInstantiate();

			laser->position = gameObject->position;
			laser->angle = gameObject->angle;
			laser->size = { 20, 60 };

			laser->sprite = App->modRender->addSprite(laser);
			laser->sprite->order = 3;
			laser->sprite->texture = App->modResources->laser;

			Laser *laserBehaviour = App->modBehaviour->addLaser(laser);
			laserBehaviour->isServer = isServer;

			laser->tag = gameObject->tag;

			const float neutralTimeSeconds = 0.1f;
			if (/*GetSecondsLived() > neutralTimeSeconds && laser->collider == nullptr) {
				laser->collider = App->modCollision->addCollider(ColliderType::Laser, gameObject);
			}
		}
	}*/
}

void Spaceship::update()
{
	//TO DELETE
	/*static const vec4 colorAlive = vec4{ 0.2f, 1.0f, 0.1f, 0.5f };
	static const vec4 colorDead = vec4{ 1.0f, 0.2f, 0.1f, 0.5f };
	const float lifeRatio = max(0.01f, (float)(hitPoints) / (MAX_HIT_POINTS));
	lifebar->position = gameObject->position + vec2{ -50.0f, -50.0f };
	lifebar->size = vec2{ lifeRatio * 80.0f, 5.0f };
	lifebar->sprite->color = lerp(colorDead, colorAlive, lifeRatio);*/
	//----------
	
}

void Spaceship::destroy()
{
	//TO DELETE
	Destroy(lifebar);
	//----------
}

void Spaceship::onCollisionTriggered(Collider &c1, Collider &c2)
{
	if (c2.type == ColliderType::Ball && c2.gameObject->tag != gameObject->tag)
	{
		//TODO(marc): change direction ball behaviour, angle
	}
	/*if (c2.type == ColliderType::Laser && c2.gameObject->tag != gameObject->tag)
	{
		if (isServer)
		{
			NetworkDestroy(c2.gameObject); // Destroy the laser
		
			if (hitPoints > 0)
			{
				hitPoints--;
				NetworkUpdate(gameObject);
			}

			float size = 30 + 50.0f * Random.next();
			vec2 position = gameObject->position + 50.0f * vec2{Random.next() - 0.5f, Random.next() - 0.5f};

			if (hitPoints <= 0)
			{
				// Centered big explosion
				size = 250.0f + 100.0f * Random.next();
				position = gameObject->position;

				NetworkDestroy(gameObject);
			}

			GameObject *explosion = NetworkInstantiate();
			explosion->position = position;
			explosion->size = vec2{ size, size };
			explosion->angle = 365.0f * Random.next();

			explosion->sprite = App->modRender->addSprite(explosion);
			explosion->sprite->texture = App->modResources->explosion1;
			explosion->sprite->order = 100;

			explosion->animation = App->modRender->addAnimation(explosion);
			explosion->animation->clip = App->modResources->explosionClip;

			NetworkDestroy(explosion, 2.0f);

			// NOTE(jesus): Only played in the server right now...
			// You need to somehow make this happen in clients
			App->modSound->playAudioClip(App->modResources->audioClipExplosion);
		}
	}*/
}

void Spaceship::write(OutputMemoryStream & packet)
{
	packet << hitPoints;
}

void Spaceship::read(const InputMemoryStream & packet)
{
	packet >> hitPoints;
}

void Score::start()
{
	gameObject->tag = (uint32)(Random.next() * UINT_MAX);
	gameObject->angle = 0;


	switch (scorePlayerNum)
	{
	case Score::SCORE_PLAYER1:
		gameObject->position = { 200,-250 };
		break;
	case Score::SCORE_PLAYER2:
		gameObject->position = { -200,-250 };
		break;
	case Score::NONE:
		break;
	default:
		break;
	}

	if (isServer)
		NetworkUpdate(gameObject);
}

//void Score::onInput(const InputController& input)
//{
//	if (input.actionDown == ButtonState::Press)
//	{
//		score_value++;
//		if (score_value > 9)
//			score_value = 0;
//		gameObject->sprite->texture = App->modResources->FindByTextureName(std::to_string(score_value) + ".png");
//		if (isServer)
//			NetworkUpdate(gameObject);
//	}
//}

void Score::update()
{
}

void Score::destroy()
{
}

void Score::write(OutputMemoryStream& packet)
{
}

void Score::read(const InputMemoryStream& packet)
{
}

int Score::GetScoreValue()
{
	return 0;
}

void Score::SetScoreValue(int value)
{
}

void Ball::start()
{
	gameObject->tag = (uint32)(Random.next() * UINT_MAX);
	gameObject->position = { 0,0 };
	gameObject->angle = 90;

	if (isServer)
		NetworkUpdate(gameObject);
}

void Ball::update()
{

	const float advanceSpeed = 200.0f;
	gameObject->position += vec2FromDegrees(gameObject->angle) * dir * advanceSpeed * Time.deltaTime;

	if (isServer)
		NetworkUpdate(gameObject);
}

void Ball::destroy()
{
}

void Ball::onCollisionTriggered(Collider& c1, Collider& c2)
{
	if (c2.type == ColliderType::Player && c2.gameObject->tag != gameObject->tag)
	{
		ChangeDirection();
	}
	if (c2.type == ColliderType::Wall && c2.gameObject->tag != gameObject->tag)
	{

	}
}

void Ball::write(OutputMemoryStream& packet)
{
}

void Ball::read(const InputMemoryStream& packet)
{
}

float Ball::GetSecondsLived()
{
	return 0.0f;
}

void Ball::ChangeDirection()
{
	dir *= -1;
}
