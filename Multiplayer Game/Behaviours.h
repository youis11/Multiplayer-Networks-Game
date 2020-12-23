#pragma once


enum class BehaviourType : uint8;

struct Behaviour
{
	GameObject *gameObject = nullptr;
	bool isServer = false;
	bool isLocalPlayer = false;

	virtual BehaviourType type() const = 0;

	virtual void start() { }

	virtual void onInput(const InputController &input) { }

	virtual void update() { }

	virtual void destroy() { }

	virtual void onCollisionTriggered(Collider &c1, Collider &c2) { }

	virtual void write(OutputMemoryStream &packet) { }

	virtual void read(const InputMemoryStream &packet) { }
};


enum class BehaviourType : uint8
{
	None,
	Spaceship,
	Laser,
	Ball,
	Score
};


struct Laser : public Behaviour
{
	float secondsSinceCreation = 0.0f;

	BehaviourType type() const override { return BehaviourType::Laser; }

	void start() override;

	void update() override;

	float GetSecondsLived();
};


struct Spaceship : public Behaviour
{
	//TO DELETE
	static const uint8 MAX_HIT_POINTS = 5;
	uint8 hitPoints = MAX_HIT_POINTS;
	GameObject *lifebar = nullptr;
	//----------

	bool player_waiting = false;

	enum PlayerNum
	{
		PLAYER1,
		PLAYER2,
		NONE
	};
	PlayerNum playerNum;

	BehaviourType type() const override { return BehaviourType::Spaceship; }

	void start() override;

	void onInput(const InputController &input) override;

	void update() override;

	void destroy() override;

	void onCollisionTriggered(Collider &c1, Collider &c2) override;

	void write(OutputMemoryStream &packet) override;

	void read(const InputMemoryStream &packet) override;
};

struct Ball : public Behaviour
{
	float ball_x = 0;
	float ball_y = 0;
	float speedX = 0;
	float speedY = 0;
	float extraSpeed = 0;
	float extraSpeedTime = 0;
	float time_to_restart = 0;
	float restart_timer = 0;
	bool restart = false;

	BehaviourType type() const override { return BehaviourType::Ball; }

	void start() override;

	void update() override;

	void destroy() override;

	void onCollisionTriggered(Collider& c1, Collider& c2) override;

	void write(OutputMemoryStream& packet) override;

	void read(const InputMemoryStream& packet) override;

	void ResetBall(int player_scored);
};

struct Score : public Behaviour
{
	int score_value_player1 = 0;
	int score_value_player2 = 0;
	bool player1_scored = false;
	bool player2_scored = false;

	enum ScorePlayerNum
	{
		SCORE_PLAYER1,
		SCORE_PLAYER2,
		NONE
	};
	ScorePlayerNum scorePlayerNum;

	BehaviourType type() const override { return BehaviourType::Score; }

	void start() override;

	void update() override;

	void destroy() override;

	void write(OutputMemoryStream& packet) override;

	void read(const InputMemoryStream& packet) override;

	Texture* NextScoreTexture(Texture* texture, int my_score);
};
