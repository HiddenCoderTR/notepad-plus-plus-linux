#include "npp_platform.h"

#include <cstdlib>
#include <cstring>
#include <map>
#include <vector>

extern "C" {
const GUID CLSID_ShellLink = {0x00021401,0,0,{0xC0,0,0,0,0,0,0,0x46}};
const GUID IID_IShellLink   = {0x000214F9,0,0,{0xC0,0,0,0,0,0,0,0x46}};
const GUID IID_IPersistFile = {0x0000010b,0,0,{0xC0,0,0,0,0,0,0,0x46}};
}

HWND ChildWindowFromPointEx(HWND parent, POINT pt, UINT)
{
	return ChildWindowFromPoint(parent, pt);
}
BOOL GetClassNameA(HWND h, LPSTR buf, int max)
{
	wchar_t w[256];
	int n = GetClassNameW(h, w, 256);
	if (!buf || max <= 0) return n > 0;
	std::string u = nppWideToUtf8(w);
	snprintf(buf, (size_t)max, "%s", u.c_str());
	return TRUE;
}
BOOL SetDlgItemTextA(HWND dlg, int id, LPCSTR text)
{
	return SetWindowTextW(GetDlgItem(dlg, id), nppUtf8ToWide(text ? text : "").c_str());
}
int lstrcmpiA(LPCSTR a, LPCSTR b) { return strcasecmp(a ? a : "", b ? b : ""); }
int lstrcpynA(LPSTR dst, LPCSTR src, int max)
{
	if (!dst || max <= 0) return 0;
	snprintf(dst, (size_t)max, "%s", src ? src : "");
	return (int)strlen(dst);
}
BOOL GetTextExtentPointW(HDC hdc, LPCWSTR s, int c, LPSIZE sz) { return GetTextExtentPoint32W(hdc, s, c, sz); }
BOOL GetTextExtentPointA(HDC hdc, LPCSTR s, int c, LPSIZE sz)
{
	std::wstring w = nppUtf8ToWide(std::string(s ? s : "", s && c > 0 ? (size_t)c : 0));
	return GetTextExtentPoint32W(hdc, w.c_str(), (int)w.size(), sz);
}
BOOL OffsetWindowOrgEx(HDC, int, int, LPPOINT pt) { if (pt) { pt->x = 0; pt->y = 0; } return TRUE; }
BOOL RectVisible(HDC, const RECT*) { return TRUE; }
BOOL GetMenuBarInfo(HWND, LONG, LONG, PMENUBARINFO pmbi)
{
	if (pmbi) memset(pmbi, 0, sizeof(*pmbi));
	return FALSE;
}
BOOL SetMenuItemBitmaps(HMENU, UINT, UINT, HBITMAP, HBITMAP) { return TRUE; }
DWORD GetWindowThreadProcessId(HWND, LPDWORD pid) { if (pid) *pid = GetCurrentProcessId(); return GetCurrentThreadId(); }

BOOL CopyFileExW(LPCWSTR exist, LPCWSTR neu, LPVOID, LPVOID, LPBOOL, DWORD) { return CopyFileW(exist, neu, FALSE); }
BOOL ReplaceFileW(LPCWSTR replaced, LPCWSTR replacement, LPCWSTR backup, DWORD, LPVOID, LPVOID)
{
	if (backup && backup[0]) CopyFileW(replaced, backup, FALSE);
	return MoveFileExW(replacement, replaced, MOVEFILE_REPLACE_EXISTING);
}

int GetDateFormatEx(LPCWSTR, DWORD, const SYSTEMTIME* st, LPCWSTR, LPWSTR buf, int cch, LPCWSTR)
{
	return GetDateFormatW(0, 0, st, nullptr, buf, cch);
}
int GetTimeFormatEx(LPCWSTR, DWORD, const SYSTEMTIME* st, LPCWSTR, LPWSTR buf, int cch)
{
	return GetTimeFormatW(0, 0, st, nullptr, buf, cch);
}
COLORREF ColorAdjustLuma(COLORREF clr, int n, BOOL)
{
	int r = std::min(255, std::max(0, (int)GetRValue(clr) + n / 10));
	int g = std::min(255, std::max(0, (int)GetGValue(clr) + n / 10));
	int b = std::min(255, std::max(0, (int)GetBValue(clr) + n / 10));
	return RGB(r, g, b);
}
HRESULT AssocQueryStringW(DWORD, DWORD, LPCWSTR, LPCWSTR, LPWSTR psz, DWORD* pcch)
{
	if (psz && pcch && *pcch) psz[0] = 0;
	return HRESULT_FROM_WIN32(ERROR_NO_ASSOCIATION);
}
BOOL PathIsNetworkPathW(LPCWSTR path) { return PathIsUNCW(path); }
int SHCreateDirectory(HWND, LPCWSTR path) { return SHCreateDirectoryExW(nullptr, path, nullptr); }
HRESULT SHOpenFolderAndSelectItems(PCIDLIST_ABSOLUTE pidl, UINT, PCUITEMID_CHILD_ARRAY, DWORD)
{
	if (!pidl) return E_INVALIDARG;
	auto* p = (std::wstring*)pidl;
	ShellExecuteW(nullptr, L"open", p->c_str(), nullptr, nullptr, SW_SHOWNORMAL);
	return S_OK;
}

BOOL ImageList_GetImageInfo(HIMAGELIST himl, int i, IMAGEINFO* info)
{
	if (!himl || !info) return FALSE;
	int cx=16, cy=16;
	ImageList_GetIconSize(himl, &cx, &cy);
	info->hbmImage = nullptr;
	info->hbmMask = nullptr;
	info->rcImage = RECT{0, 0, cx, cy};
	(void)i;
	return TRUE;
}

static LRESULT CALLBACK nppSubclassThunk(HWND h, UINT msg, WPARAM w, LPARAM l)
{
	auto* wi = implFrom(h);
	if (!wi || wi->subclasses.empty())
		return DefWindowProcW(h, msg, w, l);
	auto sc = wi->subclasses.back();
	return sc.proc(h, msg, w, l, sc.uid, sc.data);
}

BOOL SetWindowSubclass(HWND h, SUBCLASSPROC proc, UINT_PTR uid, DWORD_PTR data)
{
	auto* wi = implFrom(h);
	if (!wi || !proc) return FALSE;
	if (wi->subclasses.empty())
	{
		wi->origProc = wi->wndproc;
		wi->wndproc = nppSubclassThunk;
	}
	for (auto& sc : wi->subclasses)
	{
		if (sc.proc == proc && sc.uid == uid)
		{
			sc.data = data;
			return TRUE;
		}
	}
	wi->subclasses.push_back({proc, uid, data});
	return TRUE;
}
BOOL GetWindowSubclass(HWND h, SUBCLASSPROC proc, UINT_PTR uid, DWORD_PTR* data)
{
	auto* wi = implFrom(h);
	if (!wi) return FALSE;
	for (const auto& sc : wi->subclasses)
	{
		if (sc.proc == proc && sc.uid == uid)
		{
			if (data) *data = sc.data;
			return TRUE;
		}
	}
	return FALSE;
}
BOOL RemoveWindowSubclass(HWND h, SUBCLASSPROC proc, UINT_PTR uid)
{
	auto* wi = implFrom(h);
	if (!wi) return FALSE;
	auto it = std::remove_if(wi->subclasses.begin(), wi->subclasses.end(),
		[&](const WindowImpl::SubclassEntry& sc) { return sc.proc == proc && sc.uid == uid; });
	bool found = it != wi->subclasses.end();
	wi->subclasses.erase(it, wi->subclasses.end());
	if (wi->subclasses.empty() && wi->origProc)
	{
		wi->wndproc = wi->origProc;
		wi->origProc = nullptr;
	}
	return found;
}
LRESULT DefSubclassProc(HWND h, UINT msg, WPARAM w, LPARAM l)
{
	auto* wi = implFrom(h);
	if (wi && wi->origProc) return wi->origProc(h, msg, w, l);
	return DefWindowProcW(h, msg, w, l);
}

BOOL TrackMouseEvent(LPTRACKMOUSEEVENT tme)
{
	if (!tme) return FALSE;
	return TRUE;
}

HRESULT DrawThemeTextEx(HTHEME, HDC hdc, int, int, LPCWSTR text, int cch, DWORD flags, LPRECT rc, const DTTOPTS*)
{
	return DrawThemeText(nullptr, hdc, 0, 0, text, cch, flags, 0, rc);
}
HRESULT DrawThemeParentBackground(HWND, HDC, const RECT*) { return S_OK; }
HRESULT GetThemeBackgroundContentRect(HTHEME, HDC, int, int, LPCRECT in, LPRECT out)
{
	if (in && out) *out = *in;
	return S_OK;
}
HRESULT GetThemeFont(HTHEME, HDC, int, int, int, LOGFONTW* lf)
{
	if (lf) memset(lf, 0, sizeof(*lf));
	return E_NOTIMPL;
}
HRESULT GetThemePartSize(HTHEME, HDC, int, int, LPCRECT, int, SIZE* sz)
{
	if (sz) { sz->cx = 16; sz->cy = 16; }
	return S_OK;
}
HRESULT GetThemeTransitionDuration(HTHEME, int, int, int, int, DWORD* ms)
{
	if (ms) *ms = 0;
	return S_OK;
}
HRESULT DwmGetColorizationColor(DWORD* color, BOOL* opaque)
{
	if (color) *color = RGB(0, 120, 215);
	if (opaque) *opaque = TRUE;
	return S_OK;
}
HANIMATIONBUFFER BeginBufferedAnimation(HWND, HDC, const RECT*, BP_BUFFERFORMAT, BP_PAINTPARAMS*, BP_ANIMATIONPARAMS*, HDC* hdcFrom, HDC* hdcTo)
{
	if (hdcFrom) *hdcFrom = nullptr;
	if (hdcTo) *hdcTo = nullptr;
	return nullptr;
}
BOOL EndBufferedAnimation(HANIMATIONBUFFER, BOOL) { return TRUE; }
BOOL BufferedPaintRenderAnimation(HWND, HDC) { return FALSE; }
BOOL BufferedPaintStopAllAnimations(HWND) { return TRUE; }

HANDLE OpenProcess(DWORD, BOOL, DWORD pid) { return (HANDLE)(uintptr_t)(pid ? pid : 1); }
BOOL OpenProcessToken(HANDLE, DWORD, PHANDLE token) { if (token) *token = (HANDLE)1; return TRUE; }
BOOL GetTokenInformation(HANDLE, int, LPVOID, DWORD, PDWORD ret) { if (ret) *ret = 0; return FALSE; }
BOOL AllocateAndInitializeSid(PSID_IDENTIFIER_AUTHORITY, BYTE, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, PSID* sid)
{
	if (sid) *sid = (PSID)malloc(SECURITY_MAX_SID_SIZE);
	return sid && *sid;
}
BOOL CheckTokenMembership(HANDLE, PSID, PBOOL member) { if (member) *member = FALSE; return TRUE; }
PUCHAR GetSidSubAuthorityCount(PSID) { static UCHAR n = 1; return &n; }
PDWORD GetSidSubAuthority(PSID, DWORD) { static DWORD v = SECURITY_MANDATORY_MEDIUM_RID; return &v; }
PVOID FreeSid(PSID sid) { free(sid); return nullptr; }
BOOL QueryFullProcessImageNameW(HANDLE, DWORD, LPWSTR buf, PDWORD size)
{
	return GetModuleFileNameW(nullptr, buf, size ? *size : 0) != 0;
}
BOOL TerminateThread(HANDLE, DWORD) { return FALSE; }
BOOL GetProductInfo(DWORD, DWORD, DWORD, DWORD, PDWORD type) { if (type) *type = 0; return FALSE; }
HRESULT RegisterApplicationRestart(PCWSTR, DWORD) { return S_OK; }
HRESULT UnregisterApplicationRestart(void) { return S_OK; }
HRESULT GetApplicationRestartSettings(HANDLE, PWSTR cmd, PDWORD size, PDWORD flags)
{
	if (cmd && size && *size) cmd[0] = 0;
	if (flags) *flags = 0;
	return S_OK;
}

BOOL CryptQueryObject(DWORD, const void*, DWORD, DWORD, DWORD, DWORD*, DWORD*, DWORD*, HCERTSTORE* store, HCRYPTMSG* msg, const void**)
{
	if (store) *store = nullptr;
	if (msg) *msg = nullptr;
	return FALSE;
}
BOOL CryptMsgGetParam(HCRYPTMSG, DWORD, DWORD, void*, DWORD* size) { if (size) *size = 0; return FALSE; }
BOOL CryptMsgClose(HCRYPTMSG) { return TRUE; }
PCCERT_CONTEXT CertFindCertificateInStore(HCERTSTORE, DWORD, DWORD, DWORD, const void*, PCCERT_CONTEXT) { return nullptr; }
DWORD CertGetNameStringW(PCCERT_CONTEXT, DWORD, DWORD, void*, LPWSTR buf, DWORD cch)
{
	if (buf && cch) buf[0] = 0;
	return 0;
}
BOOL CertFreeCertificateContext(PCCERT_CONTEXT) { return TRUE; }
BOOL CertCloseStore(HCERTSTORE, DWORD) { return TRUE; }

BOOL InternetCrackUrlW(LPCWSTR, DWORD, DWORD, LPURL_COMPONENTS c)
{
	if (c) memset(c, 0, sizeof(*c));
	return FALSE;
}
LONG RegGetValueW(HKEY k, LPCWSTR sub, LPCWSTR value, DWORD, LPDWORD type, PVOID data, LPDWORD size)
{
	return RegQueryValueExW(k, value, nullptr, type, (LPBYTE)data, size);
}

int GetROP2(HDC) { return R2_COPYPEN; }
BOOL SetBrushOrgEx(HDC, int, int, LPPOINT pt) { if (pt) { pt->x = 0; pt->y = 0; } return TRUE; }
BOOL ImageList_BeginDrag(HIMAGELIST, int, int, int) { return TRUE; }
BOOL ImageList_DragEnter(HWND, int, int) { return TRUE; }
BOOL ImageList_DragLeave(HWND) { return TRUE; }
BOOL ImageList_DragMove(int, int) { return TRUE; }
BOOL ImageList_DragShowNolock(BOOL) { return TRUE; }
void ImageList_EndDrag(void) {}
HRESULT LoadIconWithScaleDown(HINSTANCE inst, PCWSTR name, int cx, int cy, HICON* icon)
{
	if (icon) *icon = LoadIconW(inst, name);
	(void)cx; (void)cy;
	return S_OK;
}
BOOL Shell_NotifyIconW(DWORD, NOTIFYICONDATAW*) { return TRUE; }
BOOL CancelIo(HANDLE) { return TRUE; }
DWORD QueueUserAPC(PAPCFUNC, HANDLE, ULONG_PTR) { return 0; }
BOOL ReadDirectoryChangesW(HANDLE, LPVOID, DWORD, BOOL, DWORD, LPDWORD ret, LPOVERLAPPED, LPOVERLAPPED_COMPLETION_ROUTINE)
{
	if (ret) *ret = 0;
	return FALSE;
}
DWORD WaitForSingleObjectEx(HANDLE h, DWORD ms, BOOL) { return WaitForSingleObject(h, ms); }
int LCMapStringEx(LPCWSTR, DWORD flags, LPCWSTR src, int cchSrc, LPWSTR dst, int cchDst, LPVOID, LPARAM, HANDLE)
{
	return LCMapStringW(0, flags, src, cchSrc, dst, cchDst);
}
BOOL IsTextUnicode(const void*, int, LPINT tests) { if (tests) *tests = 0; return FALSE; }
int ToAscii(UINT, UINT, const BYTE*, LPWORD out, UINT) { if (out) *out = 0; return 0; }
BOOL _TrackMouseEvent(LPTRACKMOUSEEVENT tme) { return TrackMouseEvent(tme); }
PIMAGE_NT_HEADERS ImageNtHeader(PVOID) { return nullptr; }
