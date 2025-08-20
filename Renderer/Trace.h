#pragma once
#include <stack>
#include <unordered_map>
#include <string>
#include <chrono>

class Trace {
	typedef std::chrono::steady_clock clock;
	struct TimeStamp {
		TimeStamp() = default;
		TimeStamp(const clock::time_point& tp, const char* msg) :timepoint(tp), msg(msg) {}
		clock::time_point timepoint;
		const char* msg;
	};
public:
	static void Begin(const char* msg);
	static void End();
	static void Gui();
private:
	std::stack<TimeStamp> times;
	std::unordered_map<std::string, std::chrono::nanoseconds> records;
};
