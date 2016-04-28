#include "mg_VS\stdafx.h"
#include "mg\platforms\DirectX\GameAudio.h"
#include "mg\game\Game.h"
#include "mg\gsm\state\GameStateManager.h"
#include "mg\gsm\sprite\SpriteManager.h"
#include "mg\gsm\sprite\PlayerSprite.h"
#include "mg\gsm\state\GameState.h"

GameAudio::GameAudio()
{
}


GameAudio::~GameAudio()
{
}

void GameAudio::initialize()
{
	bool ciFailed = FAILED(CoInitializeEx(0, COINIT_MULTITHREADED));
	UINT32 flags = 0;

	bool xa2cSuccess = SUCCEEDED(XAudio2Create(&xAudio2Engine));

	bool cmvSuccess = SUCCEEDED(xAudio2Engine->CreateMasteringVoice(&masterVoice,
		XAUDIO2_DEFAULT_CHANNELS, XAUDIO2_DEFAULT_SAMPLERATE, 0, 0, 0));
	
	//// -- still not sure if I need only one master voice or multiple(one master per one source voice)
}

void GameAudio::registerShootSoundEffect(LPWSTR wavFilePath)
{
	CWaveFile wav;	/// wav file will be destroyed at the end of this function

	//// All the file references are rooted at the directory "Apps/Rebelle/data"
	bool wavOpenSuccess = SUCCEEDED(wav.Open(wavFilePath, 0, WAVEFILE_READ));

	WAVEFORMATEX *format = wav.GetFormat();
	unsigned long wavSize = wav.GetSize();
	unsigned char *wavData = new unsigned char[wavSize];

	bool wavReadSuccess = SUCCEEDED(wav.Read(wavData, wavSize, &wavSize));

	IXAudio2SourceVoice* srcVoice;	/// this will be stored in the map of this class

	bool csvSuccess = SUCCEEDED(xAudio2Engine->CreateSourceVoice(&srcVoice, format,
		0, XAUDIO2_DEFAULT_FREQ_RATIO, 0, 0, 0));
	
	XAUDIO2_BUFFER *bufferPrototype = new XAUDIO2_BUFFER();
	bufferPrototype->pAudioData = wavData;
	bufferPrototype->Flags = XAUDIO2_END_OF_STREAM;
	bufferPrototype->AudioBytes = wavSize;
	
	//// register sound buffer prototype
	audioBufferPrototypeMap[ENUM_SOUND_EFFECT_SHOOT] = bufferPrototype;
	
	//// mark this sourceVoice as registered
	soundEffectRegistrationMap[ENUM_SOUND_EFFECT_SHOOT] = true;

	//// store the source voice
	soundEffectMap[ENUM_SOUND_EFFECT_SHOOT] = srcVoice;
}

void GameAudio::processSoundEffect()
{
	Game *game = Game::getSingleton();
	GameStateManager *gsm = game->getGSM();
	GameState gameState = gsm->getCurrentGameState();

	if (gameState == GS_GAME_IN_PROGRESS)
	{
		//// shoot sound effect
		if (soundEffectRegistrationMap[ENUM_SOUND_EFFECT_SHOOT] == true)
		{
			SpriteManager *spriteMgr = gsm->getSpriteManager();
			PlayerSprite *player = spriteMgr->getPlayer();

			wstring playerState = player->getCurrentState();

			if (playerState.compare(L"SHOOT_LEFT") == 0 || playerState.compare(L"SHOOT_RIGHT") == 0
				|| playerState.compare(L"SHOOT_BACK") == 0 || playerState.compare(L"SHOOT_FRONT") == 0)
			{
				IXAudio2SourceVoice *shootSound = soundEffectMap[ENUM_SOUND_EFFECT_SHOOT];

				XAUDIO2_VOICE_STATE voiceState;
				shootSound->GetState(&voiceState);

				//// [voiceState.BuffersQueued <= 0] means there are nothing in the buffer
				//// so let's make a new buffer to queue the sound
				if (voiceState.BuffersQueued <= 0)
				{
					XAUDIO2_BUFFER *proto = audioBufferPrototypeMap[ENUM_SOUND_EFFECT_SHOOT];
					bool ssbSuccess = SUCCEEDED(shootSound->SubmitSourceBuffer(proto));
					shootSound->Start();
				}
				//// if there is something in the buffer
				else
				{
					/// do nothing
				}
			}
		}
	}
}

void GameAudio::shutDown()
{
	if (soundEffectRegistrationMap[ENUM_SOUND_EFFECT_SHOOT] == true)
	{
		IXAudio2SourceVoice* shootSound = soundEffectMap[ENUM_SOUND_EFFECT_SHOOT];
		shootSound->DestroyVoice();

		XAUDIO2_BUFFER *buffer = audioBufferPrototypeMap[ENUM_SOUND_EFFECT_SHOOT];
		delete[] buffer->pAudioData;
		delete buffer;
	}

	//// --- put other effects' destructors here ------

	masterVoice->DestroyVoice();
	xAudio2Engine->Release();
	CoUninitialize();
}
