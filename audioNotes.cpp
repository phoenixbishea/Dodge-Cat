int main(int agrc, char** agrv)
{
	Mix_Chunk* effect1; //Mix_Chunk  Is used for SFX
	Mix_Music* music; // Mix_Music is used for music
	Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 4096); //MIX_DEFAULT_FREQUENCY (22050 (HZ)), MIX_DEFAULT_FORMAT (Always use), 2 (1 mono 2 stereo), Chunk size?
	music = Mix_LoadMUS("MetalShit.wav"); //Mix_LoadMUS loads music given a file name
	effect1 = Mix_LoadWAV("meow.wav"); //Mix_LoadWav acts like Mix_LoadMUS 

	Mix_PlayMusic(music, -1); //Mix_PlayMusic plays music... First arg is music, second is the number of loops (-1 means it plays forever)

	Mix_PlayChannel(-1, effect1, 0); //Eight Channels (-1 for the first free unreserved),  second is the sfx, and third is the number of loops

	/* Remember that at the end of this we need to free all the resources */
	Mix_FreeChunk(effect1);
	Mix_FreeMusic(music);
	Mix_CloseAudio();

	return 0;
 }     