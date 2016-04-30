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
	
	bool moneySoundSignal;

public:
	GameAudio();
	~GameAudio();

	void initialize();

	void registerSoundEffect(SoundEffectTypes seType, LPWSTR wavFilePath);

	void processSoundEffect();
	void processShootSound();
	void processMoneySound();
	void processPunchSound();
	
	void processMusic();



	void recieveMoneySoundSignal() { moneySoundSignal = true; }

	void shutDown();
};

