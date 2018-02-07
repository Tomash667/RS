#include "Pch.h"
#include <ctime>


static cstring level_names[] = {
	"INFO ",
	"WARN ",
	"ERROR"
};

Logger* Logger::global;


Logger::Logger(cstring filename)
{
	assert(filename);
	out.open(filename);
	Log(L_INFO, "Log start");
}

Logger::~Logger()
{
	Log(L_INFO, "Log end");
	Flush();
}

void Logger::Log(Level level, cstring msg)
{
	time_t t = time(0);
	tm tm;
	localtime_s(&tm, &t);
	out << Format("%02d:%02d:%02d %s - %s\n", tm.tm_hour, tm.tm_min, tm.tm_sec, level_names[level], msg);
}

void Logger::Flush()
{
	out.flush();
}
