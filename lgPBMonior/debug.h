#pragma once

#include <functional>
#include <string>
#include <sstream>
#include <utility>
#include <type_traits>
#include <chrono>

#ifdef _WIN32
#include <Windows.h>
#else
#include <errno.h>
#endif

#ifndef _In_
#define _In_ 
#endif
#ifndef _In_opt_
#define _In_opt_
#endif
#ifndef _Out_
#define _Out_ 
#endif

class logger
{
public:
    enum class out_type
    {
        file, cerr, cout
    };
    logger(_In_ out_type ot, _In_opt_ const std::string& filename = "");

    static void get_tm(_Out_ struct tm& out);

    void out_time(_In_opt_ const std::string& format = "[%02d:%02d:%02d]");
    void out(_In_ bool need_write, _In_ const char* format, ...);
    void outline(_In_ bool need_write, _In_ const char* format, ...);
    void write();

private:

    void add(_In_ const std::string& str);
    void _out(_In_ bool need_write, _In_ const char* format, _In_ va_list args);


    out_type _ot;
    FILE* _fp;

    std::string _buffer;
};

class error_detail final
{
public:
    error_detail() noexcept {}
    error_detail(_In_ const std::string& what) :_what(what) {}
//    ~error_detail()
//    {
//#       if defined _DEBUG
//            if (!_what.empty())
//            {
//#               ifdef _WIN32
//                    MessageBoxA(NULL, "Error", _what.c_str(), MB_OK);
//#               else
//                    fprintf(stderr, "%s", _what.c_str());
//#               endif
//            }
//#       endif
//    }
    std::string what()const noexcept { return _what; }
private:
    std::string _what;
};

/**
 * @brief 利用 RAII + lambda 在作用域结束后进行清理
 */
class scope_guard final
{
public:

    using callback_func = std::function<void()>;

    explicit scope_guard(_In_ callback_func on_exit)
        : _on_exit(on_exit), _is_ok(true) {}
    ~scope_guard() { if (!_is_ok) _on_exit(); }

    scope_guard(const scope_guard&) = delete;
    scope_guard& operator=(const scope_guard&) = delete;
    /**
     * @brief 在结束后不调用目标函数清理
    */
    void close() { _is_ok = false; }

private:
    std::function<void()> _on_exit;
    bool _is_ok;
};

class ensure_detail final
{
public:
    struct __flag_struct { int __unused = 0; };
    ensure_detail() {}
    ensure_detail(_In_ const char* func, _In_ int line, _In_ const char* expr)
    {
        std::ostringstream os;
        os << func << '(' << line << "):ensure:" << expr << "\n";
        _what = os.str();
    }
    ensure_detail& operator << (const __flag_struct&) { return *this; }
    template<typename _T>
    ensure_detail& operator << (_In_ const std::pair<const char*, _T>& x)
    {
        _what += ("  " + std::string(x.first) + ':' +
            to_string(x.second) + '\n');
        return *this;
    }
    std::string what()const { return _what; }
    operator error_detail()const { return error_detail(_what); }
private:
#include "debug_ensure_tostring.inl"

    std::string to_string_(_In_ const void* p)
    {
        char buf[20];
        snprintf(buf, 20, "%p", p);
        return buf;
    }

    template<typename _T>
    std::string to_string(_T* x)
    {
        return to_string_(x);
    }

    template <typename _T> std::string to_string(const _T x)
    {
        return std::to_string(x);
    }
    std::string _what;
};

static constexpr ensure_detail::__flag_struct
    __ENSURE_TRY_GET_PAR_1 = ensure_detail::__flag_struct(),
    __ENSURE_TRY_GET_PAR_2 = ensure_detail::__flag_struct();
#define __ENSURE_ADD(v,next) std::make_pair(#v,v)<<next
#define __ENSURE_TRY_GET_PAR_1(exp) __ENSURE_ADD(exp,__ENSURE_TRY_GET_PAR_2)
#define __ENSURE_TRY_GET_PAR_2(exp) __ENSURE_ADD(exp,__ENSURE_TRY_GET_PAR_1)

/* 如果表达式为真会返回空的信息 */
#define __ENSURE(expr) (expr)\
    ?ensure_detail()\
    :ensure_detail(__FUNCTION__, __LINE__, #expr) << __ENSURE_TRY_GET_PAR_1

#define ON_SCOPE_EXIT(func) error_detail __ed; scope_guard __sg(func)
#define CLOSE_SCOPE_EXIT_FUNC() __sg.close()

#ifdef _WIN32
#define ENSURE(expr) __ENSURE(expr)(GetLastError())
#else
#define ENSURE(expr) __ENSURE(expr)(errno)
#endif