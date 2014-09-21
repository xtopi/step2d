#pragma once
#include <ClanLib/core.h>
#include <ClanLib/application.h>
#include <ClanLib/display.h>
#include <ClanLib/physics2d.h>
#include <Box2D/Box2D.h>
#include <memory>

#include <cmath>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include "defs.h"

using namespace clan;
using namespace std;

static ostream& Dlog = clog;

inline void RunAssertEx(bool cond, const char* assertion, const string& msg) {
    if (!cond)
        throw runtime_error(string() + "assertion failed: " + assertion + " -- " + msg);
}
#define Assert(cond, msg) RunAssertEx(cond, #cond, msg)
