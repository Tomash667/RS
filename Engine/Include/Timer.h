#pragma once

class Timer
{
public:
	explicit Timer(bool start = true);

	void Start();
	float Tick();
	void Reset();

	int64 GetTime() const { return last_time; }
	double GetTicksPerSec() const { return ticks_per_sec; }
	bool IsStarted() const { return started; }

private:
	double ticks_per_sec;
	int64 last_time;
	float old_time;
	bool use_hpc;
	bool started;
};
