#pragma once

struct Logger
{
	enum Level
	{
		L_INFO,
		L_WARN,
		L_ERROR
	};

	explicit Logger(cstring filename);
	~Logger();
	void Log(Level level, cstring text);
	void Flush();

	static Logger* global;

private:
	std::ofstream out;
	string path;
};

inline void Info(cstring msg)
{
	Logger::global->Log(Logger::L_INFO, msg);
}
template<typename... Args>
inline void Info(cstring msg, const Args&... args)
{
	Logger::global->Log(Logger::L_INFO, Format(msg, args...));
}

inline void Warn(cstring msg)
{
	Logger::global->Log(Logger::L_WARN, msg);
}
template<typename... Args>
inline void Warn(cstring msg, const Args&... args)
{
	Logger::global->Log(Logger::L_WARN, Format(msg, args...));
}

inline void Error(cstring msg)
{
	Logger::global->Log(Logger::L_ERROR, msg);
}
template<typename... Args>
inline void Error(cstring msg, const Args&... args)
{
	Logger::global->Log(Logger::L_ERROR, Format(msg, args...));
}
