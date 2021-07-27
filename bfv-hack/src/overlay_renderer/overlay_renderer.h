#ifdef RENDER_EXTERN_WINDOW
class c_overlay_render
{
public:
	c_overlay_render() : game_window_pos_x(0), game_window_pos_y(0), game_window_width(0), game_window_height(0), hWnd(0), visible_state(0) {};
	BOOL initialize(HWND hwnd);
	void render_loop();
	void terminate();
	BOOL need_resolution_reset();
	void reset_window();
	void is_target_window_focused();
	struct window_parametric
	{
		window_parametric(int x, int y, int sz_x, int sz_y) : x(x), y(y), width(sz_x), height(sz_y) {};
		int x, y, width, height;
	};
	window_parametric get_window_param()
	{
		return window_parametric(this->game_window_pos_x, this->game_window_pos_y, this->game_window_width, this->game_window_height);
	}
private:
	int game_window_pos_x, game_window_pos_y;
	int game_window_width, game_window_height;
	HWND hWnd;
	BOOL visible_state;
};
extern std::unique_ptr<c_overlay_render> g_up_overlay_render;
#endif
