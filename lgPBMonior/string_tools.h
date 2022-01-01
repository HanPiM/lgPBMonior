#pragma once

#include <stdarg.h>
#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>

#ifndef _In_
#define _In_ 
#endif
#ifndef _Out_
#define _Out_ 
#endif

namespace string_tools
{
	int format_string(_Out_ std::string& str, _In_ const char* format, _In_ va_list args);
	int format_string(_Out_ std::string& str, _In_ const char* format, ...);
	int format_string(_Out_ std::wstring& str, _In_ const wchar_t* format, _In_ va_list args);
	int format_string(_Out_ std::wstring& str, _In_ const wchar_t* format, ...);

	std::string format_string(_In_ const char* format, _In_ va_list args);
	std::string format_string(_In_ const char* format, ...);
	std::wstring format_string(_In_ const wchar_t* format, _In_ va_list args);
	std::wstring format_string(_In_ const wchar_t* format, ...);

	std::string random_string(_In_ size_t len,
		_In_opt_ const std::string& table = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_"
	);
};
