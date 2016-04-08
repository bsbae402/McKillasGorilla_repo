// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

// Windows Header Files:
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#define _USE_MATH_DEFINES
#include <math.h>

// C++ RunTime Header Files
#include <thread>
using namespace std::this_thread;
#include <chrono>		// FOR TIMING
using namespace std::chrono;
#include <ctime>
#include <cstdlib>		// FOR RANDOM NUMBER GENERATION
#include <algorithm>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <stdio.h>
#include <iostream>
#include <string>
using std::wstring;
#include <sstream>
using namespace std;
#include <iomanip>
#include <fstream>
using namespace std;
#include <process.h>
#include <random>

// DirectX
#include <d3d9.h>
#include <d3dx9.h>

//// Box2D =>=>=> hope this will solve the problem 
#include "Box2D\Box2D.h"
