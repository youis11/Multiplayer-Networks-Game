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
	float ball_x;
	float ball_y;
	float speedX;
	float speedY;
	
	BehaviourType type() const override { return BehaviourType::Ball; }

	void start() override;

	void update() override;

	void destroy() override;

	void onCollisionTriggered(Collider& c1, Collider& c2) override;

	void write(OutputMemoryStream& packet) override;

	void read(const InputMemoryStream& packet) override;
};

struct Score : public Behaviour
{
	int score_value = 0;

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

	int GetScoreValue();
	void SetScoreValue(int value);
};
