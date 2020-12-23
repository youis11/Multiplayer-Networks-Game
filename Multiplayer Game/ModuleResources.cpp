#include "Networks.h"
#include "ModuleResources.h"
#include<string>  



#if defined(USE_TASK_MANAGER)

void ModuleResources::TaskLoadTexture::execute()
{
	*texture = App->modTextures->loadTexture(filename);
	(*texture)->id = id;
}

#endif


bool ModuleResources::init()
{
	background = App->modTextures->loadTexture("background.jpg");

#if !defined(USE_TASK_MANAGER)
	space = App->modTextures->loadTexture("space_background.jpg");
	asteroid1 = App->modTextures->loadTexture("asteroid1.png");
	asteroid2 = App->modTextures->loadTexture("asteroid2.png");
	spacecraft1 = App->modTextures->loadTexture("spacecraft1.png");
	spacecraft2 = App->modTextures->loadTexture("spacecraft2.png");
	spacecraft3 = App->modTextures->loadTexture("spacecraft3.png");

	loadingFinished = true;
	completionRatio = 1.0f;
#else
	loadTextureAsync("space_background.jpg", &space, 0);
	loadTextureAsync("asteroid1.png",        &asteroid1, 1);
	loadTextureAsync("asteroid2.png",        &asteroid2, 2);
	loadTextureAsync("spacecraft1.png",      &spacecraft1, 3);
	loadTextureAsync("spacecraft2.png",      &spacecraft2, 4);
	loadTextureAsync("spacecraft3.png",      &spacecraft3, 5);
	loadTextureAsync("laser.png",            &laser, 6);
	loadTextureAsync("explosion1.png",       &explosion1, 7);

	loadTextureAsync("0.png",       &score_0, 8);
	loadTextureAsync("1.png",       &score_1, 9);
	loadTextureAsync("2.png",       &score_2, 10);
	loadTextureAsync("3.png",       &score_3, 11);
	loadTextureAsync("4.png",       &score_4, 12);
	loadTextureAsync("5.png",       &score_5, 13);
	loadTextureAsync("6.png",       &score_6, 14);
	loadTextureAsync("7.png",       &score_7, 15);
	loadTextureAsync("8.png",       &score_8, 16);
	loadTextureAsync("9.png",       &score_9, 17);
#endif

	audioClipLaser = App->modSound->loadAudioClip("laser.wav");
	audioClipExplosion = App->modSound->loadAudioClip("explosion.wav");
	//App->modSound->playAudioClip(audioClipExplosion);

	return true;
}

#if defined(USE_TASK_MANAGER)

void ModuleResources::loadTextureAsync(const char * filename, Texture **texturePtrAddress, int id)
{
	ASSERT(taskCount < MAX_RESOURCES);
	
	TaskLoadTexture *task = &tasks[taskCount++];
	task->owner = this;
	task->filename = filename;
	task->id = id;
	task->texture = texturePtrAddress;

	App->modTaskManager->scheduleTask(task, this);
}

Texture* ModuleResources::FindByTextureName(std::string textureFilename)
{
	if (textureFilename == "space_background.jpg")
		return space;
	else if (textureFilename == "asteroid1.png") 
		return asteroid1;
	else if (textureFilename == "asteroid2.png") 
		return asteroid2;
	else if (textureFilename == "spacecraft1.png") 
		return spacecraft1;
	else if (textureFilename == "spacecraft2.png") 
		return spacecraft2;
	else if (textureFilename == "spacecraft3.png") 
		return spacecraft3;
	else if (textureFilename == "laser.png") 
		return laser;
	else if (textureFilename == "explosion1.png") 
		return explosion1;
	else if (textureFilename == "0.png")
		return score_0;
	else if (textureFilename == "1.png")
		return score_1;
	else if (textureFilename == "2.png")
		return score_2;
	else if (textureFilename == "3.png")
		return score_3;
	else if (textureFilename == "4.png")
		return score_4;
	else if (textureFilename == "5.png")
		return score_5;
	else if (textureFilename == "6.png")
		return score_6;
	else if (textureFilename == "7.png")
		return score_7;
	else if (textureFilename == "8.png")
		return score_8;
	else if (textureFilename == "9.png")
		return score_9;
}

void ModuleResources::onTaskFinished(Task * task)
{
	ASSERT(task != nullptr);

	TaskLoadTexture *taskLoadTexture = dynamic_cast<TaskLoadTexture*>(task);

	for (uint32 i = 0; i < taskCount; ++i)
	{
		if (task == &tasks[i])
		{
			finishedTaskCount++;
			task = nullptr;
			break;
		}
	}

	ASSERT(task == nullptr);

	if (finishedTaskCount == taskCount)
	{
		finishedLoading = true;

		// Create the explosion animation clip
		explosionClip = App->modRender->addAnimationClip();
		explosionClip->frameTime = 0.1f;
		explosionClip->loop = false;
		for (int i = 0; i < 16; ++i)
		{
			float x = (i % 4) / 4.0f;
			float y = (i / 4) / 4.0f;
			float w = 1.0f / 4.0f;
			float h = 1.0f / 4.0f;
			explosionClip->addFrameRect(vec4{ x, y, w, h });
		}
	}
}

#endif
