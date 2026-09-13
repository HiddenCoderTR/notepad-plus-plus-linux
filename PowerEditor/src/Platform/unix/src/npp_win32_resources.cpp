#include "npp_platform.h"

#include <map>
#include <vector>
#include <cstring>
#include <memory>

#pragma pack(push, 1)
struct UnixDlgTemplateEx {
	WORD dlgVer;
	WORD signature;
	DWORD helpID;
	DWORD exStyle;
	DWORD style;
	WORD cDlgItems;
	short x;
	short y;
	short cx;
	short cy;
};
#pragma pack(pop)

struct RcBlob {
	LPCWSTR type;
	LONG_PTR name;
	const unsigned char* data;
	DWORD size;
};

extern "C" {
extern const RcBlob npp_rc_blobs[];
extern const int npp_rc_blob_count;
}

struct LoadedRes { const unsigned char* p; DWORD size; };
static std::vector<std::unique_ptr<std::vector<unsigned char>>> g_ownedRes;

static bool nameMatch(LONG_PTR stored, LPCWSTR want)
{
	if (IS_INTRESOURCE(want))
		return stored == (LONG_PTR)want;
	if (stored < 0x10000) return false;
	return wcscmp((LPCWSTR)stored, want)==0;
}

HRSRC FindResourceW(HMODULE, LPCWSTR name, LPCWSTR type)
{
	return nppFindResource(name, type);
}
HRSRC FindResourceExW(HMODULE, LPCWSTR type, LPCWSTR name, WORD) { return FindResourceW(nullptr, name, type); }

HRSRC nppFindResource(LPCWSTR name, LPCWSTR type)
{
	for (int i=0;i<npp_rc_blob_count;++i)
	{
		bool tmatch = false;
		if (IS_INTRESOURCE(type) && IS_INTRESOURCE(npp_rc_blobs[i].type))
			tmatch = (LONG_PTR)type == (LONG_PTR)npp_rc_blobs[i].type;
		else if (!IS_INTRESOURCE(type) && !IS_INTRESOURCE(npp_rc_blobs[i].type))
			tmatch = wcscmp(type, npp_rc_blobs[i].type)==0;
		else if (IS_INTRESOURCE(type))
			tmatch = nameMatch((LONG_PTR)npp_rc_blobs[i].type, type);
		if (!tmatch) continue;
		if (nameMatch(npp_rc_blobs[i].name, name))
			return (HRSRC)(uintptr_t)(i+1);
	}
	return nullptr;
}

HGLOBAL LoadResource(HMODULE, HRSRC res) { return (HGLOBAL)res; }
LPVOID LockResource(HGLOBAL res)
{
	intptr_t i = (intptr_t)res - 1;
	if (i<0 || i>=npp_rc_blob_count) return nullptr;
	return (LPVOID)npp_rc_blobs[i].data;
}
DWORD SizeofResource(HMODULE, HRSRC res)
{
	intptr_t i = (intptr_t)res - 1;
	if (i<0 || i>=npp_rc_blob_count) return 0;
	return npp_rc_blobs[i].size;
}
DWORD nppSizeofResource(HRSRC res) { return SizeofResource(nullptr, res); }
HGLOBAL nppLoadResource(HRSRC res) { return LoadResource(nullptr, res); }
BOOL FreeResource(HGLOBAL) { return TRUE; }

int LoadStringW(HINSTANCE, UINT id, LPWSTR buf, int max)
{
	return nppLoadString(id, buf, max);
}
int nppLoadString(UINT id, LPWSTR buf, int max)
{
	HRSRC r = FindResourceW(nullptr, MAKEINTRESOURCE((id/16)+1), RT_STRING);
	if (!r || !buf || max<=0) { if(buf&&max) buf[0]=0; return 0; }
	// packed string table not fully decoded yet
	buf[0]=0;
	return 0;
}

HANDLE nppLoadImage(LPCWSTR name, UINT type, int cx, int cy)
{
	(void)name; (void)type; (void)cx; (void)cy;
	return nullptr;
}
HACCEL nppLoadAccel(LPCWSTR) { return CreateAcceleratorTableW(nullptr, 0); }
HMENU nppLoadMenu(LPCWSTR name)
{
	HRSRC r = FindResourceW(nullptr, name, RT_MENU);
	if (!r) return CreateMenu();
	return CreateMenu();
}

static void appendW(std::vector<unsigned char>& b, const wchar_t* s)
{
	if (!s) s = L"";
	const unsigned char* p = reinterpret_cast<const unsigned char*>(s);
	size_t n = (wcslen(s)+1)*sizeof(wchar_t);
	b.insert(b.end(), p, p+n);
}
static void appendWord(std::vector<unsigned char>& b, WORD v)
{
	b.insert(b.end(), (unsigned char*)&v, (unsigned char*)&v + sizeof(WORD));
}
static void padDword(std::vector<unsigned char>& b)
{
	while (b.size() % sizeof(DWORD)) b.push_back(0);
}

HWND nppCreateDialogIndirect(LPCDLGTEMPLATE tmpl, HWND parent, DLGPROC proc, LPARAM lp, bool modal, INT_PTR* result)
{
	if (!tmpl) return nullptr;
	const auto* ex = reinterpret_cast<const UnixDlgTemplateEx*>(tmpl);
	short x,y,cx,cy;
	DWORD style, exStyle;
	WORD nItems;
	const unsigned char* p = reinterpret_cast<const unsigned char*>(tmpl);
	const bool isEx = (ex->signature == 0xFFFF && ex->dlgVer == 1);
	if (isEx)
	{
		style = ex->style; exStyle = ex->exStyle; nItems = ex->cDlgItems;
		x=ex->x; y=ex->y; cx=ex->cx; cy=ex->cy;
		p += sizeof(UnixDlgTemplateEx);
	}
	else
	{
		const auto* t = reinterpret_cast<const DLGTEMPLATE*>(tmpl);
		style = t->style; exStyle = t->dwExtendedStyle; nItems = t->cdit;
		x=t->x; y=t->y; cx=t->cx; cy=t->cy;
		p += sizeof(DLGTEMPLATE);
	}
	auto skipSzOrOrd = [](const unsigned char*& q) {
		const WORD* w = reinterpret_cast<const WORD*>(q);
		if (*w == 0xFFFF) { q += 2*sizeof(WORD); return; }
		const wchar_t* s = reinterpret_cast<const wchar_t*>(q);
		q += (wcslen(s)+1)*sizeof(wchar_t);
	};
	skipSzOrOrd(p); // menu
	skipSzOrOrd(p); // class
	const wchar_t* title = reinterpret_cast<const wchar_t*>(p);
	p += (wcslen(title)+1)*sizeof(wchar_t);
	if (style & DS_SETFONT)
	{
		p += sizeof(WORD); // pointsize
		if (isEx) { p += sizeof(WORD)+2; } // weight, italic, charset
		const wchar_t* face = reinterpret_cast<const wchar_t*>(p);
		p += (wcslen(face)+1)*sizeof(wchar_t);
	}

	int px = MulDiv(cx, 6, 4) * 2; // rough DLUs
	int py = MulDiv(cy, 13, 8) * 2;
	HWND h = CreateWindowExW(exStyle | WS_EX_DLGMODALFRAME, L"#32770", title,
		style | WS_POPUP | WS_CAPTION, x, y, std::max(px,100), std::max(py,80), parent, nullptr, nullptr, nullptr);
	auto* wi = implFrom(h);
	if (wi) { wi->kind = WindowKind::Dialog; wi->wndproc = (WNDPROC)proc; }

	for (WORD i=0;i<nItems;++i)
	{
		while (((uintptr_t)p) % sizeof(DWORD)) ++p;
		DWORD help=0, ies=0, istyle=0; short ix,iy,icx,icy; DWORD id=0;
		if (isEx)
		{
			const auto* it = reinterpret_cast<const DLGITEMTEMPLATEEX*>(p);
			ies=it->exStyle; istyle=it->style; ix=it->x; iy=it->y; icx=it->cx; icy=it->cy; id=it->id;
			p += sizeof(DLGITEMTEMPLATEEX);
		}
		else
		{
			const auto* it = reinterpret_cast<const DLGITEMTEMPLATE*>(p);
			istyle=it->style; ies=it->dwExtendedStyle; ix=it->x; iy=it->y; icx=it->cx; icy=it->cy; id=it->id;
			p += sizeof(DLGITEMTEMPLATE);
		}
		std::wstring cls, cap;
		const WORD* cw = reinterpret_cast<const WORD*>(p);
		if (*cw == 0xFFFF)
		{
			WORD ord = *(cw+1);
			p += 2*sizeof(WORD);
			switch (ord)
			{
			case 0x80: cls=L"BUTTON"; break;
			case 0x81: cls=L"EDIT"; break;
			case 0x82: cls=L"STATIC"; break;
			case 0x83: cls=L"LISTBOX"; break;
			case 0x84: cls=L"SCROLLBAR"; break;
			case 0x85: cls=L"COMBOBOX"; break;
			default: cls=L"STATIC"; break;
			}
		}
		else
		{
			cls = reinterpret_cast<const wchar_t*>(p);
			p += (cls.size()+1)*sizeof(wchar_t);
		}
		cw = reinterpret_cast<const WORD*>(p);
		if (*cw == 0xFFFF) p += 2*sizeof(WORD);
		else
		{
			cap = reinterpret_cast<const wchar_t*>(p);
			p += (cap.size()+1)*sizeof(wchar_t);
		}
		WORD extra = *reinterpret_cast<const WORD*>(p); p += sizeof(WORD);
		p += extra;
		int cxp = MulDiv(icx, 6, 4)*2, cyp = MulDiv(icy, 13, 8)*2;
		CreateWindowExW(ies, cls.c_str(), cap.c_str(), istyle|WS_CHILD|WS_VISIBLE,
			MulDiv(ix,6,4)*2, MulDiv(iy,13,8)*2, std::max(cxp,8), std::max(cyp,8),
			h, (HMENU)(uintptr_t)id, nullptr, nullptr);
	}

	if (proc) proc(h, WM_INITDIALOG, 0, lp);
	if (modal)
	{
		ShowWindow(h, SW_SHOW);
		MSG msg{};
		INT_PTR res = 0;
		while (GetMessageW(&msg, nullptr, 0, 0))
		{
			if (msg.message == WM_QUIT) break;
			if (!IsDialogMessageW(h, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessageW(&msg);
			}
			auto* d = implFrom(h);
			if (!d || d->destroyed) break;
		}
		if (result) *result = res;
	}
	return h;
}

INT_PTR DialogBoxParamW(HINSTANCE, LPCWSTR tmpl, HWND parent, DLGPROC proc, LPARAM lp)
{
	HRSRC r = FindResourceW(nullptr, tmpl, RT_DIALOG);
	if (!r) return -1;
	auto* t = (LPCDLGTEMPLATE)LockResource(LoadResource(nullptr, r));
	INT_PTR res=0;
	HWND h = nppCreateDialogIndirect(t, parent, proc, lp, true, &res);
	DestroyWindow(h);
	return res;
}
INT_PTR DialogBoxIndirectParamW(HINSTANCE, LPCDLGTEMPLATE tmpl, HWND parent, DLGPROC proc, LPARAM lp)
{
	INT_PTR res=0;
	HWND h = nppCreateDialogIndirect(tmpl, parent, proc, lp, true, &res);
	DestroyWindow(h);
	return res;
}
HWND CreateDialogParamW(HINSTANCE, LPCWSTR tmpl, HWND parent, DLGPROC proc, LPARAM lp)
{
	HRSRC r = FindResourceW(nullptr, tmpl, RT_DIALOG);
	if (!r) return nullptr;
	auto* t = (LPCDLGTEMPLATE)LockResource(LoadResource(nullptr, r));
	return nppCreateDialogIndirect(t, parent, proc, lp, false, nullptr);
}
HWND CreateDialogIndirectParamW(HINSTANCE, LPCDLGTEMPLATE tmpl, HWND parent, DLGPROC proc, LPARAM lp)
{
	return nppCreateDialogIndirect(tmpl, parent, proc, lp, false, nullptr);
}
BOOL EndDialog(HWND dlg, INT_PTR result)
{
	PostMessageW(dlg, WM_CLOSE, 0, 0);
	(void)result;
	DestroyWindow(dlg);
	return TRUE;
}
LONG GetDialogBaseUnits(void) { return MAKELONG(6,13); }
BOOL MapDialogRect(HWND, LPRECT rc)
{
	if (!rc) return FALSE;
	rc->left = MulDiv(rc->left, 6, 4);
	rc->right = MulDiv(rc->right, 6, 4);
	rc->top = MulDiv(rc->top, 13, 8);
	rc->bottom = MulDiv(rc->bottom, 13, 8);
	return TRUE;
}

void nppInitResources() {}
