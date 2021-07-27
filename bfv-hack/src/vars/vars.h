namespace vars
{
	namespace global
	{
		extern bool enable;
	}
	namespace visuals
	{
		extern int player_type;
		extern int box_type;
		extern bool name;
		extern bool health;
		extern bool distance;
		extern bool radar_3d;
		extern float col_enemy_vis[3];
		extern float col_enemy_invis[3];
		extern float col_teammate_vis[3];
		extern float col_teammate_invis[3];
	}
	namespace misc
	{
		extern bool fire_rate_patched;
		extern bool recoil_patched;
		extern bool spread_patched;
	}
	namespace font
	{
		extern int style;
		extern float size;
	}
	extern void load_default_settings();
}
