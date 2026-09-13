#include "npp_platform.h"

#include <gdk/gdk.h>
#include <sys/time.h>
#include <unistd.h>
#include <cmath>
#include <cstring>
#include <locale>
#include <codecvt>

thread_local DWORD g_lastError = 0;
bool g_nppQuit = false;
int g_nppQuitCode = 0;

static std::mutex g_msgMutex;
static std::deque<MSG> g_msgQueue;
static std::unordered_map<std::wstring, WNDCLASSEXW> g_classes;
static std::mutex g_classMutex;
static HWND g_capture = nullptr;
static HWND g_focus = nullptr;
static HWND g_active = nullptr;
static HWND g_foreground = nullptr;
static std::vector<WindowImpl*> g_windows;
static std::mutex g_winMutex;
static guint g_gtkTimer = 0;

void nppSetLastError(DWORD e) { g_lastError = e; }
DWORD nppGetLastError() { return g_lastError; }

DWORD GetLastError(void) { return g_lastError; }
void SetLastError(DWORD err) { g_lastError = err; }

std::string nppWideToUtf8(std::wstring_view w)
{
	if (w.empty()) return {};
	std::string out;
	out.resize(w.size() * 4 + 1);
	char* dst = out.data();
	for (wchar_t ch : w)
	{
		uint32_t c = static_cast<uint32_t>(ch);
		if (c < 0x80) *dst++ = static_cast<char>(c);
		else if (c < 0x800)
		{
			*dst++ = static_cast<char>(0xC0 | (c >> 6));
			*dst++ = static_cast<char>(0x80 | (c & 0x3F));
		}
		else if (c < 0x10000)
		{
			*dst++ = static_cast<char>(0xE0 | (c >> 12));
			*dst++ = static_cast<char>(0x80 | ((c >> 6) & 0x3F));
			*dst++ = static_cast<char>(0x80 | (c & 0x3F));
		}
		else
		{
			*dst++ = static_cast<char>(0xF0 | (c >> 18));
			*dst++ = static_cast<char>(0x80 | ((c >> 12) & 0x3F));
			*dst++ = static_cast<char>(0x80 | ((c >> 6) & 0x3F));
			*dst++ = static_cast<char>(0x80 | (c & 0x3F));
		}
	}
	*dst = 0;
	out.resize(static_cast<size_t>(dst - out.data()));
	return out;
}

std::wstring nppUtf8ToWide(std::string_view u)
{
	std::wstring out;
	out.reserve(u.size());
	const unsigned char* p = reinterpret_cast<const unsigned char*>(u.data());
	const unsigned char* end = p + u.size();
	while (p < end)
	{
		uint32_t c = *p++;
		if (c >= 0xC2 && c <= 0xDF && p < end)
		{
			c = ((c & 0x1F) << 6) | (*p++ & 0x3F);
		}
		else if (c >= 0xE0 && c <= 0xEF && p + 1 < end)
		{
			c = ((c & 0x0F) << 12) | ((p[0] & 0x3F) << 6) | (p[1] & 0x3F);
			p += 2;
		}
		else if (c >= 0xF0 && c <= 0xF4 && p + 2 < end)
		{
			c = ((c & 0x07) << 18) | ((p[0] & 0x3F) << 12) | ((p[1] & 0x3F) << 6) | (p[2] & 0x3F);
			p += 3;
		}
		out.push_back(static_cast<wchar_t>(c));
	}
	return out;
}

std::wstring nppNormalizePath(std::wstring p)
{
	for (auto& c : p)
		if (c == L'\\') c = L'/';
	return p;
}

WindowImpl* nppAllocWindow()
{
	auto* w = new WindowImpl();
	std::lock_guard<std::mutex> lock(g_winMutex);
	g_windows.push_back(w);
	return w;
}

static void forgetWindow(WindowImpl* w)
{
	std::lock_guard<std::mutex> lock(g_winMutex);
	g_windows.erase(std::remove(g_windows.begin(), g_windows.end(), w), g_windows.end());
}

void nppRegisterClass(const std::wstring& name, const WNDCLASSEXW& wc)
{
	std::lock_guard<std::mutex> lock(g_classMutex);
	g_classes[name] = wc;
}

bool nppLookupClass(const std::wstring& name, WNDCLASSEXW& wc)
{
	std::lock_guard<std::mutex> lock(g_classMutex);
	auto it = g_classes.find(name);
	if (it == g_classes.end()) return false;
	wc = it->second;
	return true;
}

static WindowKind kindFromClass(const std::wstring& cls)
{
	if (cls == L"BUTTON") return WindowKind::Button;
	if (cls == L"EDIT") return WindowKind::Edit;
	if (cls == L"STATIC") return WindowKind::Static;
	if (cls == L"LISTBOX") return WindowKind::ListBox;
	if (cls == L"COMBOBOX") return WindowKind::Combo;
	if (cls == L"SCROLLBAR") return WindowKind::ScrollBar;
	if (cls == L"SysTabControl32") return WindowKind::Tab;
	if (cls == L"ToolbarWindow32") return WindowKind::Toolbar;
	if (cls == L"msctls_statusbar32") return WindowKind::Status;
	if (cls == L"ReBarWindow32") return WindowKind::Rebar;
	if (cls == L"SysTreeView32") return WindowKind::Tree;
	if (cls == L"SysListView32") return WindowKind::ListView;
	if (cls == L"tooltips_class32") return WindowKind::Tooltip;
	if (cls == L"msctls_progress32") return WindowKind::Progress;
	if (cls == L"msctls_trackbar32") return WindowKind::Trackbar;
	if (cls == L"msctls_updown32") return WindowKind::UpDown;
	if (cls == L"SysHeader32") return WindowKind::Header;
	if (cls == L"Scintilla") return WindowKind::Scintilla;
	if (cls == L"#32770") return WindowKind::Dialog;
	return WindowKind::Generic;
}

static gboolean onDraw(GtkWidget* widget, cairo_t* cr, gpointer data)
{
	auto* w = static_cast<WindowImpl*>(data);
	if (!w || w->destroyed) return FALSE;
	GtkAllocation alloc{};
	gtk_widget_get_allocation(widget, &alloc);
	PAINTSTRUCT ps{};
	ps.hdc = nppCreatePaintDC(w, cr, alloc.width, alloc.height);
	ps.fErase = TRUE;
	ps.rcPaint = {0, 0, alloc.width, alloc.height};
	nppDispatch(w, WM_PAINT, 0, reinterpret_cast<LPARAM>(&ps));
	nppReleasePaintDC(ps.hdc);
	return TRUE;
}

static void onSizeAllocate(GtkWidget*, GdkRectangle* alloc, gpointer data)
{
	auto* w = static_cast<WindowImpl*>(data);
	if (!w || w->destroyed) return;
	w->windowRect = {alloc->x, alloc->y, alloc->x + alloc->width, alloc->y + alloc->height};
	nppDispatch(w, WM_SIZE, SIZE_RESTORED, MAKELPARAM(alloc->width, alloc->height));
}

static gboolean onButton(GtkWidget*, GdkEventButton* ev, gpointer data)
{
	auto* w = static_cast<WindowImpl*>(data);
	if (!w || w->destroyed) return FALSE;
	UINT msg = WM_LBUTTONDOWN;
	if (ev->type == GDK_BUTTON_RELEASE)
	{
		if (ev->button == 1) msg = WM_LBUTTONUP;
		else if (ev->button == 2) msg = WM_MBUTTONUP;
		else msg = WM_RBUTTONUP;
	}
	else if (ev->type == GDK_2BUTTON_PRESS)
	{
		msg = (ev->button == 1) ? WM_LBUTTONDBLCLK : (ev->button == 3 ? WM_RBUTTONDBLCLK : WM_MBUTTONDBLCLK);
	}
	else
	{
		if (ev->button == 1) msg = WM_LBUTTONDOWN;
		else if (ev->button == 2) msg = WM_MBUTTONDOWN;
		else msg = WM_RBUTTONDOWN;
	}
	WPARAM wp = 0;
	if (ev->state & GDK_SHIFT_MASK) wp |= MK_SHIFT;
	if (ev->state & GDK_CONTROL_MASK) wp |= MK_CONTROL;
	if (ev->button == 1) wp |= MK_LBUTTON;
	nppDispatch(w, msg, wp, MAKELPARAM((int)ev->x, (int)ev->y));
	return TRUE;
}

static gboolean onMotion(GtkWidget*, GdkEventMotion* ev, gpointer data)
{
	auto* w = static_cast<WindowImpl*>(data);
	if (!w || w->destroyed) return FALSE;
	WPARAM wp = 0;
	if (ev->state & GDK_SHIFT_MASK) wp |= MK_SHIFT;
	if (ev->state & GDK_CONTROL_MASK) wp |= MK_CONTROL;
	if (ev->state & GDK_BUTTON1_MASK) wp |= MK_LBUTTON;
	nppDispatch(w, WM_MOUSEMOVE, wp, MAKELPARAM((int)ev->x, (int)ev->y));
	return TRUE;
}

static gboolean onScroll(GtkWidget*, GdkEventScroll* ev, gpointer data)
{
	auto* w = static_cast<WindowImpl*>(data);
	if (!w || w->destroyed) return FALSE;
	int delta = 0;
	if (ev->direction == GDK_SCROLL_UP) delta = 120;
	else if (ev->direction == GDK_SCROLL_DOWN) delta = -120;
	else if (ev->direction == GDK_SCROLL_SMOOTH) delta = (int)(ev->delta_y * -120);
	nppDispatch(w, WM_MOUSEWHEEL, MAKEWPARAM(0, delta), MAKELPARAM((int)ev->x, (int)ev->y));
	return TRUE;
}

static guint gdkKeyToVk(guint key)
{
	if (key >= GDK_KEY_A && key <= GDK_KEY_Z) return 'A' + (key - GDK_KEY_A);
	if (key >= GDK_KEY_a && key <= GDK_KEY_z) return 'A' + (key - GDK_KEY_a);
	if (key >= GDK_KEY_0 && key <= GDK_KEY_9) return '0' + (key - GDK_KEY_0);
	if (key >= GDK_KEY_F1 && key <= GDK_KEY_F24) return VK_F1 + (key - GDK_KEY_F1);
	switch (key)
	{
	case GDK_KEY_Return: case GDK_KEY_KP_Enter: return VK_RETURN;
	case GDK_KEY_Escape: return VK_ESCAPE;
	case GDK_KEY_Tab: return VK_TAB;
	case GDK_KEY_BackSpace: return VK_BACK;
	case GDK_KEY_Delete: return VK_DELETE;
	case GDK_KEY_Insert: return VK_INSERT;
	case GDK_KEY_Home: return VK_HOME;
	case GDK_KEY_End: return VK_END;
	case GDK_KEY_Page_Up: return VK_PRIOR;
	case GDK_KEY_Page_Down: return VK_NEXT;
	case GDK_KEY_Left: return VK_LEFT;
	case GDK_KEY_Right: return VK_RIGHT;
	case GDK_KEY_Up: return VK_UP;
	case GDK_KEY_Down: return VK_DOWN;
	case GDK_KEY_space: return VK_SPACE;
	case GDK_KEY_Shift_L: case GDK_KEY_Shift_R: return VK_SHIFT;
	case GDK_KEY_Control_L: case GDK_KEY_Control_R: return VK_CONTROL;
	case GDK_KEY_Alt_L: case GDK_KEY_Alt_R: return VK_MENU;
	case GDK_KEY_Super_L: case GDK_KEY_Super_R: return VK_LWIN;
	default: return key & 0xFF;
	}
}

static gboolean onKey(GtkWidget*, GdkEventKey* ev, gpointer data)
{
	auto* w = static_cast<WindowImpl*>(data);
	if (!w || w->destroyed) return FALSE;
	UINT vk = gdkKeyToVk(ev->keyval);
	UINT msg = (ev->type == GDK_KEY_PRESS) ? WM_KEYDOWN : WM_KEYUP;
	nppDispatch(w, msg, vk, 0);
	if (ev->type == GDK_KEY_PRESS)
	{
		gunichar uch = gdk_keyval_to_unicode(ev->keyval);
		if (uch && uch >= 32)
			nppDispatch(w, WM_CHAR, (WPARAM)uch, 0);
	}
	return FALSE; // allow GTK widgets to see keys too
}

static gboolean onDelete(GtkWidget*, GdkEvent*, gpointer data)
{
	auto* w = static_cast<WindowImpl*>(data);
	if (!w || w->destroyed) return FALSE;
	nppDispatch(w, WM_CLOSE, 0, 0);
	return TRUE;
}

static void onDestroyGtk(GtkWidget*, gpointer data)
{
	auto* w = static_cast<WindowImpl*>(data);
	if (!w || w->destroyed) return;
	nppDispatch(w, WM_DESTROY, 0, 0);
}

static void hookEvents(WindowImpl* w, GtkWidget* widget)
{
	gtk_widget_add_events(widget, GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK |
		GDK_POINTER_MOTION_MASK | GDK_KEY_PRESS_MASK | GDK_KEY_RELEASE_MASK |
		GDK_SCROLL_MASK | GDK_STRUCTURE_MASK);
	g_signal_connect(widget, "draw", G_CALLBACK(onDraw), w);
	g_signal_connect(widget, "size-allocate", G_CALLBACK(onSizeAllocate), w);
	g_signal_connect(widget, "button-press-event", G_CALLBACK(onButton), w);
	g_signal_connect(widget, "button-release-event", G_CALLBACK(onButton), w);
	g_signal_connect(widget, "motion-notify-event", G_CALLBACK(onMotion), w);
	g_signal_connect(widget, "scroll-event", G_CALLBACK(onScroll), w);
	g_signal_connect(widget, "key-press-event", G_CALLBACK(onKey), w);
	g_signal_connect(widget, "key-release-event", G_CALLBACK(onKey), w);
}

ATOM RegisterClassW(const WNDCLASSW* wc)
{
	if (!wc || !wc->lpszClassName) return 0;
	WNDCLASSEXW ex{};
	ex.cbSize = sizeof(ex);
	ex.style = wc->style;
	ex.lpfnWndProc = wc->lpfnWndProc;
	ex.cbClsExtra = wc->cbClsExtra;
	ex.cbWndExtra = wc->cbWndExtra;
	ex.hInstance = wc->hInstance;
	ex.hIcon = wc->hIcon;
	ex.hCursor = wc->hCursor;
	ex.hbrBackground = wc->hbrBackground;
	ex.lpszMenuName = wc->lpszMenuName;
	ex.lpszClassName = wc->lpszClassName;
	nppRegisterClass(wc->lpszClassName, ex);
	return 1;
}

ATOM RegisterClassExW(const WNDCLASSEXW* wc)
{
	if (!wc || !wc->lpszClassName) return 0;
	nppRegisterClass(wc->lpszClassName, *wc);
	return 1;
}

BOOL UnregisterClassW(LPCWSTR name, HINSTANCE)
{
	if (!name) return FALSE;
	std::lock_guard<std::mutex> lock(g_classMutex);
	return g_classes.erase(name) > 0;
}

static void defWndProcCreate(WindowImpl* w)
{
	GtkWidget* draw = gtk_drawing_area_new();
	gtk_widget_set_can_focus(draw, TRUE);
	gtk_widget_set_hexpand(draw, TRUE);
	gtk_widget_set_vexpand(draw, TRUE);
	w->native = draw;
	w->client = draw;
	hookEvents(w, draw);
}

HWND nppCreateWindow(DWORD exStyle, const std::wstring& cls, const std::wstring& title,
	DWORD style, int x, int y, int w, int h, HWND parent, HMENU menu, HINSTANCE inst, LPVOID param)
{
	if (x == CW_USEDEFAULT) x = 100;
	if (y == CW_USEDEFAULT) y = 100;
	if (w == CW_USEDEFAULT) w = 800;
	if (h == CW_USEDEFAULT) h = 600;

	auto* wi = nppAllocWindow();
	wi->style = style;
	wi->exStyle = exStyle;
	wi->className = cls;
	wi->title = title;
	wi->parent = parent;
	wi->menu = menu;
	wi->instance = inst;
	wi->kind = kindFromClass(cls);
	wi->id = (int)(INT_PTR)menu;
	if (parent && (style & WS_CHILD))
	{
		auto* p = implFrom(parent);
		if (p) wi->id = (int)(INT_PTR)menu;
	}

	WNDCLASSEXW wc{};
	nppLookupClass(cls, wc);
	wi->wndproc = wc.lpfnWndProc;
	wi->origProc = wc.lpfnWndProc;

	const bool topLevel = !(style & WS_CHILD);

	if (wi->kind == WindowKind::Scintilla)
	{
		// filled in npp_scintilla.cpp via nppControlSend create path below
	}

	if (topLevel)
	{
		GtkWidget* win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
		gtk_window_set_default_size(GTK_WINDOW(win), w, h);
		gtk_window_move(GTK_WINDOW(win), x, y);
		if (!title.empty())
			gtk_window_set_title(GTK_WINDOW(win), nppWideToUtf8(title).c_str());
		if (!(style & WS_CAPTION))
			gtk_window_set_decorated(GTK_WINDOW(win), FALSE);
		GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
		gtk_container_add(GTK_CONTAINER(win), vbox);
		GtkWidget* fixed = gtk_fixed_new();
		gtk_box_pack_start(GTK_BOX(vbox), fixed, TRUE, TRUE, 0);
		wi->widget = win;
		wi->client = fixed;
		g_signal_connect(win, "delete-event", G_CALLBACK(onDelete), wi);
		g_signal_connect(win, "destroy", G_CALLBACK(onDestroyGtk), wi);
		g_signal_connect(win, "size-allocate", G_CALLBACK(onSizeAllocate), wi);
		gtk_widget_add_events(win, GDK_KEY_PRESS_MASK | GDK_KEY_RELEASE_MASK);
		g_signal_connect(win, "key-press-event", G_CALLBACK(onKey), wi);
		g_signal_connect(win, "key-release-event", G_CALLBACK(onKey), wi);
		if (style & WS_VISIBLE)
			gtk_widget_show_all(win);
		g_foreground = hwndFrom(wi);
		g_active = hwndFrom(wi);
	}
	else
	{
		auto* p = implFrom(parent);
		GtkWidget* host = gtk_fixed_new();
		gtk_widget_set_size_request(host, std::max(w, 1), std::max(h, 1));
		wi->widget = host;
		wi->client = host;
		if (wi->kind == WindowKind::Generic || wi->kind == WindowKind::Drawing || wi->kind == WindowKind::Tab
			|| wi->kind == WindowKind::Toolbar || wi->kind == WindowKind::Rebar)
		{
			defWndProcCreate(wi);
			gtk_container_add(GTK_CONTAINER(host), wi->native);
		}
		else if (wi->kind == WindowKind::Button)
		{
			GtkWidget* b = gtk_button_new_with_label(nppWideToUtf8(title).c_str());
			wi->native = b;
			gtk_container_add(GTK_CONTAINER(host), b);
			g_signal_connect(b, "clicked", G_CALLBACK(+[](GtkButton*, gpointer data) {
				auto* ww = static_cast<WindowImpl*>(data);
				auto* par = implFrom(ww->parent);
				if (par) nppDispatch(par, WM_COMMAND, MAKEWPARAM(ww->id, BN_CLICKED), (LPARAM)hwndFrom(ww));
			}), wi);
		}
		else if (wi->kind == WindowKind::Static)
		{
			GtkWidget* l = gtk_label_new(nppWideToUtf8(title).c_str());
			gtk_label_set_xalign(GTK_LABEL(l), 0);
			wi->native = l;
			gtk_container_add(GTK_CONTAINER(host), l);
		}
		else if (wi->kind == WindowKind::Edit)
		{
			if (style & ES_MULTILINE)
			{
				GtkWidget* tv = gtk_text_view_new();
				GtkWidget* sw = gtk_scrolled_window_new(nullptr, nullptr);
				gtk_container_add(GTK_CONTAINER(sw), tv);
				wi->native = tv;
				gtk_container_add(GTK_CONTAINER(host), sw);
			}
			else
			{
				GtkWidget* e = gtk_entry_new();
				gtk_entry_set_text(GTK_ENTRY(e), nppWideToUtf8(title).c_str());
				wi->native = e;
				gtk_container_add(GTK_CONTAINER(host), e);
			}
		}
		else if (wi->kind == WindowKind::Combo)
		{
			GtkWidget* c = gtk_combo_box_text_new();
			wi->native = c;
			gtk_container_add(GTK_CONTAINER(host), c);
		}
		else if (wi->kind == WindowKind::Progress)
		{
			GtkWidget* pbar = gtk_progress_bar_new();
			wi->native = pbar;
			gtk_container_add(GTK_CONTAINER(host), pbar);
		}
		else if (wi->kind == WindowKind::Status)
		{
			GtkWidget* box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
			wi->native = box;
			wi->client = box;
			gtk_container_add(GTK_CONTAINER(host), box);
		}
		else
		{
			defWndProcCreate(wi);
			gtk_container_add(GTK_CONTAINER(host), wi->native);
		}

		if (p && p->client && GTK_IS_FIXED(p->client))
			gtk_fixed_put(GTK_FIXED(p->client), host, x, y);
		else if (p && p->client && GTK_IS_CONTAINER(p->client))
			gtk_container_add(GTK_CONTAINER(p->client), host);
		gtk_widget_set_size_request(host, std::max(w, 1), std::max(h, 1));
		if (p) p->children.push_back(hwndFrom(wi));
		if (style & WS_VISIBLE)
			gtk_widget_show_all(host);
	}

	wi->windowRect = {x, y, x + w, y + h};
	wi->visible = (style & WS_VISIBLE) != 0;

	CREATESTRUCTW cs{};
	cs.lpCreateParams = param;
	cs.hInstance = inst;
	cs.hMenu = menu;
	cs.hwndParent = parent;
	cs.cy = h; cs.cx = w; cs.y = y; cs.x = x;
	cs.style = style;
	cs.lpszName = wi->title.c_str();
	cs.lpszClass = wi->className.c_str();
	cs.dwExStyle = exStyle;

	if (wi->kind == WindowKind::Scintilla)
		nppScintillaSend(wi, WM_CREATE, 0, reinterpret_cast<LPARAM>(&cs));
	else
		nppDispatch(wi, WM_CREATE, 0, reinterpret_cast<LPARAM>(&cs));
	nppDispatch(wi, WM_NCCREATE, 0, reinterpret_cast<LPARAM>(&cs));
	if (style & WS_VISIBLE)
		nppDispatch(wi, WM_SHOWWINDOW, TRUE, 0);
	return hwndFrom(wi);
}

HWND CreateWindowExW(DWORD exStyle, LPCWSTR cls, LPCWSTR title, DWORD style,
	int x, int y, int w, int h, HWND parent, HMENU menu, HINSTANCE inst, LPVOID param)
{
	return nppCreateWindow(exStyle, cls ? cls : L"", title ? title : L"", style, x, y, w, h, parent, menu, inst, param);
}

HWND CreateWindowW(LPCWSTR cls, LPCWSTR title, DWORD style,
	int x, int y, int w, int h, HWND parent, HMENU menu, HINSTANCE inst, LPVOID param)
{
	return CreateWindowExW(0, cls, title, style, x, y, w, h, parent, menu, inst, param);
}

void nppDestroyWindow(WindowImpl* w)
{
	if (!w || w->destroyed) return;
	w->destroyed = true;
	nppDispatch(w, WM_DESTROY, 0, 0);
	for (HWND ch : w->children)
		DestroyWindow(ch);
	if (w->widget)
		gtk_widget_destroy(w->widget);
	w->widget = nullptr;
	forgetWindow(w);
	delete w;
}

BOOL DestroyWindow(HWND h)
{
	auto* w = implFrom(h);
	if (!w) return FALSE;
	nppDestroyWindow(w);
	return TRUE;
}

BOOL ShowWindow(HWND h, int cmd)
{
	auto* w = implFrom(h);
	if (!w || !w->widget) return FALSE;
	bool show = cmd != SW_HIDE;
	if (show) gtk_widget_show_all(w->widget);
	else gtk_widget_hide(w->widget);
	if (w->kind == WindowKind::TopLevel || GTK_IS_WINDOW(w->widget))
	{
		if (cmd == SW_MAXIMIZE) gtk_window_maximize(GTK_WINDOW(w->widget));
		if (cmd == SW_MINIMIZE || cmd == SW_SHOWMINIMIZED) gtk_window_iconify(GTK_WINDOW(w->widget));
		if (cmd == SW_RESTORE) gtk_window_unmaximize(GTK_WINDOW(w->widget));
	}
	w->visible = show;
	nppDispatch(w, WM_SHOWWINDOW, show ? TRUE : FALSE, 0);
	return TRUE;
}

BOOL UpdateWindow(HWND h)
{
	auto* w = implFrom(h);
	if (!w || !w->widget) return FALSE;
	gtk_widget_queue_draw(w->widget);
	while (gtk_events_pending()) gtk_main_iteration();
	return TRUE;
}

BOOL MoveWindow(HWND h, int x, int y, int wdt, int ht, BOOL repaint)
{
	return SetWindowPos(h, nullptr, x, y, wdt, ht, SWP_NOZORDER | (repaint ? 0 : SWP_NOREDRAW));
}

BOOL SetWindowPos(HWND h, HWND, int x, int y, int cx, int cy, UINT flags)
{
	auto* w = implFrom(h);
	if (!w || !w->widget) return FALSE;
	if (!(flags & SWP_NOMOVE) || !(flags & SWP_NOSIZE))
	{
		if (GTK_IS_WINDOW(w->widget))
		{
			if (!(flags & SWP_NOMOVE)) gtk_window_move(GTK_WINDOW(w->widget), x, y);
			if (!(flags & SWP_NOSIZE)) gtk_window_resize(GTK_WINDOW(w->widget), cx, cy);
		}
		else
		{
			auto* p = implFrom(w->parent);
			if (p && p->client && GTK_IS_FIXED(p->client) && !(flags & SWP_NOMOVE))
				gtk_fixed_move(GTK_FIXED(p->client), w->widget, x, y);
			if (!(flags & SWP_NOSIZE))
				gtk_widget_set_size_request(w->widget, cx, cy);
		}
		w->windowRect = {x, y, x + cx, y + cy};
	}
	if (flags & SWP_SHOWWINDOW) ShowWindow(h, SW_SHOW);
	if (flags & SWP_HIDEWINDOW) ShowWindow(h, SW_HIDE);
	if (!(flags & SWP_NOREDRAW) && w->widget) gtk_widget_queue_draw(w->widget);
	if (!(flags & SWP_NOSIZE))
		nppDispatch(w, WM_SIZE, SIZE_RESTORED, MAKELPARAM(cx, cy));
	if (!(flags & SWP_NOMOVE))
		nppDispatch(w, WM_MOVE, 0, MAKELPARAM(x, y));
	return TRUE;
}

BOOL GetWindowRect(HWND h, LPRECT rc)
{
	auto* w = implFrom(h);
	if (!w || !rc) return FALSE;
	if (w->widget)
	{
		GdkWindow* gw = gtk_widget_get_window(w->widget);
		if (gw)
		{
			gint x = 0, y = 0, ww = 0, hh = 0;
			gdk_window_get_origin(gw, &x, &y);
			ww = gdk_window_get_width(gw);
			hh = gdk_window_get_height(gw);
			rc->left = x; rc->top = y; rc->right = x + ww; rc->bottom = y + hh;
			return TRUE;
		}
		GtkAllocation a{};
		gtk_widget_get_allocation(w->widget, &a);
		rc->left = a.x; rc->top = a.y; rc->right = a.x + a.width; rc->bottom = a.y + a.height;
		return TRUE;
	}
	*rc = w->windowRect;
	return TRUE;
}

BOOL GetClientRect(HWND h, LPRECT rc)
{
	auto* w = implFrom(h);
	if (!w || !rc) return FALSE;
	GtkWidget* c = w->client ? w->client : w->widget;
	if (c)
	{
		GtkAllocation a{};
		gtk_widget_get_allocation(c, &a);
		rc->left = 0; rc->top = 0; rc->right = std::max(a.width, 1); rc->bottom = std::max(a.height, 1);
		return TRUE;
	}
	rc->left = 0; rc->top = 0;
	rc->right = w->windowRect.right - w->windowRect.left;
	rc->bottom = w->windowRect.bottom - w->windowRect.top;
	return TRUE;
}

BOOL ClientToScreen(HWND h, LPPOINT pt)
{
	auto* w = implFrom(h);
	if (!w || !pt || !w->widget) return FALSE;
	GdkWindow* gw = gtk_widget_get_window(w->widget);
	gint x = 0, y = 0;
	if (gw) gdk_window_get_origin(gw, &x, &y);
	pt->x += x; pt->y += y;
	return TRUE;
}

BOOL ScreenToClient(HWND h, LPPOINT pt)
{
	auto* w = implFrom(h);
	if (!w || !pt || !w->widget) return FALSE;
	GdkWindow* gw = gtk_widget_get_window(w->widget);
	gint x = 0, y = 0;
	if (gw) gdk_window_get_origin(gw, &x, &y);
	pt->x -= x; pt->y -= y;
	return TRUE;
}

int MapWindowPoints(HWND from, HWND to, LPPOINT pts, UINT count)
{
	if (!pts) return 0;
	for (UINT i = 0; i < count; ++i)
	{
		if (from) ClientToScreen(from, &pts[i]);
		if (to) ScreenToClient(to, &pts[i]);
	}
	return 0;
}

LONG_PTR GetWindowLongPtrW(HWND h, int idx)
{
	auto* w = implFrom(h);
	if (!w) return 0;
	switch (idx)
	{
	case GWLP_WNDPROC: return (LONG_PTR)w->wndproc;
	case GWLP_HINSTANCE: return (LONG_PTR)w->instance;
	case GWLP_HWNDPARENT: return (LONG_PTR)w->parent;
	case GWLP_ID: return w->id;
	case GWLP_USERDATA: return w->userData;
	case GWL_STYLE: return w->style;
	case GWL_EXSTYLE: return w->exStyle;
	default:
		if (idx >= 0 && idx < 16) return w->extra[idx];
		return 0;
	}
}

LONG_PTR SetWindowLongPtrW(HWND h, int idx, LONG_PTR val)
{
	auto* w = implFrom(h);
	if (!w) return 0;
	LONG_PTR old = GetWindowLongPtrW(h, idx);
	switch (idx)
	{
	case GWLP_WNDPROC: w->wndproc = (WNDPROC)val; break;
	case GWLP_HINSTANCE: w->instance = (HINSTANCE)val; break;
	case GWLP_HWNDPARENT: w->parent = (HWND)val; break;
	case GWLP_ID: w->id = (int)val; break;
	case GWLP_USERDATA: w->userData = val; break;
	case GWL_STYLE: w->style = (DWORD)val; break;
	case GWL_EXSTYLE: w->exStyle = (DWORD)val; break;
	default:
		if (idx >= 0 && idx < 16) w->extra[idx] = val;
		break;
	}
	return old;
}

LONG GetWindowLongW(HWND h, int idx) { return (LONG)GetWindowLongPtrW(h, idx); }
LONG SetWindowLongW(HWND h, int idx, LONG val) { return (LONG)SetWindowLongPtrW(h, idx, val); }
ULONG_PTR GetClassLongPtrW(HWND, int) { return 0; }
ULONG_PTR SetClassLongPtrW(HWND, int, LONG_PTR) { return 0; }

HWND GetParent(HWND h)
{
	auto* w = implFrom(h);
	return w ? w->parent : nullptr;
}

HWND SetParent(HWND h, HWND parent)
{
	auto* w = implFrom(h);
	if (!w) return nullptr;
	HWND old = w->parent;
	w->parent = parent;
	return old;
}

HWND GetDlgItem(HWND dlg, int id)
{
	auto* w = implFrom(dlg);
	if (!w) return nullptr;
	if (w->id == id) return dlg;
	for (HWND ch : w->children)
	{
		if (GetWindowLongPtrW(ch, GWLP_ID) == id) return ch;
		HWND nested = GetDlgItem(ch, id);
		if (nested) return nested;
	}
	return nullptr;
}

BOOL EnableWindow(HWND h, BOOL enable)
{
	auto* w = implFrom(h);
	if (!w || !w->widget) return FALSE;
	gtk_widget_set_sensitive(w->widget, enable);
	w->enabled = enable != FALSE;
	return TRUE;
}

BOOL IsWindow(HWND h) { auto* w = implFrom(h); return w && !w->destroyed; }
BOOL IsWindowVisible(HWND h) { auto* w = implFrom(h); return w && w->visible; }
BOOL IsWindowEnabled(HWND h) { auto* w = implFrom(h); return w && w->enabled; }
BOOL IsIconic(HWND h)
{
	auto* w = implFrom(h);
	return w && w->widget && GTK_IS_WINDOW(w->widget) && gtk_window_is_active(GTK_WINDOW(w->widget)) == FALSE && FALSE;
}
BOOL IsZoomed(HWND) { return FALSE; }
BOOL IsChild(HWND parent, HWND child)
{
	auto* c = implFrom(child);
	while (c)
	{
		if (c->parent == parent) return TRUE;
		c = implFrom(c->parent);
	}
	return FALSE;
}

HWND GetForegroundWindow(void) { return g_foreground; }
BOOL SetForegroundWindow(HWND h) { g_foreground = h; g_active = h; if (auto* w = implFrom(h)) { if (w->widget && GTK_IS_WINDOW(w->widget)) gtk_window_present(GTK_WINDOW(w->widget)); } return TRUE; }
HWND GetFocus(void) { return g_focus; }
HWND SetFocus(HWND h) { HWND old = g_focus; g_focus = h; if (auto* w = implFrom(h)) { if (w->widget) gtk_widget_grab_focus(w->widget); nppDispatch(w, WM_SETFOCUS, (WPARAM)old, 0); } return old; }
HWND GetActiveWindow(void) { return g_active; }
HWND SetActiveWindow(HWND h) { HWND old = g_active; g_active = h; return old; }
HWND GetCapture(void) { return g_capture; }
HWND SetCapture(HWND h) { HWND old = g_capture; g_capture = h; return old; }
BOOL ReleaseCapture(void) { g_capture = nullptr; return TRUE; }
HWND GetDesktopWindow(void) { return nullptr; }

HWND FindWindowW(LPCWSTR cls, LPCWSTR title)
{
	std::lock_guard<std::mutex> lock(g_winMutex);
	for (auto* w : g_windows)
	{
		if (cls && w->className != cls) continue;
		if (title && w->title != title) continue;
		return hwndFrom(w);
	}
	return nullptr;
}
HWND FindWindowExW(HWND parent, HWND after, LPCWSTR cls, LPCWSTR title)
{
	bool seen = after == nullptr;
	std::lock_guard<std::mutex> lock(g_winMutex);
	for (auto* w : g_windows)
	{
		if (w->parent != parent) continue;
		if (!seen) { if (hwndFrom(w) == after) seen = true; continue; }
		if (cls && w->className != cls) continue;
		if (title && w->title != title) continue;
		return hwndFrom(w);
	}
	return nullptr;
}

BOOL EnumWindows(WNDENUMPROC cb, LPARAM lp)
{
	if (!cb) return FALSE;
	std::vector<WindowImpl*> copy;
	{
		std::lock_guard<std::mutex> lock(g_winMutex);
		copy = g_windows;
	}
	for (auto* w : copy)
		if (!cb(hwndFrom(w), lp)) return FALSE;
	return TRUE;
}
BOOL EnumChildWindows(HWND parent, WNDENUMPROC cb, LPARAM lp)
{
	auto* p = implFrom(parent);
	if (!p || !cb) return FALSE;
	auto children = p->children;
	for (HWND ch : children)
		if (!cb(ch, lp)) return FALSE;
	return TRUE;
}
BOOL EnumThreadWindows(DWORD, WNDENUMPROC cb, LPARAM lp) { return EnumWindows(cb, lp); }

HWND GetWindow(HWND h, UINT cmd)
{
	auto* w = implFrom(h);
	if (!w) return nullptr;
	if (cmd == GW_CHILD) return w->children.empty() ? nullptr : w->children.front();
	if (cmd == GW_OWNER) return w->parent;
	if (cmd == GW_HWNDNEXT || cmd == GW_HWNDPREV)
	{
		auto* p = implFrom(w->parent);
		if (!p) return nullptr;
		auto& ch = p->children;
		auto it = std::find(ch.begin(), ch.end(), h);
		if (it == ch.end()) return nullptr;
		if (cmd == GW_HWNDNEXT) { ++it; return it == ch.end() ? nullptr : *it; }
		if (it == ch.begin()) return nullptr;
		--it; return *it;
	}
	return nullptr;
}
HWND GetTopWindow(HWND h) { return GetWindow(h, GW_CHILD); }
HWND GetAncestor(HWND h, UINT flags)
{
	if (flags == GA_PARENT) return GetParent(h);
	auto* w = implFrom(h);
	HWND cur = h;
	while (w && w->parent) { cur = w->parent; w = implFrom(w->parent); }
	return cur;
}

int GetWindowTextW(HWND h, LPWSTR buf, int max)
{
	auto* w = implFrom(h);
	if (!w || !buf || max <= 0) return 0;
	std::wstring t = w->title;
	if (w->native && GTK_IS_ENTRY(w->native))
		t = nppUtf8ToWide(gtk_entry_get_text(GTK_ENTRY(w->native)));
	else if (w->native && GTK_IS_LABEL(w->native))
		t = nppUtf8ToWide(gtk_label_get_text(GTK_LABEL(w->native)));
	else if (w->widget && GTK_IS_WINDOW(w->widget))
		t = nppUtf8ToWide(gtk_window_get_title(GTK_WINDOW(w->widget)));
	int n = (int)std::min((size_t)max - 1, t.size());
	wmemcpy(buf, t.c_str(), n);
	buf[n] = 0;
	return n;
}
int GetWindowTextLengthW(HWND h)
{
	wchar_t tmp[4096];
	return GetWindowTextW(h, tmp, 4096);
}
BOOL SetWindowTextW(HWND h, LPCWSTR text)
{
	auto* w = implFrom(h);
	if (!w) return FALSE;
	w->title = text ? text : L"";
	std::string u = nppWideToUtf8(w->title);
	if (w->widget && GTK_IS_WINDOW(w->widget)) gtk_window_set_title(GTK_WINDOW(w->widget), u.c_str());
	if (w->native && GTK_IS_ENTRY(w->native)) gtk_entry_set_text(GTK_ENTRY(w->native), u.c_str());
	if (w->native && GTK_IS_LABEL(w->native)) gtk_label_set_text(GTK_LABEL(w->native), u.c_str());
	if (w->native && GTK_IS_BUTTON(w->native)) gtk_button_set_label(GTK_BUTTON(w->native), u.c_str());
	nppDispatch(w, WM_SETTEXT, 0, (LPARAM)text);
	return TRUE;
}

BOOL SetDlgItemTextW(HWND dlg, int id, LPCWSTR text) { return SetWindowTextW(GetDlgItem(dlg, id), text); }
UINT GetDlgItemTextW(HWND dlg, int id, LPWSTR buf, int max) { return (UINT)GetWindowTextW(GetDlgItem(dlg, id), buf, max); }
UINT GetDlgItemTextA(HWND dlg, int id, LPSTR buf, int max)
{
	wchar_t w[4096];
	GetDlgItemTextW(dlg, id, w, 4096);
	if (!buf || max <= 0) return 0;
	std::string u = nppWideToUtf8(w);
	snprintf(buf, (size_t)max, "%s", u.c_str());
	return (UINT)strlen(buf);
}
BOOL SetDlgItemInt(HWND dlg, int id, UINT value, BOOL)
{
	wchar_t buf[32];
	swprintf(buf, 32, L"%u", value);
	return SetDlgItemTextW(dlg, id, buf);
}
UINT GetDlgItemInt(HWND dlg, int id, BOOL* translated, BOOL)
{
	wchar_t buf[64]{};
	GetDlgItemTextW(dlg, id, buf, 64);
	if (translated) *translated = TRUE;
	return (UINT)wcstoul(buf, nullptr, 10);
}

BOOL InvalidateRect(HWND h, const RECT*, BOOL)
{
	auto* w = implFrom(h);
	if (!w || !w->widget) return FALSE;
	gtk_widget_queue_draw(w->widget);
	return TRUE;
}
BOOL ValidateRect(HWND, const RECT*) { return TRUE; }
BOOL RedrawWindow(HWND h, const RECT*, HRGN, UINT) { return InvalidateRect(h, nullptr, TRUE); }
BOOL GetUpdateRect(HWND h, LPRECT rc, BOOL)
{
	if (rc) GetClientRect(h, rc);
	return TRUE;
}

int GetClassNameW(HWND h, LPWSTR buf, int max)
{
	auto* w = implFrom(h);
	if (!w || !buf || max <= 0) return 0;
	int n = (int)std::min((size_t)max - 1, w->className.size());
	wmemcpy(buf, w->className.c_str(), n);
	buf[n] = 0;
	return n;
}

int GetSystemMetrics(int idx)
{
	GdkDisplay* dpy = gdk_display_get_default();
	GdkMonitor* mon = dpy ? gdk_display_get_primary_monitor(dpy) : nullptr;
	GdkRectangle geo{};
	if (mon) gdk_monitor_get_geometry(mon, &geo);
	int scale = mon ? gdk_monitor_get_scale_factor(mon) : 1;
	switch (idx)
	{
	case SM_CXSCREEN: return geo.width ? geo.width : 1920;
	case SM_CYSCREEN: return geo.height ? geo.height : 1080;
	case SM_CXVIRTUALSCREEN: return geo.width ? geo.width : 1920;
	case SM_CYVIRTUALSCREEN: return geo.height ? geo.height : 1080;
	case SM_XVIRTUALSCREEN: return 0;
	case SM_YVIRTUALSCREEN: return 0;
	case SM_CXSMICON: return 16;
	case SM_CYSMICON: return 16;
	case SM_CXICON: return 32;
	case SM_CYICON: return 32;
	case SM_CXCURSOR: return 24;
	case SM_CYCURSOR: return 24;
	case SM_CYCAPTION: return 24;
	case SM_CYMENU: return 22;
	case SM_CXVSCROLL: return 16;
	case SM_CYHSCROLL: return 16;
	case SM_CXEDGE: return 2;
	case SM_CYEDGE: return 2;
	case SM_CXFRAME: case SM_CXSIZEFRAME: return 4;
	case SM_CYFRAME: case SM_CYSIZEFRAME: return 4;
	case SM_CXBORDER: case SM_CYBORDER: return 1;
	case SM_CYSMCAPTION: return 20;
	case SM_CXDOUBLECLK: return 4;
	case SM_CYDOUBLECLK: return 4;
	case SM_CXDRAG: return 4;
	case SM_CYDRAG: return 4;
	case SM_CXMINTRACK: return 120;
	case SM_CYMINTRACK: return 40;
	case SM_CXMAXIMIZED: return geo.width;
	case SM_CYMAXIMIZED: return geo.height;
	case SM_CMONITORS: return dpy ? gdk_display_get_n_monitors(dpy) : 1;
	case SM_MOUSEWHEELPRESENT: return 1;
	case SM_SWAPBUTTON: return 0;
	default: return 0;
	}
	(void)scale;
}

BOOL SystemParametersInfoW(UINT action, UINT, PVOID pvParam, UINT)
{
	if (action == SPI_GETWORKAREA && pvParam)
	{
		RECT* rc = (RECT*)pvParam;
		rc->left = 0; rc->top = 0;
		rc->right = GetSystemMetrics(SM_CXSCREEN);
		rc->bottom = GetSystemMetrics(SM_CYSCREEN);
		return TRUE;
	}
	if (action == SPI_GETWHEELSCROLLLINES && pvParam)
	{
		*(UINT*)pvParam = 3;
		return TRUE;
	}
	if (action == SPI_GETNONCLIENTMETRICS && pvParam)
	{
		return TRUE;
	}
	return TRUE;
}

int MessageBoxW(HWND owner, LPCWSTR text, LPCWSTR caption, UINT type)
{
	GtkWidget* parent = nullptr;
	if (auto* w = implFrom(owner)) parent = w->widget;
	GtkMessageType mt = GTK_MESSAGE_INFO;
	if (type & MB_ICONERROR) mt = GTK_MESSAGE_ERROR;
	else if (type & MB_ICONWARNING) mt = GTK_MESSAGE_WARNING;
	else if (type & MB_ICONQUESTION) mt = GTK_MESSAGE_QUESTION;
	GtkButtonsType bt = GTK_BUTTONS_OK;
	if ((type & MB_TYPEMASK) == MB_OKCANCEL) bt = GTK_BUTTONS_OK_CANCEL;
	else if ((type & MB_TYPEMASK) == MB_YESNO) bt = GTK_BUTTONS_YES_NO;
	else if ((type & MB_TYPEMASK) == MB_YESNOCANCEL) bt = GTK_BUTTONS_YES_NO;
	GtkWidget* dlg = gtk_message_dialog_new(parent && GTK_IS_WINDOW(parent) ? GTK_WINDOW(parent) : nullptr,
		GTK_DIALOG_MODAL, mt, bt, "%s", nppWideToUtf8(text ? text : L"").c_str());
	gtk_window_set_title(GTK_WINDOW(dlg), nppWideToUtf8(caption ? caption : L"Notepad++").c_str());
	if ((type & MB_TYPEMASK) == MB_YESNOCANCEL)
		gtk_dialog_add_button(GTK_DIALOG(dlg), "Cancel", GTK_RESPONSE_CANCEL);
	gint resp = gtk_dialog_run(GTK_DIALOG(dlg));
	gtk_widget_destroy(dlg);
	if (resp == GTK_RESPONSE_OK) return IDOK;
	if (resp == GTK_RESPONSE_YES) return IDYES;
	if (resp == GTK_RESPONSE_NO) return IDNO;
	if (resp == GTK_RESPONSE_CANCEL) return IDCANCEL;
	return IDOK;
}
int MessageBoxA(HWND o, LPCSTR t, LPCSTR c, UINT type)
{
	return MessageBoxW(o, nppUtf8ToWide(t ? t : "").c_str(), nppUtf8ToWide(c ? c : "").c_str(), type);
}
BOOL MessageBeep(UINT) { gdk_display_beep(gdk_display_get_default()); return TRUE; }

LRESULT nppDispatch(WindowImpl* w, UINT msg, WPARAM wp, LPARAM lp)
{
	if (!w) return 0;
	if (w->kind == WindowKind::Scintilla && msg != WM_CREATE)
		return nppScintillaSend(w, msg, wp, lp);
	if (w->wndproc)
		return w->wndproc(hwndFrom(w), msg, wp, lp);
	return nppControlSend(w, msg, wp, lp);
}

LRESULT SendMessageW(HWND h, UINT msg, WPARAM w, LPARAM l)
{
	auto* wi = implFrom(h);
	if (!wi) return 0;
	if (wi->kind == WindowKind::Scintilla)
		return nppScintillaSend(wi, msg, w, l);
	LRESULT ctrl = nppControlSend(wi, msg, w, l);
	if (wi->wndproc && (wi->kind == WindowKind::Generic || wi->kind == WindowKind::TopLevel
		|| wi->kind == WindowKind::Dialog || wi->kind == WindowKind::Drawing
		|| wi->kind == WindowKind::Tab || wi->kind == WindowKind::Toolbar
		|| wi->kind == WindowKind::Rebar || wi->kind == WindowKind::Status))
		return wi->wndproc(h, msg, w, l);
	if (wi->wndproc)
		return wi->wndproc(h, msg, w, l);
	return ctrl;
}
LRESULT SendMessageA(HWND h, UINT msg, WPARAM w, LPARAM l) { return SendMessageW(h, msg, w, l); }
LRESULT SendMessageTimeoutW(HWND h, UINT msg, WPARAM w, LPARAM l, UINT, UINT, PDWORD_PTR result)
{
	LRESULT r = SendMessageW(h, msg, w, l);
	if (result) *result = (DWORD_PTR)r;
	return TRUE;
}
BOOL PostMessageW(HWND h, UINT msg, WPARAM w, LPARAM l)
{
	std::lock_guard<std::mutex> lock(g_msgMutex);
	g_msgQueue.push_back(MSG{h, msg, w, l, 0, POINT{}});
	return TRUE;
}
BOOL PostThreadMessageW(DWORD, UINT msg, WPARAM w, LPARAM l) { return PostMessageW(nullptr, msg, w, l); }
void PostQuitMessage(int code) { nppPostQuit(code); }
void nppPostQuit(int code)
{
	g_nppQuit = true;
	g_nppQuitCode = code;
	PostMessageW(nullptr, WM_QUIT, (WPARAM)code, 0);
}

static BOOL takeQueued(LPMSG msg, HWND h, UINT min, UINT max)
{
	std::lock_guard<std::mutex> lock(g_msgMutex);
	for (auto it = g_msgQueue.begin(); it != g_msgQueue.end(); ++it)
	{
		if (h && it->hwnd != h) continue;
		if (min || max)
		{
			if (it->message < min || it->message > max) continue;
		}
		*msg = *it;
		g_msgQueue.erase(it);
		return TRUE;
	}
	return FALSE;
}

BOOL GetMessageW(LPMSG msg, HWND h, UINT min, UINT max)
{
	if (!msg) return FALSE;
	for (;;)
	{
		if (takeQueued(msg, h, min, max))
			return msg->message != WM_QUIT;
		if (g_nppQuit)
		{
			msg->message = WM_QUIT;
			msg->wParam = (WPARAM)g_nppQuitCode;
			return FALSE;
		}
		if (!gtk_events_pending())
		{
			gtk_main_iteration();
			continue;
		}
		gtk_main_iteration();
	}
}

BOOL PeekMessageW(LPMSG msg, HWND h, UINT min, UINT max, UINT remove)
{
	while (gtk_events_pending()) gtk_main_iteration_do(FALSE);
	if (!msg) return FALSE;
	if (remove & PM_REMOVE)
		return takeQueued(msg, h, min, max);
	std::lock_guard<std::mutex> lock(g_msgMutex);
	for (auto& m : g_msgQueue)
	{
		if (h && m.hwnd != h) continue;
		*msg = m;
		return TRUE;
	}
	return FALSE;
}

BOOL TranslateMessage(const MSG* msg)
{
	(void)msg;
	return FALSE;
}
LRESULT DispatchMessageW(const MSG* msg)
{
	if (!msg) return 0;
	if (!msg->hwnd) return 0;
	return SendMessageW(msg->hwnd, msg->message, msg->wParam, msg->lParam);
}
BOOL WaitMessage(void)
{
	gtk_main_iteration();
	return TRUE;
}

LRESULT CallWindowProcW(WNDPROC proc, HWND h, UINT msg, WPARAM w, LPARAM l)
{
	if (proc) return proc(h, msg, w, l);
	return DefWindowProcW(h, msg, w, l);
}

LRESULT DefWindowProcW(HWND h, UINT msg, WPARAM w, LPARAM l)
{
	auto* wi = implFrom(h);
	if (!wi) return 0;
	switch (msg)
	{
	case WM_CLOSE:
		DestroyWindow(h);
		return 0;
	case WM_DESTROY:
		if (!wi->parent)
			PostQuitMessage(0);
		return 0;
	case WM_ERASEBKGND:
		return 1;
	case WM_SETTEXT:
		return TRUE;
	case WM_GETTEXT:
		return GetWindowTextW(h, (LPWSTR)l, (int)w);
	case WM_GETTEXTLENGTH:
		return GetWindowTextLengthW(h);
	case WM_NCCREATE:
	case WM_CREATE:
		return TRUE;
	default:
		return nppControlSend(wi, msg, w, l);
	}
}

INT_PTR DefDlgProcW(HWND h, UINT msg, WPARAM w, LPARAM l)
{
	return (INT_PTR)DefWindowProcW(h, msg, w, l);
}

static gboolean timerThunk(gpointer data)
{
	auto* pack = reinterpret_cast<uintptr_t*>(data);
	HWND h = (HWND)pack[0];
	UINT_PTR id = (UINT_PTR)pack[1];
	TIMERPROC proc = (TIMERPROC)pack[2];
	if (proc) proc(h, WM_TIMER, id, GetTickCount());
	else if (h) SendMessageW(h, WM_TIMER, id, 0);
	return TRUE;
}

UINT_PTR SetTimer(HWND h, UINT_PTR id, UINT elapse, TIMERPROC proc)
{
	auto* pack = new uintptr_t[3];
	pack[0] = (uintptr_t)h;
	pack[1] = id ? id : 1;
	pack[2] = (uintptr_t)proc;
	guint gid = g_timeout_add(elapse ? elapse : 1, timerThunk, pack);
	if (auto* w = implFrom(h)) w->timerId = gid;
	return id ? id : gid;
}
BOOL KillTimer(HWND h, UINT_PTR)
{
	auto* w = implFrom(h);
	if (w && w->timerId) { g_source_remove((guint)w->timerId); w->timerId = 0; }
	return TRUE;
}

BOOL GetCursorPos(LPPOINT pt)
{
	if (!pt) return FALSE;
	GdkDisplay* dpy = gdk_display_get_default();
	GdkSeat* seat = dpy ? gdk_display_get_default_seat(dpy) : nullptr;
	GdkDevice* dev = seat ? gdk_seat_get_pointer(seat) : nullptr;
	if (!dev) { pt->x = pt->y = 0; return TRUE; }
	gint x = 0, y = 0;
	gdk_device_get_position(dev, nullptr, &x, &y);
	pt->x = x; pt->y = y;
	return TRUE;
}
BOOL SetCursorPos(int x, int y)
{
	GdkDisplay* dpy = gdk_display_get_default();
	GdkSeat* seat = dpy ? gdk_display_get_default_seat(dpy) : nullptr;
	GdkDevice* dev = seat ? gdk_seat_get_pointer(seat) : nullptr;
	if (dev) gdk_device_warp(dev, gdk_display_get_default_screen(dpy), x, y);
	return TRUE;
}

HCURSOR LoadCursorW(HINSTANCE, LPCWSTR name)
{
	const char* id = "default";
	if (name == IDC_ARROW) id = "default";
	else if (name == IDC_IBEAM) id = "text";
	else if (name == IDC_WAIT) id = "wait";
	else if (name == IDC_CROSS) id = "crosshair";
	else if (name == IDC_HAND) id = "pointer";
	else if (name == IDC_SIZEWE) id = "ew-resize";
	else if (name == IDC_SIZENS) id = "ns-resize";
	else if (name == IDC_SIZENWSE) id = "nwse-resize";
	else if (name == IDC_SIZENESW) id = "nesw-resize";
	else if (name == IDC_SIZEALL) id = "move";
	else if (name == IDC_NO) id = "not-allowed";
	GdkCursor* c = gdk_cursor_new_from_name(gdk_display_get_default(), id);
	return (HCURSOR)c;
}
HCURSOR SetCursor(HCURSOR cur)
{
	GdkDisplay* dpy = gdk_display_get_default();
	GdkSeat* seat = dpy ? gdk_display_get_default_seat(dpy) : nullptr;
	GdkDevice* dev = seat ? gdk_seat_get_pointer(seat) : nullptr;
	if (dev) gdk_window_set_cursor(gdk_get_default_root_window(), (GdkCursor*)cur);
	return cur;
}
int ShowCursor(BOOL) { return 1; }
HCURSOR GetCursor(void) { return LoadCursorW(nullptr, IDC_ARROW); }

HICON LoadIconW(HINSTANCE inst, LPCWSTR name) { return (HICON)nppLoadImage(name, IMAGE_ICON, 32, 32); }
HANDLE LoadImageW(HINSTANCE, LPCWSTR name, UINT type, int cx, int cy, UINT) { return nppLoadImage(name, type, cx, cy); }
HBITMAP LoadBitmapW(HINSTANCE, LPCWSTR name) { return (HBITMAP)nppLoadImage(name, IMAGE_BITMAP, 0, 0); }

BOOL AdjustWindowRect(LPRECT rc, DWORD, BOOL) { return rc != nullptr; }
BOOL AdjustWindowRectEx(LPRECT rc, DWORD, BOOL, DWORD) { return rc != nullptr; }
BOOL GetWindowPlacement(HWND h, WINDOWPLACEMENT* wp)
{
	if (!wp) return FALSE;
	GetWindowRect(h, &wp->rcNormalPosition);
	wp->showCmd = IsWindowVisible(h) ? SW_SHOW : SW_HIDE;
	return TRUE;
}
BOOL SetWindowPlacement(HWND h, const WINDOWPLACEMENT* wp)
{
	if (!wp) return FALSE;
	MoveWindow(h, wp->rcNormalPosition.left, wp->rcNormalPosition.top,
		wp->rcNormalPosition.right - wp->rcNormalPosition.left,
		wp->rcNormalPosition.bottom - wp->rcNormalPosition.top, TRUE);
	return TRUE;
}

int GetDlgCtrlID(HWND h) { return (int)GetWindowLongPtrW(h, GWLP_ID); }
HWND WindowFromPoint(POINT pt)
{
	std::lock_guard<std::mutex> lock(g_winMutex);
	for (auto it = g_windows.rbegin(); it != g_windows.rend(); ++it)
	{
		RECT rc{};
		GetWindowRect(hwndFrom(*it), &rc);
		if (PtInRect(&rc, pt)) return hwndFrom(*it);
	}
	return nullptr;
}
HWND ChildWindowFromPoint(HWND parent, POINT pt)
{
	auto* p = implFrom(parent);
	if (!p) return nullptr;
	for (HWND ch : p->children)
	{
		RECT rc{};
		GetClientRect(ch, &rc);
		if (PtInRect(&rc, pt)) return ch;
	}
	return nullptr;
}

BOOL GetMonitorInfoW(HMONITOR, LPMONITORINFO mi)
{
	if (!mi) return FALSE;
	mi->rcMonitor = {0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)};
	mi->rcWork = mi->rcMonitor;
	mi->dwFlags = MONITORINFOF_PRIMARY;
	return TRUE;
}
HMONITOR MonitorFromWindow(HWND, DWORD) { return (HMONITOR)1; }
HMONITOR MonitorFromRect(LPCRECT, DWORD) { return (HMONITOR)1; }
HMONITOR MonitorFromPoint(POINT, DWORD) { return (HMONITOR)1; }
BOOL EnumDisplayMonitors(HDC, LPCRECT, MONITORENUMPROC proc, LPARAM data)
{
	if (!proc) return FALSE;
	RECT rc{0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)};
	proc((HMONITOR)1, nullptr, &rc, data);
	return TRUE;
}

UINT GetDpiForWindow(HWND) { return 96; }
UINT GetDpiForSystem(void) { return 96; }
BOOL SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT) { return TRUE; }
HRESULT SetProcessDpiAwareness(PROCESS_DPI_AWARENESS) { return S_OK; }
BOOL SetProcessDPIAware(void) { return TRUE; }
int GetSystemMetricsForDpi(int idx, UINT) { return GetSystemMetrics(idx); }
BOOL AdjustWindowRectExForDpi(LPRECT rc, DWORD s, BOOL m, DWORD e, UINT) { return AdjustWindowRectEx(rc, s, m, e); }
BOOL EnableNonClientDpiScaling(HWND) { return TRUE; }
int GetDpiForMonitor(HMONITOR, MONITOR_DPI_TYPE, UINT* dx, UINT* dy)
{
	if (dx) *dx = 96;
	if (dy) *dy = 96;
	return S_OK;
}
BOOL SystemParametersInfoForDpi(UINT a, UINT u, PVOID p, UINT f, UINT) { return SystemParametersInfoW(a, u, p, f); }

DWORD GetSysColor(int idx)
{
	switch (idx)
	{
	case COLOR_WINDOW: return RGB(255, 255, 255);
	case COLOR_WINDOWTEXT: return RGB(0, 0, 0);
	case COLOR_BTNFACE: return RGB(240, 240, 240);
	case COLOR_BTNTEXT: return RGB(0, 0, 0);
	case COLOR_HIGHLIGHT: return RGB(0, 120, 215);
	case COLOR_HIGHLIGHTTEXT: return RGB(255, 255, 255);
	case COLOR_GRAYTEXT: return RGB(128, 128, 128);
	case COLOR_HOTLIGHT: return RGB(0, 102, 204);
	case COLOR_3DFACE: return RGB(240, 240, 240);
	case COLOR_3DSHADOW: return RGB(160, 160, 160);
	case COLOR_3DHIGHLIGHT: return RGB(255, 255, 255);
	case COLOR_MENU: return RGB(240, 240, 240);
	case COLOR_MENUTEXT: return RGB(0, 0, 0);
	default: return RGB(0, 0, 0);
	}
}

SHORT GetKeyState(int vk)
{
	GdkDisplay* dpy = gdk_display_get_default();
	GdkSeat* seat = dpy ? gdk_display_get_default_seat(dpy) : nullptr;
	GdkDevice* dev = seat ? gdk_seat_get_keyboard(seat) : nullptr;
	if (!dev) return 0;
	GdkModifierType mods{};
	gdk_window_get_device_position(gdk_get_default_root_window(), dev, nullptr, nullptr, &mods);
	if (vk == VK_SHIFT) return (mods & GDK_SHIFT_MASK) ? (SHORT)0x8000 : 0;
	if (vk == VK_CONTROL) return (mods & GDK_CONTROL_MASK) ? (SHORT)0x8000 : 0;
	if (vk == VK_MENU) return (mods & GDK_MOD1_MASK) ? (SHORT)0x8000 : 0;
	return 0;
}
SHORT GetAsyncKeyState(int vk) { return GetKeyState(vk); }
BOOL GetKeyboardState(PBYTE keys)
{
	if (!keys) return FALSE;
	memset(keys, 0, 256);
	if (GetKeyState(VK_SHIFT) < 0) keys[VK_SHIFT] = 0x80;
	if (GetKeyState(VK_CONTROL) < 0) keys[VK_CONTROL] = 0x80;
	if (GetKeyState(VK_MENU) < 0) keys[VK_MENU] = 0x80;
	return TRUE;
}
BOOL SetKeyboardState(LPBYTE) { return TRUE; }
UINT MapVirtualKeyW(UINT code, UINT) { return code; }
UINT MapVirtualKeyExW(UINT code, UINT m, HKL) { return MapVirtualKeyW(code, m); }

DWORD GetMessagePos(void)
{
	POINT p{};
	GetCursorPos(&p);
	return (DWORD)MAKELPARAM(p.x, p.y);
}
LONG GetMessageTime(void) { return (LONG)GetTickCount(); }
LPARAM GetMessageExtraInfo(void) { return 0; }
UINT GetDoubleClickTime(void) { return 500; }
BOOL SetDoubleClickTime(UINT) { return TRUE; }
UINT GetCaretBlinkTime(void) { return 530; }
BOOL SetCaretBlinkTime(UINT) { return TRUE; }

BOOL FlashWindow(HWND, BOOL) { return TRUE; }
BOOL FlashWindowEx(PFLASHWINFO) { return TRUE; }
BOOL BringWindowToTop(HWND h) { return SetForegroundWindow(h); }
BOOL OpenIcon(HWND h) { return ShowWindow(h, SW_RESTORE); }
BOOL CloseWindow(HWND h) { return ShowWindow(h, SW_MINIMIZE); }
BOOL LockWindowUpdate(HWND) { return TRUE; }
BOOL ChangeWindowMessageFilter(UINT, DWORD) { return TRUE; }
BOOL ChangeWindowMessageFilterEx(HWND, UINT, DWORD, PCHANGEFILTERSTRUCT) { return TRUE; }
BOOL RegisterHotKey(HWND, int, UINT, UINT) { return TRUE; }
BOOL UnregisterHotKey(HWND, int) { return TRUE; }
UINT RegisterWindowMessageW(LPCWSTR name)
{
	static UINT next = 0xC000;
	static std::unordered_map<std::wstring, UINT> map;
	if (!name) return 0;
	auto it = map.find(name);
	if (it != map.end()) return it->second;
	UINT id = next++;
	map[name] = id;
	return id;
}

int SetWindowRgn(HWND, HRGN, BOOL) { return 1; }
int GetWindowRgn(HWND, HRGN) { return NULLREGION; }
BOOL ShowScrollBar(HWND, int, BOOL) { return TRUE; }
int SetScrollInfo(HWND, int, LPCSCROLLINFO si, BOOL) { return si ? si->nPos : 0; }
BOOL GetScrollInfo(HWND, int, LPSCROLLINFO si) { if (si) { si->nMin = 0; si->nMax = 0; si->nPos = 0; } return TRUE; }
int SetScrollPos(HWND, int, int pos, BOOL) { return pos; }
int GetScrollPos(HWND, int) { return 0; }
int SetScrollRange(HWND, int, int, int, BOOL) { return TRUE; }
BOOL GetScrollRange(HWND, int, LPINT min, LPINT max) { if (min) *min = 0; if (max) *max = 0; return TRUE; }
BOOL ScrollWindow(HWND, int, int, const RECT*, const RECT*) { return TRUE; }
int ScrollWindowEx(HWND, int, int, const RECT*, const RECT*, HRGN, LPRECT, UINT) { return SIMPLEREGION; }

HWND GetNextDlgTabItem(HWND dlg, HWND ctl, BOOL prev)
{
	return FindWindowExW(dlg, prev ? nullptr : ctl, nullptr, nullptr);
}
HWND GetNextDlgGroupItem(HWND dlg, HWND ctl, BOOL prev) { return GetNextDlgTabItem(dlg, ctl, prev); }

BOOL SetPropW(HWND h, LPCWSTR, HANDLE data) { SetWindowLongPtrW(h, GWLP_USERDATA, (LONG_PTR)data); return TRUE; }
HANDLE GetPropW(HWND h, LPCWSTR) { return (HANDLE)GetWindowLongPtrW(h, GWLP_USERDATA); }
HANDLE RemovePropW(HWND h, LPCWSTR) { HANDLE old = GetPropW(h, nullptr); SetWindowLongPtrW(h, GWLP_USERDATA, 0); return old; }

BOOL IsDialogMessageW(HWND dlg, LPMSG msg)
{
	if (!dlg || !msg) return FALSE;
	if (msg->message == WM_KEYDOWN && msg->wParam == VK_TAB)
	{
		HWND next = GetNextDlgTabItem(dlg, msg->hwnd, GetKeyState(VK_SHIFT) < 0);
		if (next) { SetFocus(next); return TRUE; }
	}
	if (msg->message == WM_KEYDOWN && msg->wParam == VK_ESCAPE)
	{
		SendMessageW(dlg, WM_COMMAND, IDCANCEL, 0);
		return TRUE;
	}
	if (msg->message == WM_KEYDOWN && msg->wParam == VK_RETURN)
	{
		SendMessageW(dlg, WM_COMMAND, IDOK, 0);
		return TRUE;
	}
	return FALSE;
}

LRESULT SendDlgItemMessageW(HWND dlg, int id, UINT msg, WPARAM w, LPARAM l)
{
	return SendMessageW(GetDlgItem(dlg, id), msg, w, l);
}
BOOL CheckDlgButton(HWND dlg, int id, UINT check)
{
	return (BOOL)SendMessageW(GetDlgItem(dlg, id), BM_SETCHECK, check, 0);
}
UINT IsDlgButtonChecked(HWND dlg, int id)
{
	return (UINT)SendMessageW(GetDlgItem(dlg, id), BM_GETCHECK, 0, 0);
}
BOOL CheckRadioButton(HWND dlg, int first, int last, int check)
{
	for (int i = first; i <= last; ++i)
		CheckDlgButton(dlg, i, i == check ? BST_CHECKED : BST_UNCHECKED);
	return TRUE;
}

BOOL EnumDisplayDevicesW(LPCWSTR, DWORD, PDISPLAY_DEVICEW dd, DWORD)
{
	if (!dd) return FALSE;
	wcscpy(dd->DeviceName, L"DISPLAY");
	wcscpy(dd->DeviceString, L"GTK Display");
	dd->StateFlags = 1;
	return TRUE;
}
BOOL EnumDisplaySettingsW(LPCWSTR, DWORD, LPDEVMODEW dm)
{
	if (!dm) return FALSE;
	dm->dmPelsWidth = GetSystemMetrics(SM_CXSCREEN);
	dm->dmPelsHeight = GetSystemMetrics(SM_CYSCREEN);
	dm->dmBitsPerPel = 32;
	dm->dmFields = 0;
	return TRUE;
}

int GetWindowTextA(HWND h, LPSTR buf, int max)
{
	wchar_t wbuf[4096]{};
	GetWindowTextW(h, wbuf, 4096);
	std::string u = nppWideToUtf8(wbuf);
	if (!buf || max <= 0) return (int)u.size();
	std::snprintf(buf, (size_t)max, "%s", u.c_str());
	return (int)std::strlen(buf);
}
BOOL SetWindowTextA(HWND h, LPCSTR text)
{
	return SetWindowTextW(h, nppUtf8ToWide(text ? text : "").c_str());
}

BOOL IsHungAppWindow(HWND) { return FALSE; }
HWND GetLastActivePopup(HWND h) { return h; }
BOOL AnyPopup(void) { return FALSE; }
void SwitchToThisWindow(HWND h, BOOL) { SetForegroundWindow(h); }
HDWP BeginDeferWindowPos(int) { return (HDWP)1; }
HDWP DeferWindowPos(HDWP hdwp, HWND h, HWND after, int x, int y, int cx, int cy, UINT flags)
{
	SetWindowPos(h, after, x, y, cx, cy, flags);
	return hdwp;
}
BOOL EndDeferWindowPos(HDWP) { return TRUE; }
BOOL GetGUIThreadInfo(DWORD, PGUITHREADINFO info)
{
	if (info) { info->hwndFocus = g_focus; info->hwndActive = g_active; }
	return TRUE;
}
int GetKeyboardType(int typeFlag) { return typeFlag == 0 ? 4 : 0; }
BOOL GetKeyboardLayoutNameW(LPWSTR pwszKLID) { if (pwszKLID) wcscpy(pwszKLID, L"00000409"); return TRUE; }
HKL GetKeyboardLayout(DWORD) { return (HKL)(uintptr_t)0x0409; }
int GetKeyboardLayoutList(int n, HKL* layouts) { if (n > 0 && layouts) layouts[0] = GetKeyboardLayout(0); return 1; }
UINT GetKBCodePage(void) { return 1252; }
int ToUnicode(UINT vk, UINT, const BYTE*, LPWSTR buf, int cch, UINT)
{
	if (!buf || cch <= 0) return 0;
	if (vk >= 32 && vk < 127) { buf[0] = (WCHAR)vk; if (cch > 1) buf[1] = 0; return 1; }
	return 0;
}
int ToUnicodeEx(UINT vk, UINT s, const BYTE* k, LPWSTR b, int c, UINT f, HKL) { return ToUnicode(vk, s, k, b, c, f); }

BOOL SetLayeredWindowAttributes(HWND, COLORREF, BYTE, DWORD) { return TRUE; }
BOOL GetLayeredWindowAttributes(HWND, COLORREF*, BYTE*, DWORD*) { return TRUE; }
BOOL AnimateWindow(HWND, DWORD, DWORD) { return TRUE; }
BOOL DrawAnimatedRects(HWND, int, const RECT*, const RECT*) { return TRUE; }
BOOL DragDetect(HWND, POINT) { return FALSE; }
int GetWindowRgnBox(HWND h, LPRECT rc) { return GetWindowRect(h, rc) ? SIMPLEREGION : ERROR; }
BOOL GetWindowInfo(HWND h, PWINDOWINFO pwi)
{
	if (!pwi) return FALSE;
	GetWindowRect(h, &pwi->rcWindow);
	GetClientRect(h, &pwi->rcClient);
	pwi->dwStyle = (DWORD)GetWindowLongPtrW(h, GWL_STYLE);
	pwi->dwExStyle = (DWORD)GetWindowLongPtrW(h, GWL_EXSTYLE);
	return TRUE;
}
BOOL GetTitleBarInfo(HWND, PTITLEBARINFO) { return TRUE; }
BOOL GetComboBoxInfo(HWND h, PCOMBOBOXINFO pcbi)
{
	if (!pcbi) return FALSE;
	pcbi->hwndCombo = h;
	return TRUE;
}

static WNDPROC stockProc = DefWindowProcW;

void nppRegisterStockClasses()
{
	static const wchar_t* names[] = {
		L"BUTTON", L"EDIT", L"STATIC", L"LISTBOX", L"COMBOBOX", L"SCROLLBAR",
		L"SysTabControl32", L"ToolbarWindow32", L"msctls_statusbar32", L"ReBarWindow32",
		L"SysTreeView32", L"SysListView32", L"tooltips_class32", L"msctls_progress32",
		L"msctls_trackbar32", L"msctls_updown32", L"SysHeader32", L"#32770", L"Scintilla"
	};
	for (auto* n : names)
	{
		WNDCLASSEXW wc{};
		wc.cbSize = sizeof(wc);
		wc.lpfnWndProc = stockProc;
		wc.lpszClassName = n;
		nppRegisterClass(n, wc);
	}
}

void nppInitPlatform()
{
	gtk_init(nullptr, nullptr);
	nppInitGdi();
	nppInitResources();
	nppRegisterStockClasses();
}

DWORD GetTickCount(void)
{
	struct timespec ts{};
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (DWORD)(ts.tv_sec * 1000u + ts.tv_nsec / 1000000u);
}
ULONGLONG GetTickCount64(void)
{
	struct timespec ts{};
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (ULONGLONG)ts.tv_sec * 1000ull + ts.tv_nsec / 1000000ull;
}
void Sleep(DWORD ms)
{
	g_usleep(ms * 1000);
}
DWORD SleepEx(DWORD ms, BOOL) { Sleep(ms); return 0; }

int MulDiv(int n, int num, int den)
{
	if (den == 0) return 0;
	return (int)((long long)n * num / den);
}

struct HookImpl {
	int id = 0;
	HOOKPROC proc = nullptr;
	HINSTANCE inst = nullptr;
	DWORD tid = 0;
};
static std::vector<HookImpl*> g_hooks;

HHOOK SetWindowsHookExW(int idHook, HOOKPROC proc, HINSTANCE inst, DWORD tid)
{
	auto* h = new HookImpl{idHook, proc, inst, tid};
	g_hooks.push_back(h);
	return (HHOOK)h;
}
HHOOK SetWindowsHookExA(int idHook, HOOKPROC proc, HINSTANCE inst, DWORD tid)
{
	return SetWindowsHookExW(idHook, proc, inst, tid);
}
BOOL UnhookWindowsHookEx(HHOOK hook)
{
	auto* h = (HookImpl*)hook;
	if (!h) return FALSE;
	g_hooks.erase(std::remove(g_hooks.begin(), g_hooks.end(), h), g_hooks.end());
	delete h;
	return TRUE;
}
LRESULT CallNextHookEx(HHOOK, int, WPARAM, LPARAM)
{
	return 0;
}
