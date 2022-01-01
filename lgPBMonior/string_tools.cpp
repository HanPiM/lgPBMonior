#include <stdio.h>
#include <chrono>
#include <random>
#include <ctype.h>

#include "string_tools.h"

static std::mt19937 _mtengine((unsigned int)std::chrono::steady_clock::now().time_since_epoch().count());
static size_t _mt19937rand(size_t l, size_t r)
{
	return std::uniform_int_distribution<size_t>(l, r)(_mtengine);
}

namespace string_tools{

int format_string(_Out_ std::string& out_str, _In_ const char* format, _In_ va_list in_args)
{
	va_list args;
	va_copy(args, in_args);
	size_t need = vsnprintf(NULL, 0, format, args);
	va_end(args);
	out_str.resize(need);
	return vsnprintf((char*)out_str.data(), need + 1, format, in_args);
}
int format_string(_Out_ std::wstring& out_str, _In_ const wchar_t* format, _In_ va_list in_args)
{
	va_list args;
	va_copy(args, in_args);
	size_t need = vswprintf(NULL, 0, format, args);
	va_end(args);
	out_str.resize(need);
	return vswprintf((wchar_t*)out_str.data(), need + 1, format, in_args);
}
std::string format_string(_In_ const char* format, _In_ va_list args)
{
	std::string res;
	format_string(res, format, args);
	return res;
}
std::wstring format_string(_In_ const wchar_t* format, _In_ va_list args)
{
	std::wstring res;
	format_string(res, format, args);
	return res;
}

int format_string(_Out_ std::string& str, _In_ const char* format, ...)
{
	va_list args;
	va_start(args, format);
	int r = format_string(str, format, args);
	va_end(args);
	return r;
}
int format_string(_Out_ std::wstring& str, _In_ const wchar_t* format, ...)
{
	va_list args;
	va_start(args, format);
	int r = format_string(str, format, args);
	va_end(args);
	return r;
}
std::string format_string(_In_ const char* format, ...)
{
	va_list args;
	std::string res;
	va_start(args, format);
	format_string(res, format, args);
	va_end(args);
	return res;
}
std::wstring format_string(_In_ const wchar_t* format, ...)
{
	va_list args;
	std::wstring res;
	va_start(args, format);
	format_string(res, format, args);
	va_end(args);
	return res;
}

std::string random_string(_In_ size_t len, _In_opt_ const std::string& table)
{
	std::string res;
	res.reserve(len);
	for (size_t i = 0; i < len; ++i)
		res += table[_mt19937rand(0, table.size() - 1)];
	return res;
}

}