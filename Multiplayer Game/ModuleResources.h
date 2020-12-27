#pragma once

#define USE_TASK_MANAGER
struct Texture;

class ModuleResources : public Module
{
public:
#define MAX_SCORE_TEXTURES 10

	Texture *background = nullptr;
	Texture *space = nullptr;
	Texture *asteroid1 = nullptr;
	Texture *asteroid2 = nullptr;
	Texture *spacecraft1 = nullptr;
	Texture *spacecraft2 = nullptr;
	Texture *spacecraft3 = nullptr;
	Texture *laser = nullptr;
	Texture *explosion1 = nullptr;
	Texture *score_0 = nullptr;
	Texture *score_1 = nullptr;
	Texture *score_2 = nullptr;
	Texture *score_3 = nullptr;
	Texture *score_4 = nullptr;
	Texture *score_5 = nullptr;
	Texture *score_6 = nullptr;
	Texture *score_7 = nullptr;
	Texture *score_8 = nullptr;
	Texture *score_9 = nullptr;

	AnimationClip *explosionClip = nullptr;

	AudioClip *audioClipLaser = nullptr;
	AudioClip *audioClipExplosion = nullptr;
	AudioClip *audioClipPlayerJoined = nullptr;
	AudioClip *audioClipBallPlayer = nullptr;
	AudioClip *audioClipBallWall = nullptr;
	AudioClip *audioClipGoal = nullptr;
	AudioClip *audioClipBGM = nullptr;
	AudioClip *audioClipPlayerWin = nullptr;
	AudioClip *audioClipStartGame = nullptr;

	bool finishedLoading = false;

	Texture* FindByTextureName(std::string textureFilename);
	AudioClip* FindByAudioClipName(std::string audioFilename);
private:

	bool init() override;

#if defined(USE_TASK_MANAGER)
	
	class TaskLoadTexture : public Task
	{
	public:

		const char *filename = nullptr;
		Texture **texture = nullptr;
		int id = 0;

		void execute() override;
	};

	static const int MAX_RESOURCES = 64;
	TaskLoadTexture tasks[MAX_RESOURCES] = {};
	uint32 taskCount = 0;
	uint32 finishedTaskCount = 0;

	void onTaskFinished(Task *task) override;

	void loadTextureAsync(const char *filename, Texture **texturePtrAddress, int id);


#endif

};

