#include "npp_platform.h"

#include <pwd.h>
#include <unistd.h>
#include <sys/stat.h>
#include <cstring>
#include <map>
#include <vector>

// ---- paths ----
static void slash(std::wstring& p) { for (auto& c : p) if (c == L'\\') c = L'/'; }

BOOL PathFileExistsW(LPCWSTR path)
{
	return GetFileAttributesW(path) != INVALID_FILE_ATTRIBUTES;
}
BOOL PathIsDirectoryW(LPCWSTR path)
{
	DWORD a = GetFileAttributesW(path);
	return a != INVALID_FILE_ATTRIBUTES && (a & FILE_ATTRIBUTE_DIRECTORY);
}
BOOL PathIsRelativeW(LPCWSTR path)
{
	if (!path || !*path) return TRUE;
	return path[0] != L'/' && !(path[0] && path[1] == L':');
}
BOOL PathIsUNCW(LPCWSTR path) { return path && path[0]==L'\\' && path[1]==L'\\'; }
BOOL PathIsURLW(LPCWSTR path) { return path && (wcsstr(path, L"://") != nullptr); }
LPWSTR PathFindFileNameW(LPCWSTR path)
{
	if (!path) return (LPWSTR)L"";
	const wchar_t* s = path;
	for (const wchar_t* p = path; *p; ++p)
		if (*p == L'/' || *p == L'\\') s = p + 1;
	return (LPWSTR)s;
}
LPWSTR PathFindExtensionW(LPCWSTR path)
{
	LPWSTR name = PathFindFileNameW(path);
	LPWSTR dot = wcsrchr(name, L'.');
	return dot ? dot : (LPWSTR)(name + wcslen(name));
}
BOOL PathRemoveFileSpecW(LPWSTR path)
{
	if (!path) return FALSE;
	LPWSTR n = PathFindFileNameW(path);
	if (n != path) { if (n > path) *(n-1)=0; return TRUE; }
	return FALSE;
}
void PathRemoveExtensionW(LPWSTR path)
{
	LPWSTR e = PathFindExtensionW(path);
	if (e) *e = 0;
}
void PathStripPathW(LPWSTR path)
{
	if (!path) return;
	std::wstring n = PathFindFileNameW(path);
	wcscpy(path, n.c_str());
}
LPWSTR PathAddBackslashW(LPWSTR path)
{
	if (!path) return path;
	size_t n = wcslen(path);
	if (n && path[n-1] != L'/' && path[n-1] != L'\\') { path[n]=L'/'; path[n+1]=0; }
	return path;
}
BOOL PathAppendW(LPWSTR path, LPCWSTR more)
{
	if (!path || !more) return FALSE;
	PathAddBackslashW(path);
	if (*more == L'/' || *more == L'\\') ++more;
	wcscat(path, more);
	slash(*(new std::wstring())); // no-op keep slash helper used
	for (wchar_t* p = path; *p; ++p) if (*p==L'\\') *p=L'/';
	return TRUE;
}
LPWSTR PathCombineW(LPWSTR dest, LPCWSTR dir, LPCWSTR file)
{
	if (!dest) return nullptr;
	dest[0]=0;
	if (dir) wcscpy(dest, dir);
	if (file) PathAppendW(dest, file);
	return dest;
}
BOOL PathCanonicalizeW(LPWSTR dest, LPCWSTR src)
{
	if (!dest || !src) return FALSE;
	wcscpy(dest, src);
	for (wchar_t* p=dest; *p; ++p) if (*p==L'\\') *p=L'/';
	return TRUE;
}
BOOL PathQuoteSpacesW(LPWSTR path)
{
	if (!path || !wcschr(path, L' ')) return FALSE;
	std::wstring s = std::wstring(L"\"") + path + L"\"";
	wcscpy(path, s.c_str());
	return TRUE;
}
void PathUnquoteSpacesW(LPWSTR path)
{
	if (!path) return;
	size_t n = wcslen(path);
	if (n>=2 && path[0]==L'"' && path[n-1]==L'"') { memmove(path, path+1, (n-2)*sizeof(wchar_t)); path[n-2]=0; }
}
BOOL PathMatchSpecW(LPCWSTR file, LPCWSTR spec)
{
	if (!file || !spec) return FALSE;
	std::wstring f=file, s=spec;
	for (auto& c: f) c=towlower(c);
	for (auto& c: s) { c=towlower(c); if (c==L'\\') c=L'/'; }
	// very small glob: *
	if (s == L"*" || s == L"*.*") return TRUE;
	if (s.size()>=2 && s[0]==L'*' && s[1]==L'.')
	{
		std::wstring ext = s.substr(1);
		return f.size()>=ext.size() && f.compare(f.size()-ext.size(), ext.size(), ext)==0;
	}
	return f == s;
}
BOOL PathIsPrefixW(LPCWSTR prefix, LPCWSTR path)
{
	if (!prefix || !path) return FALSE;
	return wcsncmp(path, prefix, wcslen(prefix))==0;
}
BOOL PathRelativePathToW(LPWSTR path, LPCWSTR from, DWORD, LPCWSTR to, DWORD)
{
	if (!path || !to) return FALSE;
	wcscpy(path, to);
	return TRUE;
}
void PathRemoveBlanksW(LPWSTR path)
{
	if (!path) return;
	wchar_t* d=path; for (wchar_t* s=path; *s; ++s) if (*s!=L' ') *d++=*s; *d=0;
}
LPCWSTR PathFindNextComponentW(LPCWSTR path)
{
	if (!path) return nullptr;
	const wchar_t* p = wcschr(path, L'/');
	if (!p) p = wcschr(path, L'\\');
	return p ? p+1 : path+wcslen(path);
}
LPCWSTR PathSkipRootW(LPCWSTR path)
{
	if (!path) return nullptr;
	if (path[0]==L'/') return path+1;
	if (path[0] && path[1]==L':') return path+2;
	return path;
}
BOOL PathStripToRootW(LPWSTR path)
{
	if (!path) return FALSE;
	if (path[0]==L'/') { path[1]=0; return TRUE; }
	return FALSE;
}
int PathCommonPrefixW(LPCWSTR a, LPCWSTR b, LPWSTR prefix)
{
	int i=0; if(!a||!b) return 0;
	while (a[i] && a[i]==b[i]) ++i;
	if (prefix) { wcsncpy(prefix, a, i); prefix[i]=0; }
	return i;
}
BOOL PathCompactPathW(HDC, LPWSTR, UINT) { return TRUE; }
BOOL PathCompactPathExW(LPWSTR out, LPCWSTR src, UINT cch, DWORD)
{
	if (!out||!src) return FALSE;
	wcsncpy(out, src, cch-1); out[cch-1]=0; return TRUE;
}
int PathGetDriveNumberW(LPCWSTR) { return -1; }
BOOL PathIsFileSpecW(LPCWSTR path) { return path && !wcschr(path,L'/') && !wcschr(path,L'\\'); }
BOOL PathIsRootW(LPCWSTR path) { return path && ((path[0]==L'/' && path[1]==0) || (path[0] && path[1]==L':' && path[2]==0)); }
BOOL PathIsSameRootW(LPCWSTR a, LPCWSTR b) { return a && b && a[0]==b[0]; }
BOOL PathRenameExtensionW(LPWSTR path, LPCWSTR ext)
{
	PathRemoveExtensionW(path);
	if (ext) wcscat(path, ext);
	return TRUE;
}
void PathSetDlgItemPathW(HWND dlg, int id, LPCWSTR path) { SetDlgItemTextW(dlg, id, path); }

LPWSTR StrCpyNW(LPWSTR dst, LPCWSTR src, int max) { return lstrcpynW(dst, src, max); }
LPWSTR StrCatW(LPWSTR dst, LPCWSTR src) { return lstrcatW(dst, src); }
LPWSTR StrStrW(LPCWSTR hay, LPCWSTR needle) { return hay && needle ? (LPWSTR)wcsstr(hay, needle) : nullptr; }
LPWSTR StrStrIW(LPCWSTR hay, LPCWSTR needle)
{
	if (!hay || !needle) return nullptr;
	std::wstring h=hay, n=needle;
	for (auto& c: h) c=towlower(c);
	for (auto& c: n) c=towlower(c);
	auto p = h.find(n);
	return p==std::wstring::npos ? nullptr : (LPWSTR)(hay + p);
}
int StrCmpW(LPCWSTR a, LPCWSTR b) { return lstrcmpW(a,b); }
int StrCmpIW(LPCWSTR a, LPCWSTR b) { return lstrcmpiW(a,b); }
int StrCmpNW(LPCWSTR a, LPCWSTR b, int n) { return wcsncmp(a?a:L"", b?b:L"", n); }
int StrCmpNIW(LPCWSTR a, LPCWSTR b, int n) { return _wcsnicmp(a,b,(size_t)n); }
BOOL StrTrimW(LPWSTR s, LPCWSTR trim)
{
	if (!s) return FALSE;
	std::wstring t = trim?trim:L" ";
	std::wstring v=s;
	while (!v.empty() && t.find(v.front())!=std::wstring::npos) v.erase(v.begin());
	while (!v.empty() && t.find(v.back())!=std::wstring::npos) v.pop_back();
	wcscpy(s, v.c_str());
	return TRUE;
}
int StrToIntW(LPCWSTR s) { return _wtoi(s); }
BOOL StrToInt64ExW(LPCWSTR s, DWORD, LONGLONG* ret) { if(ret)*ret = s?wcstoll(s,nullptr,10):0; return TRUE; }
LPWSTR StrDupW(LPCWSTR s) { return s ? wcsdup(s) : nullptr; }
LPWSTR StrRChrW(LPCWSTR start, LPCWSTR end, WCHAR ch)
{
	if (!start) return nullptr;
	if (!end) end = start + wcslen(start);
	for (const wchar_t* p=end; p>start; --p) if (p[-1]==ch) return (LPWSTR)(p-1);
	return nullptr;
}
LPWSTR StrChrW(LPCWSTR s, WCHAR ch) { return s ? (LPWSTR)wcschr(s, ch) : nullptr; }
LPWSTR StrRStrIW(LPCWSTR hay, LPCWSTR, LPCWSTR needle) { return StrStrIW(hay, needle); }
int wvnsprintfW(LPWSTR buf, int cch, LPCWSTR fmt, va_list args) { return vswprintf(buf, cch, fmt, args); }
int wnsprintfW(LPWSTR buf, int cch, LPCWSTR fmt, ...)
{
	va_list ap; va_start(ap, fmt); int r=vswprintf(buf,cch,fmt,ap); va_end(ap); return r;
}
HRESULT StringCchCopyW(STRSAFE_LPWSTR dest, size_t cch, STRSAFE_LPCWSTR src)
{
	if (!dest || !cch) return E_INVALIDARG;
	wcsncpy(dest, src?src:L"", cch-1); dest[cch-1]=0; return S_OK;
}
HRESULT StringCchCatW(STRSAFE_LPWSTR dest, size_t cch, STRSAFE_LPCWSTR src)
{
	if (!dest||!cch) return E_INVALIDARG;
	size_t n=wcslen(dest);
	if (n>=cch) return E_INVALIDARG;
	wcsncpy(dest+n, src?src:L"", cch-n-1); dest[cch-1]=0; return S_OK;
}
HRESULT StringCchVPrintfW(STRSAFE_LPWSTR dest, size_t cch, STRSAFE_LPCWSTR fmt, va_list args)
{
	if (!dest||!cch) return E_INVALIDARG;
	vswprintf(dest, cch, fmt, args); return S_OK;
}
HRESULT StringCchPrintfW(STRSAFE_LPWSTR dest, size_t cch, STRSAFE_LPCWSTR fmt, ...)
{
	va_list ap; va_start(ap, fmt); HRESULT hr=StringCchVPrintfW(dest,cch,fmt,ap); va_end(ap); return hr;
}
HRESULT StringCchLengthW(STRSAFE_LPCWSTR psz, size_t cchMax, size_t* length)
{
	if (!psz) return E_INVALIDARG;
	size_t n=wcsnlen(psz, cchMax);
	if (length) *length=n;
	return S_OK;
}

static std::wstring homeDir()
{
	const char* h = getenv("HOME");
	if (!h) { auto* pw = getpwuid(getuid()); h = pw ? pw->pw_dir : "/tmp"; }
	return nppUtf8ToWide(h);
}

HRESULT SHGetFolderPathW(HWND, int csidl, HANDLE, DWORD, LPWSTR path)
{
	if (!path) return E_FAIL;
	std::wstring base = homeDir();
	std::wstring out;
	switch (csidl & 0xFF)
	{
	case CSIDL_APPDATA: out = base + L"/.config/notepad++"; break;
	case CSIDL_LOCAL_APPDATA: out = base + L"/.local/share/notepad++"; break;
	case CSIDL_PROFILE: out = base; break;
	case CSIDL_MYDOCUMENTS: out = base + L"/Documents"; break;
	case CSIDL_DESKTOP: out = base + L"/Desktop"; break;
	case CSIDL_COMMON_APPDATA: out = L"/etc/notepad++"; break;
	default: out = base + L"/.config/notepad++"; break;
	}
	CreateDirectoryW(out.c_str(), nullptr);
	wcsncpy(path, out.c_str(), MAX_PATH-1);
	path[MAX_PATH-1]=0;
	return S_OK;
}
BOOL SHGetSpecialFolderPathW(HWND h, LPWSTR path, int csidl, BOOL)
{
	return SUCCEEDED(SHGetFolderPathW(h, csidl, nullptr, 0, path));
}
HRESULT SHGetKnownFolderPath(REFKNOWNFOLDERID, DWORD, HANDLE, PWSTR* path)
{
	if (!path) return E_FAIL;
	wchar_t buf[MAX_PATH];
	SHGetFolderPathW(nullptr, CSIDL_APPDATA, nullptr, 0, buf);
	*path = (PWSTR)CoTaskMemAlloc((wcslen(buf)+1)*sizeof(wchar_t));
	wcscpy(*path, buf);
	return S_OK;
}

LPITEMIDLIST SHBrowseForFolderW(LPBROWSEINFOW bi)
{
	std::wstring title = bi && bi->lpszTitle ? bi->lpszTitle : L"Select Folder";
	GtkWidget* dlg = gtk_file_chooser_dialog_new(nppWideToUtf8(title).c_str(), nullptr,
		GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER, "Cancel", GTK_RESPONSE_CANCEL, "Open", GTK_RESPONSE_ACCEPT, nullptr);
	std::wstring* result = nullptr;
	if (gtk_dialog_run(GTK_DIALOG(dlg)) == GTK_RESPONSE_ACCEPT)
	{
		char* f = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dlg));
		result = new std::wstring(nppUtf8ToWide(f));
		g_free(f);
	}
	gtk_widget_destroy(dlg);
	return (LPITEMIDLIST)result;
}
BOOL SHGetPathFromIDListW(LPCITEMIDLIST pidl, LPWSTR path)
{
	auto* s = (std::wstring*)pidl;
	if (!s || !path) return FALSE;
	wcsncpy(path, s->c_str(), MAX_PATH-1);
	return TRUE;
}
int SHFileOperationW(LPSHFILEOPSTRUCTW op)
{
	if (!op || !op->pFrom) return 1;
	if (op->wFunc == FO_DELETE) return DeleteFileW(op->pFrom) ? 0 : 1;
	if (op->wFunc == FO_COPY) return CopyFileW(op->pFrom, op->pTo, FALSE) ? 0 : 1;
	if (op->wFunc == FO_MOVE) return MoveFileW(op->pFrom, op->pTo) ? 0 : 1;
	return 1;
}
DWORD_PTR SHGetFileInfoW(LPCWSTR path, DWORD, SHFILEINFOW* info, UINT, UINT)
{
	if (info)
	{
		memset(info, 0, sizeof(*info));
		LPWSTR name = PathFindFileNameW(path);
		if (name) wcsncpy(info->szDisplayName, name, MAX_PATH-1);
	}
	return 1;
}
UINT DragQueryFileW(HDROP drop, UINT i, LPWSTR buf, UINT cch)
{
	auto* files = (std::vector<std::wstring>*)drop;
	if (!files) return 0;
	if (i == 0xFFFFFFFF) return (UINT)files->size();
	if (i >= files->size()) return 0;
	if (buf && cch) { wcsncpy(buf, (*files)[i].c_str(), cch-1); buf[cch-1]=0; }
	return (UINT)(*files)[i].size();
}
void DragFinish(HDROP drop) { delete (std::vector<std::wstring>*)drop; }
void DragAcceptFiles(HWND, BOOL) {}
BOOL DragQueryPoint(HDROP, LPPOINT pt) { if(pt) pt->x=pt->y=0; return TRUE; }
HRESULT SHCreateItemFromParsingName(PCWSTR, IBindCtx*, REFIID, void** ppv) { if(ppv)*ppv=nullptr; return E_NOTIMPL; }
LPWSTR* CommandLineToArgvW(LPCWSTR cmd, int* argc)
{
	static std::vector<std::wstring> args;
	args.clear();
	if (cmd && *cmd) args.push_back(cmd);
	if (argc) *argc = (int)args.size();
	auto* out = (LPWSTR*)malloc((args.size()+1)*sizeof(LPWSTR));
	for (size_t i=0;i<args.size();++i) out[i] = args[i].data();
	out[args.size()] = nullptr;
	return out;
}
UINT SHAppBarMessage(DWORD, PAPPBARDATA) { return 0; }
UINT ExtractIconExW(LPCWSTR, int, HICON* large, HICON* small, UINT n)
{
	if (large) *large = nullptr;
	if (small) *small = nullptr;
	return 0;
}
int SHCreateDirectoryExW(HWND, LPCWSTR path, const SECURITY_ATTRIBUTES*)
{
	return CreateDirectoryW(path, nullptr) ? 0 : -1;
}
HRESULT SHParseDisplayName(PCWSTR, IBindCtx*, PIDLIST_ABSOLUTE* pidl, SFGAOF, SFGAOF*)
{
	if (pidl) *pidl = nullptr;
	return E_NOTIMPL;
}
HRESULT SHGetStockIconInfo(SHSTOCKICONID, UINT, SHSTOCKICONINFO* info)
{
	if (info) memset(info, 0, sizeof(*info));
	return S_OK;
}

HINSTANCE ShellExecuteW(HWND, LPCWSTR, LPCWSTR file, LPCWSTR, LPCWSTR, INT)
{
	if (!file) return (HINSTANCE)(uintptr_t)32;
	std::string cmd = "xdg-open \"" + nppWideToUtf8(file) + "\" &";
	system(cmd.c_str());
	return (HINSTANCE)(uintptr_t)33;
}
BOOL ShellExecuteExW(SHELLEXECUTEINFOW* info)
{
	if (!info) return FALSE;
	return (uintptr_t)ShellExecuteW(info->hwnd, info->lpVerb, info->lpFile, info->lpParameters, info->lpDirectory, info->nShow) > 32;
}

void CoTaskMemFree(LPVOID pv) { free(pv); }
LPVOID CoTaskMemAlloc(SIZE_T cb) { return malloc(cb); }
LPVOID CoTaskMemRealloc(LPVOID pv, SIZE_T cb) { return realloc(pv, cb); }
HRESULT CoInitialize(LPVOID) { return S_OK; }
HRESULT CoInitializeEx(LPVOID, DWORD) { return S_OK; }
void CoUninitialize(void) {}
HRESULT OleInitialize(LPVOID) { return S_OK; }
void OleUninitialize(void) {}
HRESULT CoCreateInstance(REFCLSID, LPUNKNOWN, DWORD, REFIID, LPVOID* ppv) { if(ppv)*ppv=nullptr; return E_NOINTERFACE; }

// clipboard
static std::wstring g_clipW;
static std::string g_clipA;
static bool g_clipOpen = false;

BOOL OpenClipboard(HWND) { g_clipOpen = true; return TRUE; }
BOOL CloseClipboard(void) { g_clipOpen = false; return TRUE; }
BOOL EmptyClipboard(void) { g_clipW.clear(); g_clipA.clear(); return TRUE; }
HANDLE SetClipboardData(UINT fmt, HANDLE mem)
{
	if (fmt == CF_UNICODETEXT && mem)
		g_clipW = (LPCWSTR)GlobalLock(mem);
	if (fmt == CF_TEXT && mem)
		g_clipA = (LPCSTR)GlobalLock(mem);
	GtkClipboard* cb = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
	if (!g_clipW.empty())
		gtk_clipboard_set_text(cb, nppWideToUtf8(g_clipW).c_str(), -1);
	else if (!g_clipA.empty())
		gtk_clipboard_set_text(cb, g_clipA.c_str(), -1);
	return mem;
}
HANDLE GetClipboardData(UINT fmt)
{
	GtkClipboard* cb = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
	char* t = gtk_clipboard_wait_for_text(cb);
	if (t)
	{
		g_clipW = nppUtf8ToWide(t);
		g_clipA = t;
		g_free(t);
	}
	if (fmt == CF_UNICODETEXT)
	{
		SIZE_T n = (g_clipW.size()+1)*sizeof(wchar_t);
		HGLOBAL h = GlobalAlloc(GMEM_MOVEABLE, n);
		memcpy(GlobalLock(h), g_clipW.c_str(), n);
		return h;
	}
	if (fmt == CF_TEXT)
	{
		HGLOBAL h = GlobalAlloc(GMEM_MOVEABLE, g_clipA.size()+1);
		memcpy(GlobalLock(h), g_clipA.c_str(), g_clipA.size()+1);
		return h;
	}
	return nullptr;
}
BOOL IsClipboardFormatAvailable(UINT fmt) { return fmt==CF_UNICODETEXT || fmt==CF_TEXT; }
int CountClipboardFormats(void) { return 2; }
UINT EnumClipboardFormats(UINT fmt) { return fmt==0 ? CF_UNICODETEXT : 0; }
int GetClipboardFormatNameW(UINT, LPWSTR buf, int cch) { if(buf&&cch) buf[0]=0; return 0; }
UINT RegisterClipboardFormatW(LPCWSTR) { static UINT n=0xC000; return n++; }
HWND SetClipboardViewer(HWND h) { return h; }
BOOL ChangeClipboardChain(HWND, HWND) { return TRUE; }
HWND GetClipboardOwner(void) { return nullptr; }
HWND GetOpenClipboardWindow(void) { return nullptr; }
DWORD GetClipboardSequenceNumber(void) { return 1; }
BOOL AddClipboardFormatListener(HWND) { return TRUE; }
BOOL RemoveClipboardFormatListener(HWND) { return TRUE; }

// registry in-memory
struct RegKey { std::map<std::wstring, std::wstring> vals; std::map<std::wstring, RegKey*> subs; };
static RegKey g_hkcu, g_hklm;

static RegKey* rootOf(HKEY k)
{
	if (k == HKEY_CURRENT_USER) return &g_hkcu;
	if (k == HKEY_LOCAL_MACHINE) return &g_hklm;
	return (RegKey*)k;
}

LSTATUS RegOpenKeyExW(HKEY key, LPCWSTR sub, DWORD, REGSAM, PHKEY result)
{
	RegKey* r = rootOf(key);
	if (!r) return 2;
	if (!sub || !*sub) { if(result)*result=(HKEY)r; return 0; }
	auto it = r->subs.find(sub);
	if (it == r->subs.end()) { auto* n = new RegKey(); r->subs[sub]=n; it=r->subs.find(sub); }
	if (result) *result = (HKEY)it->second;
	return 0;
}
LSTATUS RegOpenKeyExA(HKEY k, LPCSTR sub, DWORD o, REGSAM s, PHKEY r)
{
	std::wstring w = nppUtf8ToWide(sub?sub:"");
	return RegOpenKeyExW(k, w.c_str(), o, s, r);
}
LSTATUS RegCreateKeyExW(HKEY key, LPCWSTR sub, DWORD, LPWSTR, DWORD, REGSAM, const LPSECURITY_ATTRIBUTES, PHKEY result, LPDWORD disp)
{
	if (disp) *disp = 1;
	return RegOpenKeyExW(key, sub, 0, 0, result);
}
LSTATUS RegCloseKey(HKEY) { return 0; }
LSTATUS RegQueryValueExW(HKEY key, LPCWSTR name, LPDWORD, LPDWORD type, LPBYTE data, LPDWORD cb)
{
	RegKey* r = rootOf(key);
	if (!r) return 2;
	auto it = r->vals.find(name?name:L"");
	if (it == r->vals.end()) return 2;
	if (type) *type = REG_SZ;
	size_t n = (it->second.size()+1)*sizeof(wchar_t);
	if (cb && data && *cb >= n) memcpy(data, it->second.c_str(), n);
	if (cb) *cb = (DWORD)n;
	return 0;
}
LSTATUS RegQueryValueExA(HKEY k, LPCSTR name, LPDWORD a, LPDWORD t, LPBYTE d, LPDWORD cb)
{
	std::wstring w = nppUtf8ToWide(name?name:"");
	return RegQueryValueExW(k, w.c_str(), a, t, d, cb);
}
LSTATUS RegSetValueExW(HKEY key, LPCWSTR name, DWORD, DWORD, const BYTE* data, DWORD)
{
	RegKey* r = rootOf(key);
	if (!r) return 2;
	r->vals[name?name:L""] = data ? (LPCWSTR)data : L"";
	return 0;
}
LSTATUS RegDeleteValueW(HKEY key, LPCWSTR name)
{
	RegKey* r = rootOf(key);
	if (r) r->vals.erase(name?name:L"");
	return 0;
}
LSTATUS RegDeleteKeyW(HKEY key, LPCWSTR sub)
{
	RegKey* r = rootOf(key);
	if (r) r->subs.erase(sub?sub:L"");
	return 0;
}
LSTATUS RegEnumKeyExW(HKEY, DWORD, LPWSTR name, LPDWORD cchName, LPDWORD, LPWSTR, LPDWORD, PFILETIME)
{
	if (name && cchName) { name[0]=0; *cchName=0; }
	return 259; // ERROR_NO_MORE_ITEMS
}
LSTATUS RegEnumValueW(HKEY, DWORD, LPWSTR name, LPDWORD cchName, LPDWORD, LPDWORD, LPBYTE, LPDWORD)
{
	if (name && cchName) { name[0]=0; *cchName=0; }
	return 259;
}
LSTATUS RegQueryInfoKeyW(HKEY, LPWSTR, LPDWORD, LPDWORD, LPDWORD subKeys, LPDWORD, LPDWORD, LPDWORD values, LPDWORD, LPDWORD, LPDWORD, PFILETIME)
{
	if (subKeys) *subKeys=0;
	if (values) *values=0;
	return 0;
}
LSTATUS SHDeleteKeyW(HKEY k, LPCWSTR sub) { return RegDeleteKeyW(k, sub); }
LSTATUS SHGetValueW(HKEY key, LPCWSTR sub, LPCWSTR value, DWORD* type, void* data, DWORD* cb)
{
	HKEY h; if (RegOpenKeyExW(key, sub, 0, 0, &h)!=0) return 2;
	return RegQueryValueExW(h, value, nullptr, type, (LPBYTE)data, cb);
}
LSTATUS SHSetValueW(HKEY key, LPCWSTR sub, LPCWSTR value, DWORD type, LPCVOID data, DWORD cb)
{
	HKEY h; RegCreateKeyExW(key, sub, 0, nullptr, 0, 0, nullptr, &h, nullptr);
	return RegSetValueExW(h, value, 0, type, (const BYTE*)data, cb);
}

BOOL GetOpenFileNameW(LPOPENFILENAMEW ofn)
{
	if (!ofn) return FALSE;
	GtkWidget* dlg = gtk_file_chooser_dialog_new("Open", nullptr, GTK_FILE_CHOOSER_ACTION_OPEN,
		"Cancel", GTK_RESPONSE_CANCEL, "Open", GTK_RESPONSE_ACCEPT, nullptr);
	if (ofn->lpstrInitialDir) gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dlg), nppWideToUtf8(ofn->lpstrInitialDir).c_str());
	gboolean ok = gtk_dialog_run(GTK_DIALOG(dlg)) == GTK_RESPONSE_ACCEPT;
	if (ok && ofn->lpstrFile)
	{
		char* f = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dlg));
		std::wstring w = nppUtf8ToWide(f);
		wcsncpy(ofn->lpstrFile, w.c_str(), ofn->nMaxFile-1);
		g_free(f);
	}
	gtk_widget_destroy(dlg);
	return ok;
}
BOOL GetSaveFileNameW(LPOPENFILENAMEW ofn)
{
	if (!ofn) return FALSE;
	GtkWidget* dlg = gtk_file_chooser_dialog_new("Save", nullptr, GTK_FILE_CHOOSER_ACTION_SAVE,
		"Cancel", GTK_RESPONSE_CANCEL, "Save", GTK_RESPONSE_ACCEPT, nullptr);
	gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dlg), TRUE);
	if (ofn->lpstrFile && ofn->lpstrFile[0])
		gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dlg), nppWideToUtf8(PathFindFileNameW(ofn->lpstrFile)).c_str());
	gboolean ok = gtk_dialog_run(GTK_DIALOG(dlg)) == GTK_RESPONSE_ACCEPT;
	if (ok && ofn->lpstrFile)
	{
		char* f = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dlg));
		std::wstring w = nppUtf8ToWide(f);
		wcsncpy(ofn->lpstrFile, w.c_str(), ofn->nMaxFile-1);
		g_free(f);
	}
	gtk_widget_destroy(dlg);
	return ok;
}
BOOL ChooseColorW(LPCHOOSECOLORW cc)
{
	if (!cc) return FALSE;
	GtkWidget* dlg = gtk_color_chooser_dialog_new("Color", nullptr);
	gint resp = gtk_dialog_run(GTK_DIALOG(dlg));
	if (resp == GTK_RESPONSE_OK)
	{
		GdkRGBA c{};
		gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(dlg), &c);
		cc->rgbResult = RGB((int)(c.red*255), (int)(c.green*255), (int)(c.blue*255));
	}
	gtk_widget_destroy(dlg);
	return resp == GTK_RESPONSE_OK;
}
BOOL ChooseFontW(LPCHOOSEFONTW) { return FALSE; }
HWND FindTextW(LPFINDREPLACEW) { return nullptr; }
HWND ReplaceTextW(LPFINDREPLACEW) { return nullptr; }
DWORD CommDlgExtendedError(void) { return 0; }
BOOL PrintDlgW(LPPRINTDLGW) { return FALSE; }
BOOL PageSetupDlgW(LPPAGESETUPDLGW) { return FALSE; }
HRESULT TaskDialog(HWND owner, HINSTANCE, PCWSTR title, PCWSTR main, PCWSTR content, TASKDIALOG_COMMON_BUTTON_FLAGS, PCWSTR, int* button)
{
	std::wstring t = std::wstring(main?main:L"") + L"\n" + (content?content:L"");
	int r = MessageBoxW(owner, t.c_str(), title, MB_OK);
	if (button) *button = r;
	return S_OK;
}
HRESULT TaskDialogIndirect(const TASKDIALOGCONFIG* cfg, int* button, int*, BOOL*)
{
	return TaskDialog(cfg?cfg->hwndParent:nullptr, nullptr, cfg?cfg->pszWindowTitle:nullptr,
		cfg?cfg->pszMainInstruction:nullptr, cfg?cfg->pszContent:nullptr, 0, nullptr, button);
}

void ColorRGBToHLS(COLORREF clr, WORD* hue, WORD* luminance, WORD* saturation)
{
	const int r = GetRValue(clr);
	const int g = GetGValue(clr);
	const int b = GetBValue(clr);
	const int maxc = std::max({r, g, b});
	const int minc = std::min({r, g, b});
	int l = ((maxc + minc) * 240 + 255) / (2 * 255);
	int s = 0;
	int h = 160;
	const int delta = maxc - minc;
	if (delta)
	{
		if (l <= 120)
			s = (delta * 240) / std::max(1, maxc + minc);
		else
			s = (delta * 240) / std::max(1, 510 - maxc - minc);
		if (r == maxc)
			h = ((g - b) * 40) / delta;
		else if (g == maxc)
			h = 80 + ((b - r) * 40) / delta;
		else
			h = 160 + ((r - g) * 40) / delta;
		if (h < 0) h += 240;
		if (h >= 240) h -= 240;
	}
	if (hue) *hue = (WORD)h;
	if (luminance) *luminance = (WORD)l;
	if (saturation) *saturation = (WORD)s;
}

static int hlsValue(int m1, int m2, int hue)
{
	if (hue < 0) hue += 240;
	if (hue >= 240) hue -= 240;
	if (hue < 40) return m1 + (m2 - m1) * hue / 40;
	if (hue < 120) return m2;
	if (hue < 160) return m1 + (m2 - m1) * (160 - hue) / 40;
	return m1;
}

COLORREF ColorHLSToRGB(WORD hue, WORD lum, WORD sat)
{
	if (sat == 0)
	{
		int v = (lum * 255) / 240;
		return RGB(v, v, v);
	}
	int m2 = (lum <= 120) ? (lum * (240 + sat) / 240) : (lum + sat - (lum * sat / 240));
	int m1 = 2 * lum - m2;
	int r = (hlsValue(m1, m2, hue + 80) * 255) / 240;
	int g = (hlsValue(m1, m2, hue) * 255) / 240;
	int b = (hlsValue(m1, m2, hue - 80) * 255) / 240;
	auto clip = [](int v) { return v < 0 ? 0 : (v > 255 ? 255 : v); };
	return RGB(clip(r), clip(g), clip(b));
}
