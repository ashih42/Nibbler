#ifndef AUDIO_MANAGER_HPP
# define AUDIO_MANAGER_HPP

# include <string>
# include <vector>
# include <unordered_map>
# include <SDL.h>
# include <SDL_mixer.h>

# define MIX_FLAGS									(MIX_INIT_OGG)

# define BGM_00										"bgm/ANewMorning.ogg"
# define BGM_01										"bgm/Crystals.ogg"
# define BGM_02										"bgm/Daisuke.ogg"
# define BGM_03										"bgm/DeepCover.ogg"
# define BGM_04										"bgm/ElectricDreams.ogg"
# define BGM_05										"bgm/Flatline.ogg"
# define BGM_06										"bgm/Hotline.ogg"
# define BGM_07										"bgm/Hydrogen.ogg"
# define BGM_08										"bgm/InnerAnimal.ogg"
# define BGM_09										"bgm/ItsSafeNow.ogg"

# define EATING_SFX									"sounds/om_nom.wav"
# define DEATH_SFX									"sounds/wilheim_scream.wav"
# define HURT_SFX									"sounds/minecraft_ugh.wav"

# define _1P_WIN_SFX								"say/1P_win.wav"
# define _1P_LOSE_SFX								"say/1P_lose.wav"

# define _2P_P1_WIN_SFX								"say/2P_P1_win.wav"
# define _2P_P2_WIN_SFX								"say/2P_P2_win.wav"
# define _2P_ALL_LOSE_SFX							"say/2P_all_lose.wav"

class AudioManager
{
public:

	AudioManager(void);
	~AudioManager(void);

	static AudioManager &							getInstance(void);

	int												playBGM(void);
	void											playBGM(int index);
	void											playSFX(std::string id);

private:

	std::vector<Mix_Music *>						_bgmClips;
	std::unordered_map<std::string, Mix_Chunk *>	_sfxClips;

	AudioManager(const AudioManager & src);
	AudioManager & operator=(const AudioManager & rhs);

	Mix_Music *										_loadBGM(std::string filename);
	Mix_Chunk *										_loadSFX(std::string filename);

};

#endif
