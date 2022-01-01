#include <stdarg.h>

#include "debug.h"
#include "string_tools.h"

#define BUFFER_MAXLEN 1020

logger::logger(_In_ out_type ot, _In_opt_ const std::string& filename)
{
    _fp = NULL;
    switch (ot)
    {
    case logger::out_type::file:
        _fp = fopen(filename.c_str(), "w");
        break;
    case logger::out_type::cerr:
    case logger::out_type::cout:
        break;
    }
    _ot = ot;
}

void logger::get_tm(_Out_ tm& out)
{
    auto now = std::chrono::system_clock::now();
    time_t tt = std::chrono::system_clock::to_time_t(now);
#ifdef _WIN32
    localtime_s(&out, &tt);
#else
    localtime_r(&tt, &out);
#endif
}

void logger::write()
{
    FILE* p = _fp;
    switch (_ot)
    {
    case logger::out_type::cerr:
        p = stderr;
        break;
    case logger::out_type::cout:
        p = stdout;
        break;
    }
    if (p != NULL)fprintf(p, "%s", _buffer.c_str());
    _buffer.clear();
}

void logger::add(_In_ const std::string& str)
{
    _buffer += str;
    if (_buffer.size() >= BUFFER_MAXLEN)write();
}

void logger::out_time(_In_opt_ const std::string& format)
{
    struct tm x;
    get_tm(x);
    add(string_tools::format_string(format.c_str(), x.tm_hour, x.tm_min, x.tm_sec));
}

void logger::_out(_In_ bool need_write, _In_ const char* format, _In_ va_list args)
{
    std::string str;
    string_tools::format_string(str, format, args);
    add(str);
    if (need_write)write();
}
void logger::out(_In_ bool need_write, _In_ const char* format, ...)
{
    va_list args;
    va_start(args, format);
    _out(need_write, format, args);
    va_end(args);
}
void logger::outline(_In_ bool need_write, _In_ const char* format, ...)
{
    va_list args;
    va_start(args, format);
    _out(false, format, args);
    va_end(args);
    add("\n");
    if (need_write)write();
}