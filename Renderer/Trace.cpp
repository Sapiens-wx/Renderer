#include "Trace.h"
#include <chrono>
#include <imgui.h>

namespace im = ImGui;
static Trace inst;

inline long long AsMicroSec(const std::chrono::nanoseconds& time) {
	return std::chrono::duration_cast<std::chrono::microseconds>(time).count();
}

void Trace::Begin(const char* msg) {
	inst.times.emplace(clock::now(), msg);
}
void Trace::End() {
	inst.records[inst.times.top().msg] = clock::now()-inst.times.top().timepoint;
	inst.times.pop();
}
void Trace::Gui() {
	if (im::Begin("Trace")) {
		for (auto pair : inst.records) {
			im::Text("%s: %dus", pair.first.c_str(), AsMicroSec(pair.second));
		}
	}
	im::End();
}