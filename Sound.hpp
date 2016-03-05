#ifndef Sound_hpp
#define Sound_hpp

#include <SDL.h>
#include <SDL_mixer.h>

#include <vector>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

class Sound 
{
private:
	Mix_Music* mBackgroundMusic;
    Mix_Chunk* mEffect;
    std::vector<Mix_Chunk*> mMeowEffects;
    
    Mix_Chunk* mScoreUp;
    Mix_Chunk* mSpray;

    float mMusicVolume;
    float mEffectVolume; 
public:
	Sound();

	void initSound();

	void playSound(const char* effectName);

	void setMusicVolume(const float volume);
	void setEffectVolume(const float volume);

	void muteUnmuteMusic();
	void muteUnmuteEffects();
};

#endif
