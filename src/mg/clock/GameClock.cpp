/*
	Author: Richard McKenna
	Stony Brook University
	Computer Science Department

	GameClock.cpp

	See GameClock.h for a class description.
*/

#include "mg_VS\stdafx.h"
#include "mg\game\Game.h"
#include "mg\game\GameTypes.h"
#include "mg\clock\GameClock.h"
#include "mg\resources\EngineConfigProperties.h"
#include "mg\resources\GameResources.h"
#include "mg\resources\importers\MGEngineConfigImporter.h"
#include "mg\text\TextFileWriter.h"

/*
	GameClock - Default constructor, this method
	initializes the timer resolution to the best the
	system has to offer. In order to start timing,
	call the resetTimer method outside the main game
	loop, right before it starts. The timeGameLoop
	method is the only one needed inside the game
	loop.
*/
GameClock::GameClock()
{

}

/*
	~GameClock - Destructor, we only have to clean up
	the writer, since we declared it as a pointer instance
	variable.
*/
GameClock::~GameClock() {}

void GameClock::startUp()
{
	// GET THE GAME SINGLETON
	Game *game = Game::getSingleton();
	GameResources *resources = game->getResources();
	MGEngineConfigImporter *importer = (MGEngineConfigImporter*)resources->getEngineConfigImporter();
	void* properties = importer->getPropertyTypes();
	EngineConfigProperties props;

	// LOAD THE TARGET FRAME RATE
	wstring fpsProp = importer->getProperty(props.PROP_NAME_TARGET_FPS);
	wstringstream(fpsProp) >> targetFPS;

	// WE SHOULD ASK THE TIMER ITS RESOLUTION
	timerResolution = std::chrono::steady_clock::period::den;

	// CALCULATE THE NUMBER OF MILLISECONDS WE
	// WANT PER FRAME ACCORDING TO THE TARGET_FPS
	// AND THE TIMER RESOLUTION
	targetTimePerFrame = (timerResolution / targetFPS);
}

void GameClock::shutDown()
{

}

/*
	calculateAverageActualFramerate - This method
	calculates how long on average a frame takes in
	the game loop, including time spent sleeping. Note
	that this calculation is done using the resolution
	of the system.
*/
mg_long GameClock::calculateAverageActualFrameRate()
{
	if (loopCounter == 0)
		return 0;
	else
	{
		mg_int averageTime = totalTime / loopCounter;
		if (averageTime != 0)
			return (timerResolution / averageTime);
		else
			// IT IS AT LEAST THE TIMER RESOLUTION, BUT WE REALLY
			// DON'T KNOW DUE TO TIMER RESOLUTION
			return timerResolution;
	}
}

/*
	calculateAverageSleeplessFramerate - This method
	calculates how long on average a frame takes in
	the game loop, not including time spent sleeping.
*/
mg_long GameClock::calculateAverageSleeplessFrameRate()
{
	if (loopCounter == 0)
		return 0;
	else
	{
		mg_double averageTime = ((mg_double)sleeplessTotalTime) / (mg_double)loopCounter;
		if ((int)averageTime != 0)
			return (timerResolution / averageTime);
		else
			// IT IS AT LEAST THE TIMER RESOLUTION, BUT WE REALLY
			// DON'T KNOW DUE TO TIMER RESOLUTION
			return timerResolution;
	}
}

/*
	calculateAverageSleepTime - This method calculates
	on average how much time is spent sleeping each
	frame since the game loop started.
*/
mg_long GameClock::calculateAverageSleepTime()
{
	if (loopCounter == 0)
		return 0;
	else
		return totalSleepTime / loopCounter;
}

/*
	resetTimer - This method resents all variables used
	for compiling statistics as well as getting the first
	start times for the first iteration through the
	game loop. This should be placed outside the game
	loop, right before it is to start.
*/
void GameClock::resetTimer()
{
	gameLoopStartTime = std::chrono::steady_clock::now();
	sleeplessGameLoopStartTime = std::chrono::steady_clock::now();
	loopCounter = 0;
	totalTime = 0;
	sleeplessTotalTime = 0;
	totalSleepTime = 0;
	actualLoopRate = 0;
	sleeplessLoopRate = 0;
}

/*
timeGameLoop - This method gets the current time
and then calculates the difference between now
and the last time it got the time. This is the
game time, which can then be used to calculate
the current frame rate. If the current frame rate
is too fast, we sleep for a little to clamp it
to the TARGET_FPS. If it is too slow, we calculate
a scaling factor that can be used for moving
sprites and for scrolling.
*/
void GameClock::timeGameLoop()
{
	// GET THE END OF FRAME TIME
	gameLoopEndTime = steady_clock::now();

	// HOW MUCH TIME PASSED DURING THE LAST FRAME?
	loopTime = (duration_cast<duration<double>>(gameLoopEndTime - gameLoopStartTime)).count();
	
	// GET THE START TIME FOR NEXT FRAME, IF THERE IS ONE
	gameLoopStartTime = steady_clock::now();
	
	// ADD THE LAST FRAME'S TIME TO THE TOTAL
	totalTime += loopTime;

	// HOW MUCH TIME PASSED NOT INCLUDING
	// OUR FORCED SLEEPING?
	sleeplessLoopTime = (gameLoopEndTime - sleeplessGameLoopStartTime).count();

	// ADD THE LAST FRAME'S SLEEPLESS TIME TO THE TOTAL
	sleeplessTotalTime += sleeplessLoopTime;

	if (loopTime == 0)
		actualLoopRate = timerResolution;
	else
		actualLoopRate = timerResolution / loopTime;

	if (sleeplessLoopTime == 0)
		actualLoopRate = timerResolution;
	else
		sleeplessLoopRate = timerResolution / sleeplessLoopTime;

	// IF THIS PAST FRAME RAN TOO FAST IT'S
	// LIKELY THE NEXT FRAME WILL RUN FAST ALSO
	if (targetTimePerFrame > sleeplessLoopTime)
	{
		// SO LET'S CLAMP IT TO OUR TARGET FRAME RATE
		sleepTime = targetTimePerFrame - sleeplessLoopTime;
		totalSleepTime += sleepTime;

		std::chrono::nanoseconds sleepDuration(sleepTime);
		sleep_for(sleepDuration);
		deltaTime = 1;
	}
	else
	{
		sleepTime = 0;

		// WE MIGHT USE THIS timeScaleFactor TO SCALE
		// MOVEMENTS OF GAME SPRITES AND SCROLLING TO
		// MAKE UP FOR THE SLOWING DOWN OF THE GAME LOOP
		deltaTime = ((mg_double)targetFPS) /
			((mg_double)sleeplessLoopRate);
	}

	// GET THE START TIME FOR THE LOOP
	// NOT INCLUDING THE SLEEP TIME FROM THE LAST LOOP
	sleeplessGameLoopStartTime = steady_clock::now();
	loopCounter++;
}