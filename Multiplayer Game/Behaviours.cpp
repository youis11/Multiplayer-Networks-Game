#include "Networks.h"
#include "Behaviours.h"
#include <string>
#include <random>

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
			
		}

		if (gameObject->position.y < min_high)
			gameObject->position.y = min_high;
		if (gameObject->position.y > max_high)
			gameObject->position.y = max_high;
	}
	if (isServer)
		NetworkUpdate(gameObject);

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
}

void Spaceship::onCollisionTriggered(Collider &c1, Collider &c2)
{
	//TO DELETE
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
	//----------
}

void Spaceship::write(OutputMemoryStream & packet)
{
	//packet << hitPoints;
}

void Spaceship::read(const InputMemoryStream & packet)
{
	//packet >> hitPoints;
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

Texture* Score::NextScoreTexture(Texture* texture, int my_score)
{
	std::string next_t_name = texture->filename;
	next_t_name.replace(0, 1, std::to_string(++my_score));
	Texture* next_t = App->modResources->FindByTextureName(next_t_name);
	return next_t;
}


void Ball::start()
{
	gameObject->tag = (uint32)(Random.next() * UINT_MAX);
	gameObject->position = { 0,0 };
	gameObject->angle = 0;

	speedX = 300;
	std::default_random_engine generator;
	std::uniform_real_distribution<double> distribution(-200, +200); 
	speedY = distribution(generator);

	if (isServer)
		NetworkUpdate(gameObject);
}

void Ball::update()
{
	if (!restart)
	{
		ball_y += speedY * Time.deltaTime;
		ball_x += speedX * Time.deltaTime;

		extraSpeedTime += Time.deltaTime;
		if (extraSpeedTime >= 4) {
			extraSpeed += 20;
			extraSpeedTime = 0;
		}

		gameObject->position = { ball_x + extraSpeed, ball_y + extraSpeed };
	}
	else
	{
		if (restart_timer >= time_to_restart)
		{
			restart = false;
			restart_timer = 0;
			std::default_random_engine generator;
			std::uniform_real_distribution<double> distribution(-200, +200);
			int dir = 0;
			if (gameObject->position.x < 0) dir = 1;
			if (gameObject->position.x > 0) dir = -1;
			
			speedY = distribution(generator);
			speedX = 300 * dir
				;
		}
		else
			restart_timer += Time.deltaTime;
	}
	

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
		if (isServer)
		{			
			speedX = -speedX;
			std::default_random_engine generator;
			std::uniform_real_distribution<double> distribution(5, 8);
			speedY = (gameObject->position.y - c2.gameObject->position.y) * distribution(generator);
		}
		
	}
	if (c2.type == ColliderType::Wall && c2.gameObject->tag != gameObject->tag)
	{
		if (isServer)
		{			
			speedY = -speedY;
		}
	}
	if (c2.type == ColliderType::Goal && c2.gameObject->tag != gameObject->tag)
	{
		if (isServer)
		{		
			if (c2.gameObject->position.x > 0) //Right goal, P2 scores 1
			{
				for (Score& behaviour : App->modBehaviour->scores)
				{
					if (behaviour.scorePlayerNum == behaviour.ScorePlayerNum::SCORE_PLAYER2)
					{
						//Change to new Texture
						behaviour.gameObject->sprite->texture = behaviour.NextScoreTexture(behaviour.gameObject->sprite->texture, behaviour.score_value_player2);
						behaviour.score_value_player2++;
						ResetBall(2);
						NetworkUpdate(behaviour.gameObject);
						break;
					}
				}
			}
			else if (c2.gameObject->position.x < 0) // Left goal, P1 scores 1
			{
				for (Score& behaviour : App->modBehaviour->scores)
				{
					if (behaviour.scorePlayerNum == behaviour.ScorePlayerNum::SCORE_PLAYER1)
					{
						//Change to new Texture
						behaviour.gameObject->sprite->texture = behaviour.NextScoreTexture(behaviour.gameObject->sprite->texture, behaviour.score_value_player1);
						behaviour.score_value_player1++;
						ResetBall(1);
						NetworkUpdate(behaviour.gameObject);
						break;
					}
				}
			}
		}
	}
}

void Ball::write(OutputMemoryStream& packet)
{
}

void Ball::read(const InputMemoryStream& packet)
{
}

void Ball::ResetBall(int player_scored)
{
	speedX = 0;
	speedY = 0;
	extraSpeed = 0;
	extraSpeedTime = 0;
	time_to_restart = 2;
	restart_timer = 0;
	restart = true;

	ball_y = 0;
	if(player_scored == 1)	ball_x = -50;
	else if(player_scored == 2)	ball_x = 50;

	gameObject->position = {ball_x, ball_y};

}
