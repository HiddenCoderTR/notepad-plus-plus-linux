#include "npp_platform.h"

#include <vector>
#include <string>

struct ImageListImpl {
	int cx=16, cy=16;
	std::vector<HICON> icons;
	COLORREF bk = CLR_NONE;
};

HIMAGELIST ImageList_Create(int cx, int cy, UINT, int, int)
{
	auto* il = new ImageListImpl(); il->cx=cx; il->cy=cy; return (HIMAGELIST)il;
}
BOOL ImageList_Destroy(HIMAGELIST himl) { delete (ImageListImpl*)himl; return TRUE; }
int ImageList_Add(HIMAGELIST himl, HBITMAP, HBITMAP)
{
	auto* il = (ImageListImpl*)himl; if(!il) return -1;
	il->icons.push_back((HICON)(uintptr_t)il->icons.size());
	return (int)il->icons.size()-1;
}
int ImageList_AddMasked(HIMAGELIST himl, HBITMAP b, COLORREF) { return ImageList_Add(himl,b,nullptr); }
int ImageList_AddIcon(HIMAGELIST himl, HICON icon)
{
	auto* il = (ImageListImpl*)himl; if(!il) return -1;
	il->icons.push_back(icon); return (int)il->icons.size()-1;
}
int ImageList_ReplaceIcon(HIMAGELIST himl, int i, HICON icon)
{
	auto* il = (ImageListImpl*)himl; if(!il) return -1;
	if (i<0) return ImageList_AddIcon(himl, icon);
	if (i<(int)il->icons.size()) il->icons[i]=icon;
	return i;
}
BOOL ImageList_Remove(HIMAGELIST himl, int i)
{
	auto* il = (ImageListImpl*)himl; if(!il) return FALSE;
	if (i<0) il->icons.clear();
	else if (i<(int)il->icons.size()) il->icons.erase(il->icons.begin()+i);
	return TRUE;
}
BOOL ImageList_Draw(HIMAGELIST, int, HDC hdc, int x, int y, UINT)
{
	RECT rc{x,y,x+16,y+16}; FrameRect(hdc,&rc,GetStockObject(BLACK_BRUSH)); return TRUE;
}
BOOL ImageList_DrawEx(HIMAGELIST il, int i, HDC hdc, int x, int y, int, int, COLORREF, COLORREF, UINT s)
{ return ImageList_Draw(il,i,hdc,x,y,s); }
HICON ImageList_GetIcon(HIMAGELIST himl, int i, UINT)
{
	auto* il = (ImageListImpl*)himl;
	if (!il || i<0 || i>=(int)il->icons.size()) return nullptr;
	return il->icons[i];
}
int ImageList_GetImageCount(HIMAGELIST himl) { auto* il=(ImageListImpl*)himl; return il?(int)il->icons.size():0; }
BOOL ImageList_GetIconSize(HIMAGELIST himl, int* cx, int* cy)
{
	auto* il=(ImageListImpl*)himl; if(!il) return FALSE;
	if(cx)*cx=il->cx; if(cy)*cy=il->cy; return TRUE;
}
BOOL ImageList_SetIconSize(HIMAGELIST himl, int cx, int cy)
{
	auto* il=(ImageListImpl*)himl; if(!il) return FALSE; il->cx=cx; il->cy=cy; return TRUE;
}
COLORREF ImageList_SetBkColor(HIMAGELIST himl, COLORREF bk)
{
	auto* il=(ImageListImpl*)himl; COLORREF o=il?il->bk:0; if(il) il->bk=bk; return o;
}
COLORREF ImageList_GetBkColor(HIMAGELIST himl) { auto* il=(ImageListImpl*)himl; return il?il->bk:0; }
HIMAGELIST ImageList_Duplicate(HIMAGELIST himl)
{
	auto* il=(ImageListImpl*)himl; if(!il) return nullptr;
	auto* n=new ImageListImpl(*il); return (HIMAGELIST)n;
}
HIMAGELIST ImageList_LoadImageW(HINSTANCE, LPCWSTR, int cx, int, COLORREF, UINT, UINT)
{
	return ImageList_Create(cx?cx:16, cx?cx:16, 0, 1, 1);
}

void InitCommonControls(void) {}
BOOL InitCommonControlsEx(const INITCOMMONCONTROLSEX*) { return TRUE; }
HWND CreateStatusWindowW(LONG style, LPCWSTR text, HWND parent, UINT id)
{
	return CreateWindowExW(0, L"msctls_statusbar32", text, style|WS_CHILD, 0,0,100,20, parent, (HMENU)(uintptr_t)id, nullptr, nullptr);
}
HWND CreateToolbarEx(HWND parent, DWORD style, UINT id, int, HINSTANCE, UINT_PTR, LPCTBBUTTON btns, int nButtons, int, int, int, int, UINT)
{
	HWND h = CreateWindowExW(0, L"ToolbarWindow32", L"", style|WS_CHILD, 0,0,100,28, parent, (HMENU)(uintptr_t)id, nullptr, nullptr);
	if (btns && nButtons)
		SendMessageW(h, TB_ADDBUTTONS, nButtons, (LPARAM)btns);
	return h;
}

struct ListData {
	std::vector<std::wstring> items;
	int sel = -1;
	int cols = 1;
};
struct TabData { std::vector<std::wstring> tabs; int sel=0; };
struct TreeData { std::vector<std::wstring> nodes; };
struct ComboData { std::vector<std::wstring> items; int sel=-1; };
struct StatusData { std::vector<std::wstring> parts; };
struct ToolbarData { std::vector<TBBUTTON> buttons; HIMAGELIST himl=nullptr; };

static ListData* listOf(WindowImpl* w)
{
	if (!w->controlData) w->controlData = new ListData();
	return (ListData*)w->controlData;
}

LRESULT nppControlSend(WindowImpl* w, UINT msg, WPARAM wp, LPARAM lp)
{
	if (!w) return 0;
	switch (w->kind)
	{
	case WindowKind::Edit:
		if (msg == WM_GETTEXT) return GetWindowTextW(hwndFrom(w), (LPWSTR)lp, (int)wp);
		if (msg == WM_SETTEXT) return SetWindowTextW(hwndFrom(w), (LPCWSTR)lp);
		if (msg == EM_SETSEL || msg == EM_REPLACESEL) return 0;
		if (msg == EM_GETSEL) return 0;
		break;
	case WindowKind::Button:
		if (msg == BM_SETCHECK && w->native && GTK_IS_TOGGLE_BUTTON(w->native))
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w->native), wp != BST_UNCHECKED);
		if (msg == BM_GETCHECK && w->native && GTK_IS_TOGGLE_BUTTON(w->native))
			return gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w->native)) ? BST_CHECKED : BST_UNCHECKED;
		break;
	case WindowKind::ListBox:
	case WindowKind::ListView:
	{
		auto* ld = listOf(w);
		if (msg == LB_ADDSTRING || msg == CB_ADDSTRING || msg == LVM_INSERTITEMW)
		{
			LPCWSTR s = (LPCWSTR)lp;
			if (msg == LVM_INSERTITEMW && lp) s = ((LVITEMW*)lp)->pszText;
			ld->items.push_back(s?s:L"");
			return (LRESULT)ld->items.size()-1;
		}
		if (msg == LB_GETCOUNT || msg == LVM_GETITEMCOUNT) return (LRESULT)ld->items.size();
		if (msg == LB_GETCURSEL || msg == LVM_GETNEXTITEM) return ld->sel;
		if (msg == LB_SETCURSEL) { ld->sel = (int)wp; return 0; }
		if (msg == LB_GETTEXT && wp < ld->items.size())
		{
			wcscpy((LPWSTR)lp, ld->items[wp].c_str());
			return (LRESULT)ld->items[wp].size();
		}
		if (msg == LB_RESETCONTENT || msg == LVM_DELETEALLITEMS) { ld->items.clear(); ld->sel=-1; return TRUE; }
		if (msg == LVM_SETITEMW) return TRUE;
		if (msg == LVM_INSERTCOLUMNW) { ld->cols++; return ld->cols-1; }
		if (msg == LVM_SETIMAGELIST) return 0;
		if (msg == LVM_GETITEMW && lp)
		{
			auto* it = (LVITEMW*)lp;
			if (it->iItem >= 0 && it->iItem < (int)ld->items.size() && it->pszText)
				wcsncpy(it->pszText, ld->items[it->iItem].c_str(), it->cchTextMax);
			return TRUE;
		}
		if (msg == LVM_GETSELECTIONMARK) return ld->sel;
		if (msg == LVM_SETSELECTIONMARK) { ld->sel = (int)lp; return 0; }
		break;
	}
	case WindowKind::Combo:
	{
		auto* ld = listOf(w);
		if (msg == CB_ADDSTRING) { ld->items.push_back(lp?(LPCWSTR)lp:L""); return ld->items.size()-1; }
		if (msg == CB_GETCOUNT) return ld->items.size();
		if (msg == CB_GETCURSEL) return ld->sel;
		if (msg == CB_SETCURSEL) { ld->sel=(int)wp; return wp; }
		if (msg == CB_RESETCONTENT) { ld->items.clear(); ld->sel=-1; return TRUE; }
		if (msg == CB_GETLBTEXT && wp < ld->items.size())
		{ wcscpy((LPWSTR)lp, ld->items[wp].c_str()); return ld->items[wp].size(); }
		break;
	}
	case WindowKind::Tab:
	{
		if (!w->controlData) w->controlData = new TabData();
		auto* td = (TabData*)w->controlData;
		if (msg == TCM_INSERTITEMW && lp)
		{
			auto* it = (TCITEMW*)lp;
			td->tabs.insert(td->tabs.begin()+std::min((int)wp,(int)td->tabs.size()), it->pszText?it->pszText:L"");
			return wp;
		}
		if (msg == TCM_GETITEMCOUNT) return td->tabs.size();
		if (msg == TCM_GETCURSEL) return td->sel;
		if (msg == TCM_SETCURSEL) { int o=td->sel; td->sel=(int)wp; return o; }
		if (msg == TCM_DELETEALLITEMS) { td->tabs.clear(); return TRUE; }
		if (msg == TCM_GETITEMW && lp && wp < td->tabs.size())
		{
			auto* it=(TCITEMW*)lp;
			if (it->pszText) wcsncpy(it->pszText, td->tabs[wp].c_str(), it->cchTextMax);
			return TRUE;
		}
		if (msg == TCM_ADJUSTRECT && lp)
		{
			RECT* rc=(RECT*)lp;
			if (wp) InflateRect(rc, 2, 20);
			else InflateRect(rc, -2, -20);
			return 0;
		}
		break;
	}
	case WindowKind::Status:
		if (msg == SB_SETTEXTW)
		{
			SetWindowTextW(hwndFrom(w), (LPCWSTR)lp);
			return TRUE;
		}
		if (msg == SB_SETPARTS) return TRUE;
		break;
	case WindowKind::Toolbar:
		if (!w->controlData) w->controlData = new ToolbarData();
		{
			auto* tb = (ToolbarData*)w->controlData;
			if (msg == TB_BUTTONSTRUCTSIZE) return 0;
			if (msg == TB_ADDBUTTONS && lp)
			{
				auto* b = (TBBUTTON*)lp;
				for (WPARAM i=0;i<wp;++i) tb->buttons.push_back(b[i]);
				return TRUE;
			}
			if (msg == TB_BUTTONCOUNT) return tb->buttons.size();
			if (msg == TB_GETBUTTON && lp && wp < tb->buttons.size())
			{ *(TBBUTTON*)lp = tb->buttons[wp]; return TRUE; }
			if (msg == TB_ENABLEBUTTON) return TRUE;
			if (msg == TB_SETIMAGELIST) { tb->himl=(HIMAGELIST)lp; return 0; }
			if (msg == TB_GETIMAGELIST) return (LRESULT)tb->himl;
			if (msg == TB_AUTOSIZE) { gtk_widget_queue_resize(w->widget); return 0; }
			if (msg == TB_GETITEMRECT && lp)
			{
				RECT* rc=(RECT*)lp;
				rc->left = (int)wp * 24; rc->top=0; rc->right=rc->left+24; rc->bottom=24;
				return TRUE;
			}
		}
		break;
	case WindowKind::Tree:
		if (msg == TVM_INSERTITEMW) return (LRESULT)(HTREEITEM)(uintptr_t)1;
		if (msg == TVM_DELETEITEM) return TRUE;
		if (msg == TVM_GETCOUNT) return 0;
		if (msg == TVM_SELECTITEM) return TRUE;
		if (msg == TVM_GETNEXTITEM) return 0;
		if (msg == TVM_SETIMAGELIST) return 0;
		if (msg == TVM_EXPAND) return TRUE;
		break;
	case WindowKind::Progress:
		if (msg == PBM_SETRANGE32 && w->native && GTK_IS_PROGRESS_BAR(w->native))
			gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(w->native), 0);
		if (msg == PBM_SETPOS && w->native && GTK_IS_PROGRESS_BAR(w->native))
			gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(w->native), std::min(1.0, wp/100.0));
		return TRUE;
	default:
		break;
	}
	if (msg == WM_SETFONT) return 0;
	if (msg == WM_GETFONT) return 0;
	if (msg == WM_ENABLE) { EnableWindow(hwndFrom(w), (BOOL)wp); return 0; }
	return 0;
}

// ---- menus ----
struct MenuItem {
	UINT flags = 0;
	UINT_PTR id = 0;
	std::wstring text;
	HMENU popup = nullptr;
	bool checked = false;
	bool enabled = true;
};
struct MenuImpl {
	std::vector<MenuItem> items;
	bool popup = false;
};

static MenuImpl* menuOf(HMENU h) { return (MenuImpl*)h; }

HMENU CreateMenu(void) { return (HMENU)new MenuImpl(); }
HMENU CreatePopupMenu(void) { auto* m=new MenuImpl(); m->popup=true; return (HMENU)m; }
BOOL DestroyMenu(HMENU menu) { delete menuOf(menu); return TRUE; }
BOOL AppendMenuW(HMENU menu, UINT flags, UINT_PTR id, LPCWSTR item)
{
	auto* m = menuOf(menu); if(!m) return FALSE;
	MenuItem it; it.flags=flags; it.id=id; it.text=item?item:L"";
	if (flags & MF_POPUP) it.popup = (HMENU)id;
	m->items.push_back(it);
	return TRUE;
}
BOOL InsertMenuW(HMENU menu, UINT pos, UINT flags, UINT_PTR id, LPCWSTR item)
{
	auto* m = menuOf(menu); if(!m) return FALSE;
	MenuItem it; it.flags=flags; it.id=id; it.text=item?item:L"";
	size_t i = (flags & MF_BYPOSITION) ? pos : m->items.size();
	if (i > m->items.size()) i = m->items.size();
	m->items.insert(m->items.begin()+i, it);
	return TRUE;
}
BOOL InsertMenuItemW(HMENU menu, UINT item, BOOL byPos, LPCMENUITEMINFOW mii)
{
	if (!mii) return FALSE;
	return InsertMenuW(menu, item, byPos?MF_BYPOSITION:MF_BYCOMMAND, mii->wID, mii->dwTypeData);
}
BOOL DeleteMenu(HMENU menu, UINT pos, UINT flags)
{
	auto* m = menuOf(menu); if(!m) return FALSE;
	if (flags & MF_BYPOSITION)
	{
		if (pos < m->items.size()) m->items.erase(m->items.begin()+pos);
	}
	else
		m->items.erase(std::remove_if(m->items.begin(), m->items.end(), [&](auto& it){ return it.id==pos; }), m->items.end());
	return TRUE;
}
BOOL RemoveMenu(HMENU m, UINT p, UINT f) { return DeleteMenu(m,p,f); }
DWORD CheckMenuItem(HMENU menu, UINT id, UINT check)
{
	auto* m = menuOf(menu); if(!m) return (DWORD)-1;
	for (auto& it : m->items) if (it.id==id) { DWORD o=it.checked?MF_CHECKED:MF_UNCHECKED; it.checked = (check&MF_CHECKED)!=0; return o; }
	return (DWORD)-1;
}
BOOL EnableMenuItem(HMENU menu, UINT id, UINT enable)
{
	auto* m = menuOf(menu); if(!m) return FALSE;
	for (auto& it : m->items) if (it.id==id) { it.enabled = (enable&MF_GRAYED)==0; return TRUE; }
	return TRUE;
}
HMENU GetMenu(HWND h) { auto* w=implFrom(h); return w?w->menu:nullptr; }
BOOL SetMenu(HWND h, HMENU menu)
{
	auto* w=implFrom(h); if(!w) return FALSE; w->menu=menu;
	if (w->widget && GTK_IS_WINDOW(w->widget) && menu)
	{
		// GTK menubar is constructed lazily; N++ owner-draws via WM_INITMENUPOPUP
	}
	return TRUE;
}
BOOL DrawMenuBar(HWND h) { auto* w=implFrom(h); if(w&&w->widget) gtk_widget_queue_draw(w->widget); return TRUE; }
int GetMenuItemCount(HMENU menu) { auto* m=menuOf(menu); return m?(int)m->items.size():0; }
UINT GetMenuItemID(HMENU menu, int pos)
{
	auto* m=menuOf(menu); if(!m||pos<0||pos>=(int)m->items.size()) return (UINT)-1;
	return (UINT)m->items[pos].id;
}
int GetMenuStringW(HMENU menu, UINT id, LPWSTR buf, int max, UINT flags)
{
	auto* m=menuOf(menu); if(!m) return 0;
	for (size_t i=0;i<m->items.size();++i)
	{
		bool match = (flags&MF_BYPOSITION) ? (i==id) : (m->items[i].id==id);
		if (match)
		{
			if (buf&&max) { wcsncpy(buf, m->items[i].text.c_str(), max-1); buf[max-1]=0; }
			return (int)m->items[i].text.size();
		}
	}
	return 0;
}
UINT GetMenuState(HMENU menu, UINT id, UINT flags)
{
	auto* m=menuOf(menu); if(!m) return (UINT)-1;
	for (size_t i=0;i<m->items.size();++i)
	{
		bool match = (flags&MF_BYPOSITION) ? (i==id) : (m->items[i].id==id);
		if (match)
		{
			UINT s=0;
			if (m->items[i].checked) s|=MF_CHECKED;
			if (!m->items[i].enabled) s|=MF_GRAYED;
			if (m->items[i].popup) s|=MF_POPUP;
			return s;
		}
	}
	return (UINT)-1;
}
BOOL GetMenuItemInfoW(HMENU menu, UINT item, BOOL byPos, LPMENUITEMINFOW mii)
{
	auto* m=menuOf(menu); if(!m||!mii) return FALSE;
	int idx=-1;
	if (byPos) idx=(int)item;
	else { for(size_t i=0;i<m->items.size();++i) if(m->items[i].id==item){idx=(int)i;break;} }
	if (idx<0||idx>=(int)m->items.size()) return FALSE;
	mii->wID = (UINT)m->items[idx].id;
	mii->fState = GetMenuState(menu, item, byPos?MF_BYPOSITION:0);
	if (mii->dwTypeData && mii->cch)
		wcsncpy(mii->dwTypeData, m->items[idx].text.c_str(), mii->cch);
	return TRUE;
}
BOOL SetMenuItemInfoW(HMENU menu, UINT item, BOOL byPos, LPCMENUITEMINFOW mii)
{
	if (!mii) return FALSE;
	if (mii->fMask & MIIM_STRING) ModifyMenuW(menu, item, byPos?MF_BYPOSITION:0, mii->wID, mii->dwTypeData);
	return TRUE;
}
BOOL ModifyMenuW(HMENU menu, UINT pos, UINT flags, UINT_PTR id, LPCWSTR item)
{
	DeleteMenu(menu, pos, flags);
	return InsertMenuW(menu, pos, flags, id, item);
}
BOOL CheckMenuRadioItem(HMENU menu, UINT first, UINT last, UINT check, UINT)
{
	for (UINT i=first;i<=last;++i) CheckMenuItem(menu, i, i==check?MF_CHECKED:MF_UNCHECKED);
	return TRUE;
}
HMENU GetSubMenu(HMENU menu, int pos)
{
	auto* m=menuOf(menu); if(!m||pos<0||pos>=(int)m->items.size()) return nullptr;
	return m->items[pos].popup;
}

static GtkWidget* buildGtkMenu(HMENU menu, HWND owner);

static void onMenuActivate(GtkMenuItem*, gpointer data)
{
	auto* pack = (uintptr_t*)data;
	HWND owner = (HWND)pack[0];
	UINT id = (UINT)pack[1];
	SendMessageW(owner, WM_COMMAND, id, 0);
}

static GtkWidget* buildGtkMenu(HMENU menu, HWND owner)
{
	auto* m = menuOf(menu);
	GtkWidget* gmenu = gtk_menu_new();
	if (!m) return gmenu;
	for (auto& it : m->items)
	{
		if (it.flags & MF_SEPARATOR)
		{
			gtk_menu_shell_append(GTK_MENU_SHELL(gmenu), gtk_separator_menu_item_new());
			continue;
		}
		GtkWidget* mi = gtk_menu_item_new_with_label(nppWideToUtf8(it.text).c_str());
		if (it.popup)
			gtk_menu_item_set_submenu(GTK_MENU_ITEM(mi), buildGtkMenu(it.popup, owner));
		else
		{
			auto* pack = new uintptr_t[2]; pack[0]=(uintptr_t)owner; pack[1]=it.id;
			g_signal_connect(mi, "activate", G_CALLBACK(onMenuActivate), pack);
		}
		gtk_widget_set_sensitive(mi, it.enabled);
		gtk_menu_shell_append(GTK_MENU_SHELL(gmenu), mi);
	}
	gtk_widget_show_all(gmenu);
	return gmenu;
}

BOOL TrackPopupMenu(HMENU menu, UINT, int x, int y, int, HWND owner, const RECT*)
{
	GtkWidget* gmenu = buildGtkMenu(menu, owner);
	gtk_menu_popup_at_pointer(GTK_MENU(gmenu), nullptr);
	return TRUE;
}
BOOL TrackPopupMenuEx(HMENU m, UINT f, int x, int y, HWND o, LPTPMPARAMS) { return TrackPopupMenu(m,f,x,y,0,o,nullptr); }

struct AccelImpl { std::vector<ACCEL> accels; };
HACCEL CreateAcceleratorTableW(LPACCEL accels, int count)
{
	auto* a = new AccelImpl();
	if (accels) a->accels.assign(accels, accels+count);
	return (HACCEL)a;
}
BOOL DestroyAcceleratorTable(HACCEL acc) { delete (AccelImpl*)acc; return TRUE; }
int CopyAcceleratorTableW(HACCEL acc, LPACCEL out, int count)
{
	auto* a=(AccelImpl*)acc; if(!a) return 0;
	if (!out) return (int)a->accels.size();
	int n=std::min(count,(int)a->accels.size());
	for(int i=0;i<n;++i) out[i]=a->accels[i];
	return n;
}
HACCEL LoadAcceleratorsW(HINSTANCE, LPCWSTR name) { return nppLoadAccel(name); }
int TranslateAcceleratorW(HWND h, HACCEL acc, LPMSG msg)
{
	auto* a=(AccelImpl*)acc;
	if (!a || !msg || msg->message != WM_KEYDOWN) return 0;
	for (auto& ac : a->accels)
	{
		if (ac.key == msg->wParam)
		{
			SendMessageW(h, WM_COMMAND, ac.cmd, 0);
			return 1;
		}
	}
	return 0;
}

HMENU LoadMenuW(HINSTANCE, LPCWSTR name) { return nppLoadMenu(name); }
