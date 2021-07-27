#include "../includes.h"

void data_calc_thread();

void features::initialize_features() {
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)data_calc_thread, NULL, NULL, NULL);
}

void features::notification()
{
	g_up_notification_panel->render();
}

enum LINE_STATUS_BAR
{
	LEFT,
	RIGHT,
	UPPER,
	BOTTOM
};

struct drawing_data
{
public:
	drawing_data() : m_x(0.f), m_y(0.f), m_w(0.f), m_h(0.f), m_health(0.f), m_max_health(0.f), m_team(0) { };

	drawing_data(float x, float y, float w, float h, float health, float max_health, int team) 
		: m_x(x), m_y(y), m_w(w), m_h(h), m_health(health), m_max_health(max_health), m_team(team) {};

	float m_x, m_y, m_w, m_h;
	float m_health, m_max_health;
	int m_team;
};

std::mutex mtx_collect_data, mtx_drawing;
std::deque<DWORD_PTR>dq_cse_list;
DWORD_PTR p_local_player;
int my_team;
std::deque<drawing_data>dq_draw_data_temp, dq_drawing_data_present;

Matrix4x4 get_matrix()
{
	return memory_utils::read_value<Matrix4x4>({ memory_utils::get_base_address(), 0x47F7880, 0x890, 0x220, 0xF0 });
}

bool world_to_screen(Matrix4x4 matrix, const Vector& vIn, float* flOut)
{
	float w = matrix.m[0][3] * vIn.x + matrix.m[1][3] * vIn.y + matrix.m[2][3] * vIn.z + matrix.m[3][3];

	if (w < 0.5f/*0.01*/)
		return false;

	flOut[0] = matrix.m[0][0] * vIn.x + matrix.m[1][0] * vIn.y + matrix.m[2][0] * vIn.z + matrix.m[3][0];
	flOut[1] = matrix.m[0][1] * vIn.x + matrix.m[1][1] * vIn.y + matrix.m[2][1] * vIn.z + matrix.m[3][1];

	float invw = 1.0f / w;

	flOut[0] *= invw;
	flOut[1] *= invw;

	auto window_param = g_up_overlay_render->get_window_param();

	int width = window_param.width;
	int height = window_param.height;

	float x = (float)width / 2;
	float y = (float)height / 2;

	x += 0.5 * flOut[0] * (float)width + 0.5;
	y -= 0.5 * flOut[1] * (float)height + 0.5;

	flOut[0] = x;
	flOut[1] = y;

	return true;
}

void features::cse_list_grabber(DWORD_PTR cse)
{
	auto duplicate_search_result = std::find(dq_cse_list.begin(), dq_cse_list.end(), cse) != dq_cse_list.end();

	if (duplicate_search_result == false)
	{
		mtx_collect_data.lock();
		dq_cse_list.push_back(cse);
		mtx_collect_data.unlock();
	}
}

void features::local_cse_grabber(DWORD_PTR p_cse)
{
	p_local_player = p_cse;
}

void esp(float x, float y, float w, float h, float health, float max_health, int team, char* name);
void draw_box(float x, float y, float w, float h, const ImColor col);
void draw_name(const char* pcsz_name, float x, float y, float w, ImColor col);
void draw_status_line(float x, float y, float w, float h, float status_value, float max_of_status_value, ImColor col, LINE_STATUS_BAR status_side);
void draw_distance(float x, float y, float w, float h, float distance);
ImColor player_color(int i_team, int my_team, const bool is_visible);

void data_calc_thread()
{
	while (!global_vars::unload)
	{
		dq_draw_data_temp.clear();
		mtx_collect_data.lock();
		if (dq_cse_list.size())
		{
			auto matrix = get_matrix();

			DWORD_PTR local_player = NULL;
			if (memory_utils::is_valid_ptr((void*)p_local_player))
			{
				local_player = memory_utils::read_value<DWORD_PTR>({ p_local_player });
				if (memory_utils::is_valid_ptr((void*)local_player))
				{
					auto health_component = memory_utils::read_value<DWORD_PTR>({ local_player, 0x2E8 });
					if (health_component != NULL)
					{
						auto health = memory_utils::read_value<float>({ health_component, 0x20 });
						auto max_health = memory_utils::read_value<float>({ health_component, 0x24 });
						if (health > 0.1f && health <= max_health)
							my_team = memory_utils::read_value<int>({ local_player, 0x234 });
					}
				}
			}

			for (auto it = dq_cse_list.begin(); it < dq_cse_list.end(); it++)
			{
				auto cse = *it;

				if (cse == local_player)
					continue;

				if (memory_utils::is_valid_ptr((void*)cse))
				{
					if (vars::global::enable && vars::visuals::player_type != 2)
					{
						auto health_component = memory_utils::read_value<DWORD_PTR>({ cse, 0x2E8 });

						if (health_component == NULL)
							continue;

						auto team_idx = memory_utils::read_value<int>({ cse, 0x234 });

						if (team_idx < 1 || team_idx > 64) //firestorm have > 2 team
							goto erase;

						if (vars::visuals::player_type == 0 && my_team == team_idx)
							continue;

						auto health = memory_utils::read_value<float>({ health_component, 0x20 });
						auto max_health = memory_utils::read_value<float>({ health_component, 0x24 });

						if (max_health > 150.f || health < 0.1f || health > max_health)
							continue;

						auto pos_type = memory_utils::read_value<int>({ cse, 0x9A8 });

						auto origin = memory_utils::read_value<Vector>({ cse, 0xC90 });
						auto aabb_min = origin + Vector();
						auto aabb_max = origin + [&pos_type]() -> Vector {
							Vector aabb_max;
							if (pos_type == 0)
								aabb_max = Vector(0.f, 1.7f, 0.f);
							if (pos_type == 1)
								aabb_max = Vector(0.f, 1.15f, 0.f);
							if (pos_type == 2)
								aabb_max = Vector(0.f, 0.4f, 0.f);
							return aabb_max;
						}();

						float out_min[2], out_max[2];
						if (world_to_screen(matrix, aabb_min, out_min) && world_to_screen(matrix, aabb_max, out_max))
						{
							float h = out_min[1] - out_max[1];
							float w = h / 2.f;
							float x = out_min[0] - w / 2.f;
							float y = out_max[1];

							dq_draw_data_temp.emplace_back(x, y, w, h, health, max_health, team_idx);
						}
					}
				}
				else
				{
				erase:
					dq_cse_list.erase(it);
				}
			}
		}
		mtx_collect_data.unlock();

		mtx_drawing.lock();
		dq_drawing_data_present = dq_draw_data_temp;
		mtx_drawing.unlock();

		Sleep(1);
	}
}

void features::draw()
{
	mtx_drawing.lock();
	if (!GetAsyncKeyState(VK_RBUTTON))
	{
		for (const auto& d : dq_drawing_data_present)
		{
			esp(d.m_x, d.m_y, d.m_w, d.m_h, d.m_health, d.m_max_health, d.m_team, NULL);
		}
	}
	mtx_drawing.unlock();
}

void esp(float x, float y, float w, float h, float health, float max_health, int team, char* name)
{
	auto color = player_color(team, my_team, false);

	draw_box(x, y, w, h, color);

	//draw_name(name, x, y, w, color);

	draw_status_line(x, y, w, h, health, max_health, ImColor(0.f, 1.f, 0.f), LINE_STATUS_BAR::LEFT);
}

void draw_box(float x, float y, float w, float h, const ImColor col)
{
	m_pDrawing->DrawEspBox(vars::visuals::box_type, x, y, w, h, col.Value.x, col.Value.y, col.Value.z, col.Value.w);
}

void draw_name(const char* pcsz_name, float x, float y, float w, ImColor col)
{
	if (vars::visuals::name == false)
		return;

	if (pcsz_name == NULL)
		return;

	ImFont* Font = ImGui::GetIO().Fonts->Fonts[0];
	ImVec2 text_size = Font->CalcTextSizeA(vars::font::size ? vars::font::size : Font->FontSize, FLT_MAX, 0, "");

	m_pDrawing->AddText(x + w / 2.f, y - text_size.y - 2.f, ImColor(1.f, 1.f, 1.f, col.Value.w), vars::font::size, FL_CENTER_X, u8"%s", pcsz_name);
}

void draw_status_line(float x, float y, float w, float h, float status_value, float max_of_status_value, ImColor col, LINE_STATUS_BAR status_side)
{
	if (status_value <= 0.f)
		return;

	status_value = ImClamp(status_value, 0.f, max_of_status_value);

	const auto size_h = h / max_of_status_value * status_value;
	const auto size_w = w / max_of_status_value * status_value;

	const auto thickness = 2.f;

	switch (status_side)
	{
	case LINE_STATUS_BAR::LEFT:
		m_pDrawing->DrawFillArea(x - thickness - 1.9f, y + h, thickness, -size_h, col);

		if (vars::visuals::box_type == 2 || vars::visuals::box_type == 4 || vars::visuals::box_type == 6)
			m_pDrawing->DrawBox(x - thickness - 2.9f, y - 1.f, thickness + 2.f, h + 2.f, ImColor(0.f, 0.f, 0.f, col.Value.w));

		break;
	case LINE_STATUS_BAR::RIGHT:
		m_pDrawing->DrawFillArea(x + w - thickness + (2.9f * 2.f), y + h, thickness, -size_h, col);

		if (vars::visuals::box_type == 2 || vars::visuals::box_type == 4 || vars::visuals::box_type == 6)
			m_pDrawing->DrawBox(x + w - thickness + (2.9f * 2.f) - 0.9f, y - 1.f, thickness + 2.f, h + 2.f, ImColor(0.f, 0.f, 0.f, col.Value.w));

		break;
	case LINE_STATUS_BAR::UPPER:
		m_pDrawing->DrawFillArea(x, y - thickness - 1.9f, size_w + 1.9f, thickness, col);

		if (vars::visuals::box_type == 2 || vars::visuals::box_type == 4 || vars::visuals::box_type == 6)
			m_pDrawing->DrawBox(x - 1.9f, y - thickness - 2.9f, w + 3.9f, thickness + 2.f, ImColor(0.f, 0.f, 0.f, col.Value.w));

		break;
	case LINE_STATUS_BAR::BOTTOM:
		m_pDrawing->DrawFillArea(x, y + h + thickness + 0.9f, size_w + 1.9f, thickness, col);

		if (vars::visuals::box_type == 2 || vars::visuals::box_type == 4 || vars::visuals::box_type == 6)
			m_pDrawing->DrawBox(x - 1.9f, y + h + thickness, w + 3.9f, thickness + 2.f, ImColor(0.f, 0.f, 0.f, col.Value.w));

		break;
	default: break;
	}
}

void draw_distance(float x, float y, float w, float h, float distance)
{
	if (vars::visuals::distance == false)
		return;

	m_pDrawing->AddText(x + w / 2.f, y + h + 4.9f, ImColor(1.f, 1.f, 1.f), vars::font::size, FL_CENTER_X, "%.0f", distance);
}

ImColor player_color(int i_team, int my_team, const bool is_visible)
{
	if (i_team == my_team)
	{
		if (is_visible)
			return ImColor(vars::visuals::col_teammate_vis[0], vars::visuals::col_teammate_vis[1], vars::visuals::col_teammate_vis[2], 1.f);
		else
			return ImColor(vars::visuals::col_teammate_invis[0], vars::visuals::col_teammate_invis[1], vars::visuals::col_teammate_invis[2], 1.f);
	}
	else if (i_team != my_team)
	{
		if (is_visible)
			return ImColor(vars::visuals::col_enemy_vis[0], vars::visuals::col_enemy_vis[1], vars::visuals::col_enemy_vis[2], 1.f);
		else
			return ImColor(vars::visuals::col_enemy_invis[0], vars::visuals::col_enemy_invis[1], vars::visuals::col_enemy_invis[2], 1.f);
	}
	else
		return ImColor(1.f, 1.f, 1.f);
}

void features::draw_cringeshot_counter()
{
	static int save_screenshot_count = global_vars::screenshot_counter;

	static float color_fade = 1.f;

	static auto col = ImColor(1.f, 1.f, 1.f);

	if (save_screenshot_count != global_vars::screenshot_counter)
	{
		color_fade = 0.f;
		save_screenshot_count = global_vars::screenshot_counter;
	}

	if (color_fade <= 1.f)
	{
		color_fade += 0.005f;
		col.Value.y = color_fade;
		col.Value.z = color_fade;
	}

	m_pDrawing->AddText(10, 10, col, 20, FL_NONE, "Taked screenshot: %d", global_vars::screenshot_counter);
}

#define WEAPON_FIRE_RATE_INTRUCTION_ADDRESS 0x142395DA2
/*
	pattern: F3 0F 11 87 D4 01 00 00 E9 55, bytes: F3 0F11 87 D4010000

	"\xF3\x0F\x00\x00\x00\x00\x00\x00\xE9\x00\x00\x00\x00\x85\xD2", "xx??????x????xx"
	"F3 0F ? ? ? ? ? ? E9 ? ? ? ? 85 D2"
*/

#define WEAPON_PITCH_RECOIL_INTRUCTION_ADDRESS 0x1492E22B2 
/*
	pattern: F3 0F 11 47 7C F3 41, bytes: F3 0F11 47 7C

	Address of signature = bfv.exe + 0x092E22B2
	"\xF3\x0F\x00\x00\x00\xF3\x41\x00\x00\x00\x00\x41\x0F\x00\x00\x76", "xx???xx????xx??x"
	"F3 0F ? ? ? F3 41 ? ? ? ? 41 0F ? ? 76"
*/

#define WEAPON_YAW_RECOIL_INTRUCTION_ADDRESS 0x1492E22AD
/*
	pattern: F3 0F 11 77 78 F3, bytes: F3 0F11 77 78

	"\xF3\x0F\x00\x00\x00\xF3\x0F\x00\x00\x00\xF3\x41\x00\x00\x00\x00\x41\x0F\x00\x00\x76", "xx???xx???xx????xx??x"
	"F3 0F ? ? ? F3 0F ? ? ? F3 41 ? ? ? ? 41 0F ? ? 76"
*/

#define WEAPON_SPREAD_INSTRUCTION_ADDRESS 0x14218AB8A 
/*
	pattern: F3 0F 11 47 48 F3 41, bytes: F3 0F11 47 48

	"\xF3\x0F\x00\x00\x00\xF3\x41\x00\x00\x00\x0F\x2F\x00\x00\x00\x00\x00\x76", "xx???xx???xx?????x"
	"F3 0F ? ? ? F3 41 ? ? ? 0F 2F ? ? ? ? ? 76"
*/

void features::norecoil(bool is_enable)
{
	if (is_enable)
	{
		BYTE patch[5];
		memset(&patch, 0x90, 5);

		memory_utils::patch_instruction((void*)WEAPON_PITCH_RECOIL_INTRUCTION_ADDRESS, patch, 5);
		memory_utils::patch_instruction((void*)WEAPON_YAW_RECOIL_INTRUCTION_ADDRESS, patch, 5);
	}
	else
	{
		BYTE original_recoil_pitch[5] = { 0xf3, 0x0f, 0x11, 0x47, 0x7c };
		BYTE origin_recoil_yaw[5] = { 0xf3, 0x0f, 0x11, 0x77, 0x78 };

		memory_utils::patch_instruction((void*)WEAPON_PITCH_RECOIL_INTRUCTION_ADDRESS, original_recoil_pitch, 5);
		memory_utils::patch_instruction((void*)WEAPON_YAW_RECOIL_INTRUCTION_ADDRESS, origin_recoil_yaw, 5);
	}
}

void features::nospread(bool is_enable)
{
	if (is_enable)
	{
		BYTE patch[10] = { 0xc7, 0x47, 0x48, 0x00, 0x00, 0x00, 0x00, 0x90, 0x90, 0x90 };

		memory_utils::patch_instruction((void*)WEAPON_SPREAD_INSTRUCTION_ADDRESS, patch, 10);
	}
	else
	{
		BYTE original[10] = { 0xf3, 0x0f, 0x11, 0x47, 0x48, 0xf3, 0x41, 0x0f, 0x5c, 0xf8 };

		memory_utils::patch_instruction((void*)WEAPON_SPREAD_INSTRUCTION_ADDRESS, original, 10);
	}
}

void features::increase_fire_rate(bool is_enable, float rate)
{
	if (is_enable)
	{
		BYTE patch[14] = { 0xc7, 0x87, 0xd4, 0x01, 0x00, 0x00, 0x0a, 0xd7, 0xa3, 0x3c, 0x90, 0x90, 0x90, 0x90 };

		memcpy(patch + 0x6, &rate, 4);

		memory_utils::patch_instruction((void*)(WEAPON_FIRE_RATE_INTRUCTION_ADDRESS - 0x6), patch, 14);
	}
	else
	{
		BYTE original[14] = { 0x89, 0x87, 0x60, 0x01, 0x00, 0x00, 0xf3, 0x0f, 0x11, 0x87, 0xd4, 0x01, 0x00, 0x00 };

		memory_utils::patch_instruction((void*)(WEAPON_FIRE_RATE_INTRUCTION_ADDRESS - 0x6), original, 14);
	}
}

