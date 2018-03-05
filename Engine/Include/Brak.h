// Brak engine
#pragma once

#ifdef _WIN64
#	define X64
#endif
#ifndef _DEBUG
#	define NDEBUG
#	define _SECURE_SCL 0
#	define _HAS_ITERATOR_DEBUGGING 0
#endif

#ifdef _DEBUG
#	include <vld.h>
#endif

#include <algorithm>
#include <vector>
#include <unordered_map>
#include <string>
#include <set>
#include <fstream>
#include <DirectXMath.h>
#include "FastFunc.h"

using std::vector;
using std::unordered_map;
using std::string;
using std::min;
using std::max;
template<typename T>
using delegate = ssvu::FastFunc<T>;

#include "Core.h"
#include "CoreMath.h"
#include "Color.h"
#include "Containers.h"
#include "Text.h"
#include "File.h"
#include "Logger.h"
#include "Forward.h"
