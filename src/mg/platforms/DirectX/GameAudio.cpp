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
	currentMusicPlaying = ENUM_MUSIC_NONE;	//// initially, current music is none
	
	bool ciFailed = FAILED(CoInitializeEx(0, COINIT_MULTITHREADED));
	UINT32 flags = 0;

	bool xa2cSuccess = SUCCEEDED(XAudio2Create(&xAudio2Engine));

	bool cmvSuccess = SUCCEEDED(xAudio2Engine->CreateMasteringVoice(&masterVoice,
		XAUDIO2_DEFAULT_CHANNELS, XAUDIO2_DEFAULT_SAMPLERATE, 0, 0, 0));
	
	//// -- still not sure if I need only one master voice or multiple(one master per one source voice)
}

void GameAudio::registerSoundEffect(SoundEffectTypes seType, LPWSTR wavFilePath)
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
	audioBufferPrototypeMap[seType] = bufferPrototype;

	//// mark this sourceVoice as registered
	soundEffectRegistrationMap[seType] = true;

	//// store the source voice
	soundEffectMap[seType] = srcVoice;
}

void GameAudio::registerMusic(MusicTypes muType, LPWSTR wavFilePath)
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
	musicBufferPrototypeMap[muType] = bufferPrototype;

	//// mark this sourceVoice as registered
	musicRegistrationMap[muType] = true;

	//// store the source voice
	musicMap[muType] = srcVoice;
}

void GameAudio::processSoundEffect()
{
	Game *game = Game::getSingleton();
	GameStateManager *gsm = game->getGSM();
	GameState gameState = gsm->getCurrentGameState();

	if (gameState == GS_GAME_IN_PROGRESS)
	{
		//// shoot sound effect
		processShootSound();

		//// money sound effect
		processMoneySound();

		//// punch sound effect
		processPunchSound();

		//// heal sound effect
		processHealSound();

		//// damage sound effect
		processDamageSound();
	}
}

void GameAudio::processShootSound()
{
	if (soundEffectRegistrationMap[ENUM_SOUND_EFFECT_SHOOT] == true)
	{
		Game *game = Game::getSingleton();
		GameStateManager *gsm = game->getGSM();
		SpriteManager *spriteMgr = gsm->getSpriteManager();
		/*if (moneySoundSignal == true)
		{
			IXAudio2SourceVoice *moneySound = soundEffectMap[ENUM_SOUND_EFFECT_MONEY];

			XAUDIO2_VOICE_STATE voiceState;
			moneySound->GetState(&voiceState);

			XAUDIO2_BUFFER *proto = audioBufferPrototypeMap[ENUM_SOUND_EFFECT_MONEY];
			bool ssbSuccess = SUCCEEDED(moneySound->SubmitSourceBuffer(proto));
			moneySound->Start();

			moneySoundSignal = false;
		}
		*/
		if (shootSoundSignal == true)
		{
			IXAudio2SourceVoice *shootSound = soundEffectMap[ENUM_SOUND_EFFECT_SHOOT];

			XAUDIO2_VOICE_STATE voiceState;
			shootSound->GetState(&voiceState);

			XAUDIO2_BUFFER *proto = audioBufferPrototypeMap[ENUM_SOUND_EFFECT_SHOOT];
			bool ssbSuccess = SUCCEEDED(shootSound->SubmitSourceBuffer(proto));
			shootSound->Start();

			shootSoundSignal = false;
		}

		/*
		list<Bot*>::iterator botIt = spriteMgr->getBotsIterator();
		list<Bot*>::iterator endBotIt = spriteMgr->getEndOfBotsIterator();
		while (botIt != endBotIt)
		{
		Bot *bot = (*botIt);
		wstring botCurState = bot->getCurrentState();
		if (botCurState.compare(L"SHOOT_LEFT") == 0 || botCurState.compare(L"SHOOT_RIGHT") == 0
		|| botCurState.compare(L"SHOOT_BACK") == 0 || botCurState.compare(L"SHOOT_FRONT") == 0)
		{
		IXAudio2SourceVoice *shootSound = soundEffectMap[ENUM_SOUND_EFFECT_SHOOT];

		XAUDIO2_VOICE_STATE voiceState;
		shootSound->GetState(&voiceState);

		if (voiceState.BuffersQueued <= 0)
		{
		XAUDIO2_BUFFER *proto = audioBufferPrototypeMap[ENUM_SOUND_EFFECT_SHOOT];
		bool ssbSuccess = SUCCEEDED(shootSound->SubmitSourceBuffer(proto));
		shootSound->Start();
		}
		}
		botIt++;
		}

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
		*/
		
	}
}

void GameAudio::processMoneySound()
{
	if (soundEffectRegistrationMap[ENUM_SOUND_EFFECT_MONEY] == true)
	{
		if (moneySoundSignal == true)
		{
			IXAudio2SourceVoice *moneySound = soundEffectMap[ENUM_SOUND_EFFECT_MONEY];

			XAUDIO2_VOICE_STATE voiceState;
			moneySound->GetState(&voiceState);

			XAUDIO2_BUFFER *proto = audioBufferPrototypeMap[ENUM_SOUND_EFFECT_MONEY];
			bool ssbSuccess = SUCCEEDED(moneySound->SubmitSourceBuffer(proto));
			moneySound->Start();

			moneySoundSignal = false;
		}
	}
}

void GameAudio::processPunchSound()
{
	if (soundEffectRegistrationMap[ENUM_SOUND_EFFECT_PUNCH] == true)
	{
		Game *game = Game::getSingleton();
		GameStateManager *gsm = game->getGSM();
		SpriteManager *spriteMgr = gsm->getSpriteManager();
		PlayerSprite *player = spriteMgr->getPlayer();

		wstring playerState = player->getCurrentState();

		if (playerState.compare(L"PUNCH_LEFT") == 0 || playerState.compare(L"PUNCH_RIGHT") == 0
			|| playerState.compare(L"PUNCH_BACK") == 0 || playerState.compare(L"PUNCH_FRONT") == 0)
		{
			IXAudio2SourceVoice *punchSound = soundEffectMap[ENUM_SOUND_EFFECT_PUNCH];

			XAUDIO2_VOICE_STATE voiceState;
			punchSound->GetState(&voiceState);

			//// [voiceState.BuffersQueued <= 0] means there are nothing in the buffer
			//// so let's make a new buffer to queue the sound
			if (voiceState.BuffersQueued <= 0)
			{
				XAUDIO2_BUFFER *proto = audioBufferPrototypeMap[ENUM_SOUND_EFFECT_PUNCH];
				bool ssbSuccess = SUCCEEDED(punchSound->SubmitSourceBuffer(proto));
				punchSound->Start();
			}
			//// if there is something in the buffer
			else
			{
				/// do nothing
			}
		}
	}
}

void GameAudio::processHealSound()
{
	if (soundEffectRegistrationMap[ENUM_SOUND_EFFECT_HEAL] == true)
	{
		Game *game = Game::getSingleton();
		GameStateManager *gsm = game->getGSM();
		SpriteManager *spriteMgr = gsm->getSpriteManager();
		PlayerSprite *player = spriteMgr->getPlayer();

		bool isHealing = player->getIshealing();

		if (isHealing == true)
		{
			IXAudio2SourceVoice *healSound = soundEffectMap[ENUM_SOUND_EFFECT_HEAL];

			XAUDIO2_VOICE_STATE voiceState;
			healSound->GetState(&voiceState);

			//// [voiceState.BuffersQueued <= 0] means there are nothing in the buffer
			//// so let's make a new buffer to queue the sound
			if (voiceState.BuffersQueued <= 0)
			{
				XAUDIO2_BUFFER *proto = audioBufferPrototypeMap[ENUM_SOUND_EFFECT_HEAL];
				bool ssbSuccess = SUCCEEDED(healSound->SubmitSourceBuffer(proto));
				healSound->Start();
				//// After all, there will be only one buffer node in the queue always ...
			}
			//// if there is something in the buffer
			else
			{
				/// do nothing
			}
		}
	}
}

void GameAudio::processDamageSound()
{
	if (soundEffectRegistrationMap[ENUM_SOUND_EFFECT_DAMAGE] == true)
	{
		Game *game = Game::getSingleton();
		GameStateManager *gsm = game->getGSM();
		SpriteManager *spriteMgr = gsm->getSpriteManager();
		PlayerSprite *player = spriteMgr->getPlayer();

		wstring playerState = player->getCurrentState();

		if (playerState.compare(L"DAMAGE_BACK") == 0 || playerState.compare(L"DAMAGE_LEFT") == 0
			|| playerState.compare(L"DAMAGE_RIGHT") == 0 || playerState.compare(L"DAMAGE_FRONT") == 0 )
		{
			IXAudio2SourceVoice *damageSound = soundEffectMap[ENUM_SOUND_EFFECT_DAMAGE];

			XAUDIO2_VOICE_STATE voiceState;
			damageSound->GetState(&voiceState);

			//// [voiceState.BuffersQueued <= 0] means there are nothing in the buffer
			//// so let's make a new buffer to queue the sound
			if (voiceState.BuffersQueued <= 0)
			{
				XAUDIO2_BUFFER *proto = audioBufferPrototypeMap[ENUM_SOUND_EFFECT_DAMAGE];
				bool ssbSuccess = SUCCEEDED(damageSound->SubmitSourceBuffer(proto));
				damageSound->Start();
				//// After all, there will be only one buffer node in the queue always ...
			}
			//// if there is something in the buffer
			else
			{
				/// do nothing
			}
		}
	}
}

void GameAudio::processMusic()
{
	Game *game = Game::getSingleton();
	GameStateManager *gsm = game->getGSM();
	GameState gameState = gsm->getCurrentGameState();

	if (gameState == GS_GAME_IN_PROGRESS)
	{
		/// check if the game is over (player is dying or player shooted enemy w/o safety)
		if (gsm->getLose() == true)
		{
			if (currentMusicPlaying != ENUM_MUSIC_GAMEOVER)
			{
				if (currentMusicPlaying != ENUM_MUSIC_NONE)
					stopMusic(currentMusicPlaying);
				currentMusicPlaying = ENUM_MUSIC_GAMEOVER;
				gameOverMusicBuffered = false;	/// this value will checked in the playMusicOnce() function
			}
			playMusicOnce(ENUM_MUSIC_GAMEOVER);
		}

		//// game is in progress without game over
		else 
		{
			if (currentMusicPlaying != ENUM_MUSIC_INGAME)
			{
				if (currentMusicPlaying != ENUM_MUSIC_NONE)
					stopMusic(currentMusicPlaying);

				currentMusicPlaying = ENUM_MUSIC_INGAME;
			}
			//// not yet have file to play
		}
	}

	else if (gameState == GS_MAIN_MENU || gameState == GS_SPLASH_SCREEN)
	{
		//// if currently playing music is not main theme, 
		//// it means that the game was in-game or other states
		//// need to start the main theme music, and turn the previous music off.
		////	musicMap[currentMusicPlaying];
		if (currentMusicPlaying != ENUM_MUSIC_MAIN_THEME)
		{
			//// IF current music exists, then it means the game
			//// was previously playing other music
			if (currentMusicPlaying != ENUM_MUSIC_NONE)
				stopMusic(currentMusicPlaying);

			currentMusicPlaying = ENUM_MUSIC_MAIN_THEME;
		}
		playMusicRepeat(ENUM_MUSIC_MAIN_THEME);
	}

	else if (gameState == GS_LEVEL_COMPLETE)
	{
		if (currentMusicPlaying != ENUM_MUSIC_LEVEL_COMPLETE)
		{
			if (currentMusicPlaying != ENUM_MUSIC_NONE)
				stopMusic(currentMusicPlaying);
			levelCompleteMusicBuffered = false;		/// this value will checked in the playMusicOnce() function
			currentMusicPlaying = ENUM_MUSIC_LEVEL_COMPLETE;
		}		
		playMusicOnce(ENUM_MUSIC_LEVEL_COMPLETE);
	}

	else if (gameState == GS_GAME_OVER)
	{
		if (currentMusicPlaying != ENUM_MUSIC_GAMEOVER)
		{
			if (currentMusicPlaying != ENUM_MUSIC_NONE)
				stopMusic(currentMusicPlaying);
			currentMusicPlaying = ENUM_MUSIC_GAMEOVER;
		}
		playMusicOnce(ENUM_MUSIC_GAMEOVER);
	}
}

void GameAudio::stopMusic(MusicTypes musicType)
{
	if (musicRegistrationMap[musicType] == true)
	{
		IXAudio2SourceVoice *sourceVoice = musicMap[musicType];
		sourceVoice->Stop();
		sourceVoice->FlushSourceBuffers();
	}
}

void GameAudio::playMusicRepeat(MusicTypes musicType)
{
	if (musicRegistrationMap[musicType] == true)
	{
		IXAudio2SourceVoice *sourceVoice = musicMap[musicType];

		XAUDIO2_VOICE_STATE voiceState;
		sourceVoice->GetState(&voiceState);

		if (voiceState.BuffersQueued <= 0)
		{
			XAUDIO2_BUFFER *proto = musicBufferPrototypeMap[musicType];
			bool ssbSuccess = SUCCEEDED(sourceVoice->SubmitSourceBuffer(proto));
			sourceVoice->Start();
		}
	}
}

void GameAudio::playMusicOnce(MusicTypes musicType)
{
	if (musicRegistrationMap[musicType] == true)
	{
		IXAudio2SourceVoice *sourceVoice = musicMap[musicType];

		XAUDIO2_VOICE_STATE voiceState;
		sourceVoice->GetState(&voiceState);

		if (musicType == ENUM_MUSIC_LEVEL_COMPLETE)
		{
			if (levelCompleteMusicBuffered == false)
			{
				XAUDIO2_BUFFER *proto = musicBufferPrototypeMap[musicType];
				bool ssbSuccess = SUCCEEDED(sourceVoice->SubmitSourceBuffer(proto));
				sourceVoice->Start();
				levelCompleteMusicBuffered = true;
			}
		}

		/// here put the game over music
		else if (musicType == ENUM_MUSIC_GAMEOVER)
		{
			if (gameOverMusicBuffered == false)
			{
				XAUDIO2_BUFFER *proto = musicBufferPrototypeMap[musicType];
				bool ssbSuccess = SUCCEEDED(sourceVoice->SubmitSourceBuffer(proto));
				sourceVoice->Start();
				gameOverMusicBuffered = true;
			}
		}
	}
}

void GameAudio::shutDown()
{
	//// ---- destroy sound effects
	if (soundEffectRegistrationMap[ENUM_SOUND_EFFECT_SHOOT] == true)
	{
		IXAudio2SourceVoice* shootSound = soundEffectMap[ENUM_SOUND_EFFECT_SHOOT];
		shootSound->DestroyVoice();

		XAUDIO2_BUFFER *buffer = audioBufferPrototypeMap[ENUM_SOUND_EFFECT_SHOOT];
		delete[] buffer->pAudioData;
		delete buffer;
	}

	if (soundEffectRegistrationMap[ENUM_SOUND_EFFECT_MONEY] == true)
	{
		IXAudio2SourceVoice* moneySound = soundEffectMap[ENUM_SOUND_EFFECT_MONEY];
		moneySound->DestroyVoice();

		XAUDIO2_BUFFER *buffer = audioBufferPrototypeMap[ENUM_SOUND_EFFECT_MONEY];
		delete[] buffer->pAudioData;
		delete buffer;
	}

	if (soundEffectRegistrationMap[ENUM_SOUND_EFFECT_PUNCH] == true)
	{
		IXAudio2SourceVoice* punchSound = soundEffectMap[ENUM_SOUND_EFFECT_PUNCH];
		punchSound->DestroyVoice();

		XAUDIO2_BUFFER *buffer = audioBufferPrototypeMap[ENUM_SOUND_EFFECT_PUNCH];
		delete[] buffer->pAudioData;
		delete buffer;
	}

	if (soundEffectRegistrationMap[ENUM_SOUND_EFFECT_HEAL] == true)
	{
		IXAudio2SourceVoice* healSound = soundEffectMap[ENUM_SOUND_EFFECT_HEAL];
		healSound->DestroyVoice();

		XAUDIO2_BUFFER *buffer = audioBufferPrototypeMap[ENUM_SOUND_EFFECT_HEAL];
		delete[] buffer->pAudioData;
		delete buffer;
	}

	if (soundEffectRegistrationMap[ENUM_SOUND_EFFECT_DAMAGE] == true)
	{
		IXAudio2SourceVoice* damageSound = soundEffectMap[ENUM_SOUND_EFFECT_DAMAGE];
		damageSound->DestroyVoice();

		XAUDIO2_BUFFER *buffer = audioBufferPrototypeMap[ENUM_SOUND_EFFECT_DAMAGE];
		delete[] buffer->pAudioData;
		delete buffer;
	}
	//// --- destory sound effects complete

	//// --- destroy musics
	if (musicRegistrationMap[ENUM_MUSIC_MAIN_THEME] == true)
	{
		IXAudio2SourceVoice* mainTheme = musicMap[ENUM_MUSIC_MAIN_THEME];
		mainTheme->DestroyVoice();

		XAUDIO2_BUFFER *buffer = musicBufferPrototypeMap[ENUM_MUSIC_MAIN_THEME];
		delete[] buffer->pAudioData;
		delete buffer;
	}

	if (musicRegistrationMap[ENUM_MUSIC_LEVEL_COMPLETE] == true)
	{
		IXAudio2SourceVoice* completeMusic = musicMap[ENUM_MUSIC_LEVEL_COMPLETE];
		completeMusic->DestroyVoice();

		XAUDIO2_BUFFER *buffer = musicBufferPrototypeMap[ENUM_MUSIC_LEVEL_COMPLETE];
		delete[] buffer->pAudioData;
		delete buffer;
	}

	if (musicRegistrationMap[ENUM_MUSIC_INGAME] == true)
	{
		IXAudio2SourceVoice* ingameMusic = musicMap[ENUM_MUSIC_INGAME];
		ingameMusic->DestroyVoice();

		XAUDIO2_BUFFER *buffer = musicBufferPrototypeMap[ENUM_MUSIC_INGAME];
		delete[] buffer->pAudioData;
		delete buffer;
	}

	if (musicRegistrationMap[ENUM_MUSIC_GAMEOVER] == true)
	{
		IXAudio2SourceVoice* gameoverMusic = musicMap[ENUM_MUSIC_GAMEOVER];
		gameoverMusic->DestroyVoice();

		XAUDIO2_BUFFER *buffer = musicBufferPrototypeMap[ENUM_MUSIC_GAMEOVER];
		delete[] buffer->pAudioData;
		delete buffer;
	}
	//// --- destroy musics complete


	//// --- put other effects' destructors here ------

	masterVoice->DestroyVoice();
	xAudio2Engine->Release();
	CoUninitialize();
}
