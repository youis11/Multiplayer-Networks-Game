#pragma once

#include "Behaviours.h"

class ModuleBehaviour : public Module
{
public:

	bool update() override;

	Behaviour * addBehaviour(BehaviourType behaviourType, GameObject *parentGameObject, uint32 space_ship_type);
	Spaceship * addSpaceship(GameObject *parentGameObject, uint32 type);
	Laser     * addLaser(GameObject *parentGameObject);
	Ball* addBall(GameObject *parentGameObject);
	Score* addScore(GameObject* parentGameObject, uint32 type);

private:

	void handleBehaviourLifeCycle(Behaviour * behaviour);

	Spaceship spaceships[MAX_CLIENTS];
	Laser lasers[MAX_GAME_OBJECTS];
	Score scores[MAX_GAME_OBJECTS];
	Ball balls[MAX_GAME_OBJECTS];
};

