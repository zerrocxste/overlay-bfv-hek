namespace render
{
	extern void shutdown_imgui();
	extern void begin_scene();
	extern void reset();

	static ImFont* font_Main;
	static ImFont* font_Credits;

	namespace menu
	{
		extern void run();
	}
}