#pragma once

#include <windows.h>

#include <gtk/gtk.h>
#include <cairo.h>

#include <cwchar>
#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include <deque>
#include <mutex>
#include <unordered_map>
#include <algorithm>

struct WindowImpl;
struct DcImpl;
struct MenuImpl;
struct FontImpl;
struct BitmapImpl;

enum class WindowKind {
	Generic,
	TopLevel,
	Dialog,
	Button,
	Edit,
	Static,
	ListBox,
	Combo,
	ScrollBar,
	Tab,
	Toolbar,
	Status,
	Rebar,
	Tree,
	ListView,
	Tooltip,
	Progress,
	Trackbar,
	UpDown,
	Header,
	Scintilla,
	Drawing
};

struct WindowImpl {
	GtkWidget* widget = nullptr;
	GtkWidget* client = nullptr;
	GtkWidget* native = nullptr;
	WNDPROC wndproc = nullptr;
	WNDPROC origProc = nullptr;
	DWORD style = 0;
	DWORD exStyle = 0;
	std::wstring className;
	std::wstring title;
	HWND parent = nullptr;
	int id = 0;
	WindowKind kind = WindowKind::Generic;
	LONG_PTR userData = 0;
	LONG_PTR extra[16]{};
	HMENU menu = nullptr;
	HINSTANCE instance = nullptr;
	bool destroyed = false;
	bool visible = false;
	bool enabled = true;
	RECT windowRect{};
	UINT_PTR timerId = 0;
	cairo_t* paintCr = nullptr;
	std::vector<HWND> children;
	void* controlData = nullptr;
	struct SubclassEntry {
		SUBCLASSPROC proc = nullptr;
		UINT_PTR uid = 0;
		DWORD_PTR data = 0;
	};
	std::vector<SubclassEntry> subclasses;
};

inline HWND hwndFrom(WindowImpl* w) { return reinterpret_cast<HWND>(w); }
inline WindowImpl* implFrom(HWND h)
{
	if (!h || h == INVALID_HANDLE_VALUE) return nullptr;
	return reinterpret_cast<WindowImpl*>(h);
}

std::string nppWideToUtf8(std::wstring_view w);
std::wstring nppUtf8ToWide(std::string_view u);
std::wstring nppNormalizePath(std::wstring p);
void nppSetLastError(DWORD e);
DWORD nppGetLastError();
void nppInitPlatform();
void nppRegisterStockClasses();
WindowImpl* nppAllocWindow();
void nppDestroyWindow(WindowImpl* w);
void nppPostQuit(int code);
bool nppPumpGtk(bool block);
LRESULT nppDispatch(WindowImpl* w, UINT msg, WPARAM wp, LPARAM lp);
HWND nppCreateWindow(DWORD exStyle, const std::wstring& cls, const std::wstring& title,
	DWORD style, int x, int y, int w, int h, HWND parent, HMENU menu, HINSTANCE inst, LPVOID param);
void nppRegisterClass(const std::wstring& name, const WNDCLASSEXW& wc);
bool nppLookupClass(const std::wstring& name, WNDCLASSEXW& wc);
LRESULT nppControlSend(WindowImpl* w, UINT msg, WPARAM wp, LPARAM lp);
LRESULT nppScintillaSend(WindowImpl* w, UINT msg, WPARAM wp, LPARAM lp);
void nppInitResources();
HRSRC nppFindResource(LPCWSTR name, LPCWSTR type);
HGLOBAL nppLoadResource(HRSRC res);
DWORD nppSizeofResource(HRSRC res);
int nppLoadString(UINT id, LPWSTR buf, int max);
HMENU nppLoadMenu(LPCWSTR name);
HACCEL nppLoadAccel(LPCWSTR name);
HANDLE nppLoadImage(LPCWSTR name, UINT type, int cx, int cy);
HWND nppCreateDialogIndirect(LPCDLGTEMPLATE tmpl, HWND parent, DLGPROC proc, LPARAM lp, bool modal, INT_PTR* result);
void nppInitGdi();
HDC nppCreatePaintDC(WindowImpl* w, cairo_t* cr, int width, int height);
void nppReleasePaintDC(HDC hdc);

extern thread_local DWORD g_lastError;
extern bool g_nppQuit;
extern int g_nppQuitCode;
