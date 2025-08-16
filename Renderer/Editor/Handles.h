#pragma once
#include <glm/glm.hpp>

union SDL_Event;

class Handles {
public:
	static inline Handles& Get();
	static void Axis(glm::vec3& pos);
	static void HandleEvent(SDL_Event& event);
	static void EndFrame();
private:
};
