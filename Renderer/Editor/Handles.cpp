#include <SDL3/SDL.h>
#include <iostream>
#include "Handles.h"
#include "../Render/Gizmos.h"
#include "../Geometry.h"
#include "../Render/Render.h"
#define MOUSE_LEFT 0
#define MOUSE_MIDDLE 1
#define MOUSE_RIGHT 2

static bool mouseButton[3] = { 0,0,0 }, mouseButtonDownCaptured[3] = { 0,0,0 };
//transient data. Kept only in one frame
static bool mouseButtonUp[3] = { 0,0,0 };
static int mouseX, mouseY;
static Ray mouseRay;
static const constexpr float epsilon = .01f;
//dragging
struct DragData {
	glm::vec3 origin, direction;
	float offset;
};
static const void* draggingObj = nullptr;
static DragData dragStartInfo;

void Handles::Axis(glm::vec3& pos) {
	glm::vec3 right = Transform::right + pos;
	glm::vec3 forward = Transform::forward + pos;
	glm::vec3 up = Transform::up + pos;
	//which direction did the user click
	const constexpr int r = 1, f = 2, u = 3;
	int click = 0;
	if (mouseRay.IntersectLineSegment(pos, right, epsilon))
		click = r;
	else if (mouseRay.IntersectLineSegment(pos, forward, epsilon))
		click = f;
	else if (mouseRay.IntersectLineSegment(pos, up, epsilon))
		click = u;
	Gizmos::AddLine(pos, right, click==r?Color::Yellow:Color::Red);
	Gizmos::AddLine(pos, forward, click==f?Color::Yellow:Color::Blue);
	Gizmos::AddLine(pos, up, click==u?Color::Yellow:Color::Green);
	const glm::vec3* direction = nullptr;
	switch (click) {
	case r:
		direction = &Transform::right;
		break;
	case f:
		direction = &Transform::forward;
		break;
	case u:
		direction = &Transform::up;
		break;
	}
	if (!mouseButtonDownCaptured[MOUSE_LEFT]) {
		mouseButtonDownCaptured[MOUSE_LEFT] = true;
		if (click) {
			draggingObj = &pos;
			dragStartInfo.origin = pos;
			dragStartInfo.direction = *direction;
			dragStartInfo.offset = mouseRay.IntersectAtLineSegment(pos, right); //record the offset
		}
	}
	else if (mouseButtonUp[MOUSE_LEFT]) {
		if (draggingObj == &pos) {
			draggingObj = nullptr;
		}
	}
	else if(draggingObj==&pos) {
		const constexpr glm::vec3 origin = { 0,0,0 };
		glm::vec3 unitVector = { clickRight, clickForward, clickUp };
		pos = dragStartInfo.v + mouseRay.IntersectPointAtLineSegment(origin, unitVector);
		pos = dragStartInfo.origin+unitVector
	}
}

void Handles::HandleEvent(SDL_Event& event) {
	switch (event.type) {
	case SDL_EVENT_MOUSE_BUTTON_DOWN:
		switch (event.button.button) {
		case SDL_BUTTON_LEFT:
			mouseButton[MOUSE_LEFT] = true;
			mouseButtonDownCaptured[MOUSE_LEFT] = false;
			break;
		case SDL_BUTTON_MIDDLE:
			mouseButton[MOUSE_MIDDLE] = true;
			mouseButtonDownCaptured[MOUSE_MIDDLE] = false;
			break;
		case SDL_BUTTON_RIGHT:
			mouseButton[MOUSE_RIGHT] = true;
			mouseButtonDownCaptured[MOUSE_RIGHT] = false;
			break;
		}
		break;
	case SDL_EVENT_MOUSE_BUTTON_UP:
		switch (event.button.button) {
		case SDL_BUTTON_LEFT:
			mouseButton[MOUSE_LEFT] = false;
			mouseButtonDownCaptured[MOUSE_LEFT] = true;
			mouseButtonUp[MOUSE_LEFT] = true;
			break;
		case SDL_BUTTON_MIDDLE:
			mouseButton[MOUSE_MIDDLE] = false;
			mouseButtonDownCaptured[MOUSE_MIDDLE] = true;
			mouseButtonUp[MOUSE_MIDDLE] = true;
			break;
		case SDL_BUTTON_RIGHT:
			mouseButton[MOUSE_RIGHT] = false;
			mouseButtonDownCaptured[MOUSE_RIGHT] = true;
			mouseButtonUp[MOUSE_RIGHT] = true;
			break;
		}
		break;
	case SDL_EVENT_MOUSE_MOTION:
		mouseX = event.motion.x;
		mouseY = event.motion.y;
		mouseRay = Renderer::Get().camera.Screen2WorldRay(mouseX, mouseY);
		break;
	}
}
void Handles::EndFrame() {
	memset(mouseButtonUp, 0, sizeof(mouseButtonUp));
}
