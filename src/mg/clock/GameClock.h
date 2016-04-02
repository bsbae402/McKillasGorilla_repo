/*
	Author: Richard McKenna
	Stony Brook University
	Computer Science Department

	GameClock.h

	This class provides a platform-independent implementation
	for all game timing. You give it a target frame rate, and 
	then once per frame call timeGameLoop. This will clamp the 
	frame rate to your target loop. If the loop is running slower
	than the target rate, you'll have to either settle
	for a slower game, which means you should improve
	performance in other parts of the game, or you should
	scale all game movements proportionally to the
	slowdown.

	This timer works by calculating the total time a given
	frame has taken and comparing it to the target framerate.
	It will then sleep for the difference if the measured
	framerate is faster than the target.

	Note that it uses a platform independent high resolutions
	C++ timer.
*/

#pragma once

#include "mg_VS\stdafx.h"
#include "mg\game\Game.h"
#include "mg\game\GameTypes.h"
#include "mg\text\TextFileWriter.h"

class GameClock
{
private:
	const mg_long NANO_RES = 1000000000;
	const mg_long MICRO_RES = 1000000;
	const mg_long MILLI_RES = 1000;

	// TARGET FRAMES PER SECOND
	mg_long targetFPS;

	// THE MINIMUM TIMER INCREMENT
	mg_long timerResolution;

	// THE INVERSE OF THE TARGET FRAME RATE IN WHATEVER RESOLUTION THE TIMER ALLOWS
	mg_long targetTimePerFrame;

	// THE TIME TAKEN AT THE START OF THE FRAME,
	// INCLUDING AND NOT INCLUDING SLEEP TIME
	steady_clock::time_point gameLoopStartTime;
	steady_clock::time_point sleeplessGameLoopStartTime;

	// THE TIME TAKEN AT THE END OF THE FRAME
	steady_clock::time_point gameLoopEndTime;

	// THE TOTAL TIME A SINGLE FRAME TOOK TO ITERATE,
	// INCLUDING AND NOT INCLUDING SLEEP TIME
	mg_long loopTime;
	mg_long sleeplessLoopTime;

	// THE TOTAL TIME TAKEN SINCE ENTERING THE GAME LOOP,
	// INCLUDING AND NOT INCLUDING SLEEP TIME
	mg_long totalTime;
	mg_long sleeplessTotalTime;

	// THE FRAME RATE OF THE LATEST FRAME,
	// INCLUDING AND NOT INCLUDING SLEEP TIME
	mg_long actualLoopRate;
	mg_long sleeplessLoopRate;

	// THE AMOUNT WE WILL SET TO SLEEP EACH FRAME
	// IF THE GAME IS RUNNING FASTER THAN THE TARGET FPS
	mg_long sleepTime;

	// THE TOTAL AMOUNT OF TIME WE HAVE MADE OUR APPLICATION
	// SLEEP SINCE THE TIMER WAS RESET
	mg_long totalSleepTime;

	// WHEN A LOOP RUNS TOO FAST, WE CLAMP OUR FRAME RATE,
	// SO THIS FACTOR IS SIMPLY 1, BUT WHEN OUR LOOP RUNS
	// TO SLOW, THIS INCREASES PROPORTIONATELY SUCH THAT
	// WE CAN SCALE ALL MOVEMENTS OF SPRITES & SCROLLING
	mg_double deltaTime;

	// NUMBER OF ITERATIONS, USED FOR CALCULATIONS
	mg_long loopCounter;

	// FOR WRITING OUR TIMER STATS TO A TEXT FILE
	TextFileWriter *writer;

public:

	GameClock();
	~GameClock();

	// INLINED GET METHODS
	mg_long		getActualFPS() {	return actualLoopRate;	}
	mg_long		getSleeplessFPS() { return sleeplessLoopRate; }
	mg_long		getTargetFPS() { return targetFPS; }
	mg_long		getLoopCounter() { return loopCounter; }
	mg_long		getSleeplessTotalTime() { return sleeplessTotalTime; }
	mg_double	getDeltaTime() { return deltaTime; }
	mg_long		getTotalTime() { return totalTime; }
	void		setTargetFPS(mg_long initTargetFPS)
	{
		targetFPS = initTargetFPS;
		targetTimePerFrame = timerResolution / targetFPS;
	}

	// METHODS DEFINED IN GameClock.cpp
	// DEFINED IN GameClock.cpp
	mg_long calculateAverageSleeplessFrameRate();
	mg_long calculateAverageActualFrameRate();
	mg_long calculateAverageSleepTime();
	void	startUp();
	void	shutDown();
	void	resetTimer();
	void	timeGameLoop();
};