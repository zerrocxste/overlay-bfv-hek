#include "includes.h"

namespace console
{
	FILE* output_stream = nullptr;

	void detach()
	{
		if (output_stream)
			fclose(output_stream);
		FreeConsole();
	}

	void attach(const char* name)
	{
		if (AllocConsole())
		{
			SetConsoleTitle(name);
			freopen_s(&output_stream, "conout$", "w", stdout);
		}
		else
		{
			detach();
			attach(name);
		}
	}
}

struct find_window_s
{
	char* window_name;
	HWND hWnd;
};

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	auto* window_inform_s = (find_window_s*)lParam;

	if ((!GetWindow(hwnd, GW_OWNER)) && IsWindow(hwnd))
	{
		DWORD process_id = NULL;
		GetWindowThreadProcessId(hwnd, &process_id);

		char* text_window = new char[255];

		GetWindowText(hwnd, text_window, 255);

		if (GetCurrentProcessId() == process_id && strstr(text_window, window_inform_s->window_name) && !strstr(text_window, ".exe"))
		{
#ifdef ENABLE_DEBUG_CONSOLE
			std::cout << "Window name: " << text_window << std::endl;
#endif // ENABLE_DEBUG_CONSOLE
			window_inform_s->hWnd = hwnd;
			return 0;
		}
	}

	return 1;
}

HWND find_game_window(const char* psz_part_of_word_window_name)
{
	find_window_s window_inform_struct{};

	window_inform_struct.window_name = new char[strlen(psz_part_of_word_window_name)];

	strcpy(window_inform_struct.window_name, psz_part_of_word_window_name);

	EnumWindows(EnumWindowsProc, (LPARAM)&window_inform_struct);

	delete[] window_inform_struct.window_name;

	return window_inform_struct.hWnd;
}

void thread(void* arg)
{
	global_vars::module = (HMODULE)arg;

#ifdef ENABLE_DEBUG_CONSOLE
	console::attach("debug");
#endif

	global_vars::hwnd_game = find_game_window("Battlefield");

	if (global_vars::hwnd_game == NULL)
	{
		MessageBox(NULL, "Not found game window", "mat' checkay", MB_OK | MB_ICONERROR);
		goto failed;
	}

	vars::load_default_settings();
	
	g_up_notification_panel = std::make_unique<c_notification_panel>();
	g_up_notification_panel->set_timer(15000);

#ifdef RENDER_EXTERN_WINDOW
	g_up_overlay_render = std::make_unique<c_overlay_render>();
	if (g_up_overlay_render->initialize(global_vars::hwnd_game) == FALSE)
	{
		MessageBox(NULL, "Error initialize overlay", "mat' checkay", MB_OK | MB_ICONERROR);
		goto failed;
	}
#endif

	hook_manager::install_hooks();

	features::initialize_features();

#ifdef RENDER_EXTERN_WINDOW
	g_up_overlay_render->render_loop();
#endif

	hook_manager::shutdown_hooks();

#ifdef RENDER_EXTERN_WINDOW
	g_up_overlay_render->terminate();
#endif

	failed:
#ifdef ENABLE_DEBUG_CONSOLE
	printf("exit...\n");
#endif
	FreeLibraryAndExitThread(global_vars::module, 0);
}

BOOL APIENTRY DllMain( HMODULE hModule,
					   DWORD  ul_reason_for_call,
					   LPVOID lpReserved
					 )
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)thread, hModule, NULL, NULL);

	return TRUE;
}

/*

BOOL __fastcall BitBltScreenshot(HWND Window, void* OutScreenshotBuffer, __int64 MaxScreenshotBufferSize, int ScreenSizeX, int ScreenSizeY)
{
	void* pOutScreenshotBuffer; // r15
	HDC hdcSrc; // rsi
	HDC hDC; // rdi
	HBITMAP hBitMap; // rbp
	BITMAPINFO BitMapInfo; // [rsp+50h] [rbp-78h]

	pOutScreenshotBuffer = OutScreenshotBuffer;
	hdcSrc = GetDC(Window);
	hDC = CreateCompatibleDC(hdcSrc);
	hBitMap = CreateCompatibleBitmap(hdcSrc, ScreenSizeX, ScreenSizeY);
	SelectObject(hDC, hBitMap);
	BitBlt(hDC, 0, 0, ScreenSizeX, ScreenSizeY, hdcSrc, 0, 0, 0xCC0020u);
	memset(&BitMapInfo, 0, 0x2Cui64);
	BitMapInfo.bmiHeader.biSize = 40;
	GetDIBits(hDC, hBitMap, 0, 0, 0i64, &BitMapInfo, 0);
	if (BitMapInfo.bmiHeader.biBitCount != 32 || BitMapInfo.bmiHeader.biSizeImage <= 0)
	{
		BitMapInfo.bmiHeader.biBitCount = 32;
		BitMapInfo.bmiHeader.biSizeImage = 4 * ScreenSizeX * ScreenSizeY;
	}
	BitMapInfo.bmiHeader.biCompression = 0;
	GetDIBits(hDC, hBitMap, 0, BitMapInfo.bmiHeader.biHeight, pOutScreenshotBuffer, &BitMapInfo, 0);
	if (hdcSrc)
		ReleaseDC(0i64, hdcSrc);
	DeleteDC(hDC);
	return DeleteObject(hBitMap);
}

char __fastcall IsWindowed(HWND hWnd)
{
	HWND hParentWnd; // rbx
	RECT rcSrc1; // [rsp+20h] [rbp-58h]
	RECT rcSrc2; // [rsp+30h] [rbp-48h]
	struct tagRECT Rect; // [rsp+40h] [rbp-38h]
	struct tagRECT rcDst; // [rsp+50h] [rbp-28h]

	if (IsIconic(hWnd))
		return 0;
	GetWindowRect(hWnd, &Rect);
	GetClientRect(hWnd, &rcSrc1);
	ClientToScreen(hWnd, (LPPOINT)&rcSrc1);
	ClientToScreen(hWnd, (LPPOINT)&rcSrc1.right);
	hParentWnd = GetWindow(hWnd, 3u);             // GW_HWNDPREV
	if (hParentWnd)
	{
		while (!IsWindowVisible(hParentWnd)
			|| !GetWindowRect(hParentWnd, &rcSrc2)
			|| !IntersectRect(&rcDst, &rcSrc1, &rcSrc2))
		{
			hParentWnd = GetWindow(hParentWnd, 3u);
			if (!hParentWnd)
				return 1;
		}
		return 0;
	}
	return 1;
}

void __fastcall fb::FairfightScreenshot::takeScreenshotLayered(__int64 pFairFight, Xite ? Message * ScreenshotMessage)
{
	__int64 Fairfight; // r13
	HWND Window; // rsi
	unsigned __int32 ScreenSizeX; // ebp
	unsigned __int32 ScreenSizeY; // er14
	unsigned int MaxScreenshotBufferSize; // edi
	__int64 pOutScreenshotBuffer; // rax
	__int64 pScreenshotBuffer; // rbx
	__int64 DxRenderer; // rcx
	char bIsFullscreen; // al
	char IsBGR; // r12
	unsigned int ScreenshotSizeX; // esi
	unsigned int ScreenshotSizeY; // eax
	bool bScreenshotWidthBiggerThenScreen; // cf
	unsigned int _ScreenshotSizeY; // edi
	__int64 ScreenshotBuffer; // rdi
	unsigned int ScreenshotBufferSize; // eax
	struct tagRECT Rect; // [rsp+50h] [rbp-178h]
	MemoryOutBuffer MemoryOutBuffer; // [rsp+60h] [rbp-168h]

	pScreenshotMessage = (Xite ? Message*)ScreenshotMessage;
	Fairfight = pFairFight;
	Window = (HWND)GetGameWindow(0);
	GetClientRect(Window, &Rect);
	ScreenSizeX = Rect.right - Rect.left;
	ScreenSizeY = Rect.bottom - Rect.top;
	MaxScreenshotBufferSize = 4 * (Rect.right - Rect.left) * (Rect.bottom - Rect.top);
	LODWORD(pOutScreenshotBuffer) = operator new[](
		(__int64)&qword_1429236F0,
		(unsigned int)(4 * (Rect.right - Rect.left) * (Rect.bottom - Rect.top)),
		1i64,
		0);
	pScreenshotBuffer = pOutScreenshotBuffer;
	DxRenderer = fb::g_dxRenderer;
	if (fb::g_dxRenderer)
	{
		if (*((_BYTE*)fb::DxRenderer::DxDisplaySettings(fb::g_dxRenderer) + 0x76))// DxDisplaySettings->m_Fullscreen
		{
		LABEL_6:
			bIsFullscreen = 1;
			DxRenderer = fb::g_dxRenderer;
			goto LABEL_9;
		}
		DxRenderer = fb::g_dxRenderer;
	}
	if (g_GameRenderer)
	{
		if (*((_BYTE*)fb::GameRenderer::GameRenderSettings(g_GameRenderer) + 0x52))// GameRenderSettings->m_Fullscreen
			goto LABEL_6;
		DxRenderer = fb::g_dxRenderer;
	}
	bIsFullscreen = 0;
LABEL_9:
	IsBGR = 1;
	if (bIsFullscreen)
	{
		if (DxRenderer && fb::DxRenderer::takeScreenshot(DxRenderer, (void*)pScreenshotBuffer, MaxScreenshotBufferSize))
		{
			IsBGR = 0;                                // so its RGB ordererd
			goto LABEL_15;
		}
	}
	else if (IsWindowed(Window))
	{
		BitBltScreenshot(Window, (void*)pScreenshotBuffer, MaxScreenshotBufferSize, ScreenSizeX, ScreenSizeY);// 
													// typedef struct tagRGBQUAD {
													//         BYTE    rgbBlue;
													//         BYTE    rgbGreen;
													//         BYTE    rgbRed;
													//         BYTE    rgbReserved;
													// } RGBQUAD;
	LABEL_15:
		ScreenshotSizeX = pScreenshotMessage->ScreenshotSizeX;
		ScreenshotSizeY = pScreenshotMessage->ScreenshotSizeY;
		if (pScreenshotMessage->bAdjustSizeWithAspectRatio)
			ScreenshotSizeY = (signed int)(float)((float)(signed int)ScreenshotSizeX
				* (float)((float)(signed int)ScreenSizeY / (float)(signed int)ScreenSizeX));
		bScreenshotWidthBiggerThenScreen = ScreenSizeX < ScreenshotSizeX;
		if (ScreenSizeX <= ScreenshotSizeX)
		{
			if (ScreenSizeY <= ScreenshotSizeY)
			{
			LABEL_25:
				fb::MemoryOutBuffer::MemoryOutBuffer(&MemoryOutBuffer, (__int64)&qword_1429236F0, MaxScreenshotBufferSize, 0);
				fb::FairFightScreenshot::CompressJPG(pScreenshotBuffer, MaxScreenshotBufferSize, ScreenSizeX, ScreenSizeY, &MemoryOutBuffer, pScreenshotMessage->ScreenshotFormat, IsBGR); // << using https://github.com/LuaDist/libjpeg
				ScreenshotBuffer = MemoryOutBuffer.m_start;
				ScreenshotBufferSize = fb::MemoryInBuffer::getPosition(&MemoryOutBuffer);
				fb::FairFightScreenshot::screenshotDone(Fairfight, ScreenshotBuffer, ScreenshotBufferSize);
				fb::MemoryOutBuffer::~MemoryOutBuffer((__int64)&MemoryOutBuffer);
				goto LABEL_26;
			}
			bScreenshotWidthBiggerThenScreen = ScreenSizeX < ScreenshotSizeX;
		}
		if (bScreenshotWidthBiggerThenScreen)
			ScreenshotSizeX = ScreenSizeX;
		_ScreenshotSizeY = ScreenshotSizeY;
		if (ScreenSizeY < ScreenshotSizeY)
			_ScreenshotSizeY = ScreenSizeY;
		ShrinkToScreenSize(pScreenshotBuffer, ScreenSizeX, ScreenSizeY, ScreenshotSizeX, _ScreenshotSizeY);// in case screenshot is bigger then the screen
		ScreenSizeX = ScreenshotSizeX;
		ScreenSizeY = _ScreenshotSizeY;
		MaxScreenshotBufferSize = 4 * ScreenshotSizeX * _ScreenshotSizeY;
		goto LABEL_25;
	}
LABEL_26:
	operator delete(pScreenshotBuffer);
}

// https://github.com/LuaDist/libjpeg/blob/6c0fcb8ddee365e7abc4d332662b06900612e923/example.c#L72
void __fastcall fb::FairFightScreenshot::CompressJPG(__int64 ScreenshotBuffer, __int64 MaxScreenshotBufferSize, unsigned int ScreenSizeX, unsigned int ScreenSizeY, MemoryOutBuffer* pMemoryOutBuffer, int ScreenshotFormat, char IsBGR)
{
	unsigned int dwScreenSizeY; // esi
	__int64 dwScreenSizeX; // r15
	__int64 pScreenshotBuffer; // r14
	__int64 v10; // rax
	__int64 v11; // rdi
	signed int Quality; // edx
	__int64 v13; // rax
	__int64 v14; // rbx
	unsigned int v15; // ecx
	unsigned int i; // eax
	int v17; // eax
	int v18; // eax
	__int64 v19; // r8
	__int64 v20; // rax
	__int64 v21; // r9
	__int64 row_pointer; // [rsp+20h] [rbp-318h]
	__int64 v23; // [rsp+28h] [rbp-310h]
	int v24; // [rsp+30h] [rbp-308h]
	MemoryOutBuffer* MemoryOutBuffer; // [rsp+38h] [rbp-300h]
	__int64 v26; // [rsp+40h] [rbp-2F8h]
	__int64 v27; // [rsp+48h] [rbp-2F0h]
	__int64 cinfo; // [rsp+50h] [rbp-2E8h]
	__int64* v29; // [rsp+68h] [rbp-2D0h]
	__int64 v30; // [rsp+78h] [rbp-2C0h]
	int v31; // [rsp+80h] [rbp-2B8h]
	unsigned int v32; // [rsp+84h] [rbp-2B4h]
	int v33; // [rsp+88h] [rbp-2B0h]
	int v34; // [rsp+8Ch] [rbp-2ACh]
	unsigned int v35; // [rsp+170h] [rbp-1C8h]
	__int64 v36; // [rsp+250h] [rbp-E8h]

	v27 = -2i64;
	dwScreenSizeY = ScreenSizeY;
	dwScreenSizeX = ScreenSizeX;
	pScreenshotBuffer = ScreenshotBuffer;
	v24 = 204800;
	LODWORD(v10) = operator new[]((__int64)&qword_1429236F0, 0x32000i64, 1i64, 0);
	v11 = v10;
	v26 = v10;
	v23 = v10;
	MemoryOutBuffer = pMemoryOutBuffer;
	memset(&cinfo, 0, 0x1F8ui64);                 // jpeg_compress_struct
	v29 = &v23;
	cinfo = sub_140F6B8E0((__int64)&v36);
	sub_140F6B9D0(&cinfo, 62, 504i64);
	sub_140F6BD30((__int64)&cinfo, 0i64);
	*(_QWORD*)(v30 + 16) = sub_1402C1270;
	*(_QWORD*)(v30 + 24) = sub_1402C1220;
	*(_QWORD*)(v30 + 32) = sub_140F00440;
	v31 = dwScreenSizeX;
	v32 = dwScreenSizeY;
	v33 = 3;
	v34 = 2;
	jpeg_set_defaults((jpeg_compress_struct*)&cinfo);
	Quality = 20;
	if (ScreenshotFormat != 3)                  // ScreenshotFormat_Jpeg
		Quality = 100;
	jpeg_set_quality((__int64)&cinfo, Quality, 1);
	jpeg_start_compress(&cinfo, 1);
	LODWORD(v13) = operator new[]((__int64)&qword_1429236F0, (unsigned int)(3 * dwScreenSizeX), 1i64, 0);
	v14 = v13;
	row_pointer = v13;
	v15 = v35;
	for (i = v32; v35 < v32; i = v32)
	{
		if (IsBGR)
			v17 = i - v15;
		else
			v17 = v15;
		v18 = 4 * dwScreenSizeX * v17;
		if ((_DWORD)dwScreenSizeX)
		{
			v19 = (unsigned int)(v18 + 2);
			v20 = v14 + 2;
			v21 = dwScreenSizeX;
			do
			{
				if (IsBGR)
				{
					*(_BYTE*)(v20 - 2) = *(_BYTE*)(v19 + pScreenshotBuffer);// v20->rgbRed = v19->Red
					*(_BYTE*)(v20 - 1) = *(_BYTE*)((unsigned int)(v19 - 1) + pScreenshotBuffer);// v20->rgbGreen = v19->Green
					*(_BYTE*)v20 = *(_BYTE*)((unsigned int)(v19 - 2) + pScreenshotBuffer);// v20->rgbBlue = v19->Blue
				}
				else
				{
					*(_BYTE*)(v20 - 2) = *(_BYTE*)((unsigned int)(v19 - 2) + pScreenshotBuffer);// v20->rgbRed = v19->Red
					*(_BYTE*)(v20 - 1) = *(_BYTE*)((unsigned int)(v19 - 1) + pScreenshotBuffer);// v20->rgbGreen = v19->Green
					*(_BYTE*)v20 = *(_BYTE*)(v19 + pScreenshotBuffer);// v20->rgbBlue = v19->Blue
				}
				v19 = (unsigned int)(v19 + 4);
				v20 += 3i64;
				--v21;
			} while (v21);
		}
		jpeg_write_scanlines((__int64)&cinfo, (__int64)&row_pointer, 1u);
		v15 = v35;
	}
	jpeg_finish_compress((__int64)&cinfo);
	jpeg_destroy_compress((__int64)&cinfo);
	operator delete(v14);
	operator delete(v11);
}
bool __fastcall fb::DxRenderer::takeScreenshot(__int64 pDxRenderer, void* pScreenshotBuffer, int MaxScreenshotBufferSize)
{
	unsigned int v3; // er15
	void* v4; // r12
	__int64 DxRenderer; // r14
	int v6; // esi
	__int64 pDxTexture; // rax
	__int64 DxTexture; // rbx
	__int64 v10; // [rsp+38h] [rbp-98h]
	__int64 pSrcResource; // [rsp+40h] [rbp-90h]
	void* RenderMapInfoOut; // [rsp+48h] [rbp-88h]
	__int64 v13; // [rsp+58h] [rbp-78h]
	__int64 DxTextureCreateDesc; // [rsp+60h] [rbp-70h]
	char v15; // [rsp+78h] [rbp-58h]
	__int64 v16; // [rsp+110h] [rbp+40h]
	__int64 v17; // [rsp+120h] [rbp+50h]
	int v18; // [rsp+128h] [rbp+58h]
	int v19; // [rsp+12Ch] [rbp+5Ch]
	__int64 v20; // [rsp+130h] [rbp+60h]
	__int64 v21; // [rsp+138h] [rbp+68h]
	char v22; // [rsp+140h] [rbp+70h]
	__int64 v23; // [rsp+144h] [rbp+74h]
	__int128 v24; // [rsp+150h] [rbp+80h]
	__int64 v25; // [rsp+160h] [rbp+90h]
	__int64 v26; // [rsp+170h] [rbp+A0h]
	int v27; // [rsp+178h] [rbp+A8h]
	__int128 v28; // [rsp+180h] [rbp+B0h]
	__int64 v29; // [rsp+190h] [rbp+C0h]
	char v30; // [rsp+1A8h] [rbp+D8h]

	v13 = -2i64;
	v3 = MaxScreenshotBufferSize;
	v4 = pScreenshotBuffer;                       // https://[censored]/threads/d3d11-vtable-indexes.3813/
	DxRenderer = pDxRenderer;                     // DxRenderer->Screen->IDXGISwapChain->GetBuffer
	v6 = (*(__int64(__fastcall**)(_QWORD, __int64*))(**(_QWORD**)(*(_QWORD*)(pDxRenderer + 0x38) + 0xF0i64) + 0x78i64))(
		*(_QWORD*)(*(_QWORD*)(pDxRenderer + 0x38) + 0xF0i64),
		&v10);
	if (!v6)
	{
		fb::StringBuilderBaseImpl<char>::initWorkaround((__int64)&DxTextureCreateDesc, &v15, 0xC8u);
		v16 = 0i64;
		v17 = 0i64;
		v18 = 1;
		v19 = 1;
		v20 = 1i64;
		v21 = 11i64;
		v22 = 0;
		v23 = 8i64;
		_mm_store_si128((__m128i*) & v24, (__m128i)0i64);
		v25 = 0i64;
		v26 = 0i64;
		v27 = 0;
		_mm_store_si128((__m128i*) & v28, (__m128i)0i64);
		fb::StringBuilderBaseImpl<char>::initWorkaround((__int64)&v29, &v30, 0xC8u);
		fb::StringBuilderBaseImpl<char>::append((__int64)&v29, "Renderer/Screenshot");
		fb::StringBuilderBaseImpl<char>::reset(&DxTextureCreateDesc);
		fb::StringBuilderBaseImpl<char>::append((__int64)&DxTextureCreateDesc, (char*)(v29 & 0xFFFFFFFFFFFFFFFCui64));
		sub_140748160(&v29);
		HIDWORD(v20) = 0;
		v16 = 0x100000003i64;
		v17 = *(_QWORD*)(*(_QWORD*)(DxRenderer + 0x38) + 0x58i64);
		LODWORD(v23) = 0;
		LODWORD(v21) = 11;
		pDxTexture = fb::DxTexture::create(*(_QWORD*)(DxRenderer + 8), (__int64)&DxTextureCreateDesc);
		DxTexture = pDxTexture;
		if (pDxTexture)
			(*(void(__fastcall**)(__int64))(*(_QWORD*)pDxTexture + 8i64))(pDxTexture);
		(***(void(__fastcall****)(_QWORD, void*, __int64*))(DxTexture + 0x178))(// ID3D11DeviceContext->CopyResource
			*(_QWORD*)(DxTexture + 0x178),
			&pDstResource,
			&pSrcResource);
		v6 = (*(__int64(__fastcall**)(__int64, __int64))(*(_QWORD*)v10 + 0x88i64))(v10, pSrcResource);
		if (!v6)
		{
			fb::DxRenderer::map((fb*)DxRenderer, DxTexture, 0i64, 1i64, (__int64)&RenderMapInfoOut);//
													  // 
			if (4
				* *(_DWORD*)(*(_QWORD*)(DxRenderer + 0x38) + 0x58i64)
				* *(_DWORD*)(*(_QWORD*)(DxRenderer + 0x38) + 0x5Ci64) < v3)
				v3 = 4
				* *(_DWORD*)(*(_QWORD*)(DxRenderer + 0x38) + 0x58i64)
				* *(_DWORD*)(*(_QWORD*)(DxRenderer + 0x38) + 0x5Ci64);
			memcpy(v4, RenderMapInfoOut, v3);
			fb::DxRenderer::unmap(DxRenderer, (_QWORD*)DxTexture, 0);
		}
		(*(void(__fastcall**)(__int64))(*(_QWORD*)DxTexture + 16i64))(DxTexture);
		sub_140748160(&DxTextureCreateDesc);
	}
	return v6 == 0;
}

*/