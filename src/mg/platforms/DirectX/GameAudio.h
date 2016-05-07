#pragma once

#include "mg\platforms\DirectX\SDKwavefile.h"

enum SoundEffectTypes
{
	ENUM_SOUND_EFFECT_DAMAGE,
	ENUM_SOUND_EFFECT_HEAL,
	ENUM_SOUND_EFFECT_MONEY,
	ENUM_SOUND_EFFECT_PUNCH,
	ENUM_SOUND_EFFECT_SHOOT
};

enum MusicTypes
{
	ENUM_MUSIC_MAIN_THEME,
	ENUM_MUSIC_LEVEL_COMPLETE,
	ENUM_MUSIC_GAMEOVER,
	ENUM_MUSIC_INGAME,
	ENUM_MUSIC_NONE
};

class GameAudio
{
private:
	IXAudio2* xAudio2Engine = 0;
	IXAudio2MasteringVoice* masterVoice = 0;

	map<SoundEffectTypes, bool> soundEffectRegistrationMap;
	map<SoundEffectTypes, IXAudio2SourceVoice*> soundEffectMap;
	map<SoundEffectTypes, XAUDIO2_BUFFER*> audioBufferPrototypeMap;

	// music map
	map<MusicTypes, bool> musicRegistrationMap;
	map<MusicTypes, IXAudio2SourceVoice*> musicMap;
	map<MusicTypes, XAUDIO2_BUFFER*> musicBufferPrototypeMap;
	////map<MusicTypes, bool> musicSwitchMap;
	MusicTypes currentMusicPlaying;

	bool moneySoundSignal;
	bool healSoundSignal;

	bool levelCompleteMusicBuffered;

public:
	GameAudio();
	~GameAudio();

	void initialize();

	void registerSoundEffect(SoundEffectTypes seType, LPWSTR wavFilePath);
	void registerMusic(MusicTypes muType, LPWSTR wavFilePath);

	void processSoundEffect();
	void processShootSound();
	void processMoneySound();
	void processPunchSound();
	void processHealSound();
	void processDamageSound();

	void processMusic();
	void stopMusic(MusicTypes musicType);
	void playMusicRepeat(MusicTypes musicType);
	void playMusicOnce(MusicTypes musicType);


	void recieveMoneySoundSignal() { moneySoundSignal = true; }
	void recieveHealSoundSignal() { healSoundSignal = true; }

	void shutDown();
};

