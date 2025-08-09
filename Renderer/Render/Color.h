#pragma once
#include <glm/glm.hpp>

enum class Color:unsigned int {
    White      = 0xFFFFFF00,
	Black      = 0x000000FF,
	Red        = 0xFF0000FF,
	Green      = 0x00FF00FF,
	Blue       = 0x0000FF00,
	Yellow     = 0xFFFF00FF,
	Cyan       = 0x00FFFF00,
	Magenta    = 0xFF00FF00,
	Gray       = 0x808080FF,
	DarkGray   = 0x404040FF,
	LightGray  = 0xD3D3D3FF,
	Orange     = 0xFF7F00FF,
	Purple     = 0x800080FF,
	Pink       = 0xFFC0CBFF,
	Brown      = 0xA52A2AFF,
	Olive      = 0x808000FF,
	Teal       = 0x008080FF,
	Navy       = 0x000080FF,
	Indigo     = 0x4B0082FF,
	Gold       = 0xFFD700FF,
	Silver     = 0xC0C0C0FF,
	Coral      = 0xFF7F50FF,
	Lime       = 0x00FF00FF,
	Mint       = 0x98FF98FF,
	Lavender   = 0xE6E6FAFF,
	Peach      = 0xFFD5B0FF,
	Khaki      = 0xF0E68CFF,
	SlateBlue  = 0x6A5ACDFF,
	MediumVioletRed = 0xC71585FF,
	RoyalBlue  = 0x4169E1FF,
	Chartreuse = 0x7FFF00FF,
	SpringGreen = 0x00FF7FFF,
	Crimson    = 0xDC143CFF,
	Aquamarine = 0x7FFFD4FF,
	MintCream  = 0xF5FFFAFF,
	MediumSeaGreen = 0x3CB371FF,
	Wheat      = 0xF5DEB3FF,
	Plum       = 0xDDA0DDFF,
	DarkOliveGreen = 0x556B2FFF,
	Snow       = 0xFAFAFFFF,
	LemonChiffon = 0xFAFAD2FF,
	SkyBlue    = 0x87CEEBFF,
	LavenderBlush = 0xFFF0F5FF,
	AliceBlue  = 0xF0F8FFFF
};

static glm::vec3 Color32ToFloat(const Color color) {
	uint32_t red   = (uint32_t(color) >> 24) & 0xFF;  // 提取红色通道
    uint32_t green = (uint32_t(color) >> 16) & 0xFF;  // 提取绿色通道
    uint32_t blue  = (uint32_t(color) >> 8)  & 0xFF;  // 提取蓝色通道
    // 转换为 float 范围 [0.0, 1.0]
	return glm::vec3(red / 255.f, green / 255.f, blue / 255.f);
}
