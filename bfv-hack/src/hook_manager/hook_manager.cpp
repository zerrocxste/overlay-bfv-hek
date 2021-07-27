#include "../includes.h"

void hide_window()
{
	global_vars::stop_render = true;
	DXWFSetWindowVisibleState(false);
	Sleep(100);
}

void show_window()
{
	DXWFSetWindowVisibleState(true);
	global_vars::stop_render = false;
}

void timer_thread()
{
	Sleep(10000);
	show_window();
}

bool HDCToFile(const char* FilePath, HDC Context, RECT Area, bool save_in_file, uint16_t BitsPerPixel = 24)
{
	uint32_t Width = Area.right - Area.left;
	uint32_t Height = Area.bottom - Area.top;
	BITMAPINFO Info;
	BITMAPFILEHEADER Header;
	memset(&Info, 0, sizeof(Info));
	memset(&Header, 0, sizeof(Header));
	Info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	Info.bmiHeader.biWidth = Width;
	Info.bmiHeader.biHeight = Height;
	Info.bmiHeader.biPlanes = 1;
	Info.bmiHeader.biBitCount = BitsPerPixel;
	Info.bmiHeader.biCompression = BI_RGB;
	Info.bmiHeader.biSizeImage = Width * Height * (BitsPerPixel > 24 ? 4 : 3);
	Header.bfType = 0x4D42;
	Header.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	char* Pixels = NULL;
	HDC MemDC = CreateCompatibleDC(Context);
	HBITMAP Section = CreateDIBSection(Context, &Info, DIB_RGB_COLORS, (void**)&Pixels, 0, 0);
	DeleteObject(SelectObject(MemDC, Section));
	BOOL ReturnValue = TRUE;
	ReturnValue = BitBlt(MemDC, 0, 0, Width, Height, Context, Area.left, Area.top, SRCCOPY);
	DeleteDC(MemDC);
	if (save_in_file)
	{
		std::fstream hFile(FilePath, std::ios::out | std::ios::binary);
		if (hFile.is_open())
		{
			hFile.write((char*)&Header, sizeof(Header));
			hFile.write((char*)&Info.bmiHeader, sizeof(Info.bmiHeader));
			hFile.write(Pixels, (((BitsPerPixel * Width + 31) & ~31) / 8) * Height);
			hFile.close();
			DeleteObject(Section);
			return true;
		}
		DeleteObject(Section);
	}
	return false;
}

using fsub1420C7C90 = void(__fastcall*)(__int64);
fsub1420C7C90 pfsub1420C7C90 = NULL;

void __fastcall sub1420C7C90_hooked(__int64 a1) //a1 cse structure
{
	features::cse_list_grabber((DWORD_PTR)(a1 - 0x338));

	pfsub1420C7C90(a1);
}

using fsub140970280 = __int64(__fastcall*)(__int64, __int64, __int64);
fsub140970280 pfsub140970280 = NULL;

__int64 __fastcall sub140970280_hooked(__int64 a1, __int64 a2, __int64 a3) //a1 local cse pointer
{
	features::local_cse_grabber((DWORD_PTR)(a1 + 0x110));

	return pfsub140970280(a1, a2, a3);
}

using fsub_141BE97A0 = __int64(__fastcall*)(__int64);
fsub_141BE97A0 pfsub_141BE97A0 = NULL;

//#1 execute screenshot shit
__int64 __fastcall sub141BE97A0_hooked(__int64 a1)
{
	hide_window();
	printf("called screenshot func, a1: %I64X\n", a1);
	auto ret = pfsub_141BE97A0(a1);
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)timer_thread, NULL, NULL, NULL);
	return ret;
}

using fsub_1409790D0 = signed __int64(__fastcall*)(__int64, __int64, _int128*, _int128*);
fsub_1409790D0 pfsub_1409790D0 = NULL;

//#1 call screenshot shit
signed __int64 __fastcall sub_1409790D0_hooked(__int64 a1, __int64 a2, _int128* a3, _int128* a4)
{
	hide_window();
	printf("CALLED SCREENSHOT SHIT DUDE\n");
	auto ret = pfsub_1409790D0(a1, a2, a3, a4);
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)timer_thread, NULL, NULL, NULL);
	return ret;
}

using fsub_140977A80 = __int64(__fastcall*)(DWORD64, DWORD64);
fsub_140977A80 pfsub_140977A80 = NULL;

//#2 execute screenshot shit
__int64 __fastcall sub_140977A80_hooked(DWORD64 a1, DWORD64 a2)
{
	hide_window();
	printf("MAKED SCREENSHOT DUDE\n");
	auto ret = pfsub_140977A80(a1, a2);
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)timer_thread, NULL, NULL, NULL);
	return ret;
}

using fsub_1466B5490 = __int64(__fastcall*)(__int64, int, unsigned int, unsigned int, unsigned int);
fsub_1466B5490 pfsub_1466B5490 = NULL;

//this called by various SS func
__int64 __fastcall sub_1466B5490_hooked(__int64 a1, int a2, unsigned int a3, unsigned int a4, unsigned int a5)
{
	printf("CALLED VARIOUS FUNC FOR SS\n");
	return pfsub_1466B5490(a1, a2, a3, a4, a5);
}

using fsub_141BF2F80 = __int64(__fastcall*)(__int64, __int64, unsigned int, int);
fsub_141BF2F80 pfsub_141BF2F80 = NULL;

//a1 = ffss shit maybe
__int64 __fastcall sub_141BF2F80_hooked(__int64 a1, __int64 a2, unsigned int a3, int a4)
{
	printf("CALLED FUNC WITH a1 FFSS\n");
	return pfsub_141BF2F80(a1, a2, a3, a4);
}

using fAcquireNextFrame = HRESULT(__fastcall*)(IDXGIOutputDuplication*, unsigned int, struct DXGI_OUTDUPL_FRAME_INFO*, struct IDXGIResource**);
fAcquireNextFrame pfAcquireNextFrame = NULL;

HRESULT __fastcall /*CDXGIOutputDuplication::*/AcquireNextFrame_hooked(IDXGIOutputDuplication* _this, unsigned int TimeoutInMilliseconds, struct DXGI_OUTDUPL_FRAME_INFO* pFrameInfo, struct IDXGIResource** ppDesktopResource)
{
	printf("%s called! invoke by: %p\n", __FUNCSIG__, _ReturnAddress());
	return pfAcquireNextFrame(_this, TimeoutInMilliseconds, pFrameInfo, ppDesktopResource);
}

void* find_sub1420C7C90()
{
	/*
		Address of signature = bfv.exe + 0x020C7C90
		"\x48\x89\x00\x00\x00\x57\x48\x83\xEC\x00\x48\x8B\x00\x00\x48\x8B\x00\x48\x8B\x00\x48\x8B\x00\xFF\x90", "xx???xxxx?xx??xx?xx?xx?xx"
		"48 89 ? ? ? 57 48 83 EC ? 48 8B ? ? 48 8B ? 48 8B ? 48 8B ? FF 90"
	*/

	static void* address = (void*)0x1420C7C90;
	
	return address;
}

void* find_sub140970280()
{
	/*
		Address of signature = bfv.exe + 0x00970280
		"\x40\x00\x57\x41\x00\x48\x83\xEC\x00\x48\xC7\x44\x24\x28\x00\x00\x00\x00\x48\x89\x00\x00\x00\x48\x89\x00\x00\x00\x48\x8B\x00\x4C\x8B", "x?xx?xxx?xxxxx????xx???xx???xx?xx"
		"40 ? 57 41 ? 48 83 EC ? 48 C7 44 24 28 ? ? ? ? 48 89 ? ? ? 48 89 ? ? ? 48 8B ? 4C 8B"
	*/

	static void* address = (void*)0x140970280;

	return address;
}

void* find_sub141BE97A0()
{
	static void* address = (void*)0x141BE97A0;

	return address;
}

void* find_sub1409790D0()
{
	static void* address = (void*)0x1409790D0;

	return address;
}

void* find_sub140977A80()
{
	static void* address = (void*)0x140977A80;

	return address;
}

void* find_AcquireNextFrame()
{
	/*
		Address of signature = dxgi.dll + 0x00048D00
		"\x40\x00\x56\x57\x41\x00\x41\x00\x41\x00\x41\x00\x48\x81\xEC\x00\x00\x00\x00\x48\xC7\x84\x24\xA8\x00\x00\x00", "x?xxx?x?x?x?xxx????xxxxxxxx"
		"40 ? 56 57 41 ? 41 ? 41 ? 41 ? 48 81 EC ? ? ? ? 48 C7 84 24 A8 00 00 00"
	*/

	static void* address = (void*)memory_utils::find_pattern(GetModuleHandle("dxgi.dll"), 
		"\x40\x00\x56\x57\x41\x00\x41\x00\x41\x00\x41\x00\x48\x81\xEC\x00\x00\x00\x00\x48\xC7\x84\x24\xA8\x00\x00\x00", "x?xxx?x?x?x?xxx????xxxxxxxx");

	return address;
}

void install_hook_sub1420C7C90()
{
	MH_CreateHook(find_sub1420C7C90(), &sub1420C7C90_hooked, (LPVOID*)&pfsub1420C7C90);
	MH_EnableHook(find_sub1420C7C90());
}

void install_hook_sub140970280()
{
	MH_CreateHook(find_sub140970280(), &sub140970280_hooked, (LPVOID*)&pfsub140970280);
	MH_EnableHook(find_sub140970280());
}

void install_hook_sub141BE97A0()
{
	MH_CreateHook(find_sub141BE97A0(), &sub141BE97A0_hooked, (LPVOID*)&pfsub_141BE97A0);
	MH_EnableHook(find_sub141BE97A0());
}

void install_hook_sub1409790D0()
{
	MH_CreateHook(find_sub1409790D0(), &sub_1409790D0_hooked, (LPVOID*)&pfsub_1409790D0);
	MH_EnableHook(find_sub1409790D0());
}

void install_hook_sub140977A80()
{
	MH_CreateHook(find_sub140977A80(), &sub_140977A80_hooked, (LPVOID*)&pfsub_140977A80);
	MH_EnableHook(find_sub140977A80());
}

void hook_manager::install_hooks()
{
	MH_Initialize();
	
	install_hook_sub1420C7C90();
	install_hook_sub140970280();

	install_hook_sub141BE97A0();
	install_hook_sub1409790D0();
	install_hook_sub140977A80();

	MH_CreateHook((void*)0x1466B5490, &sub_1466B5490_hooked, (LPVOID*)&pfsub_1466B5490);
	MH_EnableHook((void*)0x1466B5490);

	MH_CreateHook((void*)0x141BF2F80, &sub_141BF2F80_hooked, (LPVOID*)&pfsub_141BF2F80);
	MH_EnableHook((void*)0x141BF2F80);

	MH_CreateHook(find_AcquireNextFrame(), &AcquireNextFrame_hooked, (LPVOID*)&pfAcquireNextFrame);
	MH_EnableHook(find_AcquireNextFrame());

	/*
		present
		Address of signature = dxgi.dll + 0x000059E0
		"\x48\x89\x00\x00\x00\x48\x89\x00\x00\x00\x55\x57\x41\x00\x48\x8D\x00\x00\x00\x48\x81\xEC\x00\x00\x00\x00\x48\x8B\x00\x00\x00\x00\x00\x48\x33\x00\x48\x89\x00\x00\x45\x33", "xx???xx???xxx?xx???xxx????xx?????xx?xx??xx"
		"48 89 ? ? ? 48 89 ? ? ? 55 57 41 ? 48 8D ? ? ? 48 81 EC ? ? ? ? 48 8B ? ? ? ? ? 48 33 ? 48 89 ? ? 45 33"

		resize buffers
		Address of signature = dxgi.dll + 0x000211E0
		"\x48\x8B\x00\x55\x41\x00\x41\x00\x41\x00\x41\x00\x48\x8D\x00\x00\x48\x81\xEC\x00\x00\x00\x00\x48\xC7\x45\x1F", "xx?xx?x?x?x?xx??xxx????xxxx"
		"48 8B ? 55 41 ? 41 ? 41 ? 41 ? 48 8D ? ? 48 81 EC ? ? ? ? 48 C7 45 1F"
	*/
}

void shutdown_hook_sub1420C7C90()
{
	MH_DisableHook(find_sub1420C7C90());
	MH_RemoveHook(find_sub1420C7C90());
	Sleep(100);
}

void shutdown_hook_sub140970280()
{
	MH_DisableHook(find_sub140970280());
	MH_RemoveHook(find_sub140970280());
	Sleep(100);
}

void hook_manager::shutdown_hooks()
{
	shutdown_hook_sub1420C7C90();
	shutdown_hook_sub140970280();

	MH_Uninitialize();
	Sleep(100);
}

void print_console_log(const char* func_name)
{
	time_t rawtime;
	struct tm* timeinfo;
	char buffer[80];

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer, 80, "%H:%M:%S", timeinfo);

	printf("[%s] %s\n", buffer, func_name);
}

//sub_141FB9080


/* WARNING: Could not reconcile some variable overlaps */
/*
* void UndefinedFunction_141be97a0(longlong param_1)

{
	int iVar1;
	bool bVar2;
	char cVar3;
	longlong lVar4;
	undefined8 uVar5;
	undefined8 uVar6;
	code** ppcVar7;
	uint uVar8;
	int iVar9;
	uint uVar10;
	code** ppcVar11;
	undefined auStack216[32];
	undefined8 uStack184;
	undefined8* puStack176;
	undefined4 uStack168;
	undefined4 uStack160;
	undefined4 uStack152;
	longlong lStack136;
	undefined8 uStack128;
	undefined8 uStack120;
	undefined8 uStack112;
	undefined8 uStack104;
	undefined8 uStack96;
	undefined8 uStack88;
	undefined8 uStack80;
	undefined4 uStack72;
	int iStack64;
	int iStack60;
	int iStack56;
	int iStack52;
	ulonglong uStack48;

	uStack120 = 0xfffffffffffffffe;
	uStack48 = DAT_144399318 ^ (ulonglong)auStack216;
	if ((DAT_1447f7a18 == (longlong*)0x0) ||
		(lVar4 = (**(code**)(*DAT_1447f7a18 + 0x1b0))(), *(char*)(lVar4 + 0xaf) == '\0')) {
		bVar2 = false;
	}
	else {
		bVar2 = true;
	}
	lVar4 = thunk_FUN_146055c90();
	ppcVar7 = (code**)0x0;
	ppcVar11 = ppcVar7;
	if (lVar4 != 0) {
		ppcVar11 = *(code***)(lVar4 + 0x140);
	}
	if ((bVar2) || (ppcVar11 != (code**)0x0)) {
		(*(code*)0x7ff8ff082d30)();
		uVar10 = iStack56 - iStack64;
		*(uint*)(param_1 + 0x34) = uVar10;
		uVar8 = iStack52 - iStack60;
		*(uint*)(param_1 + 0x38) = uVar8;
		if (*(char*)(param_1 + 0x48) != '\0') {
			*(int*)(param_1 + 0x40) =
				(int)((float)(ulonglong) * (uint*)(param_1 + 0x3c) *
					((float)(ulonglong)uVar8 / (float)(ulonglong)uVar10));
		}
		iVar9 = uVar8 * uVar10 * 4;
		*(int*)(param_1 + 0x30) = iVar9;
		lVar4 = thunk_FUN_145d6ce80(&PTR_PTR_LAB_1443da480, iVar9, 1);
		if (*(longlong*)(param_1 + 0x20) != lVar4) {
			thunk_FUN_140575b80(*(longlong*)(param_1 + 0x20), 1);
			*(longlong*)(param_1 + 0x20) = lVar4;
		}
		if (bVar2) {
			cVar3 = (**(code**)(*DAT_1447f7a18 + 0x1c0))();
			if (cVar3 == '\x01') {
				*(undefined*)(param_1 + 0x3c0) = 1;
			}
		}
		else {
			iVar9 = (*(code*)0x7ff8ff07c1f0)(ppcVar11);
			if (iVar9 == 0) {
				uStack128 = *(undefined8*)(param_1 + 0x20);
				iVar9 = *(int*)(param_1 + 0x38);
				iVar1 = *(int*)(param_1 + 0x34);
				uVar8 = *(uint*)(param_1 + 0x30);
				lVar4 = (*(code*)0x7ff8ff096130)(ppcVar11);
				uVar5 = (*(code*)0x7ff8ffdc3b70)(lVar4);
				uVar6 = (*(code*)0x7ff8ffdc4aa0)(lVar4, iVar1, iVar9);
				lStack136 = (*(code*)0x7ff8ffdc3660)(uVar5, uVar6);
				uStack152 = 0xcc0020;
				uStack160 = 0;
				uStack168 = 0;
				uStack184 = CONCAT44(uStack184._4_4_, iVar9);
				puStack176 = (undefined8*)lVar4;
				(*(code*)0x7ff8ffdc3980)(uVar5, 0, 0, iVar1);
				uStack104 = 0;
				uStack96 = 0;
				uStack88 = 0;
				uStack80 = 0;
				uStack72 = 0;
				uStack112 = 0x28;
				uStack168 = 0;
				puStack176 = &uStack112;
				uStack184 = 0;
				(*(code*)0x7ff8ffdc4560)(uVar5, uVar6, 0, 0);
				if ((uStack104._6_2_ != 0x20) || (uStack96._4_4_ == 0)) {
					uStack96._4_4_ = iVar1 * iVar9 * 4;
					uStack96 = (ulonglong)uStack96._4_4_ << 0x20;
					uStack104 = CONCAT26(0x20, (undefined6)uStack104);
				}
				uStack96 = uStack96 & 0xffffffff00000000;
				if (uStack96._4_4_ <= uVar8) {
					uStack168 = 0;
					puStack176 = &uStack112;
					uStack184 = uStack128;
					(*(code*)0x7ff8ffdc4560)(uVar5, uVar6, 0, uStack104 & 0xffffffff);
				}
				if (lVar4 != 0) {
					(*(code*)0x7ff8ff093b40)(0, lVar4);
				}
				(*(code*)0x7ff8ffdc3660)(uVar5, lStack136);
				(*(code*)0x7ff8ffdc2c70)(uVar5);
				(*(code*)0x7ff8ffdc2130)(uVar6);
				thunk_FUN_1488aa1b0(param_1);
				lStack136 = thunk_FUN_145d6bb10(0x118, &PTR_PTR_LAB_1443da480);
				if (lStack136 != 0) {
					ppcVar7 = (code**)FUN_140582e00(lStack136, &PTR_PTR_LAB_1443da480,
						*(undefined4*)(param_1 + 0x30), 0);
				}
				ppcVar11 = *(code***)(param_1 + 0x28);
				if (ppcVar11 != ppcVar7) {
					if (ppcVar11 != (code**)0x0) {
						(**(code**)*ppcVar11)(ppcVar11, 1);
					}
					*(code***)(param_1 + 0x28) = ppcVar7;
				}
				thunk_FUN_1489b1270(param_1, 1);
			}
		}
	}
	thunk_FUN_1496491b0(uStack48 ^ (ulonglong)auStack216);
	return;
}
*/


