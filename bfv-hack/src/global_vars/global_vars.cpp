#include "../includes.h"

namespace global_vars
{
	HMODULE module;
	HWND hwnd_game;
	bool unload;
	bool menu_open;
	int overlay_max_fps = 25;
	int screenshot_counter;
	bool stop_render;
}