#include "Networks.h"
#include "ModuleBehaviour.h"

bool ModuleBehaviour::update()
{
	for (PongBarrier &behaviour : spaceships)
	{
		handleBehaviourLifeCycle(&behaviour);
	}
	
	for (Score &behaviour : scores)
	{
		handleBehaviourLifeCycle(&behaviour);
	}
	
	for (Ball &behaviour : balls)
	{
		handleBehaviourLifeCycle(&behaviour);
	}
	return true;
}

Behaviour *ModuleBehaviour::addBehaviour(BehaviourType behaviourType, GameObject *parentGameObject, uint32 space_ship_type)
{
	switch (behaviourType)
	{
	case BehaviourType::Score:
		return addScore(parentGameObject, space_ship_type);
	case BehaviourType::Spaceship:
		return addSpaceship(parentGameObject, space_ship_type);
	case BehaviourType::Ball:
		return addBall(parentGameObject);
	default:
		break;
	}
	
}

PongBarrier *ModuleBehaviour::addSpaceship(GameObject *parentGameObject, uint32 type)
{
	for (PongBarrier &behaviour : spaceships)
	{
		if (behaviour.gameObject == nullptr)
		{
			behaviour = {};
			behaviour.gameObject = parentGameObject;
			parentGameObject->behaviour = &behaviour;

			if (type == 1)
				behaviour.playerNum = behaviour.PlayerNum::PLAYER1;
			else if(type == 2)
				behaviour.playerNum = behaviour.PlayerNum::PLAYER2;

			return &behaviour;
		}
	}

	ASSERT(false);
	return nullptr;
}

Ball* ModuleBehaviour::addBall(GameObject* parentGameObject)
{
	for (Ball& behaviour : balls)
	{
		if (behaviour.gameObject == nullptr)
		{
			behaviour = {};
			behaviour.gameObject = parentGameObject;
			parentGameObject->behaviour = &behaviour;
			
			return &behaviour;
		}
	}

	ASSERT(false);
	return nullptr;
}

Score* ModuleBehaviour::addScore(GameObject* parentGameObject, uint32 type)
{
	for (Score& behaviour : scores)
	{
		if (behaviour.gameObject == nullptr)
		{
			behaviour = {};
			behaviour.gameObject = parentGameObject;
			parentGameObject->behaviour = &behaviour;
			if (type == 1)
				behaviour.scorePlayerNum = behaviour.ScorePlayerNum::SCORE_PLAYER1;
			else if (type == 2)
				behaviour.scorePlayerNum = behaviour.ScorePlayerNum::SCORE_PLAYER2;

			return &behaviour;
		}
	}

	ASSERT(false);
	return nullptr;
}

void ModuleBehaviour::handleBehaviourLifeCycle(Behaviour *behaviour)
{
	GameObject *gameObject = behaviour->gameObject;

	if (gameObject != nullptr)
	{
		switch (gameObject->state)
		{
		case GameObject::STARTING:
			behaviour->start();
			break;
		case GameObject::UPDATING:
			behaviour->update();
			break;
		case GameObject::DESTROYING:
			behaviour->destroy();
			gameObject->behaviour = nullptr;
			behaviour->gameObject = nullptr;
			break;
		default:;
		}
	}
}
