#include "Networks.h"
#include "Behaviours.h"
#include <string>
#include <random>


void PongBarrier::start()
{
	gameObject->tag = (uint32)(Random.next() * UINT_MAX);
	gameObject->angle = 0;
	PlayAudioClip("playerEnterGame.wav");

	switch (playerNum)
	{
	case PongBarrier::PLAYER1:
		gameObject->position = {450,0};
		break;
	case PongBarrier::PLAYER2:
		gameObject->position = { -450,0 };
		break;
	case PongBarrier::NONE:
		break;
	default:
		break;
	}
	if (isServer)
		NetworkUpdate(gameObject);
}

void PongBarrier::onInput(const InputController &input)
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
}

void PongBarrier::update()
{
}

void PongBarrier::destroy()
{
}

void PongBarrier::onCollisionTriggered(Collider &c1, Collider &c2)
{
}

void PongBarrier::write(OutputMemoryStream & packet)
{
}

void PongBarrier::read(const InputMemoryStream & packet)
{
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


void Score::update()
{
	if (score_value_player1 >= 10 || score_value_player2 >= 10)
	{
		score_value_player1 = 0;
		score_value_player2 = 0;
		for (Score& behaviour : App->modBehaviour->scores)
		{
			if (scorePlayerNum == ScorePlayerNum::SCORE_PLAYER2 || scorePlayerNum == ScorePlayerNum::SCORE_PLAYER1)
			{
				//Change to new Texture
				gameObject->sprite->texture = NextScoreTexture(gameObject->sprite->texture, 9);
				NetworkUpdate(gameObject);
			}
		}

	}
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
	my_score++;
	if (my_score >= 10) my_score = 0;
	next_t_name.replace(0, 1, std::to_string(my_score));
	Texture* next_t = App->modResources->FindByTextureName(next_t_name);
	return next_t;
}


void Ball::start()
{
	gameObject->tag = (uint32)(Random.next() * UINT_MAX);
	gameObject->position = { 0,0 };
	gameObject->angle = 0;

	speedX = 500;
	std::default_random_engine generator;
	std::uniform_real_distribution<double> distribution(-200, +200); 
	speedY = distribution(generator);

	PlayAudioClip("startGame.wav");


	if (isServer)
		NetworkUpdate(gameObject);
}

void Ball::update()
{
	if (!restart)
	{
		ball_y += speedY * Time.deltaTime;
		ball_x += speedX * Time.deltaTime;

		//speedX+=25; //Unlock this if you want to play pong for one ;) ;) ;)

		gameObject->position = { ball_x + extraSpeed, ball_y };
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
			speedX = 500 * dir;
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
			PlayAudioClip("playerBall.wav");
		}
		
	}
	if (c2.type == ColliderType::Wall && c2.gameObject->tag != gameObject->tag)
	{
		if (isServer)
		{			
			speedY = -speedY;
			PlayAudioClip("playerBall.wav");
		}
	}
	if (c2.type == ColliderType::Goal && c2.gameObject->tag != gameObject->tag)
	{
		if (isServer)
		{		
			PlayAudioClip("startGame.wav");

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

void Behaviour::PlayAudioClip(std::string fileName)
{
	App->modSound->playAudioClip(App->modResources->FindByAudioClipName(fileName));
	App->modNetServer->playNetworkAudio(fileName);
}
