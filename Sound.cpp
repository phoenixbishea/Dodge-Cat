#include "Sound.hpp"

Sound::Sound()
	: mBackgroundMusic(0),
	mEffect(0),
	mMusicVolume(0.3f),
	mEffectVolume(0.1f),
	mScoreUp(0),
	mSpray(0),
	mMovement(0)
{
}

//---------------------------------------------------------------------------
void Sound::initSound()
{
    srand (time(NULL));
    Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 4096);

    mBackgroundMusic = Mix_LoadMUS("The-Power-I-Feel.mp3");

    mMeowEffects.push_back(Mix_LoadWAV("angryMeow.wav"));
    mMeowEffects.push_back(Mix_LoadWAV("cat.wav"));
    mMeowEffects.push_back(Mix_LoadWAV("happyPurr.wav"));

    mScoreUp = Mix_LoadWAV("scoreUp1.wav");
    mSpray = Mix_LoadWAV("spraySound1.wav");
    mMovement = Mix_LoadWAV("turningMove.wav");

    setMusicVolume(mMusicVolume);
    setEffectVolume(mEffectVolume);

    Mix_PlayMusic(mBackgroundMusic, -1);
}

//---------------------------------------------------------------------------
void Sound::playSound(const char* effectName)
{
	if (effectName == "meow")
	{
		Mix_PlayChannel(-1, mMeowEffects.at(rand() % mMeowEffects.size()) ,0);
	}

	if (effectName == "score")
	{
		Mix_PlayChannel(0, mScoreUp, 0);
	}

	if (effectName == "spray")
	{
		Mix_PlayChannel(1, mSpray, 0);
	}

	if (effectName == "move")
	{
		std::cout << "Move sound\n";
		Mix_PlayChannel(2, mMovement, 0);
		Mix_Volume(2, MIX_MAX_VOLUME * mEffectVolume * 100);
	}

}

//---------------------------------------------------------------------------
void Sound::setMusicVolume(const float volume)
{
	Mix_VolumeMusic(MIX_MAX_VOLUME * volume);
	mMusicVolume = volume;
}

//---------------------------------------------------------------------------
void Sound::setEffectVolume(const float volume)
{
	Mix_Volume(-1, MIX_MAX_VOLUME * volume);
	mEffectVolume = volume;
}

//---------------------------------------------------------------------------
void Sound::muteUnmuteMusic()
{
	if (Mix_VolumeMusic(-1) == 0)
	{
		Mix_VolumeMusic(MIX_MAX_VOLUME * mMusicVolume);
	}
	else
	{
		Mix_VolumeMusic(0);
	}
}

//---------------------------------------------------------------------------
void Sound::muteUnmuteEffects()
{
	if (Mix_Volume(-1, -1) == 0)
	{
		Mix_Volume(-1, MIX_MAX_VOLUME * mEffectVolume);
	}
	else
	{
		Mix_Volume(-1, 0);
	}
}
