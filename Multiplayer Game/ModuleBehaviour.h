#pragma once

#include "Behaviours.h"

class ModuleBehaviour : public Module
{
public:

	bool update() override;

	Behaviour * addBehaviour(BehaviourType behaviourType, GameObject *parentGameObject, uint32 space_ship_type);
	PongBarrier * addSpaceship(GameObject *parentGameObject, uint32 type);
	Ball* addBall(GameObject *parentGameObject);
	Score* addScore(GameObject* parentGameObject, uint32 type);

private:

	void handleBehaviourLifeCycle(Behaviour * behaviour);

	PongBarrier spaceships[MAX_CLIENTS];
	Ball balls[MAX_GAME_OBJECTS];
public:
	Score scores[MAX_GAME_OBJECTS];

};

