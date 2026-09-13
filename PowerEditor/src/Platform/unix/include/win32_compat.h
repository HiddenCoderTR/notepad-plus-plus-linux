// Notepad++ Unix Win32 compatibility layer.
// Provides enough of the Win32 API for the existing Notepad++ UI to compile
// and run on Linux/macOS without changing application source.

#pragma once

#ifndef NPP_WIN32_COMPAT_H
#define NPP_WIN32_COMPAT_H

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <cwchar>
#include <cstdarg>
#include <climits>
#include <ctime>
#include <locale.h>
#include <string>
#include <vector>

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

#ifndef WINAPI
#define WINAPI
#endif
#ifndef CALLBACK
#define CALLBACK
#endif
#ifndef APIENTRY
#define APIENTRY
#endif
#ifndef PASCAL
#define PASCAL
#endif
#ifndef NEAR
#define NEAR
#endif
#ifndef FAR
#define FAR
#endif
#ifndef CONST
#define CONST const
#endif
#ifndef VOID
#define VOID void
#endif

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#ifndef MAX_PATH
#define MAX_PATH 260
#endif
#ifndef MAX_PATH_EX
#define MAX_PATH_EX 32767
#endif
#ifndef _MAX_PATH
#define _MAX_PATH MAX_PATH
#endif
#ifndef MAX_LONG_PATH
#define MAX_LONG_PATH 32767
#endif

#ifndef UNREFERENCED_PARAMETER
#define UNREFERENCED_PARAMETER(x) (void)(x)
#endif
#ifndef UNUSED_PARAM
#define UNUSED_PARAM(x) (void)(x)
#endif

#ifndef __out
#define __out
#endif
#ifndef __in
#define __in
#endif
#ifndef __in_opt
#define __in_opt
#endif
#ifndef __out_opt
#define __out_opt
#endif
#ifndef __inout
#define __inout
#endif
#ifndef __inout_opt
#define __inout_opt
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
#ifndef _Out_opt_
#define _Out_opt_
#endif
#ifndef _Inout_
#define _Inout_
#endif
#ifndef _In_z_
#define _In_z_
#endif
#ifndef _In_opt_z_
#define _In_opt_z_
#endif
#ifndef _Out_z_cap_
#define _Out_z_cap_(n)
#endif
#ifndef _Out_writes_
#define _Out_writes_(n)
#endif
#ifndef _Out_writes_z_
#define _Out_writes_z_(n)
#endif
#ifndef _Out_writes_opt_
#define _Out_writes_opt_(n)
#endif
#ifndef _In_reads_
#define _In_reads_(n)
#endif
#ifndef _In_reads_bytes_
#define _In_reads_bytes_(n)
#endif
#ifndef _In_reads_opt_
#define _In_reads_opt_(n)
#endif
#ifndef _Out_writes_bytes_
#define _Out_writes_bytes_(n)
#endif
#ifndef _Out_writes_bytes_opt_
#define _Out_writes_bytes_opt_(n)
#endif
#ifndef _Out_writes_to_opt_
#define _Out_writes_to_opt_(n,m)
#endif
#ifndef _Null_terminated_
#define _Null_terminated_
#endif
#ifndef _Success_
#define _Success_(x)
#endif
#ifndef _Check_return_
#define _Check_return_
#endif
#ifndef _Ret_maybenull_
#define _Ret_maybenull_
#endif
#ifndef _Post_equals_last_error_
#define _Post_equals_last_error_
#endif

#ifndef DECLSPEC_NOVTABLE
#define DECLSPEC_NOVTABLE
#endif
#ifndef DECLSPEC_SELECTANY
#define DECLSPEC_SELECTANY
#endif
#ifndef __declspec
#define __declspec(x)
#endif
#ifndef __stdcall
#define __stdcall
#endif
#ifndef __cdecl
#define __cdecl
#endif
#ifndef __forceinline
#define __forceinline inline
#endif
#ifndef FORCEINLINE
#define FORCEINLINE inline
#endif
#ifndef OPTIONAL
#define OPTIONAL
#endif
#ifndef IN
#define IN
#endif
#ifndef OUT
#define OUT
#endif
#ifndef PURE
#define PURE = 0
#endif
#ifndef interface
#define interface struct
#endif
#ifndef STDMETHODCALLTYPE
#define STDMETHODCALLTYPE
#endif
#ifndef STDMETHODIMP
#define STDMETHODIMP HRESULT
#endif
#ifndef STDMETHODIMP_
#define STDMETHODIMP_(type) type
#endif
#ifndef STDMETHOD
#define STDMETHOD(method) virtual HRESULT STDMETHODCALLTYPE method
#endif
#ifndef STDMETHOD_
#define STDMETHOD_(type, method) virtual type STDMETHODCALLTYPE method
#endif
#ifndef THIS_
#define THIS_
#endif
#ifndef THIS
#define THIS void
#endif
#ifndef DECLSPEC_UUID
#define DECLSPEC_UUID(x)
#endif
#ifndef __uuidof
#define __uuidof(T) GUID{}
#endif

#ifndef C_ASSERT
#define C_ASSERT(e) static_assert(e, #e)
#endif

#ifndef LOBYTE
#define LOBYTE(w) ((BYTE)((DWORD_PTR)(w) & 0xff))
#define HIBYTE(w) ((BYTE)(((DWORD_PTR)(w) >> 8) & 0xff))
#define LOWORD(l) ((WORD)((DWORD_PTR)(l) & 0xffff))
#define HIWORD(l) ((WORD)(((DWORD_PTR)(l) >> 16) & 0xffff))
#define MAKEWORD(a, b) ((WORD)(((BYTE)((DWORD_PTR)(a) & 0xff)) | ((WORD)((BYTE)((DWORD_PTR)(b) & 0xff))) << 8))
#define MAKELONG(a, b) ((LONG)(((WORD)((DWORD_PTR)(a) & 0xffff)) | ((DWORD)((WORD)((DWORD_PTR)(b) & 0xffff))) << 16))
#define MAKELPARAM(l, h) ((LPARAM)(DWORD)MAKELONG(l, h))
#define MAKEWPARAM(l, h) ((WPARAM)(DWORD)MAKELONG(l, h))
#define MAKELRESULT(l, h) ((LRESULT)(DWORD)MAKELONG(l, h))
#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))
#endif

#ifndef RGB
#define RGB(r,g,b) ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))
#define GetRValue(rgb) (LOBYTE(rgb))
#define GetGValue(rgb) (LOBYTE(((WORD)(rgb)) >> 8))
#define GetBValue(rgb) (LOBYTE((rgb) >> 16))
#endif

#ifndef PALETTERGB
#define PALETTERGB(r,g,b) (0x02000000 | RGB(r,g,b))
#endif

#ifndef MAKEINTRESOURCE
#define MAKEINTRESOURCEA(i) ((LPSTR)((ULONG_PTR)((WORD)(i))))
#define MAKEINTRESOURCEW(i) ((LPWSTR)((ULONG_PTR)((WORD)(i))))
#define MAKEINTRESOURCE MAKEINTRESOURCEW
#define IS_INTRESOURCE(r) ((((ULONG_PTR)(r)) >> 16) == 0)
#endif

#ifndef MAKEINTATOM
#define MAKEINTATOM(i) ((LPTSTR)((ULONG_PTR)((WORD)(i))))
#endif

#ifndef UNICODE
#define UNICODE 1
#endif
#ifndef _UNICODE
#define _UNICODE 1
#endif

#ifndef TEXT
#define __TEXT(s) L##s
#define TEXT(s) __TEXT(s)
#define _T(s) TEXT(s)
#define _TEXT(s) TEXT(s)
#endif

#ifndef ZeroMemory
#define RtlCopyMemory(d,s,n) memcpy((d),(s),(n))
#define CopyMemory RtlCopyMemory
#define RtlMoveMemory(d,s,n) memmove((d),(s),(n))
#define MoveMemory RtlMoveMemory
#define RtlFillMemory(d,n,f) memset((d),(f),(n))
#define FillMemory RtlFillMemory
#define RtlZeroMemory(d,n) memset((d),0,(n))
#define ZeroMemory RtlZeroMemory
#define SecureZeroMemory RtlZeroMemory
#endif

#ifndef ARRAYSIZE
#define ARRAYSIZE(A) (sizeof(A)/sizeof((A)[0]))
#endif
#ifndef _countof
#define _countof ARRAYSIZE
#endif

// ---- basic types ----
typedef void* HANDLE;
typedef HANDLE HWND;
typedef HANDLE HINSTANCE;
typedef HANDLE HMODULE;
typedef HANDLE HMENU;
typedef HANDLE HDC;
typedef HANDLE HGDIOBJ;
typedef HANDLE HBITMAP;
typedef HANDLE HBRUSH;
typedef HANDLE HPEN;
typedef HANDLE HFONT;
typedef HANDLE HICON;
typedef HANDLE HCURSOR;
typedef HANDLE HKL;
typedef HANDLE HRGN;
typedef HANDLE HPALETTE;
typedef HANDLE HMONITOR;
typedef HANDLE HHOOK;
typedef HANDLE HGLOBAL;
typedef HANDLE HLOCAL;
typedef HANDLE HRSRC;
typedef HANDLE HACCEL;
typedef HANDLE HDROP;
typedef HANDLE HIMAGELIST;
typedef HANDLE HKEY;
typedef HANDLE HDESK;
typedef HANDLE HWINSTA;
typedef HANDLE HTOKEN;
typedef HANDLE HWAVEOUT;
typedef HANDLE HTHEME;
typedef HANDLE HTHUMBNAIL;
typedef HANDLE HRAWINPUT;
typedef HANDLE HDWP;
typedef HANDLE HSZ;
typedef HANDLE HDDEDATA;
typedef HANDLE HCONV;
typedef HANDLE HCONVLIST;
typedef HANDLE HIMC;
typedef HANDLE HIMCC;
typedef HANDLE HENHMETAFILE;
typedef HANDLE HMETAFILE;
typedef HANDLE HCOLORSPACE;
typedef HANDLE HGLRC;
typedef HANDLE HFILENOTIFY;
typedef HANDLE HWND_MESSAGE_T;
typedef int BOOL;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int UINT;
typedef int INT;
typedef long LONG;
typedef unsigned long DWORD;
typedef unsigned long ULONG;
typedef short SHORT;
typedef unsigned short USHORT;
typedef long long LONGLONG;
typedef unsigned long long ULONGLONG;
typedef long long LONG64;
typedef unsigned long long ULONG64;
typedef ULONGLONG DWORDLONG;
typedef ULONGLONG DWORD64;
typedef ULONGLONG ULONGLONG_PTR;
typedef DWORD* LPDWORD;
typedef WORD* LPWORD;
typedef BYTE* LPBYTE;
typedef BYTE* PBYTE;
typedef char CHAR;
typedef unsigned char UCHAR;
typedef UINT* PUINT;
typedef BOOL* PBOOL;
typedef BOOL* LPBOOL;
typedef DWORD* PDWORD;
typedef UCHAR* PUCHAR;
typedef locale_t _locale_t;
typedef wchar_t WCHAR;
typedef WCHAR TCHAR;
typedef WCHAR OLECHAR;
typedef float FLOAT;
typedef double DOUBLE;
typedef void* LPVOID;
typedef const void* LPCVOID;
typedef void* PVOID;
typedef char* LPSTR;
typedef const char* LPCSTR;
typedef wchar_t* LPWSTR;
typedef const wchar_t* LPCWSTR;
typedef wchar_t* LPTSTR;
typedef const wchar_t* LPCTSTR;
typedef WCHAR* BSTR;
typedef WCHAR* POLESTR;
typedef const WCHAR* PCOLESTR;
typedef DWORD COLORREF;
typedef DWORD* LPCOLORREF;
typedef intptr_t LONG_PTR;
typedef uintptr_t ULONG_PTR;
typedef uintptr_t DWORD_PTR;
typedef uintptr_t UINT_PTR;
typedef intptr_t INT_PTR;
typedef ULONG_PTR SIZE_T;
typedef LONG_PTR SSIZE_T;
typedef UINT_PTR WPARAM;
typedef LONG_PTR LPARAM;
typedef LONG_PTR LRESULT;
typedef int HRESULT;
typedef LONG NTSTATUS;
typedef DWORD ACCESS_MASK;
typedef UINT_PTR WNDPROC_PARAM;
typedef unsigned int UINT32;
typedef int INT32;
typedef short INT16;
typedef unsigned short UINT16;
typedef int64_t INT64;
typedef uint64_t UINT64;
typedef void* HCRYPTPROV;
typedef void* HCRYPTHASH;
typedef void* HCRYPTKEY;
typedef void* BCRYPT_ALG_HANDLE;
typedef void* BCRYPT_HASH_HANDLE;
typedef void* BCRYPT_KEY_HANDLE;
typedef unsigned char UCHAR;
typedef ULONG* PULONG;
typedef USHORT* PUSHORT;
typedef LONG* PLONG;
typedef CHAR* PCHAR;
typedef WCHAR* PWCHAR;
typedef TCHAR* PTCHAR;
typedef DWORD LCID;
typedef DWORD LANGID;
typedef DWORD LCTYPE;
typedef DWORD LGRPID;
typedef WORD ATOM;
typedef int HFILE;
typedef DWORD WINT;

typedef LONG_PTR LSTATUS;
typedef DWORD SECURITY_INFORMATION;
typedef PVOID PSID;
typedef DWORD REGSAM;

#ifndef INVALID_HANDLE_VALUE
#define INVALID_HANDLE_VALUE ((HANDLE)(LONG_PTR)-1)
#endif
#ifndef INVALID_FILE_ATTRIBUTES
#define INVALID_FILE_ATTRIBUTES ((DWORD)-1)
#endif
#ifndef INVALID_FILE_SIZE
#define INVALID_FILE_SIZE ((DWORD)0xFFFFFFFF)
#endif
#ifndef INVALID_SET_FILE_POINTER
#define INVALID_SET_FILE_POINTER ((DWORD)-1)
#endif

#define DECLARE_HANDLE(name) typedef HANDLE name

typedef INT_PTR (CALLBACK *FARPROC)();
typedef INT_PTR (CALLBACK *NEARPROC)();
typedef INT_PTR (CALLBACK *PROC)();

struct HWND__;
struct HMENU__;
struct HDC__;
struct HINSTANCE__;

typedef struct _GUID {
	DWORD Data1;
	WORD Data2;
	WORD Data3;
	BYTE Data4[8];
} GUID;
typedef GUID CLSID;
typedef GUID IID;
typedef GUID UUID;
typedef GUID FMTID;
typedef const GUID* REFGUID;
typedef const IID* REFIID;
typedef const CLSID* REFCLSID;
#define REFIID const IID &
#ifndef _REFGUID_DEFINED
// keep pointer-style as well via macros below
#endif
#undef REFIID
#define REFIID const IID &
#undef REFCLSID
#define REFCLSID const CLSID &
#undef REFGUID
#define REFGUID const GUID &

typedef GUID* LPGUID;
#define DECLSPEC_ALIGN(x)

inline bool operator==(const GUID& a, const GUID& b)
{
	return std::memcmp(&a, &b, sizeof(GUID)) == 0;
}
inline bool operator!=(const GUID& a, const GUID& b) { return !(a == b); }

typedef struct _FILETIME {
	DWORD dwLowDateTime;
	DWORD dwHighDateTime;
} FILETIME, *PFILETIME, *LPFILETIME;

typedef struct _SYSTEMTIME {
	WORD wYear;
	WORD wMonth;
	WORD wDayOfWeek;
	WORD wDay;
	WORD wHour;
	WORD wMinute;
	WORD wSecond;
	WORD wMilliseconds;
} SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME;

typedef union _LARGE_INTEGER {
	struct { DWORD LowPart; LONG HighPart; };
	struct { DWORD LowPart; LONG HighPart; } u;
	LONGLONG QuadPart;
} LARGE_INTEGER, *PLARGE_INTEGER;

typedef union _ULARGE_INTEGER {
	struct { DWORD LowPart; DWORD HighPart; };
	struct { DWORD LowPart; DWORD HighPart; } u;
	ULONGLONG QuadPart;
} ULARGE_INTEGER, *PULARGE_INTEGER;

typedef struct tagPOINT {
	LONG x;
	LONG y;
} POINT, *PPOINT, *LPPOINT;

typedef struct tagSIZE {
	LONG cx;
	LONG cy;
} SIZE, *PSIZE, *LPSIZE, SIZEL;

typedef struct tagRECT {
	LONG left;
	LONG top;
	LONG right;
	LONG bottom;
} RECT, *PRECT, *LPRECT, RECTL;
typedef const RECT* LPCRECT;

typedef struct tagPOINTS {
	SHORT x;
	SHORT y;
} POINTS, *PPOINTS;

typedef struct tagPAINTSTRUCT {
	HDC hdc;
	BOOL fErase;
	RECT rcPaint;
	BOOL fRestore;
	BOOL fIncUpdate;
	BYTE rgbReserved[32];
} PAINTSTRUCT, *PPAINTSTRUCT, *LPPAINTSTRUCT;

typedef struct tagMSG {
	HWND hwnd;
	UINT message;
	WPARAM wParam;
	LPARAM lParam;
	DWORD time;
	POINT pt;
} MSG, *PMSG, *LPMSG;

typedef struct tagCREATESTRUCTW {
	LPVOID lpCreateParams;
	HINSTANCE hInstance;
	HMENU hMenu;
	HWND hwndParent;
	int cy;
	int cx;
	int y;
	int x;
	LONG style;
	LPCWSTR lpszName;
	LPCWSTR lpszClass;
	DWORD dwExStyle;
} CREATESTRUCTW, CREATESTRUCT, *LPCREATESTRUCTW, *LPCREATESTRUCT;

typedef struct tagWINDOWPOS {
	HWND hwnd;
	HWND hwndInsertAfter;
	int x;
	int y;
	int cx;
	int cy;
	UINT flags;
} WINDOWPOS, *LPWINDOWPOS, *PWINDOWPOS;

typedef struct tagMINMAXINFO {
	POINT ptReserved;
	POINT ptMaxSize;
	POINT ptMaxPosition;
	POINT ptMinTrackSize;
	POINT ptMaxTrackSize;
} MINMAXINFO, *PMINMAXINFO, *LPMINMAXINFO;

typedef struct tagWINDOWPLACEMENT {
	UINT length;
	UINT flags;
	UINT showCmd;
	POINT ptMinPosition;
	POINT ptMaxPosition;
	RECT rcNormalPosition;
} WINDOWPLACEMENT, *PWINDOWPLACEMENT, *LPWINDOWPLACEMENT;

typedef struct tagSCROLLINFO {
	UINT cbSize;
	UINT fMask;
	int nMin;
	int nMax;
	UINT nPage;
	int nPos;
	int nTrackPos;
} SCROLLINFO, *LPSCROLLINFO;
typedef const SCROLLINFO* LPCSCROLLINFO;

typedef struct tagSCROLLBARINFO {
	DWORD cbSize;
	RECT rcScrollBar;
	int dxyLineButton;
	int xyThumbTop;
	int xyThumbBottom;
	int reserved;
	DWORD rgstate[6];
} SCROLLBARINFO, *PSCROLLBARINFO, *LPSCROLLBARINFO;

typedef struct tagSTYLESTRUCT {
	DWORD styleOld;
	DWORD styleNew;
} STYLESTRUCT, *LPSTYLESTRUCT;

typedef struct tagNCCALCSIZE_PARAMS {
	RECT rgrc[3];
	PWINDOWPOS lppos;
} NCCALCSIZE_PARAMS, *LPNCCALCSIZE_PARAMS;

typedef struct tagDRAWITEMSTRUCT {
	UINT CtlType;
	UINT CtlID;
	UINT itemID;
	UINT itemAction;
	UINT itemState;
	HWND hwndItem;
	HDC hDC;
	RECT rcItem;
	ULONG_PTR itemData;
} DRAWITEMSTRUCT, *PDRAWITEMSTRUCT, *LPDRAWITEMSTRUCT;

typedef struct tagMEASUREITEMSTRUCT {
	UINT CtlType;
	UINT CtlID;
	UINT itemID;
	UINT itemWidth;
	UINT itemHeight;
	ULONG_PTR itemData;
} MEASUREITEMSTRUCT, *PMEASUREITEMSTRUCT, *LPMEASUREITEMSTRUCT;

typedef struct tagDELETEITEMSTRUCT {
	UINT CtlType;
	UINT CtlID;
	UINT itemID;
	HWND hwndItem;
	ULONG_PTR itemData;
} DELETEITEMSTRUCT;

typedef struct tagCOMPAREITEMSTRUCT {
	UINT CtlType;
	UINT CtlID;
	HWND hwndItem;
	UINT itemID1;
	ULONG_PTR itemData1;
	UINT itemID2;
	ULONG_PTR itemData2;
	DWORD dwLocaleId;
} COMPAREITEMSTRUCT;

typedef struct _NMHDR {
	HWND hwndFrom;
	UINT_PTR idFrom;
	UINT code;
} NMHDR, *LPNMHDR;

typedef struct tagNMMOUSE {
	NMHDR hdr;
	DWORD_PTR dwItemSpec;
	DWORD_PTR dwItemData;
	POINT pt;
	LPARAM dwHitInfo;
} NMMOUSE, *LPNMMOUSE;

typedef struct tagNMCUSTOMDRAWINFO {
	NMHDR hdr;
	DWORD dwDrawStage;
	HDC hdc;
	RECT rc;
	DWORD_PTR dwItemSpec;
	UINT uItemState;
	LPARAM lItemlParam;
} NMCUSTOMDRAW, *LPNMCUSTOMDRAW;

typedef struct tagNMTBKCUSTOMDRAW {
	NMCUSTOMDRAW nmcd;
	HBRUSH hbrMonoDither;
	HBRUSH hbrLines;
	HPEN hpenLines;
	COLORREF clrText;
	COLORREF clrTextHighlight;
	COLORREF clrBtnFace;
	COLORREF clrBtnHighlight;
	COLORREF clrHighlightHotTrack;
	RECT rcText;
	int nStringBkMode;
	int nHLStringBkMode;
	int iListGap;
} NMTBCUSTOMDRAW, *LPNMTBCUSTOMDRAW;

typedef LRESULT (CALLBACK *WNDPROC)(HWND, UINT, WPARAM, LPARAM);
typedef INT_PTR (CALLBACK *DLGPROC)(HWND, UINT, WPARAM, LPARAM);
typedef BOOL (CALLBACK *DLGPROCBOOL)(HWND, UINT, WPARAM, LPARAM);
typedef LRESULT (CALLBACK *HOOKPROC)(int code, WPARAM wParam, LPARAM lParam);
typedef BOOL (CALLBACK *WNDENUMPROC)(HWND, LPARAM);
typedef BOOL (CALLBACK *MONITORENUMPROC)(HMONITOR, HDC, LPRECT, LPARAM);
typedef DWORD (WINAPI *PTHREAD_START_ROUTINE)(LPVOID);
typedef PTHREAD_START_ROUTINE LPTHREAD_START_ROUTINE;
typedef UINT (CALLBACK *LPCCHOOKPROC)(HWND, UINT, WPARAM, LPARAM);
typedef UINT (CALLBACK *LPOFNHOOKPROC)(HWND, UINT, WPARAM, LPARAM);
typedef UINT (CALLBACK *LPPRINTHOOKPROC)(HWND, UINT, WPARAM, LPARAM);
typedef UINT (CALLBACK *LPSETUPHOOKPROC)(HWND, UINT, WPARAM, LPARAM);
typedef int (CALLBACK *FONTENUMPROCW)(const void*, const void*, DWORD, LPARAM);
typedef BOOL (CALLBACK *PROPENUMPROCW)(HWND, LPCWSTR, HANDLE);
typedef VOID (CALLBACK *SENDASYNCPROC)(HWND, UINT, ULONG_PTR, LRESULT);
typedef VOID (CALLBACK *TIMERPROC)(HWND, UINT, UINT_PTR, DWORD);
typedef BOOL (CALLBACK *DRAWSTATEPROC)(HDC, LPARAM, WPARAM, int, int);
typedef int (CALLBACK *BFFCALLBACK)(HWND, UINT, LPARAM, LPARAM);

typedef struct tagWNDCLASSW {
	UINT style;
	WNDPROC lpfnWndProc;
	int cbClsExtra;
	int cbWndExtra;
	HINSTANCE hInstance;
	HICON hIcon;
	HCURSOR hCursor;
	HBRUSH hbrBackground;
	LPCWSTR lpszMenuName;
	LPCWSTR lpszClassName;
} WNDCLASSW, WNDCLASS, *PWNDCLASSW, *LPWNDCLASSW, *PWNDCLASS, *LPWNDCLASS;

typedef struct tagWNDCLASSEXW {
	UINT cbSize;
	UINT style;
	WNDPROC lpfnWndProc;
	int cbClsExtra;
	int cbWndExtra;
	HINSTANCE hInstance;
	HICON hIcon;
	HCURSOR hCursor;
	HBRUSH hbrBackground;
	LPCWSTR lpszMenuName;
	LPCWSTR lpszClassName;
	HICON hIconSm;
} WNDCLASSEXW, WNDCLASSEX, *PWNDCLASSEXW, *LPWNDCLASSEXW;

typedef struct tagLOGFONTW {
	LONG lfHeight;
	LONG lfWidth;
	LONG lfEscapement;
	LONG lfOrientation;
	LONG lfWeight;
	BYTE lfItalic;
	BYTE lfUnderline;
	BYTE lfStrikeOut;
	BYTE lfCharSet;
	BYTE lfOutPrecision;
	BYTE lfClipPrecision;
	BYTE lfQuality;
	BYTE lfPitchAndFamily;
	WCHAR lfFaceName[32];
} LOGFONTW, LOGFONT, *PLOGFONTW, *LPLOGFONTW, *PLOGFONT, *LPLOGFONT;

typedef struct tagTEXTMETRICW {
	LONG tmHeight;
	LONG tmAscent;
	LONG tmDescent;
	LONG tmInternalLeading;
	LONG tmExternalLeading;
	LONG tmAveCharWidth;
	LONG tmMaxCharWidth;
	LONG tmWeight;
	LONG tmOverhang;
	LONG tmDigitizedAspectX;
	LONG tmDigitizedAspectY;
	WCHAR tmFirstChar;
	WCHAR tmLastChar;
	WCHAR tmDefaultChar;
	WCHAR tmBreakChar;
	BYTE tmItalic;
	BYTE tmUnderlined;
	BYTE tmStruckOut;
	BYTE tmPitchAndFamily;
	BYTE tmCharSet;
} TEXTMETRICW, TEXTMETRIC, *PTEXTMETRICW, *LPTEXTMETRICW;

typedef struct tagBITMAP {
	LONG bmType;
	LONG bmWidth;
	LONG bmHeight;
	LONG bmWidthBytes;
	WORD bmPlanes;
	WORD bmBitsPixel;
	LPVOID bmBits;
} BITMAP, *PBITMAP, *LPBITMAP;

typedef struct tagBITMAPINFOHEADER {
	DWORD biSize;
	LONG biWidth;
	LONG biHeight;
	WORD biPlanes;
	WORD biBitCount;
	DWORD biCompression;
	DWORD biSizeImage;
	LONG biXPelsPerMeter;
	LONG biYPelsPerMeter;
	DWORD biClrUsed;
	DWORD biClrImportant;
} BITMAPINFOHEADER, *LPBITMAPINFOHEADER, *PBITMAPINFOHEADER;

typedef struct tagRGBQUAD {
	BYTE rgbBlue;
	BYTE rgbGreen;
	BYTE rgbRed;
	BYTE rgbReserved;
} RGBQUAD;

typedef struct tagBITMAPINFO {
	BITMAPINFOHEADER bmiHeader;
	RGBQUAD bmiColors[1];
} BITMAPINFO, *LPBITMAPINFO, *PBITMAPINFO;

typedef struct tagLOGBRUSH {
	UINT lbStyle;
	COLORREF lbColor;
	ULONG_PTR lbHatch;
} LOGBRUSH, *PLOGBRUSH;

typedef struct tagLOGPEN {
	UINT lopnStyle;
	POINT lopnWidth;
	COLORREF lopnColor;
} LOGPEN, *PLOGPEN;

typedef struct _ICONINFO {
	BOOL fIcon;
	DWORD xHotspot;
	DWORD yHotspot;
	HBITMAP hbmMask;
	HBITMAP hbmColor;
} ICONINFO, *PICONINFO;

typedef struct tagNONCLIENTMETRICSW {
	UINT cbSize;
	int iBorderWidth;
	int iScrollWidth;
	int iScrollHeight;
	int iCaptionWidth;
	int iCaptionHeight;
	LOGFONTW lfCaptionFont;
	int iSmCaptionWidth;
	int iSmCaptionHeight;
	LOGFONTW lfSmCaptionFont;
	int iMenuWidth;
	int iMenuHeight;
	LOGFONTW lfMenuFont;
	LOGFONTW lfStatusFont;
	LOGFONTW lfMessageFont;
	int iPaddedBorderWidth;
} NONCLIENTMETRICSW, NONCLIENTMETRICS, *PNONCLIENTMETRICSW, *LPNONCLIENTMETRICS;

typedef struct _BLENDFUNCTION {
	BYTE BlendOp;
	BYTE BlendFlags;
	BYTE SourceConstantAlpha;
	BYTE AlphaFormat;
} BLENDFUNCTION, *PBLENDFUNCTION;

typedef struct _DOCINFOW {
	int cbSize;
	LPCWSTR lpszDocName;
	LPCWSTR lpszOutput;
	LPCWSTR lpszDatatype;
	DWORD fwType;
} DOCINFOW, DOCINFO, *LPDOCINFOW;

typedef struct tagMONITORINFO {
	DWORD cbSize;
	RECT rcMonitor;
	RECT rcWork;
	DWORD dwFlags;
} MONITORINFO, *LPMONITORINFO;

typedef struct tagMONITORINFOEXW {
	DWORD cbSize;
	RECT rcMonitor;
	RECT rcWork;
	DWORD dwFlags;
	WCHAR szDevice[32];
} MONITORINFOEXW, MONITORINFOEX, *LPMONITORINFOEXW;

typedef struct _OSVERSIONINFOW {
	DWORD dwOSVersionInfoSize;
	DWORD dwMajorVersion;
	DWORD dwMinorVersion;
	DWORD dwBuildNumber;
	DWORD dwPlatformId;
	WCHAR szCSDVersion[128];
} OSVERSIONINFOW, OSVERSIONINFO, *POSVERSIONINFOW, *LPOSVERSIONINFOW;

typedef struct _OSVERSIONINFOEXW {
	DWORD dwOSVersionInfoSize;
	DWORD dwMajorVersion;
	DWORD dwMinorVersion;
	DWORD dwBuildNumber;
	DWORD dwPlatformId;
	WCHAR szCSDVersion[128];
	WORD wServicePackMajor;
	WORD wServicePackMinor;
	WORD wSuiteMask;
	BYTE wProductType;
	BYTE wReserved;
} OSVERSIONINFOEXW, OSVERSIONINFOEX, *POSVERSIONINFOEXW, *LPOSVERSIONINFOEXW;

typedef struct _MEMORYSTATUSEX {
	DWORD dwLength;
	DWORD dwMemoryLoad;
	DWORDLONG ullTotalPhys;
	DWORDLONG ullAvailPhys;
	DWORDLONG ullTotalPageFile;
	DWORDLONG ullAvailPageFile;
	DWORDLONG ullTotalVirtual;
	DWORDLONG ullAvailVirtual;
	DWORDLONG ullAvailExtendedVirtual;
} MEMORYSTATUSEX, *LPMEMORYSTATUSEX;

typedef struct _SYSTEM_INFO {
	union {
		DWORD dwOemId;
		struct {
			WORD wProcessorArchitecture;
			WORD wReserved;
		};
	};
	DWORD dwPageSize;
	LPVOID lpMinimumApplicationAddress;
	LPVOID lpMaximumApplicationAddress;
	DWORD_PTR dwActiveProcessorMask;
	DWORD dwNumberOfProcessors;
	DWORD dwProcessorType;
	DWORD dwAllocationGranularity;
	WORD wProcessorLevel;
	WORD wProcessorRevision;
} SYSTEM_INFO, *LPSYSTEM_INFO;

typedef struct _SECURITY_ATTRIBUTES {
	DWORD nLength;
	LPVOID lpSecurityDescriptor;
	BOOL bInheritHandle;
} SECURITY_ATTRIBUTES, *PSECURITY_ATTRIBUTES, *LPSECURITY_ATTRIBUTES;

typedef struct _RTL_CRITICAL_SECTION {
	PVOID DebugInfo;
	LONG LockCount;
	LONG RecursionCount;
	HANDLE OwningThread;
	HANDLE LockSemaphore;
	ULONG_PTR SpinCount;
	PVOID posixMutex;
} CRITICAL_SECTION, *LPCRITICAL_SECTION, *PCRITICAL_SECTION;
typedef CRITICAL_SECTION RTL_CRITICAL_SECTION;

#pragma pack(push, 2)
typedef struct {
	DWORD style;
	DWORD dwExtendedStyle;
	WORD cdit;
	short x;
	short y;
	short cx;
	short cy;
} DLGTEMPLATE, *LPDLGTEMPLATE, *LPDLGTEMPLATEW, *LPDLGTEMPLATEA;

typedef struct {
	DWORD style;
	DWORD dwExtendedStyle;
	short x;
	short y;
	short cx;
	short cy;
	WORD id;
} DLGITEMTEMPLATE, *PDLGITEMTEMPLATE, *LPDLGITEMTEMPLATE;

// DLGTEMPLATEEX is defined by Notepad++ (StaticDialog.h). Only the item layout lives here.
typedef struct {
	DWORD helpID;
	DWORD exStyle;
	DWORD style;
	short x;
	short y;
	short cx;
	short cy;
	DWORD id;
} DLGITEMTEMPLATEEX;
#pragma pack(pop)

typedef struct _OVERLAPPED {
	ULONG_PTR Internal;
	ULONG_PTR InternalHigh;
	union {
		struct { DWORD Offset; DWORD OffsetHigh; };
		PVOID Pointer;
	};
	HANDLE hEvent;
} OVERLAPPED, *LPOVERLAPPED;

typedef struct _WIN32_FIND_DATAW {
	DWORD dwFileAttributes;
	FILETIME ftCreationTime;
	FILETIME ftLastAccessTime;
	FILETIME ftLastWriteTime;
	DWORD nFileSizeHigh;
	DWORD nFileSizeLow;
	DWORD dwReserved0;
	DWORD dwReserved1;
	WCHAR cFileName[MAX_PATH];
	WCHAR cAlternateFileName[14];
} WIN32_FIND_DATAW, WIN32_FIND_DATA, *PWIN32_FIND_DATAW, *LPWIN32_FIND_DATAW, *LPWIN32_FIND_DATA;

typedef struct _WIN32_FILE_ATTRIBUTE_DATA {
	DWORD dwFileAttributes;
	FILETIME ftCreationTime;
	FILETIME ftLastAccessTime;
	FILETIME ftLastWriteTime;
	DWORD nFileSizeHigh;
	DWORD nFileSizeLow;
} WIN32_FILE_ATTRIBUTE_DATA, *LPWIN32_FILE_ATTRIBUTE_DATA;

typedef struct _BY_HANDLE_FILE_INFORMATION {
	DWORD dwFileAttributes;
	FILETIME ftCreationTime;
	FILETIME ftLastAccessTime;
	FILETIME ftLastWriteTime;
	DWORD dwVolumeSerialNumber;
	DWORD nFileSizeHigh;
	DWORD nFileSizeLow;
	DWORD nNumberOfLinks;
	DWORD nFileIndexHigh;
	DWORD nFileIndexLow;
} BY_HANDLE_FILE_INFORMATION, *PBY_HANDLE_FILE_INFORMATION, *LPBY_HANDLE_FILE_INFORMATION;

typedef struct _WIN32_FIND_STREAM_DATA {
	LARGE_INTEGER StreamSize;
	WCHAR cStreamName[296];
} WIN32_FIND_STREAM_DATA, *PWIN32_FIND_STREAM_DATA;

typedef enum _STREAM_INFO_LEVELS {
	FindStreamInfoStandard,
	FindStreamInfoMaxInfoLevel
} STREAM_INFO_LEVELS;

typedef struct _STARTUPINFOW {
	DWORD cb;
	LPWSTR lpReserved;
	LPWSTR lpDesktop;
	LPWSTR lpTitle;
	DWORD dwX;
	DWORD dwY;
	DWORD dwXSize;
	DWORD dwYSize;
	DWORD dwXCountChars;
	DWORD dwYCountChars;
	DWORD dwFillAttribute;
	DWORD dwFlags;
	WORD wShowWindow;
	WORD cbReserved2;
	LPBYTE lpReserved2;
	HANDLE hStdInput;
	HANDLE hStdOutput;
	HANDLE hStdError;
} STARTUPINFOW, STARTUPINFO, *LPSTARTUPINFOW;

typedef struct _PROCESS_INFORMATION {
	HANDLE hProcess;
	HANDLE hThread;
	DWORD dwProcessId;
	DWORD dwThreadId;
} PROCESS_INFORMATION, *PPROCESS_INFORMATION, *LPPROCESS_INFORMATION;

typedef struct _SHELLEXECUTEINFOW {
	DWORD cbSize;
	ULONG fMask;
	HWND hwnd;
	LPCWSTR lpVerb;
	LPCWSTR lpFile;
	LPCWSTR lpParameters;
	LPCWSTR lpDirectory;
	int nShow;
	HINSTANCE hInstApp;
	void* lpIDList;
	LPCWSTR lpClass;
	HKEY hkeyClass;
	DWORD dwHotKey;
	HANDLE hProcess;
	union {
		HANDLE hIcon;
		HANDLE hMonitor;
	};
} SHELLEXECUTEINFOW, SHELLEXECUTEINFO, *LPSHELLEXECUTEINFOW;

typedef struct _NOTIFYICONDATAW {
	DWORD cbSize;
	HWND hWnd;
	UINT uID;
	UINT uFlags;
	UINT uCallbackMessage;
	HICON hIcon;
	WCHAR szTip[128];
	DWORD dwState;
	DWORD dwStateMask;
	WCHAR szInfo[256];
	union { UINT uTimeout; UINT uVersion; };
	WCHAR szInfoTitle[64];
	DWORD dwInfoFlags;
	GUID guidItem;
	HICON hBalloonIcon;
} NOTIFYICONDATAW, NOTIFYICONDATA, *PNOTIFYICONDATAW;

typedef struct tagMENUITEMINFOW {
	UINT cbSize;
	UINT fMask;
	UINT fType;
	UINT fState;
	UINT wID;
	HMENU hSubMenu;
	HBITMAP hbmpChecked;
	HBITMAP hbmpUnchecked;
	ULONG_PTR dwItemData;
	LPWSTR dwTypeData;
	UINT cch;
	HBITMAP hbmpItem;
} MENUITEMINFOW, MENUITEMINFO, *LPMENUITEMINFOW, *LPMENUITEMINFO;
typedef const MENUITEMINFOW* LPCMENUITEMINFOW;

typedef struct tagMENUINFO {
	DWORD cbSize;
	DWORD fMask;
	DWORD dwStyle;
	UINT cyMax;
	HBRUSH hbrBack;
	DWORD dwContextHelpID;
	ULONG_PTR dwMenuData;
} MENUINFO, *LPMENUINFO;
typedef const MENUINFO* LPCMENUINFO;

typedef struct tagTPMPARAMS {
	UINT cbSize;
	RECT rcExclude;
} TPMPARAMS, *LPTPMPARAMS;

typedef struct tagACCEL {
	BYTE fVirt;
	WORD key;
	WORD cmd;
} ACCEL, *LPACCEL;

typedef struct tagCOPYDATASTRUCT {
	ULONG_PTR dwData;
	DWORD cbData;
	PVOID lpData;
} COPYDATASTRUCT, *PCOPYDATASTRUCT;

typedef struct _DROPFILES {
	DWORD pFiles;
	POINT pt;
	BOOL fNC;
	BOOL fWide;
} DROPFILES, *LPDROPFILES;

typedef struct tagHELPINFO {
	UINT cbSize;
	int iContextType;
	int iCtrlId;
	HANDLE hItemHandle;
	DWORD_PTR dwContextId;
	POINT MousePos;
} HELPINFO, *LPHELPINFO;

typedef struct _EXCEPTION_RECORD {
	DWORD ExceptionCode;
	DWORD ExceptionFlags;
	struct _EXCEPTION_RECORD* ExceptionRecord;
	PVOID ExceptionAddress;
	DWORD NumberParameters;
	ULONG_PTR ExceptionInformation[15];
} EXCEPTION_RECORD, *PEXCEPTION_RECORD;

typedef struct _CONTEXT {
	DWORD ContextFlags;
	DWORD dummy;
} CONTEXT, *PCONTEXT, *LPCONTEXT;

typedef struct _EXCEPTION_POINTERS {
	PEXCEPTION_RECORD ExceptionRecord;
	PCONTEXT ContextRecord;
} EXCEPTION_POINTERS, *PEXCEPTION_POINTERS;

typedef LONG (CALLBACK *PTOP_LEVEL_EXCEPTION_FILTER)(EXCEPTION_POINTERS*);
typedef PTOP_LEVEL_EXCEPTION_FILTER LPTOP_LEVEL_EXCEPTION_FILTER;
typedef void (*_se_translator_function)(unsigned int, EXCEPTION_POINTERS*);

typedef struct _MINIDUMP_EXCEPTION_INFORMATION {
	DWORD ThreadId;
	PEXCEPTION_POINTERS ExceptionPointers;
	BOOL ClientPointers;
} MINIDUMP_EXCEPTION_INFORMATION, *PMINIDUMP_EXCEPTION_INFORMATION;
typedef void* PMINIDUMP_USER_STREAM_INFORMATION;
typedef void* PMINIDUMP_CALLBACK_INFORMATION;
typedef enum _MINIDUMP_TYPE {
	MiniDumpNormal = 0
} MINIDUMP_TYPE;

typedef struct tagCOMBOBOXINFO {
	DWORD cbSize;
	RECT rcItem;
	RECT rcButton;
	DWORD stateButton;
	HWND hwndCombo;
	HWND hwndItem;
	HWND hwndList;
} COMBOBOXINFO, *PCOMBOBOXINFO, *LPCOMBOBOXINFO;

typedef struct tagWINDOWINFO {
	DWORD cbSize;
	RECT rcWindow;
	RECT rcClient;
	DWORD dwStyle;
	DWORD dwExStyle;
	DWORD dwWindowStatus;
	UINT cxWindowBorders;
	UINT cyWindowBorders;
	ATOM atomWindowType;
	WORD wCreatorVersion;
} WINDOWINFO, *PWINDOWINFO, *LPWINDOWINFO;

typedef struct tagTITLEBARINFO {
	DWORD cbSize;
	RECT rcTitleBar;
	DWORD rgstate[6];
} TITLEBARINFO, *PTITLEBARINFO;

typedef struct tagFLASHWINFO {
	UINT cbSize;
	HWND hwnd;
	DWORD dwFlags;
	UINT uCount;
	DWORD dwTimeout;
} FLASHWINFO, *PFLASHWINFO;

typedef struct tagHIGHCONTRASTW {
	UINT cbSize;
	DWORD dwFlags;
	LPWSTR lpszDefaultScheme;
} HIGHCONTRASTW, HIGHCONTRAST, *LPHIGHCONTRASTW;

typedef struct _IMAGELISTDRAWPARAMS {
	DWORD cbSize;
	HIMAGELIST himl;
	int i;
	HDC hdcDst;
	int x;
	int y;
	int cx;
	int cy;
	int xBitmap;
	int yBitmap;
	COLORREF rgbBk;
	COLORREF rgbFg;
	UINT fStyle;
	DWORD dwRop;
	DWORD fState;
	DWORD Frame;
	COLORREF crEffect;
} IMAGELISTDRAWPARAMS, *LPIMAGELISTDRAWPARAMS;

typedef struct _IMAGEINFO {
	HBITMAP hbmImage;
	HBITMAP hbmMask;
	int Unused1;
	int Unused2;
	RECT rcImage;
} IMAGEINFO, *LPIMAGEINFO;

typedef struct tagTOOLINFOW {
	UINT cbSize;
	UINT uFlags;
	HWND hwnd;
	UINT_PTR uId;
	RECT rect;
	HINSTANCE hinst;
	LPWSTR lpszText;
	LPARAM lParam;
	void* lpReserved;
} TOOLINFOW, TOOLINFO, TTTOOLINFOW, *PTOOLINFOW, *LPTOOLINFOW, *LPTTTOOLINFOW;

typedef struct tagTBBUTTON {
	int iBitmap;
	int idCommand;
	BYTE fsState;
	BYTE fsStyle;
	BYTE bReserved[6];
	DWORD_PTR dwData;
	INT_PTR iString;
} TBBUTTON, *PTBBUTTON, *LPTBBUTTON;
typedef const TBBUTTON* LPCTBBUTTON;

typedef struct _TBBUTTONINFOW {
	UINT cbSize;
	DWORD dwMask;
	int idCommand;
	int iImage;
	BYTE fsState;
	BYTE fsStyle;
	WORD cx;
	DWORD_PTR lParam;
	LPWSTR pszText;
	int cchText;
} TBBUTTONINFOW, TBBUTTONINFO, *LPTBBUTTONINFOW;

typedef struct tagNMTOOLBARW {
	NMHDR hdr;
	int iItem;
	TBBUTTON tbButton;
	int cchText;
	LPWSTR pszText;
	RECT rcButton;
} NMTOOLBARW, NMTOOLBAR, *LPNMTOOLBARW;

typedef struct tagTBADDBITMAP {
	HINSTANCE hInst;
	UINT_PTR nID;
} TBADDBITMAP, *LPTBADDBITMAP;

typedef struct {
	int iButton;
	DWORD dwFlags;
} TBINSERTMARK, *LPTBINSERTMARK;

typedef struct tagREBARBANDINFOW {
	UINT cbSize;
	UINT fMask;
	UINT fStyle;
	COLORREF clrFore;
	COLORREF clrBack;
	LPWSTR lpText;
	UINT cch;
	int iImage;
	HWND hwndChild;
	UINT cxMinChild;
	UINT cyMinChild;
	UINT cx;
	HBITMAP hbmBack;
	UINT wID;
	UINT cyChild;
	UINT cyMaxChild;
	UINT cyIntegral;
	UINT cxIdeal;
	LPARAM lParam;
	UINT cxHeader;
	RECT rcChevronLocation;
	UINT uChevronState;
} REBARBANDINFOW, REBARBANDINFO, *LPREBARBANDINFOW;

typedef struct tagTCITEMW {
	UINT mask;
	DWORD dwState;
	DWORD dwStateMask;
	LPWSTR pszText;
	int cchTextMax;
	int iImage;
	LPARAM lParam;
} TCITEMW, TCITEM, *LPTCITEMW, *LPTCITEM;

typedef struct tagTCHITTESTINFO {
	POINT pt;
	UINT flags;
} TCHITTESTINFO, *LPTCHITTESTINFO;

typedef struct tagLVITEMW {
	UINT mask;
	int iItem;
	int iSubItem;
	UINT state;
	UINT stateMask;
	LPWSTR pszText;
	int cchTextMax;
	int iImage;
	LPARAM lParam;
	int iIndent;
	int iGroupId;
	UINT cColumns;
	PUINT puColumns;
	int* piColFmt;
	int iGroup;
} LVITEMW, LVITEM, *LPLVITEMW, *LPLVITEM;

typedef struct tagLVCOLUMNW {
	UINT mask;
	int fmt;
	int cx;
	LPWSTR pszText;
	int cchTextMax;
	int iSubItem;
	int iImage;
	int iOrder;
	int cxMin;
	int cxDefault;
	int cxIdeal;
} LVCOLUMNW, LVCOLUMN, *LPLVCOLUMNW;

typedef struct tagLVHITTESTINFO {
	POINT pt;
	UINT flags;
	int iItem;
	int iSubItem;
	int iGroup;
} LVHITTESTINFO, *LPLVHITTESTINFO;

typedef struct tagLVFINDINFOW {
	UINT flags;
	LPCWSTR psz;
	LPARAM lParam;
	POINT pt;
	UINT vkDirection;
} LVFINDINFOW, LVFINDINFO, *LPFINDINFOW;

typedef struct tagNMLISTVIEW {
	NMHDR hdr;
	int iItem;
	int iSubItem;
	UINT uNewState;
	UINT uOldState;
	UINT uChanged;
	POINT ptAction;
	LPARAM lParam;
} NMLISTVIEW, *LPNMLISTVIEW;

typedef struct tagNMLVDISPINFOW {
	NMHDR hdr;
	LVITEMW item;
} NMLVDISPINFOW, NMLVDISPINFO, *LPNMLVDISPINFOW;

typedef struct tagNMLVCUSTOMDRAW {
	NMCUSTOMDRAW nmcd;
	COLORREF clrText;
	COLORREF clrTextBk;
	int iSubItem;
	DWORD dwItemType;
	COLORREF clrFace;
	int iIconEffect;
	int iIconPhase;
	int iPartId;
	int iStateId;
	RECT rcText;
	UINT uAlign;
} NMLVCUSTOMDRAW, *LPNMLVCUSTOMDRAW;

typedef struct tagNMLVGETINFOTIPW {
	NMHDR hdr;
	DWORD dwFlags;
	LPWSTR pszText;
	int cchTextMax;
	int iItem;
	int iSubItem;
	LPARAM lParam;
} NMLVGETINFOTIPW, *LPNMLVGETINFOTIPW;

typedef struct tagTVITEMW {
	UINT mask;
	HANDLE hItem;
	UINT state;
	UINT stateMask;
	LPWSTR pszText;
	int cchTextMax;
	int iImage;
	int iSelectedImage;
	int cChildren;
	LPARAM lParam;
} TVITEMW, TVITEM, *LPTVITEMW, *LPTVITEM;
typedef HANDLE HTREEITEM;

typedef struct tagTVITEMEXW {
	UINT mask;
	HTREEITEM hItem;
	UINT state;
	UINT stateMask;
	LPWSTR pszText;
	int cchTextMax;
	int iImage;
	int iSelectedImage;
	int cChildren;
	LPARAM lParam;
	int iIntegral;
	UINT uStateEx;
	HWND hwnd;
	int iExpandedImage;
	int iReserved;
} TVITEMEXW, TVITEMEX, *LPTVITEMEXW;

typedef struct tagTVINSERTSTRUCTW {
	HTREEITEM hParent;
	HTREEITEM hInsertAfter;
	union {
		TVITEMEXW itemex;
		TVITEMW item;
	};
} TVINSERTSTRUCTW, TVINSERTSTRUCT, *LPTVINSERTSTRUCTW;

typedef struct tagTVHITTESTINFO {
	POINT pt;
	UINT flags;
	HTREEITEM hItem;
} TVHITTESTINFO, *LPTVHITTESTINFO;

typedef struct tagNMTREEVIEWW {
	NMHDR hdr;
	UINT action;
	TVITEMW itemOld;
	TVITEMW itemNew;
	POINT ptDrag;
} NMTREEVIEWW, NMTREEVIEW, *LPNMTREEVIEWW;

typedef struct tagNMTVCUSTOMDRAW {
	NMCUSTOMDRAW nmcd;
	COLORREF clrText;
	COLORREF clrTextBk;
	int iLevel;
} NMTVCUSTOMDRAW, *LPNMTVCUSTOMDRAW;

typedef struct tagNMTVDISPINFOW {
	NMHDR hdr;
	TVITEMW item;
} NMTVDISPINFOW, NMTVDISPINFO, *LPNMTVDISPINFOW;

typedef struct tagTVSORTCB {
	HTREEITEM hParent;
	int (CALLBACK *lpfnCompare)(LPARAM, LPARAM, LPARAM);
	LPARAM lParam;
} TVSORTCB, *LPTVSORTCB;

typedef struct _HD_ITEMW {
	UINT mask;
	int cxy;
	LPWSTR pszText;
	HBITMAP hbm;
	int cchTextMax;
	int fmt;
	LPARAM lParam;
	int iImage;
	int iOrder;
	UINT type;
	void* pvFilter;
	UINT state;
} HDITEMW, HD_ITEM, HDITEM, *LPHDITEMW;

typedef struct _HD_LAYOUT {
	RECT* prc;
	WINDOWPOS* pwpos;
} HDLAYOUT, *LPHDLAYOUT;

typedef struct tagNMHEADERW {
	NMHDR hdr;
	int iItem;
	int iButton;
	HDITEMW* pitem;
} NMHEADERW, NMHEADER, *LPNMHEADERW;

typedef struct tagPBRANGE {
	int iLow;
	int iHigh;
} PBRANGE, *PPBRANGE;

typedef struct tagNMBCHOTITEM {
	NMHDR hdr;
	DWORD dwFlags;
} NMBCHOTITEM, *LPNMBCHOTITEM;

typedef struct tagNMSELCHANGE {
	NMHDR nmhdr;
	SYSTEMTIME stSelStart;
	SYSTEMTIME stSelEnd;
} NMSELCHANGE, *LPNMSELCHANGE;

typedef struct tagOPENFILENAMEW {
	DWORD lStructSize;
	HWND hwndOwner;
	HINSTANCE hInstance;
	LPCWSTR lpstrFilter;
	LPWSTR lpstrCustomFilter;
	DWORD nMaxCustFilter;
	DWORD nFilterIndex;
	LPWSTR lpstrFile;
	DWORD nMaxFile;
	LPWSTR lpstrFileTitle;
	DWORD nMaxFileTitle;
	LPCWSTR lpstrInitialDir;
	LPCWSTR lpstrTitle;
	DWORD Flags;
	WORD nFileOffset;
	WORD nFileExtension;
	LPCWSTR lpstrDefExt;
	LPARAM lCustData;
	LPOFNHOOKPROC lpfnHook;
	LPCWSTR lpTemplateName;
	void* pvReserved;
	DWORD dwReserved;
	DWORD FlagsEx;
} OPENFILENAMEW, OPENFILENAME, *LPOPENFILENAMEW;

typedef struct tagCHOOSEFONTW {
	DWORD lStructSize;
	HWND hwndOwner;
	HDC hDC;
	LPLOGFONTW lpLogFont;
	INT iPointSize;
	DWORD Flags;
	COLORREF rgbColors;
	LPARAM lCustData;
	LPCCHOOKPROC lpfnHook;
	LPCWSTR lpTemplateName;
	HINSTANCE hInstance;
	LPWSTR lpszStyle;
	WORD nFontType;
	WORD ___MISSING_ALIGNMENT__;
	INT nSizeMin;
	INT nSizeMax;
} CHOOSEFONTW, CHOOSEFONT, *LPCHOOSEFONTW;

typedef struct tagCHOOSECOLORW {
	DWORD lStructSize;
	HWND hwndOwner;
	HWND hInstance;
	COLORREF rgbResult;
	COLORREF* lpCustColors;
	DWORD Flags;
	LPARAM lCustData;
	LPCCHOOKPROC lpfnHook;
	LPCWSTR lpTemplateName;
} CHOOSECOLORW, CHOOSECOLOR, *LPCHOOSECOLORW;

typedef struct tagPRINTDLGW {
	DWORD lStructSize;
	HWND hwndOwner;
	HANDLE hDevMode;
	HANDLE hDevNames;
	HDC hDC;
	DWORD Flags;
	WORD nFromPage;
	WORD nToPage;
	WORD nMinPage;
	WORD nMaxPage;
	WORD nCopies;
	HINSTANCE hInstance;
	LPARAM lCustData;
	LPPRINTHOOKPROC lpfnPrintHook;
	LPSETUPHOOKPROC lpfnSetupHook;
	LPCWSTR lpPrintTemplateName;
	LPCWSTR lpSetupTemplateName;
	HANDLE hPrintTemplate;
	HANDLE hSetupTemplate;
} PRINTDLGW, PRINTDLG, *LPPRINTDLGW;

typedef struct tagPOINTL { LONG x; LONG y; } POINTL;

typedef struct tagDEVMODEW {
	WCHAR dmDeviceName[32];
	WORD dmSpecVersion;
	WORD dmDriverVersion;
	WORD dmSize;
	WORD dmDriverExtra;
	DWORD dmFields;
	POINTL dmPosition;
	DWORD dmDisplayOrientation;
	DWORD dmDisplayFixedOutput;
	short dmColor;
	short dmDuplex;
	short dmYResolution;
	short dmTTOption;
	short dmCollate;
	WCHAR dmFormName[32];
	WORD dmLogPixels;
	DWORD dmBitsPerPel;
	DWORD dmPelsWidth;
	DWORD dmPelsHeight;
	DWORD dmDisplayFlags;
	DWORD dmDisplayFrequency;
	DWORD dmICMMethod;
	DWORD dmICMIntent;
	DWORD dmMediaType;
	DWORD dmDitherType;
	DWORD dmReserved1;
	DWORD dmReserved2;
	DWORD dmPanningWidth;
	DWORD dmPanningHeight;
} DEVMODEW, DEVMODE, *PDEVMODEW, *LPDEVMODEW;

typedef struct tagBROWSEINFOW {
	HWND hwndOwner;
	void* pidlRoot;
	LPWSTR pszDisplayName;
	LPCWSTR lpszTitle;
	UINT ulFlags;
	BFFCALLBACK lpfn;
	LPARAM lParam;
	int iImage;
} BROWSEINFOW, BROWSEINFO, *LPBROWSEINFOW;

typedef struct _SHFILEINFOW {
	HICON hIcon;
	int iIcon;
	DWORD dwAttributes;
	WCHAR szDisplayName[MAX_PATH];
	WCHAR szTypeName[80];
} SHFILEINFOW, SHFILEINFO, *PSHFILEINFOW;

typedef WORD FILEOP_FLAGS;

typedef struct _SHFILEOPSTRUCTW {
	HWND hwnd;
	UINT wFunc;
	LPCWSTR pFrom;
	LPCWSTR pTo;
	FILEOP_FLAGS fFlags;
	BOOL fAnyOperationsAborted;
	LPVOID hNameMappings;
	LPCWSTR lpszProgressTitle;
} SHFILEOPSTRUCTW, SHFILEOPSTRUCT, *LPSHFILEOPSTRUCTW;

typedef struct _ITEMIDLIST { USHORT dummy; } ITEMIDLIST, *LPITEMIDLIST;
typedef const ITEMIDLIST* LPCITEMIDLIST;
typedef ITEMIDLIST* PIDLIST_ABSOLUTE;
typedef ITEMIDLIST* PIDLIST_RELATIVE;
typedef ITEMIDLIST ITEMTYPE;
typedef void* PCIDLIST_ABSOLUTE;

typedef struct _COMDLG_FILTERSPEC {
	LPCWSTR pszName;
	LPCWSTR pszSpec;
} COMDLG_FILTERSPEC;

typedef const WCHAR* PCWSTR;
typedef WCHAR* PWSTR;

typedef struct _TASKDIALOG_BUTTON {
	int nButtonID;
	PCWSTR pszButtonText;
} TASKDIALOG_BUTTON;

typedef HRESULT (CALLBACK *PFTASKDIALOGCALLBACK)(HWND, UINT, WPARAM, LPARAM, LONG_PTR);

typedef struct _TASKDIALOGCONFIG {
	UINT cbSize;
	HWND hwndParent;
	HINSTANCE hInstance;
	DWORD dwFlags;
	DWORD dwCommonButtons;
	PCWSTR pszWindowTitle;
	union { HICON hMainIcon; PCWSTR pszMainIcon; };
	PCWSTR pszMainInstruction;
	PCWSTR pszContent;
	UINT cButtons;
	const TASKDIALOG_BUTTON* pButtons;
	int nDefaultButton;
	UINT cRadioButtons;
	const TASKDIALOG_BUTTON* pRadioButtons;
	int nDefaultRadioButton;
	PCWSTR pszVerificationText;
	PCWSTR pszExpandedInformation;
	PCWSTR pszExpandedControlText;
	PCWSTR pszCollapsedControlText;
	union { HICON hFooterIcon; PCWSTR pszFooterIcon; };
	PCWSTR pszFooter;
	PFTASKDIALOGCALLBACK pfCallback;
	LONG_PTR lpCallbackData;
	UINT cxWidth;
} TASKDIALOGCONFIG;

typedef struct tagACTCTXW {
	ULONG cbSize;
	DWORD dwFlags;
	LPCWSTR lpSource;
	USHORT wProcessorArchitecture;
	LANGID wLangId;
	LPCWSTR lpAssemblyDirectory;
	LPCWSTR lpResourceName;
	LPCWSTR lpApplicationName;
	HMODULE hModule;
} ACTCTXW, ACTCTX, *PACTCTXW;

typedef struct tagINITCOMMONCONTROLSEX {
	DWORD dwSize;
	DWORD dwICC;
} INITCOMMONCONTROLSEX, *LPINITCOMMONCONTROLSEX;

typedef struct _INITCOMMONCONTROLS {
	DWORD dummy;
} INITCOMMONCONTROLS;

typedef struct tagDLLVERSIONINFO {
	DWORD cbSize;
	DWORD dwMajorVersion;
	DWORD dwMinorVersion;
	DWORD dwBuildNumber;
	DWORD dwPlatformID;
} DLLVERSIONINFO;

typedef struct _TIME_ZONE_INFORMATION {
	LONG Bias;
	WCHAR StandardName[32];
	SYSTEMTIME StandardDate;
	LONG StandardBias;
	WCHAR DaylightName[32];
	SYSTEMTIME DaylightDate;
	LONG DaylightBias;
} TIME_ZONE_INFORMATION, *LPTIME_ZONE_INFORMATION;

typedef struct _NUMBERFMTW {
	UINT NumDigits;
	UINT LeadingZero;
	UINT Grouping;
	LPWSTR lpDecimalSep;
	LPWSTR lpThousandSep;
	UINT NegativeOrder;
} NUMBERFMTW, NUMBERFMT, *LPNUMBERFMTW;

typedef struct _CURRENCYFMTW {
	UINT NumDigits;
	UINT LeadingZero;
	UINT Grouping;
	LPWSTR lpDecimalSep;
	LPWSTR lpThousandSep;
	UINT NegativeOrder;
	UINT PositiveOrder;
	LPWSTR lpCurrencySymbol;
} CURRENCYFMTW;

typedef struct _CPINFO {
	UINT MaxCharSize;
	BYTE DefaultChar[2];
	BYTE LeadByte[12];
} CPINFO, *LPCPINFO;

typedef struct _COORD {
	SHORT X;
	SHORT Y;
} COORD, *PCOORD;

typedef struct _SMALL_RECT {
	SHORT Left;
	SHORT Top;
	SHORT Right;
	SHORT Bottom;
} SMALL_RECT;

typedef DWORD (*LPPROGRESS_ROUTINE)(LARGE_INTEGER, LARGE_INTEGER, LARGE_INTEGER, LARGE_INTEGER, DWORD, DWORD, HANDLE, HANDLE, LPVOID);

typedef struct _PROCESSENTRY32W {
	DWORD dwSize;
	DWORD cntUsage;
	DWORD th32ProcessID;
	ULONG_PTR th32DefaultHeapID;
	DWORD th32ModuleID;
	DWORD cntThreads;
	DWORD th32ParentProcessID;
	LONG pcPriClassBase;
	DWORD dwFlags;
	WCHAR szExeFile[MAX_PATH];
} PROCESSENTRY32W, PROCESSENTRY32, *LPPROCESSENTRY32W;

typedef struct _MODULEENTRY32W {
	DWORD dwSize;
	DWORD th32ModuleID;
	DWORD th32ProcessID;
	DWORD GlblcntUsage;
	DWORD ProccntUsage;
	BYTE* modBaseAddr;
	DWORD modBaseSize;
	HMODULE hModule;
	WCHAR szModule[256];
	WCHAR szExePath[MAX_PATH];
} MODULEENTRY32W, *LPMODULEENTRY32W;

typedef struct _WINTRUST_FILE_INFO {
	DWORD cbStruct;
	LPCWSTR pcwszFilePath;
	HANDLE hFile;
	GUID* pgKnownSubject;
} WINTRUST_FILE_INFO, *PWINTRUST_FILE_INFO;

typedef struct _WINTRUST_DATA {
	DWORD cbStruct;
	LPVOID pPolicyCallbackData;
	LPVOID pSIPClientData;
	DWORD dwUIChoice;
	DWORD fdwRevocationChecks;
	DWORD dwUnionChoice;
	union { WINTRUST_FILE_INFO* pFile; };
	DWORD dwStateAction;
	HANDLE hWVTStateData;
	WCHAR* pwszURLReference;
	DWORD dwProvFlags;
	DWORD dwUIContext;
} WINTRUST_DATA, *PWINTRUST_DATA;

typedef struct _CRYPT_PROVIDER_DATA { DWORD dummy; } CRYPT_PROVIDER_DATA;
typedef struct _CERT_CHAIN_CONTEXT { DWORD dummy; } CERT_CHAIN_CONTEXT, *PCCERT_CHAIN_CONTEXT;
typedef struct _CERT_CHAIN_PARA { DWORD dummy; } CERT_CHAIN_PARA;
typedef struct _CERT_CHAIN_POLICY_PARA { DWORD dummy; } CERT_CHAIN_POLICY_PARA;
typedef struct _CERT_CHAIN_POLICY_STATUS { DWORD dummy; } CERT_CHAIN_POLICY_STATUS;
typedef struct _CRYPT_HASH_BLOB { DWORD cbData; BYTE* pbData; } CRYPT_HASH_BLOB, CRYPT_DATA_BLOB, CRYPT_INTEGER_BLOB, CRYPT_OBJID_BLOB, CERT_NAME_BLOB;
typedef struct _CRYPTOAPI_BLOB { DWORD cbData; BYTE* pbData; } CRYPTOAPI_BLOB;
typedef struct _CRYPT_ALGORITHM_IDENTIFIER { LPSTR pszObjId; CRYPT_OBJID_BLOB Parameters; } CRYPT_ALGORITHM_IDENTIFIER;
typedef PVOID HCERTSTORE;
typedef struct _CERT_INFO {
	DWORD dwVersion;
	CRYPT_INTEGER_BLOB SerialNumber;
	CRYPT_ALGORITHM_IDENTIFIER SignatureAlgorithm;
	CERT_NAME_BLOB Issuer;
	FILETIME NotBefore;
	FILETIME NotAfter;
	CERT_NAME_BLOB Subject;
} CERT_INFO, *PCERT_INFO;
typedef struct _CERT_CONTEXT {
	DWORD dwCertEncodingType;
	BYTE* pbCertEncoded;
	DWORD cbCertEncoded;
	CERT_INFO* pCertInfo;
	HCERTSTORE hCertStore;
} CERT_CONTEXT, *PCCERT_CONTEXT;
typedef struct _CMSG_SIGNER_INFO {
	DWORD dwVersion;
	CERT_NAME_BLOB Issuer;
	CRYPT_INTEGER_BLOB SerialNumber;
} CMSG_SIGNER_INFO, *PCMSG_SIGNER_INFO;

typedef LONG LPARAM_COMPAT;

typedef struct _DWM_BLURBEHIND {
	DWORD dwFlags;
	BOOL fEnable;
	HRGN hRgnBlur;
	BOOL fTransitionOnMaximized;
} DWM_BLURBEHIND, *PDWM_BLURBEHIND;

typedef struct _MARGINS {
	int cxLeftWidth;
	int cxRightWidth;
	int cyTopHeight;
	int cyBottomHeight;
} MARGINS, *PMARGINS;

typedef struct _UPDATELAYEREDWINDOWINFO {
	DWORD cbSize;
	HDC hdcDst;
	const POINT* pptDst;
	const SIZE* psize;
	HDC hdcSrc;
	const POINT* pptSrc;
	COLORREF crKey;
	const BLENDFUNCTION* pblend;
	DWORD dwFlags;
	const RECT* prcDirty;
} UPDATELAYEREDWINDOWINFO;

typedef struct tagPIXELFORMATDESCRIPTOR {
	WORD nSize;
	WORD nVersion;
	DWORD dwFlags;
	BYTE iPixelType;
	BYTE cColorBits;
	BYTE cRedBits, cRedShift, cGreenBits, cGreenShift, cBlueBits, cBlueShift;
	BYTE cAlphaBits, cAlphaShift, cAccumBits, cAccumRedBits, cAccumGreenBits, cAccumBlueBits, cAccumAlphaBits;
	BYTE cDepthBits, cStencilBits, cAuxBuffers, iLayerType, bReserved;
	DWORD dwLayerMask, dwVisibleMask, dwDamageMask;
} PIXELFORMATDESCRIPTOR;

typedef uintptr_t ULONG_PTR_MSG;

typedef struct _MEMORY_BASIC_INFORMATION {
	PVOID BaseAddress;
	PVOID AllocationBase;
	DWORD AllocationProtect;
	SIZE_T RegionSize;
	DWORD State;
	DWORD Protect;
	DWORD Type;
} MEMORY_BASIC_INFORMATION, *PMEMORY_BASIC_INFORMATION;

typedef struct _SYSTEM_POWER_STATUS {
	BYTE ACLineStatus;
	BYTE BatteryFlag;
	BYTE BatteryLifePercent;
	BYTE SystemStatusFlag;
	DWORD BatteryLifeTime;
	DWORD BatteryFullLifeTime;
} SYSTEM_POWER_STATUS, *LPSYSTEM_POWER_STATUS;

typedef DWORD UIAID;

struct IUnknown {
	virtual HRESULT QueryInterface(REFIID riid, void** ppv) = 0;
	virtual ULONG AddRef() = 0;
	virtual ULONG Release() = 0;
};

struct IStream : public IUnknown {};
struct IBindCtx;
struct IShellItem : public IUnknown {
	virtual HRESULT BindToHandler(IBindCtx*, REFGUID, REFIID, void**) = 0;
	virtual HRESULT GetParent(IShellItem**) = 0;
	virtual HRESULT GetDisplayName(DWORD, LPWSTR*) = 0;
	virtual HRESULT GetAttributes(ULONG, ULONG*) = 0;
	virtual HRESULT Compare(IShellItem*, DWORD, int*) = 0;
};
struct IShellItemArray : public IUnknown {
	virtual HRESULT GetCount(DWORD*) = 0;
	virtual HRESULT GetItemAt(DWORD, IShellItem**) = 0;
};
struct IModalWindow : public IUnknown {
	virtual HRESULT Show(HWND) = 0;
};
struct IFileDialogEvents;
struct IFileDialog : public IModalWindow {
	virtual HRESULT SetFileTypes(UINT, const COMDLG_FILTERSPEC*) = 0;
	virtual HRESULT SetFileTypeIndex(UINT) = 0;
	virtual HRESULT GetFileTypeIndex(UINT*) = 0;
	virtual HRESULT Advise(IFileDialogEvents*, DWORD*) = 0;
	virtual HRESULT Unadvise(DWORD) = 0;
	virtual HRESULT SetOptions(DWORD) = 0;
	virtual HRESULT GetOptions(DWORD*) = 0;
	virtual HRESULT SetDefaultFolder(IShellItem*) = 0;
	virtual HRESULT SetFolder(IShellItem*) = 0;
	virtual HRESULT GetFolder(IShellItem**) = 0;
	virtual HRESULT GetCurrentSelection(IShellItem**) = 0;
	virtual HRESULT SetFileName(LPCWSTR) = 0;
	virtual HRESULT GetFileName(LPWSTR*) = 0;
	virtual HRESULT SetTitle(LPCWSTR) = 0;
	virtual HRESULT SetOkButtonLabel(LPCWSTR) = 0;
	virtual HRESULT SetFileNameLabel(LPCWSTR) = 0;
	virtual HRESULT GetResult(IShellItem**) = 0;
	virtual HRESULT AddPlace(IShellItem*, DWORD) = 0;
	virtual HRESULT SetDefaultExtension(LPCWSTR) = 0;
	virtual HRESULT Close(HRESULT) = 0;
	virtual HRESULT SetClientGuid(REFGUID) = 0;
	virtual HRESULT ClearClientData() = 0;
	virtual HRESULT SetFilter(void*) = 0;
};
struct IFileOpenDialog : public IFileDialog {
	virtual HRESULT GetResults(IShellItemArray**) = 0;
	virtual HRESULT GetSelectedItems(IShellItemArray**) = 0;
};
struct IFileSaveDialog : public IFileDialog {};
struct IFileDialogCustomize : public IUnknown {
	virtual HRESULT EnableOpenDropDown(DWORD) = 0;
	virtual HRESULT AddMenu(DWORD, LPCWSTR) = 0;
	virtual HRESULT AddPushButton(DWORD, LPCWSTR) = 0;
	virtual HRESULT AddComboBox(DWORD) = 0;
	virtual HRESULT AddRadioButtonList(DWORD) = 0;
	virtual HRESULT AddCheckButton(DWORD, LPCWSTR, BOOL) = 0;
	virtual HRESULT AddEditBox(DWORD, LPCWSTR) = 0;
	virtual HRESULT AddSeparator(DWORD) = 0;
	virtual HRESULT AddText(DWORD, LPCWSTR) = 0;
	virtual HRESULT SetControlLabel(DWORD, LPCWSTR) = 0;
	virtual HRESULT GetControlState(DWORD, DWORD*) = 0;
	virtual HRESULT SetControlState(DWORD, DWORD) = 0;
	virtual HRESULT GetEditBoxText(DWORD, WCHAR**) = 0;
	virtual HRESULT SetEditBoxText(DWORD, LPCWSTR) = 0;
	virtual HRESULT GetCheckButtonState(DWORD, BOOL*) = 0;
	virtual HRESULT SetCheckButtonState(DWORD, BOOL) = 0;
};
struct IFileDialogEvents : public IUnknown {};
struct IFileDialogControlEvents : public IUnknown {};
struct IOleWindow : public IUnknown {
	virtual HRESULT GetWindow(HWND*) = 0;
	virtual HRESULT ContextSensitiveHelp(BOOL) = 0;
};
struct ITaskbarList3 : public IUnknown {};
struct IDropTarget : public IUnknown {};
struct IDataObject : public IUnknown {};

#define CDCS_INACTIVE 0
#define CDCS_ENABLED 1
#define CDCS_VISIBLE 2
#define FOS_OVERWRITEPROMPT 0x2
#define FOS_STRICTFILETYPES 0x4
#define FOS_NOCHANGEDIR 0x8
#define FOS_PICKFOLDERS 0x20
#define FOS_FORCEFILESYSTEM 0x40
#define FOS_ALLNONSTORAGEITEMS 0x80
#define FOS_NOVALIDATE 0x100
#define FOS_ALLOWMULTISELECT 0x200
#define FOS_PATHMUSTEXIST 0x800
#define FOS_FILEMUSTEXIST 0x1000
#define FOS_CREATEPROMPT 0x2000
#define FOS_SHAREAWARE 0x4000
#define FOS_NOREADONLYRETURN 0x8000
#define FOS_NOTESTFILECREATE 0x10000
#define FOS_HIDEMRUPLACES 0x20000
#define FOS_HIDEPINNEDPLACES 0x40000
#define FOS_NODEREFERENCELINKS 0x100000
#define FOS_OKBUTTONNEEDSINTERACTION 0x200000
#define FOS_DONTADDTORECENT 0x2000000
#define FOS_FORCESHOWHIDDEN 0x10000000
#define FOS_DEFAULTNOMINIMODE 0x20000000
#define FOS_FORCEPREVIEWPANEON 0x40000000
#define SIGDN_FILESYSPATH 0x80058000
#define SFGAO_READONLY 0x40000000UL
#define CLSCTX_INPROC_SERVER 0x1
#define IID_PPV_ARGS(ppType) __uuidof(**(ppType)), reinterpret_cast<void**>(ppType)

// COM helpers used by CustomFileDialog (Windows-only file). Provided so other
// translation units that include COM headers still compile.
#ifndef _COM_NO_STANDARD_GUIDS
extern "C" const GUID CLSID_FileOpenDialog;
extern "C" const GUID CLSID_FileSaveDialog;
extern "C" const GUID IID_IFileDialog;
extern "C" const GUID IID_IFileOpenDialog;
extern "C" const GUID IID_IShellItem;
extern "C" const GUID IID_IOleWindow;
#endif

template<class T>
class _com_ptr_t {
public:
	_com_ptr_t() : p(nullptr) {}
	_com_ptr_t(T* t) : p(t) { if (p) p->AddRef(); }
	_com_ptr_t(const _com_ptr_t& o) : p(o.p) { if (p) p->AddRef(); }
	~_com_ptr_t() { if (p) p->Release(); }
	T** operator&() { return &p; }
	T* operator->() const { return p; }
	operator T*() const { return p; }
	T* get() const { return p; }
	_com_ptr_t& operator=(T* t) {
		if (p) p->Release();
		p = t;
		if (p) p->AddRef();
		return *this;
	}
	void Attach(T* t) { if (p) p->Release(); p = t; }
	void Release() { if (p) { p->Release(); p = nullptr; } }
	T* p;
};
template<class T, const IID* id>
struct _com_IIID { typedef T TInterface; };

class _com_error {
public:
	explicit _com_error(HRESULT hr = 0) : _hr(hr) {}
	HRESULT Error() const { return _hr; }
	const char* ErrorMessage() const { return "COM error"; }
private:
	HRESULT _hr;
};

#ifndef S_OK
#define S_OK ((HRESULT)0)
#define S_FALSE ((HRESULT)1)
#define E_FAIL ((HRESULT)0x80004005L)
#define E_NOINTERFACE ((HRESULT)0x80004002L)
#define E_POINTER ((HRESULT)0x80004003L)
#define E_NOTIMPL ((HRESULT)0x80004001L)
#define E_INVALIDARG ((HRESULT)0x80070057L)
#define E_OUTOFMEMORY ((HRESULT)0x8007000EL)
#define E_ABORT ((HRESULT)0x80004004L)
#define E_ACCESSDENIED ((HRESULT)0x80070005L)
#define E_UNEXPECTED ((HRESULT)0x8000FFFFL)
#define HRESULT_FROM_WIN32(x) ((HRESULT)(x) <= 0 ? (HRESULT)(x) : (HRESULT)(((x) & 0x0000FFFF) | 0x80070000))
#define SUCCEEDED(hr) (((HRESULT)(hr)) >= 0)
#define FAILED(hr) (((HRESULT)(hr)) < 0)
#define MAKE_HRESULT(sev,fac,code) ((HRESULT)(((unsigned long)(sev)<<31)|((unsigned long)(fac)<<16)|((unsigned long)(code))))
#endif

// Extra types referenced by win32_api.h
typedef const DLGTEMPLATE* LPCDLGTEMPLATE;
typedef const DLGTEMPLATE* LPCDLGTEMPLATEW;
typedef DWORD* PDWORD_PTR;
typedef int* LPINT;
typedef HANDLE* PHANDLE;
typedef HANDLE* LPHANDLE;
typedef LONG* LPLONG;
typedef const WCHAR* LPCWCH;
typedef IUnknown* LPUNKNOWN;
typedef HKEY* PHKEY;
typedef WCHAR* STRSAFE_LPWSTR;
typedef const WCHAR* STRSAFE_LPCWSTR;
typedef DWORD ALG_ID;
typedef PVOID BCRYPT_HANDLE;
typedef PVOID HPAINTBUFFER;
typedef PVOID DPI_AWARENESS_CONTEXT;
typedef DWORD MMRESULT;
typedef DWORD SFGAOF;
typedef DWORD TASKDIALOG_COMMON_BUTTON_FLAGS;
typedef GUID KNOWNFOLDERID;
typedef const GUID& REFKNOWNFOLDERID;

typedef BOOL (CALLBACK *GRAYSTRINGPROC)(HDC, LPARAM, int);
typedef BOOL (CALLBACK *ABORTPROC)(HDC, int);
typedef LONG (CALLBACK *PVECTORED_EXCEPTION_HANDLER)(EXCEPTION_POINTERS*);
typedef void (CALLBACK *LPOVERLAPPED_COMPLETION_ROUTINE)(DWORD, DWORD, LPOVERLAPPED);
typedef BOOL (CALLBACK *ENUMRESNAMEPROCW)(HMODULE, LPCWSTR, LPWSTR, LONG_PTR);
typedef BOOL (CALLBACK *ENUMRESTYPEPROCW)(HMODULE, LPWSTR, LONG_PTR);

typedef struct tagDRAWTEXTPARAMS {
	UINT cbSize;
	int iTabLength;
	int iLeftMargin;
	int iRightMargin;
	UINT uiLengthDrawn;
} DRAWTEXTPARAMS, *LPDRAWTEXTPARAMS;

typedef struct tagGUITHREADINFO {
	DWORD cbSize;
	DWORD flags;
	HWND hwndActive;
	HWND hwndFocus;
	HWND hwndCapture;
	HWND hwndMenuOwner;
	HWND hwndMoveSize;
	HWND hwndCaret;
	RECT rcCaret;
} GUITHREADINFO, *PGUITHREADINFO, *LPGUITHREADINFO;

typedef struct tagCHANGEFILTERSTRUCT {
	DWORD cbSize;
	DWORD ExtStatus;
} CHANGEFILTERSTRUCT, *PCHANGEFILTERSTRUCT;

typedef struct _DISPLAY_DEVICEW {
	DWORD cb;
	WCHAR DeviceName[32];
	WCHAR DeviceString[128];
	DWORD StateFlags;
	WCHAR DeviceID[128];
	WCHAR DeviceKey[128];
} DISPLAY_DEVICEW, DISPLAY_DEVICE, *PDISPLAY_DEVICEW, *PDISPLAY_DEVICE;

typedef struct _XFORM {
	FLOAT eM11, eM12, eM21, eM22, eDx, eDy;
} XFORM, *PXFORM, *LPXFORM;

typedef struct _OUTLINETEXTMETRICW {
	UINT otmSize;
	TEXTMETRICW otmTextMetrics;
	BYTE dummy[64];
} OUTLINETEXTMETRICW, *LPOUTLINETEXTMETRICW;

typedef struct tagPALETTEENTRY {
	BYTE peRed, peGreen, peBlue, peFlags;
} PALETTEENTRY, *PPALETTEENTRY, *LPPALETTEENTRY;

typedef struct tagLOGPALETTE {
	WORD palVersion;
	WORD palNumEntries;
	PALETTEENTRY palPalEntry[1];
} LOGPALETTE, *PLOGPALETTE, *LPLOGPALETTE;

typedef struct _RGNDATAHEADER {
	DWORD dwSize;
	DWORD iType;
	DWORD nCount;
	DWORD nRgnSize;
	RECT rcBound;
} RGNDATAHEADER;

typedef struct _RGNDATA {
	RGNDATAHEADER rdh;
	char Buffer[1];
} RGNDATA, *PRGNDATA, *LPRGNDATA;

typedef WORD COLOR16;

typedef struct _TRIVERTEX {
	LONG x, y;
	COLOR16 Red, Green, Blue, Alpha;
} TRIVERTEX, *PTRIVERTEX, *LPTRIVERTEX;

typedef UINT (CALLBACK *LPFRHOOKPROC)(HWND, UINT, WPARAM, LPARAM);

typedef struct tagFINDREPLACEW {
	DWORD lStructSize;
	HWND hwndOwner;
	HINSTANCE hInstance;
	DWORD Flags;
	LPWSTR lpstrFindWhat;
	LPWSTR lpstrReplaceWith;
	WORD wFindWhatLen;
	WORD wReplaceWithLen;
	LPARAM lCustData;
	LPFRHOOKPROC lpfnHook;
	LPCWSTR lpTemplateName;
} FINDREPLACEW, FINDREPLACE, *LPFINDREPLACEW;

typedef UINT (CALLBACK *LPPAGESETUPHOOK)(HWND, UINT, WPARAM, LPARAM);
typedef UINT (CALLBACK *LPPAGEPAINTHOOK)(HWND, UINT, WPARAM, LPARAM);

typedef struct tagPAGESETUPDLGW {
	DWORD lStructSize;
	HWND hwndOwner;
	HGLOBAL hDevMode;
	HGLOBAL hDevNames;
	DWORD Flags;
	POINT ptPaperSize;
	RECT rtMinMargin;
	RECT rtMargin;
	HINSTANCE hInstance;
	LPARAM lCustData;
	LPPAGESETUPHOOK lpfnPageSetupHook;
	LPPAGEPAINTHOOK lpfnPagePaintHook;
	LPCWSTR lpPageSetupTemplateName;
	HGLOBAL hPageSetupTemplate;
} PAGESETUPDLGW, PAGESETUPDLG, *LPPAGESETUPDLGW;

typedef struct _AppBarData {
	DWORD cbSize;
	HWND hWnd;
	UINT uCallbackMessage;
	UINT uEdge;
	RECT rc;
	LPARAM lParam;
} APPBARDATA, *PAPPBARDATA;

typedef struct _COMPOSITIONFORM {
	DWORD dwStyle;
	POINT ptCurrentPos;
	RECT rcArea;
} COMPOSITIONFORM, *LPCOMPOSITIONFORM, *PCOMPOSITIONFORM;

typedef struct _CANDIDATEFORM {
	DWORD dwIndex;
	DWORD dwStyle;
	POINT ptCurrentPos;
	RECT rcArea;
} CANDIDATEFORM, *LPCANDIDATEFORM, *PCANDIDATEFORM;

typedef struct tagCURSORINFO {
	DWORD cbSize;
	DWORD flags;
	HCURSOR hCursor;
	POINT ptScreenPos;
} CURSORINFO, *PCURSORINFO, *LPCURSORINFO;

typedef struct _PROCESS_MEMORY_COUNTERS {
	DWORD cb;
	DWORD PageFaultCount;
	SIZE_T PeakWorkingSetSize;
	SIZE_T WorkingSetSize;
	SIZE_T QuotaPeakPagedPoolUsage;
	SIZE_T QuotaPagedPoolUsage;
	SIZE_T QuotaPeakNonPagedPoolUsage;
	SIZE_T QuotaNonPagedPoolUsage;
	SIZE_T PagefileUsage;
	SIZE_T PeakPagefileUsage;
} PROCESS_MEMORY_COUNTERS, *PPROCESS_MEMORY_COUNTERS;

typedef struct tagMOUSEINPUT {
	LONG dx, dy;
	DWORD mouseData, dwFlags, time;
	ULONG_PTR dwExtraInfo;
} MOUSEINPUT;

typedef struct tagKEYBDINPUT {
	WORD wVk, wScan;
	DWORD dwFlags, time;
	ULONG_PTR dwExtraInfo;
} KEYBDINPUT;

typedef struct tagHARDWAREINPUT {
	DWORD uMsg;
	WORD wParamL, wParamH;
} HARDWAREINPUT;

typedef struct tagINPUT {
	DWORD type;
	union {
		MOUSEINPUT mi;
		KEYBDINPUT ki;
		HARDWAREINPUT hi;
	};
} INPUT, *LPINPUT, *PINPUT;

typedef enum {
	SIID_DOCNOASSOC = 0,
	SIID_DOCASSOC = 1,
	SIID_APPLICATION = 2,
	SIID_FOLDER = 3,
	SIID_FOLDEROPEN = 4
} SHSTOCKICONID;

typedef struct _SHSTOCKICONINFO {
	DWORD cbSize;
	HICON hIcon;
	int iSysImageIndex;
	int iIcon;
	WCHAR szPath[MAX_PATH];
} SHSTOCKICONINFO;

typedef enum _BP_BUFFERFORMAT {
	BPBF_COMPATIBLEBITMAP,
	BPBF_DIB,
	BPBF_TOPDOWNDIB,
	BPBF_TOPDOWNMONODIB
} BP_BUFFERFORMAT;

typedef struct _BP_PAINTPARAMS {
	DWORD cbSize;
	DWORD dwFlags;
	const RECT* prcExclude;
	const BLENDFUNCTION* pblend;
} BP_PAINTPARAMS, *PBP_PAINTPARAMS;

typedef enum _PROCESS_DPI_AWARENESS {
	PROCESS_DPI_UNAWARE = 0,
	PROCESS_SYSTEM_DPI_AWARE = 1,
	PROCESS_PER_MONITOR_DPI_AWARE = 2
} PROCESS_DPI_AWARENESS;

typedef enum _MONITOR_DPI_TYPE {
	MDT_EFFECTIVE_DPI = 0,
	MDT_ANGULAR_DPI = 1,
	MDT_RAW_DPI = 2,
	MDT_DEFAULT = MDT_EFFECTIVE_DPI
} MONITOR_DPI_TYPE;

#ifndef DPI_AWARENESS_CONTEXT_UNAWARE
#define DPI_AWARENESS_CONTEXT_UNAWARE ((DPI_AWARENESS_CONTEXT)-1)
#define DPI_AWARENESS_CONTEXT_SYSTEM_AWARE ((DPI_AWARENESS_CONTEXT)-2)
#define DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE ((DPI_AWARENESS_CONTEXT)-3)
#define DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 ((DPI_AWARENESS_CONTEXT)-4)
#endif

#include "win32_consts.h"

#ifdef __cplusplus
// libstdc++ uses __out/__in as parameter names (std::format, chrono).
#undef __out
#undef __in
#endif

#ifndef IMAGE_FILE_MACHINE_I386
#define IMAGE_FILE_MACHINE_UNKNOWN 0
#define IMAGE_FILE_MACHINE_I386    0x014c
#define IMAGE_FILE_MACHINE_AMD64   0x8664
#define IMAGE_FILE_MACHINE_ARM64   0xAA64
#endif

#ifndef COINIT_APARTMENTTHREADED
#define COINIT_MULTITHREADED      0x0
#define COINIT_APARTMENTTHREADED  0x2
#define COINIT_DISABLE_OLE1DDE    0x4
#define COINIT_SPEED_OVER_MEMORY  0x8
#endif

#ifndef RPC_E_CHANGED_MODE
#define RPC_E_CHANGED_MODE ((HRESULT)0x80010106L)
#endif

#ifndef ListView_GetSelectionMark
#define ListView_GetSelectionMark(hwnd) \
	(INT)::SendMessage((hwnd), LVM_GETSELECTIONMARK, 0, 0)
#define ListView_SetSelectionMark(hwnd, i) \
	(INT)::SendMessage((hwnd), LVM_SETSELECTIONMARK, 0, (LPARAM)(i))
#endif

#include "win32_api.h"

#ifdef __cplusplus
#undef __out
#undef __in
#endif

#include "win32_npp_extra.h"

#endif // NPP_WIN32_COMPAT_H
