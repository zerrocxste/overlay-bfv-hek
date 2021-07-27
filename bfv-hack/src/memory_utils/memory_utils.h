//memory utils
//author: zerrocxste

namespace memory_utils
{
#ifdef _WIN64
#define DWORD_OF_BITNESS DWORD64
#define PTRMAXVAL ((PVOID)0x000F000000000000)
#elif _WIN32
#define DWORD_OF_BITNESS DWORD
#define PTRMAXVAL ((PVOID)0xFFF00000)
#endif

	extern bool is_valid_ptr(PVOID ptr);

	extern HMODULE get_base();

	extern DWORD_OF_BITNESS get_base_address();

	extern DWORD_OF_BITNESS get_module_size(DWORD_OF_BITNESS address);

	extern DWORD_OF_BITNESS find_pattern(HMODULE module, const char* pattern, const char* mask, DWORD scan_speed = 0x1);

	extern DWORD_OF_BITNESS find_pattern_in_heap(const char* pattern, const char* mask, DWORD scan_speed = 0x1);

	extern DWORD_OF_BITNESS find_pattern_in_memory(const char* pattern, const char* mask, DWORD_OF_BITNESS base, DWORD_OF_BITNESS size, DWORD scan_speed = 0x1);

	extern std::vector<DWORD_OF_BITNESS> find_pattern_in_memory_array(const char* pattern, const char* mask, DWORD_OF_BITNESS base, DWORD_OF_BITNESS size, DWORD scan_speed = 0x1);

	extern std::vector<DWORD_OF_BITNESS> find_pattern_in_heap_array(const char* pattern, const char* mask, DWORD scan_speed = 0x1);

	template<class T>
	T read_pointer(std::vector<DWORD_OF_BITNESS> list)
	{
		if (list.empty())
			return NULL;

		DWORD_OF_BITNESS ptr = list[0];

		for (int i = 0; i < list.size(); i++)
		{
			if (!is_valid_ptr((LPVOID)ptr))
				break;

			if (i < (int)(list.size() - 2))
				ptr = *(DWORD_OF_BITNESS*)(ptr + list[i + 1]);
			else
				return list.size() == 1 ? (T)ptr : (T)(ptr + list.back());
		}

		return NULL;
	}

	template<class T>
	T read_value(std::vector<DWORD_OF_BITNESS> list)
	{
		T* ptr = read_pointer<T*>(list);

		if (ptr == NULL)
			return T();

		return *(T*)ptr;
	}

	template<class T>
	bool write(std::vector<DWORD_OF_BITNESS> list, T my_value)
	{
		T* ptr = read_pointer<T*>(list);

		if (ptr == NULL)
			return false;

		*ptr = my_value;

		return true;
	}

	extern char* read_string(std::vector<DWORD_OF_BITNESS> list);
	extern wchar_t* read_wstring(std::vector<DWORD_OF_BITNESS> list);

	extern bool write_string(std::vector<DWORD_OF_BITNESS> list, char* my_value);

	extern void patch_instruction(void* instruction_address, void* instruction_bytes, int sizeof_instruction_byte);
	extern void fill_memory_region(void* instruction_address, int byte, int sizeof_instruction_byte);
}