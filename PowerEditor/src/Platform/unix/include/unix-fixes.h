// GCC/Clang shims so Notepad++ sources compile on Unix without MSVC.
#pragma once

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <cstdarg>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <cwctype>
#include <ctime>
#include <cctype>
#include <cstdint>
#include <locale.h>
#include <algorithm>
#include <string>

using uintptr_t = std::uintptr_t;
inline int iswascii(wint_t c) { return (unsigned)c < 128; }
#ifndef strnicmp
#define strnicmp strncasecmp
#endif
inline unsigned short _byteswap_ushort(unsigned short v) { return __builtin_bswap16(v); }

inline locale_t _create_locale(int, const char* name)
{
	return newlocale(LC_ALL_MASK, name ? name : "C", (locale_t)0);
}
inline void _free_locale(locale_t loc)
{
	if (loc) freelocale(loc);
}

#define __try try
#define __except(x) catch(...)
#define __finally

#ifndef __int64
#define __int64 long long
#endif
#ifndef __int32
#define __int32 int
#endif
#ifndef __int16
#define __int16 short
#endif
#ifndef __int8
#define __int8 char
#endif

#ifndef _MSC_VER
#define __pragma(x)
#ifndef _countof
#define _countof(a) (sizeof(a)/sizeof((a)[0]))
#endif
#endif

#ifndef PROCESSOR_ARCHITECTURE_ARM64
#define PROCESSOR_ARCHITECTURE_ARM64 12
#endif

extern int __argc;
extern wchar_t** __wargv;
extern char** __argv;

// Scintilla.h only declares this under _WIN32. We never define _WIN32 on Unix.
#ifdef __cplusplus
extern "C" {
#endif
int Scintilla_RegisterClasses(void *hInstance);
int Scintilla_ReleaseResources(void);
#ifdef __cplusplus
}
#endif

inline int wcscpy_s(wchar_t* dst, size_t n, const wchar_t* src)
{
	if (!dst || !src || n == 0) return EINVAL;
	wmemset(dst, 0, n);
	wcsncpy(dst, src, n - 1);
	dst[n - 1] = 0;
	return 0;
}
template<size_t N>
inline int wcscpy_s(wchar_t (&dst)[N], const wchar_t* src)
{
	return wcscpy_s(dst, N, src);
}
inline int wcsncpy_s(wchar_t* dst, size_t n, const wchar_t* src, size_t count)
{
	if (!dst || !src || n == 0) return EINVAL;
	size_t c = std::min(count, n - 1);
	wmemset(dst, 0, n);
	wcsncpy(dst, src, c);
	dst[c] = 0;
	return 0;
}
template<size_t N>
inline int wcsncpy_s(wchar_t (&dst)[N], const wchar_t* src, size_t count)
{
	return wcsncpy_s(dst, N, src, count);
}
inline int wcscat_s(wchar_t* dst, size_t n, const wchar_t* src)
{
	if (!dst || !src || n == 0) return EINVAL;
	size_t len = wcslen(dst);
	if (len >= n) return ERANGE;
	return wcscpy_s(dst + len, n - len, src);
}
template<size_t N>
inline int wcscat_s(wchar_t (&dst)[N], const wchar_t* src)
{
	return wcscat_s(dst, N, src);
}
inline int strcpy_s(char* dst, size_t n, const char* src)
{
	if (!dst || !src || n == 0) return EINVAL;
	std::snprintf(dst, n, "%s", src);
	return 0;
}
inline int strcat_s(char* dst, size_t n, const char* src)
{
	if (!dst || !src || n == 0) return EINVAL;
	std::strncat(dst, src, n - std::strlen(dst) - 1);
	return 0;
}
inline int memcpy_s(void* dst, size_t dstsz, const void* src, size_t count)
{
	if (!dst || !src) return EINVAL;
	if (count > dstsz) return ERANGE;
	std::memcpy(dst, src, count);
	return 0;
}
inline int sprintf_s(char* buf, size_t n, const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	int r = std::vsnprintf(buf, n, fmt, ap);
	va_end(ap);
	return r;
}
template<size_t N>
inline int sprintf_s(char (&buf)[N], const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	int r = std::vsnprintf(buf, N, fmt, ap);
	va_end(ap);
	return r;
}
inline int swprintf_s(wchar_t* buf, size_t n, const wchar_t* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	int r = std::vswprintf(buf, n, fmt, ap);
	va_end(ap);
	return r;
}
template<size_t N>
inline int swprintf_s(wchar_t (&buf)[N], const wchar_t* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	int r = std::vswprintf(buf, N, fmt, ap);
	va_end(ap);
	return r;
}
inline int _snwprintf(wchar_t* buf, size_t n, const wchar_t* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	int r = std::vswprintf(buf, n, fmt, ap);
	va_end(ap);
	return r;
}
inline int _snwprintf_s(wchar_t* buf, size_t n, size_t, const wchar_t* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	int r = std::vswprintf(buf, n, fmt, ap);
	va_end(ap);
	return r;
}
inline int _vsnwprintf(wchar_t* buf, size_t n, const wchar_t* fmt, va_list ap)
{
	return std::vswprintf(buf, n, fmt, ap);
}
inline int _vsnprintf(char* buf, size_t n, const char* fmt, va_list ap)
{
	return std::vsnprintf(buf, n, fmt, ap);
}

inline int _wcsicmp(const wchar_t* a, const wchar_t* b)
{
	if (!a) a = L"";
	if (!b) b = L"";
	while (*a && *b)
	{
		wchar_t ca = towlower(*a++);
		wchar_t cb = towlower(*b++);
		if (ca != cb) return ca < cb ? -1 : 1;
	}
	if (*a) return 1;
	if (*b) return -1;
	return 0;
}
inline int _wcsnicmp(const wchar_t* a, const wchar_t* b, size_t n)
{
	if (!a) a = L"";
	if (!b) b = L"";
	for (size_t i = 0; i < n; ++i)
	{
		if (!a[i] && !b[i]) return 0;
		wchar_t ca = towlower(a[i]);
		wchar_t cb = towlower(b[i]);
		if (ca != cb) return ca < cb ? -1 : 1;
	}
	return 0;
}
inline int _stricmp(const char* a, const char* b) { return strcasecmp(a ? a : "", b ? b : ""); }
inline int _strnicmp(const char* a, const char* b, size_t n) { return strncasecmp(a ? a : "", b ? b : "", n); }
inline wchar_t* _wcslwr(wchar_t* s) { for (wchar_t* p = s; p && *p; ++p) *p = towlower(*p); return s; }
inline wchar_t* _wcsupr(wchar_t* s) { for (wchar_t* p = s; p && *p; ++p) *p = towupper(*p); return s; }
inline int _wtoi(const wchar_t* s) { return s ? static_cast<int>(wcstol(s, nullptr, 10)) : 0; }
inline long _wtol(const wchar_t* s) { return s ? wcstol(s, nullptr, 10) : 0; }
inline long long _wtoi64(const wchar_t* s) { return s ? wcstoll(s, nullptr, 10) : 0; }
inline double _wtof(const wchar_t* s) { return s ? wcstod(s, nullptr) : 0; }

#ifndef _MAX_PATH
#define _MAX_PATH 260
#endif
#ifndef _MAX_DIR
#define _MAX_DIR 260
#endif
#ifndef _MAX_FNAME
#define _MAX_FNAME 256
#endif
#ifndef _MAX_EXT
#define _MAX_EXT 256
#endif
#ifndef _MAX_DRIVE
#define _MAX_DRIVE 16
#endif

inline void _wsplitpath_s(const wchar_t* path, wchar_t* drive, size_t driveSz,
	wchar_t* dir, size_t dirSz, wchar_t* fname, size_t fnameSz, wchar_t* ext, size_t extSz)
{
	if (drive && driveSz) drive[0] = 0;
	std::wstring p = path ? path : L"";
	for (auto& c : p) if (c == L'\\') c = L'/';
	size_t slash = p.find_last_of(L'/');
	std::wstring d = (slash == std::wstring::npos) ? L"" : p.substr(0, slash + 1);
	std::wstring rest = (slash == std::wstring::npos) ? p : p.substr(slash + 1);
	size_t dot = rest.find_last_of(L'.');
	std::wstring name = (dot == std::wstring::npos) ? rest : rest.substr(0, dot);
	std::wstring e = (dot == std::wstring::npos) ? L"" : rest.substr(dot);
	if (dir && dirSz) wcsncpy(dir, d.c_str(), dirSz - 1), dir[dirSz - 1] = 0;
	if (fname && fnameSz) wcsncpy(fname, name.c_str(), fnameSz - 1), fname[fnameSz - 1] = 0;
	if (ext && extSz) wcsncpy(ext, e.c_str(), extSz - 1), ext[extSz - 1] = 0;
}

#ifndef errno_t
using errno_t = int;
#endif

inline errno_t _wfopen_s(FILE** fp, const wchar_t* path, const wchar_t* mode)
{
	if (!fp) return EINVAL;
	char p[4096], m[32];
	std::wcstombs(p, path ? path : L"", sizeof(p));
	std::wcstombs(m, mode ? mode : L"r", sizeof(m));
	for (char* c = p; *c; ++c) if (*c == '\\') *c = '/';
	*fp = std::fopen(p, m);
	return *fp ? 0 : errno;
}
inline FILE* _wfopen(const wchar_t* path, const wchar_t* mode)
{
	FILE* fp = nullptr;
	_wfopen_s(&fp, path, mode);
	return fp;
}
#ifndef _TRUNCATE
#define _TRUNCATE ((size_t)-1)
#endif
#ifndef rsize_t
using rsize_t = size_t;
#endif
inline int strncpy_s(char* dst, size_t n, const char* src, size_t count)
{
	if (!dst || !src || n == 0) return EINVAL;
	if (count == _TRUNCATE) count = n - 1;
	size_t c = std::min(count, n - 1);
	std::memcpy(dst, src, c);
	dst[c] = 0;
	return 0;
}
inline char* itoa(int v, char* buf, int)
{
	std::sprintf(buf, "%d", v);
	return buf;
}
inline wchar_t* _itow(int v, wchar_t* buf, int)
{
	std::swprintf(buf, 32, L"%d", v);
	return buf;
}
inline double _wcstod_l(const wchar_t* s, wchar_t** e, void*)
{
	return wcstod(s, e);
}
#ifndef wcsnicmp
#define wcsnicmp _wcsnicmp
#endif

#ifndef __stdcall
#define __stdcall
#endif

inline unsigned int _beginthreadex(void*, unsigned, unsigned (__stdcall *start)(void*), void* arg, unsigned, unsigned* id)
{
	(void)start; (void)arg; (void)id;
	return 1;
}

#ifndef __stdcall
#define __stdcall
#endif

inline void _splitpath_s(const char* path, char* drive, size_t, char* dir, size_t dirSz, char* fname, size_t fnameSz, char* ext, size_t extSz)
{
	if (drive) drive[0] = 0;
	std::string p = path ? path : "";
	for (auto& c : p) if (c == '\\') c = '/';
	size_t slash = p.find_last_of('/');
	std::string d = (slash == std::string::npos) ? "" : p.substr(0, slash + 1);
	std::string rest = (slash == std::string::npos) ? p : p.substr(slash + 1);
	size_t dot = rest.find_last_of('.');
	std::string name = (dot == std::string::npos) ? rest : rest.substr(0, dot);
	std::string e = (dot == std::string::npos) ? "" : rest.substr(dot);
	if (dir && dirSz) std::snprintf(dir, dirSz, "%s", d.c_str());
	if (fname && fnameSz) std::snprintf(fname, fnameSz, "%s", name.c_str());
	if (ext && extSz) std::snprintf(ext, extSz, "%s", e.c_str());
}

inline wchar_t* _wgetenv(const wchar_t* name)
{
	static thread_local wchar_t buf[1024];
	char n[256];
	std::wcstombs(n, name ? name : L"", sizeof(n));
	const char* v = std::getenv(n);
	if (!v) return nullptr;
	std::mbstowcs(buf, v, 1023);
	buf[1023] = 0;
	return buf;
}

#define _strtoi64 strtoll
#define _wcstoui64 wcstoull
#define _wcstoi64 wcstoll
#define _strdup strdup
#define _wcsdup wcsdup
#define _strlwr(s) ({ for (char* p=(s); p && *p; ++p) *p = (char)tolower((unsigned char)*p); (s); })
#define _itoa_s(v,b,n,r) snprintf((b),(n),"%d",(v))
#define _itow_s(v,b,n,r) swprintf((b),(n),L"%d",(v))
#define _i64tow_s(v,b,n,r) swprintf((b),(n),L"%lld",(long long)(v))
#define _ltoa_s(v,b,n,r) snprintf((b),(n),"%ld",(v))
#define _ultoa_s(v,b,n,r) snprintf((b),(n),"%lu",(v))
#define fopen_s(fp,p,m) ((*(fp)=fopen((p),(m))) ? 0 : errno)
#define localtime_s(tm,t) (!localtime_r((t),(tm)) ? errno : 0)
#define gmtime_s(tm,t) (!gmtime_r((t),(tm)) ? errno : 0)
#define _ftime_s(t) ({ struct timespec ts; clock_gettime(CLOCK_REALTIME,&ts); (t)->time=ts.tv_sec; (t)->millitm=(unsigned short)(ts.tv_nsec/1000000); 0; })

struct _timeb { time_t time; unsigned short millitm; short timezone; short dstflag; };

#ifndef MAXUINT
#define MAXUINT ((unsigned int)-1)
#endif
#ifndef MAXINT
#define MAXINT 0x7fffffff
#endif
#ifndef MININT
#define MININT ((int)0x80000000)
#endif
#ifndef MAXDWORD
#define MAXDWORD 0xffffffffUL
#endif
#ifndef MAXWORD
#define MAXWORD 0xffff
#endif
#ifndef MAXBYTE
#define MAXBYTE 0xff
#endif
#ifndef BYTE_MAX
#define BYTE_MAX 0xff
#endif

#ifndef __unaligned
#define __unaligned
#endif
#ifndef UNALIGNED
#define UNALIGNED
#endif
#ifndef __assume
#define __assume(x) ((void)0)
#endif
#ifndef __analysis_assume
#define __analysis_assume(x) ((void)0)
#endif
#ifndef C_ASSERT
#define C_ASSERT(e) static_assert(e, #e)
#endif


#ifndef HEAP_ZERO_MEMORY
#define HEAP_ZERO_MEMORY 0x00000008
#endif
#ifndef FILE_TYPE_DISK
#define FILE_TYPE_DISK 1
#endif
#ifndef TIME_ZONE_ID_UNKNOWN
#define TIME_ZONE_ID_UNKNOWN 0
#endif
#ifndef MB_TYPEMASK
#define MB_TYPEMASK 0x0000000FL
#endif
#ifndef CSTR_GREATER_THAN
#define CSTR_GREATER_THAN 3
#endif
#ifndef LCMAP_LOWERCASE
#define LCMAP_LOWERCASE 0x00000100
#endif
#ifndef FILE_BEGIN
#define FILE_BEGIN 0
#endif
#ifndef FO_MOVE
#define FO_MOVE 0x0001
#endif
#ifndef FO_COPY
#define FO_COPY 0x0002
#endif
#ifndef FO_DELETE
#define FO_DELETE 0x0003
#endif
#ifndef BM_GETCHECK
#define BM_GETCHECK 0x00F0
#endif
#ifndef BST_UNCHECKED
#define BST_UNCHECKED 0x0000
#endif
#ifndef IDCANCEL
#define IDCANCEL 2
#endif
#ifndef IDOK
#define IDOK 1
#endif
#ifndef IDYES
#define IDYES 6
#endif
#ifndef IDNO
#define IDNO 7
#endif
#ifndef MB_ICONERROR
#define MB_ICONERROR 0x00000010L
#endif
#ifndef MB_ICONWARNING
#define MB_ICONWARNING 0x00000030L
#endif
#ifndef MB_ICONQUESTION
#define MB_ICONQUESTION 0x00000020L
#endif
#ifndef MB_OKCANCEL
#define MB_OKCANCEL 0x00000001L
#endif
#ifndef MB_YESNO
#define MB_YESNO 0x00000004L
#endif
#ifndef MB_YESNOCANCEL
#define MB_YESNOCANCEL 0x00000003L
#endif
#ifndef SW_SHOW
#define SW_SHOW 5
#endif
#ifndef SW_MAXIMIZE
#define SW_MAXIMIZE 3
#endif
#ifndef SW_MINIMIZE
#define SW_MINIMIZE 6
#endif
#ifndef SW_SHOWMINIMIZED
#define SW_SHOWMINIMIZED 2
#endif
#ifndef SW_RESTORE
#define SW_RESTORE 9
#endif
#ifndef IMAGE_BITMAP
#define IMAGE_BITMAP 0
#endif
#ifndef GWLP_HINSTANCE
#define GWLP_HINSTANCE (-6)
#endif
#ifndef GWLP_HWNDPARENT
#define GWLP_HWNDPARENT (-8)
#endif
#ifndef GWLP_ID
#define GWLP_ID (-12)
#endif
#ifndef GWLP_USERDATA
#define GWLP_USERDATA (-21)
#endif
#ifndef GWL_EXSTYLE
#define GWL_EXSTYLE (-20)
#endif
#ifndef GW_OWNER
#define GW_OWNER 4
#endif
#ifndef GW_HWNDNEXT
#define GW_HWNDNEXT 2
#endif
#ifndef GW_HWNDPREV
#define GW_HWNDPREV 3
#endif
#ifndef SM_CXSCREEN
#define SM_CXSCREEN 0
#endif
#ifndef HORZRES
#define HORZRES 8
#endif
#ifndef VERTRES
#define VERTRES 10
#endif
#ifndef BITSPIXEL
#define BITSPIXEL 12
#endif
#ifndef PLANES
#define PLANES 14
#endif
#ifndef NUMCOLORS
#define NUMCOLORS 24
#endif
#ifndef TECHNOLOGY
#define TECHNOLOGY 2
#endif
#ifndef LOGPIXELSY
#define LOGPIXELSY 90
#endif
#ifndef OBJ_BRUSH
#define OBJ_BRUSH 2
#endif
#ifndef OBJ_FONT
#define OBJ_FONT 6
#endif
#ifndef OBJ_BITMAP
#define OBJ_BITMAP 7
#endif
#ifndef DT_CENTER
#define DT_CENTER 0x00000001
#endif
#ifndef DT_RIGHT
#define DT_RIGHT 0x00000002
#endif
#ifndef DT_VCENTER
#define DT_VCENTER 0x00000004
#endif
#ifndef DT_BOTTOM
#define DT_BOTTOM 0x00000008
#endif
#ifndef DT_CALCRECT
#define DT_CALCRECT 0x00000400
#endif
#ifndef COLOR_WINDOW
#define COLOR_WINDOW 5
#endif
#ifndef SPI_GETWORKAREA
#define SPI_GETWORKAREA 48
#endif
#ifndef SPI_GETWHEELSCROLLLINES
#define SPI_GETWHEELSCROLLLINES 104
#endif
#ifndef SPI_GETNONCLIENTMETRICS
#define SPI_GETNONCLIENTMETRICS 41
#endif
#ifndef MK_LBUTTON
#define MK_LBUTTON 0x0001
#endif
#ifndef MK_SHIFT
#define MK_SHIFT 0x0004
#endif
#ifndef MK_CONTROL
#define MK_CONTROL 0x0008
#endif
#ifndef VK_F1
#define VK_F1 0x70
#endif
#ifndef VK_RETURN
#define VK_RETURN 0x0D
#endif
#ifndef VK_ESCAPE
#define VK_ESCAPE 0x1B
#endif
#ifndef VK_TAB
#define VK_TAB 0x09
#endif
#ifndef VK_BACK
#define VK_BACK 0x08
#endif
#ifndef VK_DELETE
#define VK_DELETE 0x2E
#endif
#ifndef VK_INSERT
#define VK_INSERT 0x2D
#endif
#ifndef VK_HOME
#define VK_HOME 0x24
#endif
#ifndef VK_END
#define VK_END 0x23
#endif
#ifndef VK_PRIOR
#define VK_PRIOR 0x21
#endif
#ifndef VK_NEXT
#define VK_NEXT 0x22
#endif
#ifndef VK_LEFT
#define VK_LEFT 0x25
#endif
#ifndef VK_RIGHT
#define VK_RIGHT 0x27
#endif
#ifndef VK_UP
#define VK_UP 0x26
#endif
#ifndef VK_DOWN
#define VK_DOWN 0x28
#endif
#ifndef VK_SPACE
#define VK_SPACE 0x20
#endif
#ifndef VK_SHIFT
#define VK_SHIFT 0x10
#endif
#ifndef VK_CONTROL
#define VK_CONTROL 0x11
#endif
#ifndef VK_MENU
#define VK_MENU 0x12
#endif
#ifndef VK_LWIN
#define VK_LWIN 0x5B
#endif
#ifndef WM_PAINT
#define WM_PAINT 0x000F
#endif
#ifndef WM_CREATE
#define WM_CREATE 0x0001
#endif
#ifndef WM_NCCREATE
#define WM_NCCREATE 0x0081
#endif
#ifndef WM_DESTROY
#define WM_DESTROY 0x0002
#endif
#ifndef WM_CLOSE
#define WM_CLOSE 0x0010
#endif
#ifndef WM_SIZE
#define WM_SIZE 0x0005
#endif
#ifndef WM_MOVE
#define WM_MOVE 0x0003
#endif
#ifndef WM_COMMAND
#define WM_COMMAND 0x0111
#endif
#ifndef WM_NOTIFY
#define WM_NOTIFY 0x004E
#endif
#ifndef WM_TIMER
#define WM_TIMER 0x0113
#endif
#ifndef WM_QUIT
#define WM_QUIT 0x0012
#endif
#ifndef WM_KEYDOWN
#define WM_KEYDOWN 0x0100
#endif
#ifndef WM_KEYUP
#define WM_KEYUP 0x0101
#endif
#ifndef WM_CHAR
#define WM_CHAR 0x0102
#endif
#ifndef WM_LBUTTONDOWN
#define WM_LBUTTONDOWN 0x0201
#endif
#ifndef WM_LBUTTONUP
#define WM_LBUTTONUP 0x0202
#endif
#ifndef WM_LBUTTONDBLCLK
#define WM_LBUTTONDBLCLK 0x0203
#endif
#ifndef WM_RBUTTONDOWN
#define WM_RBUTTONDOWN 0x0204
#endif
#ifndef WM_RBUTTONUP
#define WM_RBUTTONUP 0x0205
#endif
#ifndef WM_RBUTTONDBLCLK
#define WM_RBUTTONDBLCLK 0x0206
#endif
#ifndef WM_MBUTTONDOWN
#define WM_MBUTTONDOWN 0x0207
#endif
#ifndef WM_MBUTTONUP
#define WM_MBUTTONUP 0x0208
#endif
#ifndef WM_MBUTTONDBLCLK
#define WM_MBUTTONDBLCLK 0x0209
#endif
#ifndef WM_MOUSEMOVE
#define WM_MOUSEMOVE 0x0200
#endif
#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL 0x020A
#endif
#ifndef WM_SETFOCUS
#define WM_SETFOCUS 0x0007
#endif
#ifndef WM_SETTEXT
#define WM_SETTEXT 0x000C
#endif
#ifndef WM_GETTEXT
#define WM_GETTEXT 0x000D
#endif
#ifndef WM_GETTEXTLENGTH
#define WM_GETTEXTLENGTH 0x000E
#endif
#ifndef WM_SHOWWINDOW
#define WM_SHOWWINDOW 0x0018
#endif
#ifndef WM_ERASEBKGND
#define WM_ERASEBKGND 0x0014
#endif
#ifndef CSIDL_APPDATA
#define CSIDL_APPDATA 0x001A
#endif
#ifndef CSIDL_LOCAL_APPDATA
#define CSIDL_LOCAL_APPDATA 0x001C
#endif
#ifndef CSIDL_PROFILE
#define CSIDL_PROFILE 0x0028
#endif
#ifndef CSIDL_MYDOCUMENTS
#define CSIDL_MYDOCUMENTS 0x0005
#endif
#ifndef CSIDL_DESKTOP
#define CSIDL_DESKTOP 0x0000
#endif
#ifndef CSIDL_COMMON_APPDATA
#define CSIDL_COMMON_APPDATA 0x0023
#endif
#ifndef SHGFP_TYPE_CURRENT
#define SHGFP_TYPE_CURRENT 0
#endif
#ifndef CF_UNICODETEXT
#define CF_UNICODETEXT 13
#endif
#ifndef CF_TEXT
#define CF_TEXT 1
#endif
#ifndef HKEY_CURRENT_USER
#define HKEY_CURRENT_USER ((HKEY)(ULONG_PTR)((LONG)0x80000001))
#endif
#ifndef HKEY_LOCAL_MACHINE
#define HKEY_LOCAL_MACHINE ((HKEY)(ULONG_PTR)((LONG)0x80000002))
#endif
#ifndef ERROR_SUCCESS
#define ERROR_SUCCESS 0L
#endif
#ifndef ERROR_FILE_NOT_FOUND
#define ERROR_FILE_NOT_FOUND 2L
#endif
#ifndef S_OK
#define S_OK ((HRESULT)0)
#endif
#ifndef E_FAIL
#define E_FAIL ((HRESULT)0x80004005L)
#endif
#ifndef BCRYPT_SUCCESS
#define BCRYPT_SUCCESS(s) (((NTSTATUS)(s)) >= 0)
#endif
#ifndef STATUS_SUCCESS
#define STATUS_SUCCESS ((NTSTATUS)0)
#endif
#ifndef STATUS_UNSUCCESSFUL
#define STATUS_UNSUCCESSFUL ((NTSTATUS)0xC0000001L)
#endif
#ifndef RT_STRING
#define RT_STRING MAKEINTRESOURCE(6)
#endif
#ifndef RT_MENU
#define RT_MENU MAKEINTRESOURCE(4)
#endif
#ifndef RT_ACCELERATOR
#define RT_ACCELERATOR MAKEINTRESOURCE(9)
#endif
#ifndef RT_ICON
#define RT_ICON MAKEINTRESOURCE(3)
#endif
#ifndef RT_BITMAP
#define RT_BITMAP MAKEINTRESOURCE(2)
#endif
#ifndef RT_GROUP_ICON
#define RT_GROUP_ICON MAKEINTRESOURCE(14)
#endif
#ifndef MF_STRING
#define MF_STRING 0x00000000L
#endif
#ifndef MF_SEPARATOR
#define MF_SEPARATOR 0x00000800L
#endif
#ifndef MF_POPUP
#define MF_POPUP 0x00000010L
#endif
#ifndef MF_CHECKED
#define MF_CHECKED 0x00000008L
#endif
#ifndef MF_UNCHECKED
#define MF_UNCHECKED 0x00000000L
#endif
#ifndef MF_ENABLED
#define MF_ENABLED 0x00000000L
#endif
#ifndef MF_GRAYED
#define MF_GRAYED 0x00000001L
#endif
#ifndef MF_BYCOMMAND
#define MF_BYCOMMAND 0x00000000L
#endif
#ifndef MF_BYPOSITION
#define MF_BYPOSITION 0x00000400L
#endif
#ifndef TPM_LEFTALIGN
#define TPM_LEFTALIGN 0x0000
#endif
#ifndef IDC_CROSS
#define IDC_CROSS MAKEINTRESOURCE(32515)
#endif
#ifndef IDC_SIZEWE
#define IDC_SIZEWE MAKEINTRESOURCE(32644)
#endif
#ifndef IDC_SIZENS
#define IDC_SIZENS MAKEINTRESOURCE(32645)
#endif
#ifndef IDC_SIZENWSE
#define IDC_SIZENWSE MAKEINTRESOURCE(32642)
#endif
#ifndef IDC_SIZENESW
#define IDC_SIZENESW MAKEINTRESOURCE(32643)
#endif
#ifndef IDC_SIZEALL
#define IDC_SIZEALL MAKEINTRESOURCE(32646)
#endif
#ifndef IDC_NO
#define IDC_NO MAKEINTRESOURCE(32648)
#endif
#ifndef COLOR_WINDOWTEXT
#define COLOR_WINDOWTEXT 8
#endif
#ifndef COLOR_BTNFACE
#define COLOR_BTNFACE 15
#endif
#ifndef COLOR_BTNTEXT
#define COLOR_BTNTEXT 18
#endif
#ifndef COLOR_HIGHLIGHT
#define COLOR_HIGHLIGHT 13
#endif
#ifndef COLOR_HIGHLIGHTTEXT
#define COLOR_HIGHLIGHTTEXT 14
#endif
#ifndef COLOR_GRAYTEXT
#define COLOR_GRAYTEXT 17
#endif
#ifndef COLOR_HOTLIGHT
#define COLOR_HOTLIGHT 26
#endif
#ifndef COLOR_3DFACE
#define COLOR_3DFACE COLOR_BTNFACE
#endif
#ifndef COLOR_3DSHADOW
#define COLOR_3DSHADOW 16
#endif
#ifndef COLOR_3DHIGHLIGHT
#define COLOR_3DHIGHLIGHT 20
#endif
#ifndef COLOR_MENU
#define COLOR_MENU 4
#endif
#ifndef COLOR_MENUTEXT
#define COLOR_MENUTEXT 7
#endif
#ifndef SM_CYSCREEN
#define SM_CYSCREEN 1
#endif
#ifndef SM_CXVIRTUALSCREEN
#define SM_CXVIRTUALSCREEN 78
#endif
#ifndef SM_CYVIRTUALSCREEN
#define SM_CYVIRTUALSCREEN 79
#endif
#ifndef SM_XVIRTUALSCREEN
#define SM_XVIRTUALSCREEN 76
#endif
#ifndef SM_YVIRTUALSCREEN
#define SM_YVIRTUALSCREEN 77
#endif
#ifndef SM_CXSMICON
#define SM_CXSMICON 49
#endif
#ifndef SM_CYSMICON
#define SM_CYSMICON 50
#endif
#ifndef SM_CXICON
#define SM_CXICON 11
#endif
#ifndef SM_CYICON
#define SM_CYICON 12
#endif
#ifndef SM_CXCURSOR
#define SM_CXCURSOR 13
#endif
#ifndef SM_CYCURSOR
#define SM_CYCURSOR 14
#endif
#ifndef SM_CYCAPTION
#define SM_CYCAPTION 4
#endif
#ifndef SM_CYMENU
#define SM_CYMENU 15
#endif
#ifndef SM_CXVSCROLL
#define SM_CXVSCROLL 2
#endif
#ifndef SM_CYHSCROLL
#define SM_CYHSCROLL 3
#endif
#ifndef SM_CXEDGE
#define SM_CXEDGE 45
#endif
#ifndef SM_CYEDGE
#define SM_CYEDGE 46
#endif
#ifndef SM_CXFRAME
#define SM_CXFRAME 32
#endif
#ifndef SM_CYFRAME
#define SM_CYFRAME 33
#endif
#ifndef SM_CXSIZEFRAME
#define SM_CXSIZEFRAME SM_CXFRAME
#endif
#ifndef SM_CYSIZEFRAME
#define SM_CYSIZEFRAME SM_CYFRAME
#endif
#ifndef SM_CXBORDER
#define SM_CXBORDER 5
#endif
#ifndef SM_CYBORDER
#define SM_CYBORDER 6
#endif
#ifndef SM_CYSMCAPTION
#define SM_CYSMCAPTION 51
#endif
#ifndef SM_CXDOUBLECLK
#define SM_CXDOUBLECLK 36
#endif
#ifndef SM_CYDOUBLECLK
#define SM_CYDOUBLECLK 37
#endif
#ifndef SM_CXDRAG
#define SM_CXDRAG 68
#endif
#ifndef SM_CYDRAG
#define SM_CYDRAG 69
#endif
#ifndef SM_CXMINTRACK
#define SM_CXMINTRACK 34
#endif
#ifndef SM_CYMINTRACK
#define SM_CYMINTRACK 35
#endif
#ifndef SM_CXMAXIMIZED
#define SM_CXMAXIMIZED 61
#endif
#ifndef SM_CYMAXIMIZED
#define SM_CYMAXIMIZED 62
#endif
#ifndef SM_CMONITORS
#define SM_CMONITORS 80
#endif
#ifndef SM_MOUSEWHEELPRESENT
#define SM_MOUSEWHEELPRESENT 75
#endif
#ifndef SM_SWAPBUTTON
#define SM_SWAPBUTTON 23
#endif
#ifndef WS_CHILD
#define WS_CHILD 0x40000000L
#endif
#ifndef WS_VISIBLE
#define WS_VISIBLE 0x10000000L
#endif
#ifndef WS_CAPTION
#define WS_CAPTION 0x00C00000L
#endif
#ifndef SWP_NOZORDER
#define SWP_NOZORDER 0x0004
#endif
#ifndef SWP_NOREDRAW
#define SWP_NOREDRAW 0x0008
#endif
#ifndef SWP_NOMOVE
#define SWP_NOMOVE 0x0002
#endif
#ifndef SWP_NOSIZE
#define SWP_NOSIZE 0x0001
#endif
#ifndef SWP_SHOWWINDOW
#define SWP_SHOWWINDOW 0x0040
#endif
#ifndef SWP_HIDEWINDOW
#define SWP_HIDEWINDOW 0x0080
#endif
#ifndef WM_NCCREATE
#define WM_NCCREATE 0x0081
#endif
#ifndef ERROR
#define ERROR 0
#endif
