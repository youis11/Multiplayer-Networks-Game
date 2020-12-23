#pragma once

#include "Behaviours.h"

class ModuleBehaviour : public Module
{
public:

	bool update() override;

	Behaviour * addBehaviour(BehaviourType behaviourType, GameObject *parentGameObject);
	Spaceship * addSpaceship(GameObject *parentGameObject);
	Laser     * addLaser(GameObject *parentGameObject);
	//Ball* addBall(GameObject *parentGameObject);
	Score* addScore(GameObject* parentGameObject);

private:

	void handleBehaviourLifeCycle(Behaviour * behaviour);

	Spaceship spaceships[MAX_CLIENTS];
	Laser lasers[MAX_GAME_OBJECTS/2];
	Score scores[MAX_GAME_OBJECTS/2];
	//Ball balls[MAX_GAME_OBJECTS/2];
};

