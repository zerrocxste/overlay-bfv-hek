#define _CRT_SECURE_NO_WARNINGS
#define RENDER_EXTERN_WINDOW
#define ENABLE_DEBUG_CONSOLE

#include <Windows.h>
#include <iostream>
#include <time.h>
#include <direct.h>
#include <vector>
#include <map>
#include <deque>
#include <mutex>
#include <algorithm>
#include <intrin.h>
#include <fstream>

#ifdef RENDER_EXTERN_WINDOW
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

#include <d3d9.h>
#pragma comment (lib, "d3d9.lib")

#include <dwmapi.h>
#pragma comment (lib, "dwmapi.lib")

#include "../libs/DXWF/DXWF.h"
#pragma comment (lib, "DXWF.lib")

#include <d3d11.h>
#pragma comment(lib, "d3d11")

#include <dxgi1_2.h>
#pragma comment (lib, "dxgi.lib")
#else

#endif // RENDER_EXTERN_WINDOW

#include "../libs/minhook/minhook.h"
#pragma comment (lib, "minhook.lib")

#include "memory_utils/memory_utils.h"

#include "../libs/imgui/imgui.h"
#include "../libs/imgui/imgui_internal.h"
#include "../libs/imgui/imgui_impl_win32.h"
#ifdef RENDER_EXTERN_WINDOW
#include "../libs/imgui/imgui_impl_dx9.h"
#else
#include "../libs/imgui/imgui_impl_dx11.h"
#endif

#include "global_vars/global_vars.h"
#include "vars/vars.h"

#include "draw_list/draw_list.h"

#include "math/martix4x4/matrix4x4.h"
#include "math/vector/vector.h"

#include "notification/notification.h"

#include "render/render.h"
#ifdef RENDER_EXTERN_WINDOW
#include "overlay_renderer/overlay_renderer.h"
#endif // RENDER_EXTERN_WINDOW

#include "hook_manager/hook_manager.h"

#include "features/features.h"

class _int128 {
public:
	long dw1;
	long dw2;
	long dw3;
	long dw4;
};