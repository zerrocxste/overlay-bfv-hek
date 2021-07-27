#include "../includes.h"

#ifdef RENDER_EXTERN_WINDOW
std::unique_ptr<c_overlay_render> g_up_overlay_render;

LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);

void RenderCallback();

void BeginSceneCallback();

void ResetCallback();

void WndProcCallback(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

BOOL c_overlay_render::initialize(HWND hwnd)
{
	this->hWnd = hwnd;

	if (this->need_resolution_reset())
	{
		if (DXWFInitialization(memory_utils::get_base()) == FALSE)
			return FALSE;

		DXWFWndProcCallbacks(DXWF_WNDPROC_WNDPROCHANDLER_, WndProcCallback);

		DXWFRendererCallbacks(DXWF_RENDERER_RESET_, ResetCallback);
		DXWFRendererCallbacks(DXWF_RENDERER_LOOP_, RenderCallback);
		DXWFRendererCallbacks(DXWF_RENDERER_BEGIN_SCENE_LOOP_, BeginSceneCallback);

		return DXWFCreateWindow("AMD Overlay",
			this->game_window_pos_x, this->game_window_pos_y,
			this->game_window_width, this->game_window_height,
			WS_POPUP,
			WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_COMPOSITED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE,
			user_dxwf_flags::ENABLE_WINDOW_ALPHA,
			my_color(0, 0, 0, 1),
			NULL);
	}

	return FALSE;
}

void c_overlay_render::render_loop()
{
	DXWFSetFramerateLimit(global_vars::overlay_max_fps);
	DXWFRenderLoop();
}

void c_overlay_render::terminate()
{
	DXWFTerminate();
}

BOOL c_overlay_render::need_resolution_reset()
{
	RECT rect;
	if (GetWindowRect(this->hWnd, &rect))
	{
		if (GetWindowLong(this->hWnd, GWL_STYLE) & WS_BORDER)
		{
			constexpr auto shadow_frame_offset = 8;

			TITLEBARINFO titlebar{};
			titlebar.cbSize = sizeof(TITLEBARINFO);
			if (GetTitleBarInfo(this->hWnd, &titlebar))
			{
				rect.top += titlebar.rcTitleBar.bottom - titlebar.rcTitleBar.top + shadow_frame_offset;
				rect.bottom -= shadow_frame_offset;
				rect.left += shadow_frame_offset;
				rect.right -= shadow_frame_offset;
			}
		}

		int pos_x = rect.left;
		int pos_y = rect.top;
		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;

		if (pos_x != this->game_window_pos_x
			|| pos_y != this->game_window_pos_y
			|| width != this->game_window_width
			|| height != this->game_window_height)
		{
			this->game_window_pos_x = pos_x;
			this->game_window_pos_y = pos_y;
			this->game_window_width = width;
			this->game_window_height = height;
			return TRUE;
		}
	}
	return FALSE;
}

void c_overlay_render::reset_window()
{
	DXWFSetWindowPos(this->game_window_pos_x, this->game_window_pos_y);
	DXWFSetWindowSize(this->game_window_width, this->game_window_height);
}

void c_overlay_render::is_target_window_focused()
{
	auto foreground_window = GetForegroundWindow();
	auto is_visible = (BOOL)(this->hWnd == foreground_window || DXWFGetHWND() == foreground_window);
	if (this->visible_state != is_visible)
	{
		this->visible_state = is_visible;
		DXWFSetWindowVisibleState(this->visible_state);
	}
}

void RenderCallback()
{
	g_up_overlay_render->is_target_window_focused();
	if (g_up_overlay_render->need_resolution_reset())
		g_up_overlay_render->reset_window();
	render::begin_scene();
}

void BeginSceneCallback()
{
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

void ResetCallback()
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
}

void WndProcCallback(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	ImGui_ImplWin32_WndProcHandler(hwnd, message, wParam, lParam);
}
#endif

