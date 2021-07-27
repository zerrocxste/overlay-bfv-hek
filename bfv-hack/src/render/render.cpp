#include "../includes.h"

static void initialize_imgui()
{
	ImGui::CreateContext();
	ImGui::StyleColorsClassic();

	auto& style = ImGui::GetStyle();

	style.FrameRounding = 3.f;
	style.ChildRounding = 3.f;
	style.ChildBorderSize = 1.f;
	style.ScrollbarSize = 0.6f;
	style.ScrollbarRounding = 3.f;
	style.GrabRounding = 3.f;
	style.WindowRounding = 0.f;

	style.Colors[ImGuiCol_FrameBg] = ImColor(200, 200, 200);
	style.Colors[ImGuiCol_FrameBgHovered] = ImColor(220, 220, 220);
	style.Colors[ImGuiCol_FrameBgActive] = ImColor(230, 230, 230);
	style.Colors[ImGuiCol_Separator] = ImColor(180, 180, 180);
	style.Colors[ImGuiCol_CheckMark] = ImColor(255, 172, 19);
	style.Colors[ImGuiCol_SliderGrab] = ImColor(255, 172, 19);
	style.Colors[ImGuiCol_SliderGrabActive] = ImColor(255, 172, 19);
	style.Colors[ImGuiCol_ScrollbarBg] = ImColor(120, 120, 120);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImColor(255, 172, 19);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImGui::GetStyleColorVec4(ImGuiCol_ScrollbarGrab);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImGui::GetStyleColorVec4(ImGuiCol_ScrollbarGrab);
	style.Colors[ImGuiCol_Header] = ImColor(160, 160, 160);
	style.Colors[ImGuiCol_HeaderHovered] = ImColor(200, 200, 200);
	style.Colors[ImGuiCol_Button] = ImColor(180, 180, 180);
	style.Colors[ImGuiCol_ButtonHovered] = ImColor(200, 200, 200);
	style.Colors[ImGuiCol_ButtonActive] = ImColor(230, 230, 230);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.78f, 0.78f, 0.78f, 1.f);
	style.Colors[ImGuiCol_WindowBg] = ImColor(220, 220, 220, 0.7 * 255);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.40f, 0.40f, 0.80f, 0.20f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.40f, 0.40f, 0.55f, 0.80f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.72f, 0.72f, 0.72f, 0.70f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.83f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.75f, 0.75f, 0.75f, 0.87f);
	style.Colors[ImGuiCol_Text] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
	style.Colors[ImGuiCol_ChildBg] = ImVec4(0.72f, 0.72f, 0.72f, 0.76f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.76f, 0.76f, 0.76f, 1.00f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.81f, 0.81f, 0.81f, 1.00f);
	style.Colors[ImGuiCol_Tab] = ImVec4(0.61f, 0.61f, 0.61f, 0.79f);
	style.Colors[ImGuiCol_TabHovered] = ImVec4(0.71f, 0.71f, 0.71f, 0.80f);
	style.Colors[ImGuiCol_TabActive] = ImVec4(0.77f, 0.77f, 0.77f, 0.84f);
	style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.73f, 0.73f, 0.73f, 0.82f);
	style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.58f, 0.58f, 0.58f, 0.84f);

	auto& io = ImGui::GetIO();

	io.IniFilename = NULL;

	io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Verdana.ttf", 15.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());
	render::font_Main = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Verdana.ttf", 21);
	render::font_Credits = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Verdana.ttf", 15);

	ImGui_ImplWin32_Init(DXWFGetHWND());
	ImGui_ImplDX9_Init(DXWFGetD3DDevice());
	ImGui_ImplDX9_CreateDeviceObjects();

	auto flags_color_edit = ImGuiColorEditFlags_PickerHueBar | ImGuiColorEditFlags_NoInputs;
	ImGui::SetColorEditOptions(flags_color_edit);
}

void render::shutdown_imgui()
{
	global_vars::menu_open = false;
	Sleep(100);

	ImGui_ImplDX9_Shutdown();

	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	Sleep(100);
}

void render::begin_scene()
{
	static auto once = []()
	{
		initialize_imgui();
		return true;
	}();

	if (global_vars::unload || global_vars::stop_render)
		return;

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();

	ImGui::NewFrame();

	static bool is_pressed = false;
	auto menu_key_pressed = GetAsyncKeyState(VK_INSERT);
	if (menu_key_pressed && is_pressed == false)
	{
		global_vars::menu_open = !global_vars::menu_open;
		if (global_vars::menu_open)
		{
			SetForegroundWindow(DXWFGetHWND());
			SetWindowLongA(DXWFGetHWND(), GWL_EXSTYLE, WS_EX_TOPMOST | WS_EX_COMPOSITED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE);
			DXWFEnableBlur(ENABLE_BLUR_ACRYLIC, my_color(0, 0, 0, 1));
		}
		else
		{
			SetWindowLongA(DXWFGetHWND(), GWL_EXSTYLE, WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_COMPOSITED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE);
			DXWFDisableBlur();
		}
		is_pressed = true;
	}
	else if (!menu_key_pressed)
	{
		is_pressed = false;
	}

	/* //manual invoke ff screenshot
	static bool called = false;
	auto called_pressed = GetAsyncKeyState(VK_DELETE);
	if (called_pressed && called == false)
	{
		auto get_address_struct = []() -> DWORD64
		{
			auto ret = *(DWORD64*)0x1447C8060;
			printf("0x%p\n", ret);
			return ret;
		};
		static DWORD64 address = get_address_struct();
		(*(void(__fastcall*)(__int64))0x141BE97A0)(address); //dice troll mode
		called = true;
	}
	else if (!called_pressed)
	{
		called = false;
	}
	*/

	if (global_vars::menu_open)
		menu::run();

	features::notification();

	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4());
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
	ImGui::Begin("##BackBuffer", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoSavedSettings);
	ImGui::SetWindowPos(ImVec2(), ImGuiCond_Always);
	ImGui::SetWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y), ImGuiCond_Always);

	//m_pDrawing->DrawBox(0.f, 0.f, ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y, ImColor(1.f, 0.f, 0.f));
	features::draw_cringeshot_counter();
	features::draw();

	ImGui::GetCurrentWindow()->DrawList->PushClipRectFullScreen();
	ImGui::End();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar();

	ImGui::EndFrame();
}

void render::reset()
{
	ImGui_ImplDX9_CreateDeviceObjects();
	ImGui_ImplDX9_InvalidateDeviceObjects();
}

namespace menu_utils
{
	void tabs(const char* lebel[], const int arr_, ImVec2 size, int& cur_page, float x_offset = 0.f, bool* click_callback = false)
	{
		for (int i = 0; i < arr_; i++)
		{
			auto& style = ImGui::GetStyle();
			ImVec4 save_shit[2];
			save_shit[0] = style.Colors[ImGuiCol_Button];
			save_shit[1] = style.Colors[ImGuiCol_ButtonHovered];
			if (i == cur_page)
			{
				style.Colors[ImGuiCol_Button] = style.Colors[ImGuiCol_ButtonActive];
				style.Colors[ImGuiCol_ButtonHovered] = style.Colors[ImGuiCol_ButtonActive];
			}

			if (ImGui::Button(lebel[i], ImVec2(size.x + x_offset, size.y)))
			{
				if (cur_page != i && click_callback != nullptr)
					*click_callback = true;
				cur_page = i;
			}

			if (i == cur_page)
			{
				style.Colors[ImGuiCol_Button] = save_shit[0];
				style.Colors[ImGuiCol_ButtonHovered] = save_shit[1];
			}

			if (i != arr_ - 1)
				ImGui::SameLine();
		}
	}

	void set_color(bool white)
	{
		auto& style = ImGui::GetStyle();
		if (white)
		{
			style.Colors[ImGuiCol_FrameBg] = ImColor(200, 200, 200);
			style.Colors[ImGuiCol_FrameBgHovered] = ImColor(220, 220, 220);
			style.Colors[ImGuiCol_FrameBgActive] = ImColor(230, 230, 230);
			style.Colors[ImGuiCol_Separator] = ImColor(180, 180, 180);
			style.Colors[ImGuiCol_CheckMark] = ImColor(255, 172, 19);
			style.Colors[ImGuiCol_SliderGrab] = ImColor(255, 172, 19);
			style.Colors[ImGuiCol_SliderGrabActive] = ImColor(255, 172, 19);
			style.Colors[ImGuiCol_ScrollbarBg] = ImColor(120, 120, 120);
			style.Colors[ImGuiCol_ScrollbarGrab] = ImColor(255, 172, 19);
			style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImGui::GetStyleColorVec4(ImGuiCol_ScrollbarGrab);
			style.Colors[ImGuiCol_ScrollbarGrabActive] = ImGui::GetStyleColorVec4(ImGuiCol_ScrollbarGrab);
			style.Colors[ImGuiCol_Header] = ImColor(160, 160, 160);
			style.Colors[ImGuiCol_HeaderHovered] = ImColor(200, 200, 200);
			style.Colors[ImGuiCol_Button] = ImColor(180, 180, 180);
			style.Colors[ImGuiCol_ButtonHovered] = ImColor(200, 200, 200);
			style.Colors[ImGuiCol_ButtonActive] = ImColor(230, 230, 230);
			style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.78f, 0.78f, 0.78f, 1.f);
			style.Colors[ImGuiCol_WindowBg] = ImColor(220, 220, 220, 0.7 * 255);
			style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
			style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.40f, 0.40f, 0.80f, 0.20f);
			style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.40f, 0.40f, 0.55f, 0.80f);
			style.Colors[ImGuiCol_Border] = ImVec4(0.72f, 0.72f, 0.72f, 0.70f);
			style.Colors[ImGuiCol_TitleBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.83f);
			style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.75f, 0.75f, 0.75f, 0.87f);
			style.Colors[ImGuiCol_Text] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
			style.Colors[ImGuiCol_ChildBg] = ImVec4(0.72f, 0.72f, 0.72f, 0.76f);
			style.Colors[ImGuiCol_PopupBg] = ImVec4(0.76f, 0.76f, 0.76f, 1.00f);
			style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.81f, 0.81f, 0.81f, 1.00f);
			style.Colors[ImGuiCol_Tab] = ImVec4(0.61f, 0.61f, 0.61f, 0.79f);
			style.Colors[ImGuiCol_TabHovered] = ImVec4(0.71f, 0.71f, 0.71f, 0.80f);
			style.Colors[ImGuiCol_TabActive] = ImVec4(0.77f, 0.77f, 0.77f, 0.84f);
			style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.73f, 0.73f, 0.73f, 0.82f);
			style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.58f, 0.58f, 0.58f, 0.84f);
		}
		else
		{
			ImGui::StyleColorsClassic();
			style.Colors[ImGuiCol_FrameBg] = ImColor(3, 3, 3);
			style.Colors[ImGuiCol_FrameBgHovered] = ImColor(20, 20, 20);
			style.Colors[ImGuiCol_FrameBgActive] = ImColor(30, 30, 30);
			style.Colors[ImGuiCol_Separator] = ImColor(8, 8, 8);
			style.Colors[ImGuiCol_CheckMark] = ImColor(255, 172, 19);
			style.Colors[ImGuiCol_SliderGrab] = ImColor(255, 172, 19);
			style.Colors[ImGuiCol_SliderGrabActive] = ImColor(255, 172, 19);
			style.Colors[ImGuiCol_PopupBg] = ImColor(12, 12, 12);
			style.Colors[ImGuiCol_ScrollbarBg] = ImColor(12, 12, 12);
			style.Colors[ImGuiCol_ScrollbarGrab] = ImColor(255, 172, 19);
			style.Colors[ImGuiCol_ScrollbarGrab] = ImGui::GetStyleColorVec4(ImGuiCol_ScrollbarGrab);
			style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImGui::GetStyleColorVec4(ImGuiCol_ScrollbarGrab);
			style.Colors[ImGuiCol_ScrollbarGrabActive] = ImGui::GetStyleColorVec4(ImGuiCol_ScrollbarGrab);
			style.Colors[ImGuiCol_Border] = ImColor(12, 12, 12);
			style.Colors[ImGuiCol_ChildBg] = ImColor(16, 16, 16);
			style.Colors[ImGuiCol_Header] = ImColor(16, 16, 16);
			style.Colors[ImGuiCol_HeaderHovered] = ImColor(20, 20, 20);
			style.Colors[ImGuiCol_HeaderActive] = ImColor(30, 30, 30);
			style.Colors[ImGuiCol_Button] = ImColor(8, 8, 8);
			style.Colors[ImGuiCol_ButtonHovered] = ImColor(20, 20, 20);
			style.Colors[ImGuiCol_ButtonActive] = ImColor(30, 30, 30);
			style.Colors[ImGuiCol_Text] = ImColor(255, 255, 255);
			style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.78f, 0.78f, 0.78f, 1.f);
		}
	}
}

enum GLOBALS_PAGES
{
	PAGE_GLOBALS_AIMBOT,
	PAGE_GLOBALS_VISUALS,
	PAGE_GLOBALS_MISC,
	PAGE_GLOBALS_CONFIG
};

void render::menu::run()
{
	static int m_Page = 0;

	ImGui::SetNextWindowSizeConstraints(ImVec2(499.f, 399.f), ImVec2(999.f, 899.f));
	ImGui::SetNextWindowSize(ImVec2(499.f, 399.f), ImGuiCond_Once);

	auto WndFlags = ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;
	ImGui::Begin("bfv hack / zerrocxste", nullptr, WndFlags);

	ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 4);
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 17);

	ImGui::PushFont(render::font_Main);
	ImGui::Text("alternative");
	ImGui::PopFont();

	ImGui::SameLine();

	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 17);
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 4);

	if (ImGui::GetWindowSize().x < 618.f)
		ImGui::GetFont()->FontSize = ImClamp(abs(634.f - ImGui::GetWindowSize().x) / 4.f, 16.f, 22.f);

	const char* lebels[] = {
		"Main", "Visuals", "Misc", "Config"
	};

	menu_utils::tabs(lebels, IM_ARRAYSIZE(lebels), ImVec2(ImGui::GetWindowSize().x / 4.f - 44.f, 20), m_Page);

	ImGui::GetFont()->FontSize = 15.f;
	ImGui::BeginChild("1", ImVec2(0, ImGui::GetWindowSize().y - 60.f), true);

	if (m_Page == PAGE_GLOBALS_AIMBOT)
	{
		ImGui::BeginChild("Aimbot", ImVec2(), true);

		std::string norecoil_label = vars::misc::recoil_patched ? "disable norecoil" : "enable norecoil";
		if (ImGui::Button(norecoil_label.c_str(), ImVec2(ImGui::GetWindowSize().x - 15.f, 40)))
		{
			vars::misc::recoil_patched = !vars::misc::recoil_patched;
			features::norecoil(vars::misc::recoil_patched);
		}

		std::string nospread_label = vars::misc::spread_patched ? "disable nospread" : "enable nospread";
		if (ImGui::Button(nospread_label.c_str(), ImVec2(ImGui::GetWindowSize().x - 15.f, 40)))
		{
			vars::misc::spread_patched = !vars::misc::spread_patched;
			features::nospread(vars::misc::spread_patched);
		}

		std::string fire_rate_label = vars::misc::fire_rate_patched ? "disable fire rate" : "enable fire rate";
		if (ImGui::Button(fire_rate_label.c_str(), ImVec2(ImGui::GetWindowSize().x - 15.f, 40)))
		{
			vars::misc::fire_rate_patched = !vars::misc::fire_rate_patched;
			features::increase_fire_rate(vars::misc::fire_rate_patched);
		}

		ImGui::EndChild();
	}
	else if (m_Page == PAGE_GLOBALS_VISUALS)
	{
		ImGui::BeginChild("Visuals", ImVec2(), true);

		ImGui::Checkbox("Enable", &vars::global::enable);

		if (ImGui::TreeNode("Globals"))
		{
			ImGui::ColorEdit3("##invis enemy", vars::visuals::col_enemy_invis);
			ImGui::SameLine();
			ImGui::Text("enemy   ");
			ImGui::SameLine();
			ImGui::ColorEdit3("##invis teammate", vars::visuals::col_teammate_invis);
			ImGui::SameLine();
			ImGui::Text("teammate    ");
			ImGui::SameLine();
			ImGui::ColorEdit3("##vis enemy", vars::visuals::col_enemy_vis);
			ImGui::SameLine();
			ImGui::Text("vis enemy   ");
			ImGui::SameLine();
			ImGui::ColorEdit3("##vis teammate", vars::visuals::col_teammate_vis);
			ImGui::SameLine();
			ImGui::Text("vis teammate  ");

			const char* pcszBoxesType[] = { "Off", "Box", "Box outline", "Corner box", "Corner box outline",  "Round box", "Round box outline" };
			ImGui::Combo("Bounding box", &vars::visuals::box_type, pcszBoxesType, IM_ARRAYSIZE(pcszBoxesType));

			ImGui::TreePop();
		}

		const char* pcszPlayerType[] = { "Enemies", "Enemies and teammates", "Off", };
		ImGui::Combo("Player type", &vars::visuals::player_type, pcszPlayerType, IM_ARRAYSIZE(pcszPlayerType));

		if (ImGui::TreeNode("ESP"))
		{
			ImGui::Checkbox("Health", &vars::visuals::health);
			ImGui::Spacing();
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Font"))
		{
			const char* pcszFontType[] = { "Default", "Shadow", "Outline" };
			ImGui::SliderFloat("Font size", &vars::font::size, 0.f, 20.f, "%.0f", 1.0f);
			ImGui::Combo("Font type", &vars::font::style, pcszFontType, IM_ARRAYSIZE(pcszFontType));

			ImGui::TreePop();
		}

		ImGui::EndChild();
	}
	else if (m_Page == PAGE_GLOBALS_MISC)
	{
		ImGui::BeginChild("Misc", ImVec2(), true);

		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.f, 0.5f, 1.5f, 1.f));
		if (ImGui::Button("Quit", ImVec2(ImGui::GetWindowSize().x - 15.f, 40)))
		{
			global_vars::unload = true;
			PostQuitMessage(0);
		}
		ImGui::PopStyleColor();

		if (ImGui::SliderInt("Overlay max fps", &global_vars::overlay_max_fps, 5, 120))
		{
			DXWFSetFramerateLimit(global_vars::overlay_max_fps);
		}

		ImGui::EndChild();
	}
	else if (m_Page == PAGE_GLOBALS_CONFIG)
	{
		ImGui::BeginChild("Config", ImVec2(), true);

		ImGui::Text("Coming soon...");

		if (ImGui::Button("Color style"))
		{
			static bool isWhite = false;
			isWhite = !isWhite;
			menu_utils::set_color(isWhite);
		}

		ImGui::EndChild();
	}

	ImGui::EndChild();
	ImGui::PushFont(render::font_Credits);
	ImGui::Text("Build date: %s, %s.", __DATE__, __TIME__);
	ImGui::SameLine();
	ImGui::SetCursorPosX(ImGui::GetWindowSize().x - ImGui::CalcTextSize("by zerrocxste").x - 15.f);
	ImGui::Text("by zerrocxste");
	ImGui::PopFont();

	ImGui::End();
}



//static bool TakeScreenshots(int adapterInx, int outputInx)
//{
//	HRESULT hr;
//
//	//
//	// Open IDXGIFactory1.
//	//
//
//	IDXGIFactory1* pFactory;
//	IDXGIAdapter1* pAdapter;
//	IDXGIOutput* pOutput;
//	IDXGIOutput1* pOutput1;
//
//	hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)(&pFactory));
//	if (FAILED(hr)) return false;
//
//	// Get DXGI output.
//
//	hr = pFactory->EnumAdapters1(adapterInx, &pAdapter);
//	if (FAILED(hr)) return false;
//
//	hr = pAdapter->EnumOutputs(outputInx, &pOutput);
//	if (FAILED(hr)) return false;
//
//	// Query interface for IDXGIOutput1.
//	hr = pOutput->QueryInterface(__uuidof(IDXGIOutput1), reinterpret_cast<void**>(&pOutput1));
//	if (FAILED(hr)) return false;
//
//	//
//	// Get device object.
//	//
//
//	D3D_FEATURE_LEVEL d3dFeatLvl;
//	ID3D11Device* pDevice = nullptr;
//	ID3D11DeviceContext* pImmediateContext = nullptr;
//
//	hr = D3D11CreateDevice(pAdapter, D3D_DRIVER_TYPE_UNKNOWN,
//		0, 0, 0, 0,
//		D3D11_SDK_VERSION,
//		&pDevice,
//		&d3dFeatLvl,
//		&pImmediateContext);
//	if (FAILED(hr)) return false;
//
//	// Create desktop duplication.
//	IDXGIOutputDuplication* pDeskDupl;
//	hr = pOutput1->DuplicateOutput(pDevice, &pDeskDupl);
//	if (FAILED(hr)) return false;
//
//	///////////////////////////////////////////////////////////////////////////
//
//	ID3D11Texture2D* AcquiredDesktopImage;
//	IDXGIResource* DesktopResource = nullptr;
//	DXGI_OUTDUPL_FRAME_INFO FrameInfo;
//
//	for (int i = 0; i < 10; i++)
//	{
//		Console::WriteLine(i);//!
//		Sleep(1);
//
//		// Get new frame.
//		hr = pDeskDupl->AcquireNextFrame(500, &FrameInfo, &DesktopResource);
//		if (FAILED(hr)) return false;
//
//		// Query interface for IDXGIResource.
//		hr = DesktopResource->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&AcquiredDesktopImage));
//		DesktopResource->Release();
//		if (FAILED(hr)) return false;
//
//		hr = pDeskDupl->ReleaseFrame();
//		if (FAILED(hr)) return false;
//
//		hr = AcquiredDesktopImage->Release();
//		if (FAILED(hr)) return false;
//
//		HBITMAP hBmp = (HBITMAP)ExtractBitmap(AcquiredDesktopImage, pDevice);
//		System::Drawing::Bitmap::FromHbitmap(IntPtr(hBmp))->Save("Screenshot" + i + ".bmp");
//	}
//
//	return true;
//
//	IDXGISwapChain* swapchain = nullptr;
//	ID3D11Device* device = nullptr;
//	ID3D11DeviceContext* context = nullptr;
//	ID3D11RenderTargetView* render_view = nullptr;
//
//	enum IDXGISwapChainvTable //for dx10 / dx11
//	{
//		QUERY_INTERFACE,
//		ADD_REF,
//		RELEASE,
//		SET_PRIVATE_DATA,
//		SET_PRIVATE_DATA_INTERFACE,
//		GET_PRIVATE_DATA,
//		GET_PARENT,
//		GET_DEVICE,
//		PRESENT,
//		GET_BUFFER,
//		SET_FULLSCREEN_STATE,
//		GET_FULLSCREEN_STATE,
//		GET_DESC,
//		RESIZE_BUFFERS,
//		RESIZE_TARGET,
//		GET_CONTAINING_OUTPUT,
//		GET_FRAME_STATISTICS,
//		GET_LAST_PRESENT_COUNT
//	};
//
//	HRESULT hr;
//
//	IDXGIFactory1* pFactory;
//	IDXGIAdapter1* pAdapter;
//	IDXGIOutput* pOutput;
//	IDXGIOutput1* pOutput1;
//
//	hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)(&pFactory));
//	if (FAILED(hr)) return;
//
//	// Get DXGI output.
//
//	hr = pFactory->EnumAdapters1(0, &pAdapter);
//	if (FAILED(hr)) return;
//
//	hr = pAdapter->EnumOutputs(0, &pOutput);
//	if (FAILED(hr)) return;
//
//	// Query interface for IDXGIOutput1.
//	hr = pOutput->QueryInterface(__uuidof(IDXGIOutput1), reinterpret_cast<void**>(&pOutput1));
//	if (FAILED(hr)) return;
//
//	D3D_FEATURE_LEVEL feature_level = D3D_FEATURE_LEVEL_11_0;
//	DXGI_SWAP_CHAIN_DESC scd{};
//	ZeroMemory(&scd, sizeof(scd));
//	scd.BufferCount = 1;
//	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
//	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
//	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
//	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
//	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
//	scd.OutputWindow = global_vars::hwnd_game;
//	scd.SampleDesc.Count = 1;
//	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
//	scd.Windowed = TRUE;
//	scd.BufferDesc.RefreshRate.Numerator = 60;
//	scd.BufferDesc.RefreshRate.Denominator = 1;
//
//	if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, &feature_level, 1, D3D11_SDK_VERSION, &scd, &swapchain, &device, NULL, &context)))
//	{
//		std::cout << "failed to create device\n";
//		return;
//	}
//
//	IDXGIOutputDuplication* pDeskDupl;
//	hr = pOutput1->DuplicateOutput(device, &pDeskDupl);
//
//	if (hr != S_OK)
//		std::cout << "failed\n";
//
//	void** vtavle_output_dup = *reinterpret_cast<void***>(pDeskDupl);
//
//	std::cout << vtavle_output_dup << std::endl;
//
//	
//}

//HRESULT DUPLICATIONMANAGER::GetFrame(_Out_ FRAME_DATA* Data)
//{
//    HRESULT hr = S_OK;
//
//    IDXGIResource* DesktopResource = NULL;
//    DXGI_OUTDUPL_FRAME_INFO FrameInfo;
//
//    //Get new frame
//    hr = DeskDupl->AcquireNextFrame(500, &FrameInfo, &DesktopResource);
//    if (FAILED(hr))
//    {
//        if ((hr != DXGI_ERROR_ACCESS_LOST) && (hr != DXGI_ERROR_WAIT_TIMEOUT))
//        {
//            DisplayErr(L"Failed to acquire next frame in DUPLICATIONMANAGER", L"Error", hr);
//        }
//        return hr;
//    }
//
//    // If still holding old frame, destroy it
//    if (AcquiredDesktopImage)
//    {
//        AcquiredDesktopImage->Release();
//        AcquiredDesktopImage = NULL;
//    }
//
//    // QI for IDXGIResource
//    hr = DesktopResource->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&AcquiredDesktopImage));
//    DesktopResource->Release();
//    DesktopResource = NULL;
//    if (FAILED(hr))
//    {
//        DisplayErr(L"Failed to QI for ID3D11Texture2D from acquired IDXGIResource in DUPLICATIONMANAGER", L"Error", hr);
//        return hr;
//    }
//
//    // Get metadata
//    if (FrameInfo.TotalMetadataBufferSize)
//    {
//        // Old buffer too small
//        if (FrameInfo.TotalMetadataBufferSize > MetaDataSize)
//        {
//            if (MetaDataBuffer)
//            {
//                delete [] MetaDataBuffer;
//                MetaDataBuffer = NULL;
//            }
//            MetaDataBuffer = new (std::nothrow) BYTE[FrameInfo.TotalMetadataBufferSize];
//            if (!MetaDataBuffer)
//            {
//                DisplayErr(L"Failed to allocate memory for metadata in DUPLICATIONMANAGER", L"Error", E_OUTOFMEMORY);
//                MetaDataSize = 0;
//                Data->MoveCount = 0;
//                Data->DirtyCount = 0;
//                return E_OUTOFMEMORY;
//            }
//            MetaDataSize = FrameInfo.TotalMetadataBufferSize;
//        }
//
//        UINT BufSize = FrameInfo.TotalMetadataBufferSize;
//
//        // Get move rectangles
//        hr = DeskDupl->GetFrameMoveRects(BufSize, reinterpret_cast<DXGI_OUTDUPL_MOVE_RECT*>(MetaDataBuffer), &BufSize);
//        if (FAILED(hr))
//        {
//            if (hr != DXGI_ERROR_ACCESS_LOST)
//            {
//                DisplayErr(L"Failed to get frame move rects in DUPLICATIONMANAGER", L"Error", hr);
//            }
//            Data->MoveCount = 0;
//            Data->DirtyCount = 0;
//            return hr;
//        }
//        Data->MoveCount = BufSize / sizeof(DXGI_OUTDUPL_MOVE_RECT);
//
//        BYTE* DirtyRects = MetaDataBuffer + BufSize;
//        BufSize = FrameInfo.TotalMetadataBufferSize - BufSize;
//
//        // Get dirty rectangles
//        hr = DeskDupl->GetFrameDirtyRects(BufSize, reinterpret_cast<RECT*>(DirtyRects), &BufSize);
//        if (FAILED(hr))
//        {
//            if (hr != DXGI_ERROR_ACCESS_LOST)
//            {
//                DisplayErr(L"Failed to get frame dirty rects in DUPLICATIONMANAGER", L"Error", hr);
//            }
//            Data->MoveCount = 0;
//            Data->DirtyCount = 0;
//            return hr;
//        }
//        Data->DirtyCount = BufSize / sizeof(RECT);
//
//        Data->MetaData = MetaDataBuffer;
//    }
//
//    Data->Frame = AcquiredDesktopImage;
//    Data->FrameInfo = FrameInfo;
//
//    return hr;
//}
//
////
//// Release frame
////
//HRESULT DUPLICATIONMANAGER::DoneWithFrame()
//{
//    HRESULT hr = S_OK;
//
//    hr = DeskDupl->ReleaseFrame();
//    if (FAILED(hr))
//    {
//        DisplayErr(L"Failed to release frame in DUPLICATIONMANAGER", L"Error", hr);
//        return hr;
//    }
//
//    if (AcquiredDesktopImage)
//    {
//        AcquiredDesktopImage->Release();
//        AcquiredDesktopImage = NULL;
//    }
//
//    return hr;
//}
