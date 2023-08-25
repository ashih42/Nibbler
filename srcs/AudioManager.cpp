#include "AudioManager.hpp"
#include "ResourceManager.hpp"
#include "NibblerException.hpp"

#include <dlfcn.h>

extern "C"
{
	AudioManager *			createAudioModule(void)
	{
		return (new AudioManager());
	}
}

static AudioManager *		createAudioManager(void)
{
	const std::string		libName = "myLibAudio.so";
	const std::string		funcName = "createAudioModule";

	void *					dl_handle;
	void *					func_ptr;
	AudioManager *			instance;

	dl_handle = dlopen(libName.c_str(), RTLD_LAZY | RTLD_LOCAL);
	if (dl_handle == nullptr)
		throw NibblerException("dlopen() failed on " + libName);

	func_ptr = dlsym(dl_handle, funcName.c_str());
	if (func_ptr == nullptr)
		throw NibblerException("dlsym() failed on " + libName + " : " + funcName);

	instance = reinterpret_cast<AudioManager * (*)(void)>(func_ptr)();
	if (instance == nullptr)
		throw NibblerException("createAudioManager() failed on " + libName + " : " + funcName);

	dlclose(dl_handle);
	return (instance);
}

AudioManager &				AudioManager::getInstance(void)
{
	static AudioManager *	instance = createAudioManager();

	return (*instance);
}

AudioManager::AudioManager(void)
{
	if (SDL_Init(SDL_INIT_AUDIO))
		throw NibblerException("SDL_Init() failed");
	
	if ((Mix_Init(MIX_FLAGS) & MIX_FLAGS) != MIX_FLAGS)
		throw NibblerException("Mix_Init() failed");
	
	if (Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0)
		throw NibblerException("Mix_OpenAudio() failed");

	Mix_VolumeMusic(MIX_MAX_VOLUME / 4);

	this->_bgmClips.push_back(this->_loadBGM(BGM_00));
	this->_bgmClips.push_back(this->_loadBGM(BGM_01));
	this->_bgmClips.push_back(this->_loadBGM(BGM_02));
	this->_bgmClips.push_back(this->_loadBGM(BGM_03));
	this->_bgmClips.push_back(this->_loadBGM(BGM_04));
	this->_bgmClips.push_back(this->_loadBGM(BGM_05));
	this->_bgmClips.push_back(this->_loadBGM(BGM_06));
	this->_bgmClips.push_back(this->_loadBGM(BGM_07));
	this->_bgmClips.push_back(this->_loadBGM(BGM_08));
	this->_bgmClips.push_back(this->_loadBGM(BGM_09));

	this->_sfxClips["eating"] = this->_loadSFX(EATING_SFX);
	this->_sfxClips["death"] = this->_loadSFX(DEATH_SFX);
	this->_sfxClips["hurt"] = this->_loadSFX(HURT_SFX);

	this->_sfxClips["1p_win"] = this->_loadSFX(_1P_WIN_SFX);
	this->_sfxClips["1p_lose"] = this->_loadSFX(_1P_LOSE_SFX);

	this->_sfxClips["2p_p1_win"] = this->_loadSFX(_2P_P1_WIN_SFX);
	this->_sfxClips["2p_p2_win"] = this->_loadSFX(_2P_P2_WIN_SFX);
	this->_sfxClips["2p_all_lose"] = this->_loadSFX(_2P_ALL_LOSE_SFX);
}

AudioManager::~AudioManager(void)
{
	for (Mix_Music * clip : this->_bgmClips)
		Mix_FreeMusic(clip);
	for (std::pair<std::string, Mix_Chunk *> item : this->_sfxClips)
		Mix_FreeChunk(item.second);
	Mix_Quit();
	SDL_Quit();
}

Mix_Music *					AudioManager::_loadBGM(std::string filename)
{
	Mix_Music *				clip = Mix_LoadMUS(ResourceManager::getInstance().getBasePath(filename).c_str());

	if (!clip)
		throw NibblerException("Mix_LoadMUS() failed on \'" + filename + "\'");
	return (clip);
}

Mix_Chunk *					AudioManager::_loadSFX(std::string filename)
{
	Mix_Chunk *				clip = Mix_LoadWAV(ResourceManager::getInstance().getBasePath(filename).c_str());
	
	if (!clip)
		throw NibblerException("Mix_LoadWAV() failed on \'" + filename + "\'");
	return (clip);
}

int							AudioManager::playBGM(void)
{
	int						index = std::rand() % this->_bgmClips.size();
	Mix_Music *				clip = this->_bgmClips[index];

	Mix_PlayMusic(clip, -1);		// loop forever
	return (index);
}

void						AudioManager::playBGM(int index)
{
	Mix_Music *				clip = this->_bgmClips[index];

	Mix_PlayMusic(clip, -1);		// loop forever
}

void						AudioManager::playSFX(std::string id)
{
	Mix_Chunk *				clip = this->_sfxClips[id];

	Mix_HaltChannel(-1);
	Mix_PlayChannel(-1, clip, 0);
}
