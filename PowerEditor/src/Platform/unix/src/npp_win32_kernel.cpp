#include "npp_platform.h"

#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <pthread.h>
#include <dlfcn.h>
#include <pwd.h>
#include <cerrno>
#include <chrono>
#include <cstring>
#include <fstream>
#include <unordered_map>
#include <vector>

struct FileHandle {
	static constexpr uint32_t kMagic = 0xF11E0001u;
	uint32_t magic = kMagic;
	int fd = -1;
	std::wstring path;
	bool isFind = false;
	DIR* dir = nullptr;
	std::string pattern;
	std::string dirPath;
};

struct FindCtx {
	DIR* dir = nullptr;
	std::string dirPath;
	std::string glob;
};

static DWORD g_tls[64]{};
static thread_local LPVOID g_tlsVal[64]{};
static pthread_mutex_t g_heapMx = PTHREAD_MUTEX_INITIALIZER;

static std::string toUtf8Path(LPCWSTR p)
{
	return nppWideToUtf8(nppNormalizePath(p ? p : L""));
}

static DWORD errnoToWin()
{
	switch (errno)
	{
	case ENOENT: return ERROR_FILE_NOT_FOUND;
	case EACCES: case EPERM: return ERROR_ACCESS_DENIED;
	case EEXIST: return ERROR_ALREADY_EXISTS;
	case EINVAL: return ERROR_INVALID_PARAMETER;
	case ENOMEM: return ERROR_NOT_ENOUGH_MEMORY;
	default: return ERROR_GEN_FAILURE;
	}
}

static void statToFindData(const struct stat& st, const char* name, WIN32_FIND_DATAW* data)
{
	memset(data, 0, sizeof(*data));
	if (S_ISDIR(st.st_mode)) data->dwFileAttributes |= FILE_ATTRIBUTE_DIRECTORY;
	else data->dwFileAttributes |= FILE_ATTRIBUTE_NORMAL;
	if (!(st.st_mode & S_IWUSR)) data->dwFileAttributes |= FILE_ATTRIBUTE_READONLY;
	data->nFileSizeLow = (DWORD)(st.st_size & 0xFFFFFFFF);
	data->nFileSizeHigh = (DWORD)(st.st_size >> 32);
	std::wstring wn = nppUtf8ToWide(name);
	wcsncpy(data->cFileName, wn.c_str(), MAX_PATH - 1);
}

HANDLE CreateFileW(LPCWSTR name, DWORD access, DWORD, LPSECURITY_ATTRIBUTES, DWORD disp, DWORD, HANDLE)
{
	std::string path = toUtf8Path(name);
	int flags = 0;
	if ((access & GENERIC_READ) && (access & GENERIC_WRITE)) flags = O_RDWR;
	else if (access & GENERIC_WRITE) flags = O_WRONLY;
	else flags = O_RDONLY;
	switch (disp)
	{
	case CREATE_NEW: flags |= O_CREAT | O_EXCL; break;
	case CREATE_ALWAYS: flags |= O_CREAT | O_TRUNC; break;
	case OPEN_ALWAYS: flags |= O_CREAT; break;
	case TRUNCATE_EXISTING: flags |= O_TRUNC; break;
	default: break;
	}
	int fd = open(path.c_str(), flags, 0644);
	if (fd < 0) { nppSetLastError(errnoToWin()); return INVALID_HANDLE_VALUE; }
	auto* fh = new FileHandle(); fh->fd = fd; fh->path = nppNormalizePath(name ? name : L"");
	return (HANDLE)fh;
}

BOOL ReadFile(HANDLE h, LPVOID buf, DWORD n, LPDWORD read, LPOVERLAPPED)
{
	auto* f = (FileHandle*)h;
	if (!f || f->fd < 0) return FALSE;
	ssize_t r = ::read(f->fd, buf, n);
	if (r < 0) { nppSetLastError(errnoToWin()); return FALSE; }
	if (read) *read = (DWORD)r;
	return TRUE;
}
BOOL WriteFile(HANDLE h, LPCVOID buf, DWORD n, LPDWORD written, LPOVERLAPPED)
{
	auto* f = (FileHandle*)h;
	if (!f || f->fd < 0) return FALSE;
	ssize_t r = ::write(f->fd, buf, n);
	if (r < 0) { nppSetLastError(errnoToWin()); return FALSE; }
	if (written) *written = (DWORD)r;
	return TRUE;
}
BOOL CloseHandle(HANDLE h)
{
	if (!h || h == INVALID_HANDLE_VALUE) return FALSE;
	auto* f = (FileHandle*)h;
	if (f->magic != FileHandle::kMagic) return TRUE;
	if (f->dir) closedir(f->dir);
	if (f->fd >= 0) close(f->fd);
	f->magic = 0;
	delete f;
	return TRUE;
}

static bool isFileHandle(HANDLE h)
{
	if (!h || h == INVALID_HANDLE_VALUE) return false;
	auto* f = (FileHandle*)h;
	return f->magic == FileHandle::kMagic;
}

DWORD SetFilePointer(HANDLE h, LONG dist, PLONG distHigh, DWORD method)
{
	auto* f = (FileHandle*)h;
	if (!f || f->fd < 0) return INVALID_SET_FILE_POINTER;
	off_t off = dist;
	if (distHigh) off |= ((off_t)*distHigh) << 32;
	int whence = SEEK_SET;
	if (method == FILE_CURRENT) whence = SEEK_CUR;
	if (method == FILE_END) whence = SEEK_END;
	off_t r = lseek(f->fd, off, whence);
	if (r < 0) return INVALID_SET_FILE_POINTER;
	if (distHigh) *distHigh = (LONG)(r >> 32);
	return (DWORD)(r & 0xFFFFFFFF);
}
BOOL SetFilePointerEx(HANDLE h, LARGE_INTEGER dist, PLARGE_INTEGER newPos, DWORD method)
{
	LONG high = dist.HighPart;
	DWORD low = SetFilePointer(h, dist.LowPart, &high, method);
	if (low == INVALID_SET_FILE_POINTER && GetLastError()) return FALSE;
	if (newPos) { newPos->LowPart = low; newPos->HighPart = high; }
	return TRUE;
}
DWORD GetFileSize(HANDLE h, LPDWORD high)
{
	auto* f = (FileHandle*)h;
	if (!f || f->fd < 0) return INVALID_FILE_SIZE;
	struct stat st{};
	if (fstat(f->fd, &st) != 0) return INVALID_FILE_SIZE;
	if (high) *high = (DWORD)(st.st_size >> 32);
	return (DWORD)st.st_size;
}
BOOL GetFileSizeEx(HANDLE h, PLARGE_INTEGER size)
{
	DWORD hi=0; DWORD lo = GetFileSize(h, &hi);
	if (lo == INVALID_FILE_SIZE && GetLastError()) return FALSE;
	if (size) { size->LowPart = lo; size->HighPart = hi; }
	return TRUE;
}

DWORD GetFileAttributesW(LPCWSTR name)
{
	struct stat st{};
	if (stat(toUtf8Path(name).c_str(), &st) != 0) { nppSetLastError(ERROR_FILE_NOT_FOUND); return INVALID_FILE_ATTRIBUTES; }
	DWORD a = FILE_ATTRIBUTE_NORMAL;
	if (S_ISDIR(st.st_mode)) a = FILE_ATTRIBUTE_DIRECTORY;
	if (!(st.st_mode & S_IWUSR)) a |= FILE_ATTRIBUTE_READONLY;
	return a;
}
static void unixToFileTime(time_t t, FILETIME* ft)
{
	ULONGLONG v = ((ULONGLONG)t + 11644473600ULL) * 10000000ULL;
	ft->dwLowDateTime = (DWORD)v;
	ft->dwHighDateTime = (DWORD)(v >> 32);
}
BOOL GetFileAttributesExW(LPCWSTR name, GET_FILEEX_INFO_LEVELS, LPVOID info)
{
	auto* data = (WIN32_FILE_ATTRIBUTE_DATA*)info;
	if (!data) return FALSE;
	DWORD attr = GetFileAttributesW(name);
	if (attr == INVALID_FILE_ATTRIBUTES) return FALSE;
	data->dwFileAttributes = attr;
	struct stat st{};
	if (stat(toUtf8Path(name).c_str(), &st) == 0)
	{
		unixToFileTime(st.st_ctime, &data->ftCreationTime);
		unixToFileTime(st.st_atime, &data->ftLastAccessTime);
		unixToFileTime(st.st_mtime, &data->ftLastWriteTime);
		data->nFileSizeHigh = (DWORD)((unsigned long long)st.st_size >> 32);
		data->nFileSizeLow = (DWORD)st.st_size;
	}
	return TRUE;
}
BOOL SetFileAttributesW(LPCWSTR name, DWORD attr)
{
	std::string p = toUtf8Path(name);
	struct stat st{};
	if (stat(p.c_str(), &st) != 0) return FALSE;
	mode_t m = st.st_mode;
	if (attr & FILE_ATTRIBUTE_READONLY) m &= ~S_IWUSR;
	else m |= S_IWUSR;
	return chmod(p.c_str(), m) == 0;
}

BOOL GetFileTime(HANDLE h, LPFILETIME c, LPFILETIME a, LPFILETIME w)
{
	auto* f = (FileHandle*)h;
	if (!f || f->fd < 0) return FALSE;
	struct stat st{};
	if (fstat(f->fd, &st) != 0) return FALSE;
	if (c) unixToFileTime(st.st_ctime, c);
	if (a) unixToFileTime(st.st_atime, a);
	if (w) unixToFileTime(st.st_mtime, w);
	return TRUE;
}
BOOL SetFileTime(HANDLE, const FILETIME*, const FILETIME*, const FILETIME*) { return TRUE; }
BOOL FlushFileBuffers(HANDLE h)
{
	auto* f = (FileHandle*)h;
	return f && f->fd >= 0 && fsync(f->fd) == 0;
}
BOOL GetFileInformationByHandle(HANDLE h, LPBY_HANDLE_FILE_INFORMATION info)
{
	if (!info) return FALSE;
	memset(info, 0, sizeof(*info));
	info->dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
	DWORD hi=0; info->nFileSizeLow = GetFileSize(h, &hi); info->nFileSizeHigh = hi;
	return TRUE;
}
DWORD GetFinalPathNameByHandleW(HANDLE h, LPWSTR buf, DWORD cch, DWORD)
{
	auto* f = (FileHandle*)h;
	if (!f || !buf) return 0;
	DWORD n = (DWORD)std::min((size_t)cch - 1, f->path.size());
	wmemcpy(buf, f->path.c_str(), n); buf[n]=0;
	return n;
}

static bool globMatch(const char* pat, const char* name)
{
	if (!pat || !*pat || strcmp(pat,"*") == 0) return true;
	// simple * and ?
	const char* p = pat; const char* n = name;
	const char* star = nullptr; const char* nstar = nullptr;
	while (*n)
	{
		if (*p == '*') { star = p++; nstar = n; }
		else if (*p == '?' || *p == *n) { ++p; ++n; }
		else if (star) { p = star + 1; n = ++nstar; }
		else return false;
	}
	while (*p == '*') ++p;
	return *p == 0;
}

HANDLE FindFirstFileW(LPCWSTR name, LPWIN32_FIND_DATAW data)
{
	std::wstring np = nppNormalizePath(name ? name : L"");
	std::string u = nppWideToUtf8(np);
	std::string dir = u, glob = "*";
	auto slash = u.find_last_of('/');
	if (slash != std::string::npos) { dir = u.substr(0, slash); glob = u.substr(slash+1); }
	if (dir.empty()) dir = ".";
	DIR* d = opendir(dir.c_str());
	if (!d) { nppSetLastError(ERROR_PATH_NOT_FOUND); return INVALID_HANDLE_VALUE; }
	auto* ctx = new FindCtx{d, dir, glob};
	if (!FindNextFileW((HANDLE)ctx, data))
	{
		FindClose((HANDLE)ctx);
		nppSetLastError(ERROR_FILE_NOT_FOUND);
		return INVALID_HANDLE_VALUE;
	}
	return (HANDLE)ctx;
}
BOOL FindNextFileW(HANDLE h, LPWIN32_FIND_DATAW data)
{
	auto* ctx = (FindCtx*)h;
	if (!ctx || !ctx->dir) return FALSE;
	struct dirent* ent;
	while ((ent = readdir(ctx->dir)))
	{
		if (!globMatch(ctx->glob.c_str(), ent->d_name)) continue;
		struct stat st{};
		std::string full = ctx->dirPath + "/" + ent->d_name;
		if (stat(full.c_str(), &st) != 0) continue;
		statToFindData(st, ent->d_name, data);
		return TRUE;
	}
	nppSetLastError(ERROR_NO_MORE_FILES);
	return FALSE;
}
BOOL FindClose(HANDLE h)
{
	auto* ctx = (FindCtx*)h;
	if (!ctx) return FALSE;
	if (ctx->dir) closedir(ctx->dir);
	delete ctx;
	return TRUE;
}
HANDLE FindFirstStreamW(LPCWSTR, STREAM_INFO_LEVELS, LPVOID, DWORD)
{
	nppSetLastError(ERROR_HANDLE_EOF);
	return INVALID_HANDLE_VALUE;
}
BOOL FindNextStreamW(HANDLE, LPVOID) { return FALSE; }

BOOL DeleteFileW(LPCWSTR name) { return unlink(toUtf8Path(name).c_str()) == 0; }
BOOL MoveFileW(LPCWSTR from, LPCWSTR to) { return rename(toUtf8Path(from).c_str(), toUtf8Path(to).c_str()) == 0; }
BOOL MoveFileExW(LPCWSTR from, LPCWSTR to, DWORD) { return MoveFileW(from, to); }
BOOL CopyFileW(LPCWSTR from, LPCWSTR to, BOOL failIfExists)
{
	if (failIfExists && GetFileAttributesW(to) != INVALID_FILE_ATTRIBUTES) return FALSE;
	std::ifstream in(toUtf8Path(from), std::ios::binary);
	std::ofstream out(toUtf8Path(to), std::ios::binary);
	out << in.rdbuf();
	return (bool)out;
}
BOOL CreateDirectoryW(LPCWSTR name, LPSECURITY_ATTRIBUTES)
{
	if (mkdir(toUtf8Path(name).c_str(), 0755) == 0) return TRUE;
	if (errno == EEXIST) return TRUE;
	return FALSE;
}
BOOL RemoveDirectoryW(LPCWSTR name) { return rmdir(toUtf8Path(name).c_str()) == 0; }
DWORD GetCurrentDirectoryW(DWORD n, LPWSTR buf)
{
	char cwd[4096];
	if (!getcwd(cwd, sizeof(cwd))) return 0;
	std::wstring w = nppUtf8ToWide(cwd);
	if (!buf || n == 0) return (DWORD)w.size() + 1;
	DWORD c = (DWORD)std::min((size_t)n - 1, w.size());
	wmemcpy(buf, w.c_str(), c); buf[c]=0;
	return c;
}
BOOL SetCurrentDirectoryW(LPCWSTR path) { return chdir(toUtf8Path(path).c_str()) == 0; }
DWORD GetFullPathNameW(LPCWSTR file, DWORD n, LPWSTR buf, LPWSTR* filePart)
{
	std::wstring np = nppNormalizePath(file ? file : L"");
	if (!np.empty() && np[0] != L'/')
	{
		wchar_t cwd[4096];
		GetCurrentDirectoryW(4096, cwd);
		np = std::wstring(cwd) + L"/" + np;
	}
	if (!buf || n == 0) return (DWORD)np.size() + 1;
	DWORD c = (DWORD)std::min((size_t)n - 1, np.size());
	wmemcpy(buf, np.c_str(), c); buf[c]=0;
	if (filePart)
	{
		wchar_t* slash = wcsrchr(buf, L'/');
		*filePart = slash ? slash + 1 : buf;
	}
	return c;
}
DWORD GetTempPathW(DWORD n, LPWSTR buf)
{
	const char* t = getenv("TMPDIR");
	if (!t) t = "/tmp";
	std::wstring w = nppUtf8ToWide(t) + L"/";
	DWORD c = (DWORD)std::min((size_t)n - 1, w.size());
	if (buf && n) { wmemcpy(buf, w.c_str(), c); buf[c]=0; }
	return (DWORD)w.size();
}
UINT GetTempFileNameW(LPCWSTR path, LPCWSTR prefix, UINT unique, LPWSTR buf)
{
	std::wstring p = nppNormalizePath(path ? path : L"/tmp");
	std::wstring pre = prefix ? prefix : L"npp";
	if (unique == 0) unique = (UINT)(getpid() ^ (UINT)time(nullptr));
	wchar_t name[MAX_PATH];
	swprintf(name, MAX_PATH, L"%s/%s%x.tmp", p.c_str(), pre.c_str(), unique);
	if (buf) wcscpy(buf, name);
	int fd = open(toUtf8Path(name).c_str(), O_CREAT | O_EXCL | O_WRONLY, 0644);
	if (fd >= 0) close(fd);
	return unique;
}

DWORD GetModuleFileNameW(HMODULE, LPWSTR buf, DWORD n)
{
	char path[4096]{};
	ssize_t r = readlink("/proc/self/exe", path, sizeof(path)-1);
	if (r < 0) { std::snprintf(path, sizeof(path), "/usr/bin/notepad++"); r = strlen(path); }
	std::wstring w = nppUtf8ToWide(std::string(path, r));
	DWORD c = (DWORD)std::min((size_t)n - 1, w.size());
	if (buf && n) { wmemcpy(buf, w.c_str(), c); buf[c]=0; }
	return c;
}
DWORD GetModuleFileNameA(HMODULE m, LPSTR buf, DWORD n)
{
	wchar_t w[4096];
	DWORD c = GetModuleFileNameW(m, w, 4096);
	std::string u = nppWideToUtf8(w);
	if (buf && n) std::snprintf(buf, n, "%s", u.c_str());
	return std::min(n ? n-1 : 0, (DWORD)u.size());
}
HMODULE GetModuleHandleW(LPCWSTR) { return (HMODULE)1; }
HMODULE GetModuleHandleA(LPCSTR) { return (HMODULE)1; }
BOOL GetModuleHandleExW(DWORD, LPCWSTR, HMODULE* mod) { if (mod) *mod = (HMODULE)1; return TRUE; }
HMODULE LoadLibraryW(LPCWSTR name)
{
	void* h = dlopen(toUtf8Path(name).c_str(), RTLD_NOW);
	if (!h) nppSetLastError(ERROR_MOD_NOT_FOUND);
	return (HMODULE)h;
}
HMODULE LoadLibraryA(LPCSTR name) { return (HMODULE)dlopen(name, RTLD_NOW); }
HMODULE LoadLibraryExW(LPCWSTR name, HANDLE, DWORD) { return LoadLibraryW(name); }
BOOL FreeLibrary(HMODULE mod) { if (mod && mod != (HMODULE)1) dlclose(mod); return TRUE; }
FARPROC GetProcAddress(HMODULE mod, LPCSTR name)
{
	if (!mod || mod == (HMODULE)1) return nullptr;
	return (FARPROC)dlsym(mod, name);
}

int MultiByteToWideChar(UINT, DWORD, LPCSTR src, int cb, LPWSTR dst, int cch)
{
	if (!src) return 0;
	if (cb < 0) cb = (int)strlen(src);
	std::wstring w = nppUtf8ToWide(std::string(src, src+cb));
	if (cch == 0) return (int)w.size() + 1;
	if (!dst) return 0;
	int n = std::min(cch, (int)w.size());
	wmemcpy(dst, w.c_str(), n);
	if (n < cch) dst[n] = 0;
	return n;
}
int WideCharToMultiByte(UINT, DWORD, LPCWSTR src, int cch, LPSTR dst, int cb, LPCSTR, LPBOOL)
{
	if (!src) return 0;
	if (cch < 0) cch = (int)wcslen(src);
	std::string u = nppWideToUtf8(std::wstring(src, src+cch));
	if (cb == 0) return (int)u.size() + 1;
	if (!dst) return 0;
	int n = std::min(cb, (int)u.size());
	memcpy(dst, u.data(), n);
	if (n < cb) dst[n] = 0;
	return n;
}

int lstrlenW(LPCWSTR s) { return s ? (int)wcslen(s) : 0; }
int lstrlenA(LPCSTR s) { return s ? (int)strlen(s) : 0; }
LPWSTR lstrcpyW(LPWSTR dst, LPCWSTR src) { return wcscpy(dst, src ? src : L""); }
LPWSTR lstrcpynW(LPWSTR dst, LPCWSTR src, int max)
{
	if (!dst || max <= 0) return dst;
	wcsncpy(dst, src ? src : L"", max-1); dst[max-1]=0; return dst;
}
LPWSTR lstrcatW(LPWSTR dst, LPCWSTR src) { return wcscat(dst, src ? src : L""); }
int lstrcmpW(LPCWSTR a, LPCWSTR b) { return wcscmp(a?a:L"", b?b:L""); }
int lstrcmpiW(LPCWSTR a, LPCWSTR b)
{
	std::wstring wa=a?a:L"", wb=b?b:L"";
	for (auto& c: wa) c = towlower(c);
	for (auto& c: wb) c = towlower(c);
	return wa.compare(wb);
}
int CompareStringW(LCID, DWORD flags, LPCWSTR a, int na, LPCWSTR b, int nb)
{
	std::wstring wa(a?a:L"", a && na<0 ? wcslen(a) : (size_t)std::max(na,0));
	std::wstring wb(b?b:L"", b && nb<0 ? wcslen(b) : (size_t)std::max(nb,0));
	int r = (flags & NORM_IGNORECASE) ? lstrcmpiW(wa.c_str(), wb.c_str()) : lstrcmpW(wa.c_str(), wb.c_str());
	return r < 0 ? CSTR_LESS_THAN : (r > 0 ? CSTR_GREATER_THAN : CSTR_EQUAL);
}
int CompareStringOrdinal(LPCWCH a, int na, LPCWCH b, int nb, BOOL ignoreCase)
{
	return CompareStringW(0, ignoreCase?NORM_IGNORECASE:0, a, na, b, nb);
}
LPWSTR CharUpperW(LPWSTR s) { for (wchar_t* p=s; p && *p; ++p) *p = towupper(*p); return s; }
LPWSTR CharLowerW(LPWSTR s) { for (wchar_t* p=s; p && *p; ++p) *p = towlower(*p); return s; }
DWORD CharUpperBuffW(LPWSTR s, DWORD n) { for (DWORD i=0;i<n && s;++i) s[i]=towupper(s[i]); return n; }
DWORD CharLowerBuffW(LPWSTR s, DWORD n) { for (DWORD i=0;i<n && s;++i) s[i]=towlower(s[i]); return n; }
BOOL IsCharAlphaW(WCHAR ch) { return iswalpha(ch); }
BOOL IsCharAlphaNumericW(WCHAR ch) { return iswalnum(ch); }
BOOL IsCharUpperW(WCHAR ch) { return iswupper(ch); }
BOOL IsCharLowerW(WCHAR ch) { return iswlower(ch); }
UINT GetACP(void) { return 65001; }
UINT GetOEMCP(void) { return 65001; }
BOOL GetCPInfo(UINT, LPCPINFO info) { if(info){ info->MaxCharSize=4; info->DefaultChar[0]='?'; } return TRUE; }
BOOL IsDBCSLeadByte(BYTE) { return FALSE; }
BOOL IsDBCSLeadByteEx(UINT, BYTE) { return FALSE; }
int GetLocaleInfoW(LCID, LCTYPE type, LPWSTR buf, int cch)
{
	const wchar_t* v = L"en-US";
	if (type == LOCALE_SISO639LANGNAME) v = L"en";
	if (!buf || cch==0) return (int)wcslen(v)+1;
	wcsncpy(buf, v, cch-1); buf[cch-1]=0; return (int)wcslen(buf)+1;
}
int GetLocaleInfoEx(LPCWSTR, LCTYPE type, LPWSTR buf, int cch)
{
	if (type & LOCALE_RETURN_NUMBER)
	{
		DWORD val = 65001;
		LCTYPE kind = type & ~LOCALE_RETURN_NUMBER;
		if (kind == LOCALE_IDEFAULTANSICODEPAGE)
			val = 65001;
		const int n = (int)(sizeof(DWORD) / sizeof(WCHAR));
		if (!buf || cch == 0) return n;
		if (cch < n) return 0;
		std::memcpy(buf, &val, sizeof(val));
		return n;
	}
	return GetLocaleInfoW(0, type, buf, cch);
}
LCID GetUserDefaultLCID(void) { return 0x0409; }
LANGID GetUserDefaultLangID(void) { return 0x0409; }
LANGID GetSystemDefaultLangID(void) { return 0x0409; }
LCID GetSystemDefaultLCID(void) { return 0x0409; }
int LCMapStringW(LCID, DWORD flags, LPCWSTR src, int cchSrc, LPWSTR dst, int cchDst)
{
	if (!src) return 0;
	if (cchSrc < 0) cchSrc = (int)wcslen(src);
	if (!dst || cchDst==0) return cchSrc;
	int n = std::min(cchSrc, cchDst);
	wmemcpy(dst, src, n);
	if (flags & LCMAP_UPPERCASE) CharUpperBuffW(dst, n);
	if (flags & LCMAP_LOWERCASE) CharLowerBuffW(dst, n);
	return n;
}
BOOL IsValidCodePage(UINT) { return TRUE; }
BOOL IsValidLocale(LCID, DWORD) { return TRUE; }
int GetNumberFormatW(LCID, DWORD, LPCWSTR value, const NUMBERFMTW*, LPWSTR buf, int cch)
{
	if (!buf) return 0;
	wcsncpy(buf, value?value:L"0", cch-1); return (int)wcslen(buf);
}
int GetCurrencyFormatW(LCID, DWORD, LPCWSTR value, const CURRENCYFMTW*, LPWSTR buf, int cch)
{
	return GetNumberFormatW(0,0,value,nullptr,buf,cch);
}

DWORD FormatMessageW(DWORD flags, LPCVOID src, DWORD id, DWORD, LPWSTR buf, DWORD size, va_list*)
{
	std::wstring msg = L"Error ";
	msg += std::to_wstring(id);
	if (flags & FORMAT_MESSAGE_ALLOCATE_BUFFER)
	{
		auto* p = (LPWSTR*)buf;
		*p = (LPWSTR)malloc((msg.size()+1)*sizeof(wchar_t));
		wcscpy(*p, msg.c_str());
		return (DWORD)msg.size();
	}
	if (buf && size) { wcsncpy(buf, msg.c_str(), size-1); buf[size-1]=0; }
	return (DWORD)std::min((size_t)size, msg.size());
}
DWORD FormatMessageA(DWORD flags, LPCVOID src, DWORD id, DWORD lang, LPSTR buf, DWORD size, va_list* a)
{
	wchar_t w[512];
	FormatMessageW(flags, src, id, lang, w, 512, a);
	std::string u = nppWideToUtf8(w);
	if (buf && size) snprintf(buf, size, "%s", u.c_str());
	return (DWORD)u.size();
}

struct ThreadWrap { LPTHREAD_START_ROUTINE start; LPVOID param; pthread_t th; DWORD id; };
static void* threadEntry(void* p)
{
	auto* t = (ThreadWrap*)p;
	t->start(t->param);
	return nullptr;
}
HANDLE CreateThread(LPSECURITY_ATTRIBUTES, SIZE_T, LPTHREAD_START_ROUTINE start, LPVOID param, DWORD, LPDWORD id)
{
	auto* t = new ThreadWrap{start, param, {}, 0};
	pthread_create(&t->th, nullptr, threadEntry, t);
	t->id = (DWORD)t->th;
	if (id) *id = t->id;
	return (HANDLE)t;
}
void ExitThread(DWORD) { pthread_exit(nullptr); }
HANDLE GetCurrentThread(void) { return (HANDLE)(uintptr_t)pthread_self(); }
DWORD GetCurrentThreadId(void) { return (DWORD)pthread_self(); }
HANDLE GetCurrentProcess(void) { return (HANDLE)(uintptr_t)getpid(); }
DWORD GetCurrentProcessId(void) { return (DWORD)getpid(); }
BOOL TerminateProcess(HANDLE, UINT code) { _exit(code); return TRUE; }
BOOL GetExitCodeProcess(HANDLE, LPDWORD code) { if(code)*code=0; return TRUE; }
BOOL GetExitCodeThread(HANDLE, LPDWORD code) { if(code)*code=0; return TRUE; }

struct EventHandle { pthread_mutex_t m; pthread_cond_t c; bool signaled; bool manual; };
HANDLE CreateEventW(LPSECURITY_ATTRIBUTES, BOOL manual, BOOL initial, LPCWSTR)
{
	auto* e = new EventHandle{};
	pthread_mutex_init(&e->m, nullptr);
	pthread_cond_init(&e->c, nullptr);
	e->manual = manual;
	e->signaled = initial;
	return (HANDLE)e;
}
BOOL SetEvent(HANDLE h)
{
	auto* e = (EventHandle*)h;
	pthread_mutex_lock(&e->m); e->signaled = true; pthread_cond_broadcast(&e->c); pthread_mutex_unlock(&e->m);
	return TRUE;
}
BOOL ResetEvent(HANDLE h)
{
	auto* e = (EventHandle*)h;
	pthread_mutex_lock(&e->m); e->signaled = false; pthread_mutex_unlock(&e->m);
	return TRUE;
}
BOOL PulseEvent(HANDLE h) { SetEvent(h); ResetEvent(h); return TRUE; }
DWORD WaitForSingleObject(HANDLE h, DWORD ms)
{
	auto* e = (EventHandle*)h;
	pthread_mutex_lock(&e->m);
	if (ms == 0) { bool s=e->signaled; pthread_mutex_unlock(&e->m); return s?WAIT_OBJECT_0:WAIT_TIMEOUT; }
	if (ms == INFINITE)
	{
		while (!e->signaled) pthread_cond_wait(&e->c, &e->m);
	}
	else
	{
		timespec ts{}; clock_gettime(CLOCK_REALTIME, &ts);
		ts.tv_sec += ms/1000; ts.tv_nsec += (ms%1000)*1000000L;
		if (ts.tv_nsec >= 1000000000L) { ts.tv_sec++; ts.tv_nsec -= 1000000000L; }
		int rc=0;
		while (!e->signaled && rc==0) rc = pthread_cond_timedwait(&e->c, &e->m, &ts);
		if (!e->signaled) { pthread_mutex_unlock(&e->m); return WAIT_TIMEOUT; }
	}
	if (!e->manual) e->signaled = false;
	pthread_mutex_unlock(&e->m);
	return WAIT_OBJECT_0;
}
DWORD WaitForMultipleObjects(DWORD count, const HANDLE* handles, BOOL waitAll, DWORD ms)
{
	if (waitAll)
	{
		for (DWORD i=0;i<count;++i) if (WaitForSingleObject(handles[i], ms) != WAIT_OBJECT_0) return WAIT_TIMEOUT;
		return WAIT_OBJECT_0;
	}
	for (DWORD i=0;i<count;++i) if (WaitForSingleObject(handles[i], 0) == WAIT_OBJECT_0) return WAIT_OBJECT_0+i;
	if (count) return WaitForSingleObject(handles[0], ms);
	return WAIT_FAILED;
}
HANDLE CreateMutexW(LPSECURITY_ATTRIBUTES, BOOL, LPCWSTR) { return CreateEventW(nullptr, FALSE, TRUE, nullptr); }
BOOL ReleaseMutex(HANDLE h) { return SetEvent(h); }
HANDLE CreateSemaphoreW(LPSECURITY_ATTRIBUTES, LONG initial, LONG, LPCWSTR)
{
	return CreateEventW(nullptr, FALSE, initial>0, nullptr);
}
BOOL ReleaseSemaphore(HANDLE h, LONG, LPLONG) { return SetEvent(h); }

void InitializeCriticalSection(LPCRITICAL_SECTION cs)
{
	if (!cs) return;
	auto* m = new pthread_mutex_t;
	pthread_mutex_init(m, nullptr);
	cs->posixMutex = m;
}
BOOL InitializeCriticalSectionAndSpinCount(LPCRITICAL_SECTION cs, DWORD) { InitializeCriticalSection(cs); return TRUE; }
void EnterCriticalSection(LPCRITICAL_SECTION cs) { if (cs && cs->posixMutex) pthread_mutex_lock((pthread_mutex_t*)cs->posixMutex); }
void LeaveCriticalSection(LPCRITICAL_SECTION cs) { if (cs && cs->posixMutex) pthread_mutex_unlock((pthread_mutex_t*)cs->posixMutex); }
void DeleteCriticalSection(LPCRITICAL_SECTION cs)
{
	if (!cs || !cs->posixMutex) return;
	auto* m = (pthread_mutex_t*)cs->posixMutex;
	pthread_mutex_destroy(m);
	delete m;
	cs->posixMutex = nullptr;
}
BOOL TryEnterCriticalSection(LPCRITICAL_SECTION cs)
{
	if (!cs || !cs->posixMutex) return TRUE;
	return pthread_mutex_trylock((pthread_mutex_t*)cs->posixMutex) == 0;
}
DWORD TlsAlloc(void) { static DWORD n=1; return n < 64 ? n++ : TLS_OUT_OF_INDEXES; }
BOOL TlsFree(DWORD) { return TRUE; }
LPVOID TlsGetValue(DWORD idx) { return idx<64 ? g_tlsVal[idx] : nullptr; }
BOOL TlsSetValue(DWORD idx, LPVOID val) { if (idx>=64) return FALSE; g_tlsVal[idx]=val; return TRUE; }

BOOL DuplicateHandle(HANDLE, HANDLE src, HANDLE, LPHANDLE dst, DWORD, BOOL, DWORD) { if(dst)*dst=src; return TRUE; }
BOOL SetHandleInformation(HANDLE, DWORD, DWORD) { return TRUE; }
BOOL GetHandleInformation(HANDLE, LPDWORD flags) { if(flags)*flags=0; return TRUE; }
DWORD ResumeThread(HANDLE) { return 0; }
DWORD SuspendThread(HANDLE) { return 0; }
BOOL SetThreadPriority(HANDLE, int) { return TRUE; }
int GetThreadPriority(HANDLE) { return 0; }
void ExitProcess(UINT code) { _exit(code); }
BOOL QueryPerformanceCounter(LARGE_INTEGER* li)
{
	timespec ts{}; clock_gettime(CLOCK_MONOTONIC, &ts);
	if (li) li->QuadPart = (LONGLONG)ts.tv_sec * 1000000000LL + ts.tv_nsec;
	return TRUE;
}
BOOL QueryPerformanceFrequency(LARGE_INTEGER* li) { if (li) li->QuadPart = 1000000000LL; return TRUE; }
BOOL GetVersionExW(LPOSVERSIONINFOW vi)
{
	if (!vi) return FALSE;
	vi->dwMajorVersion = 10; vi->dwMinorVersion = 0; vi->dwBuildNumber = 19045; vi->dwPlatformId = 2;
	wcscpy(vi->szCSDVersion, L"Unix");
	return TRUE;
}
DWORD GetVersion(void) { return 0x0A000000; }
void GetSystemInfo(LPSYSTEM_INFO si)
{
	if (!si) return;
	memset(si, 0, sizeof(*si));
	si->dwPageSize = 4096;
	si->dwNumberOfProcessors = (DWORD)sysconf(_SC_NPROCESSORS_ONLN);
	si->dwProcessorType = 8664;
}
BOOL GetComputerNameW(LPWSTR buf, LPDWORD n)
{
	char h[256]{}; gethostname(h, sizeof(h));
	std::wstring w = nppUtf8ToWide(h);
	if (buf && n) { wcsncpy(buf, w.c_str(), *n); }
	if (n) *n = (DWORD)w.size()+1;
	return TRUE;
}
BOOL GetUserNameW(LPWSTR buf, LPDWORD n)
{
	const char* u = getenv("USER"); if (!u) u = "user";
	std::wstring w = nppUtf8ToWide(u);
	if (buf && n) wcsncpy(buf, w.c_str(), *n);
	if (n) *n = (DWORD)w.size()+1;
	return TRUE;
}
UINT GetWindowsDirectoryW(LPWSTR buf, UINT n) { if(buf&&n) wcsncpy(buf, L"/usr", n); return 4; }
UINT GetSystemDirectoryW(LPWSTR buf, UINT n) { if(buf&&n) wcsncpy(buf, L"/usr/lib", n); return 8; }
DWORD GetEnvironmentVariableW(LPCWSTR name, LPWSTR buf, DWORD n)
{
	std::string key = nppWideToUtf8(name?name:L"");
	const char* v = getenv(key.c_str());
	if (!v) return 0;
	std::wstring w = nppUtf8ToWide(v);
	if (!buf || n==0) return (DWORD)w.size()+1;
	wcsncpy(buf, w.c_str(), n-1); buf[n-1]=0;
	return (DWORD)w.size();
}
BOOL SetEnvironmentVariableW(LPCWSTR name, LPCWSTR value)
{
	std::string k = nppWideToUtf8(name?name:L"");
	if (!value) { unsetenv(k.c_str()); return TRUE; }
	return setenv(k.c_str(), nppWideToUtf8(value).c_str(), 1)==0;
}
LPWSTR GetEnvironmentStringsW(void)
{
	static wchar_t empty[] = L"\0";
	return empty;
}
BOOL FreeEnvironmentStringsW(LPWSTR) { return TRUE; }
DWORD ExpandEnvironmentStringsW(LPCWSTR src, LPWSTR dst, DWORD n)
{
	if (!src) return 0;
	if (dst && n) { wcsncpy(dst, src, n-1); dst[n-1]=0; }
	return (DWORD)wcslen(src)+1;
}
BOOL CreateProcessW(LPCWSTR app, LPWSTR cmd, LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES,
	BOOL, DWORD, LPVOID, LPCWSTR, LPSTARTUPINFOW, LPPROCESS_INFORMATION pi)
{
	std::string c = nppWideToUtf8(cmd ? cmd : (app ? app : L""));
	int r = system(c.c_str());
	if (pi) { memset(pi, 0, sizeof(*pi)); pi->dwProcessId = (DWORD)r; }
	return r >= 0;
}

LPVOID HeapAlloc(HANDLE, DWORD flags, SIZE_T bytes)
{
	void* p = malloc(bytes);
	if (p && (flags & HEAP_ZERO_MEMORY)) memset(p, 0, bytes);
	return p;
}
LPVOID HeapReAlloc(HANDLE, DWORD, LPVOID mem, SIZE_T bytes) { return realloc(mem, bytes); }
BOOL HeapFree(HANDLE, DWORD, LPVOID mem) { free(mem); return TRUE; }
SIZE_T HeapSize(HANDLE, DWORD, LPCVOID) { return 0; }
HANDLE GetProcessHeap(void) { return (HANDLE)1; }
HANDLE HeapCreate(DWORD, SIZE_T, SIZE_T) { return (HANDLE)1; }
BOOL HeapDestroy(HANDLE) { return TRUE; }
HGLOBAL GlobalAlloc(UINT flags, SIZE_T bytes)
{
	SIZE_T* p = (SIZE_T*)malloc(bytes + sizeof(SIZE_T));
	if (!p) return nullptr;
	*p = bytes;
	void* d = p+1;
	if (flags & GMEM_ZEROINIT) memset(d, 0, bytes);
	return (HGLOBAL)d;
}
HGLOBAL GlobalReAlloc(HGLOBAL h, SIZE_T bytes, UINT)
{
	if (!h) return GlobalAlloc(0, bytes);
	SIZE_T* p = (SIZE_T*)h - 1;
	p = (SIZE_T*)realloc(p, bytes + sizeof(SIZE_T));
	if (!p) return nullptr;
	*p = bytes;
	return (HGLOBAL)(p+1);
}
HGLOBAL GlobalFree(HGLOBAL h) { if (h) free((SIZE_T*)h - 1); return nullptr; }
LPVOID GlobalLock(HGLOBAL h) { return h; }
BOOL GlobalUnlock(HGLOBAL) { return TRUE; }
SIZE_T GlobalSize(HGLOBAL h) { return h ? *((SIZE_T*)h - 1) : 0; }
UINT GlobalFlags(HGLOBAL) { return 0; }
HLOCAL LocalAlloc(UINT f, SIZE_T b) { return GlobalAlloc(f,b); }
HLOCAL LocalReAlloc(HLOCAL h, SIZE_T b, UINT f) { return GlobalReAlloc(h,b,f); }
HLOCAL LocalFree(HLOCAL h) { return GlobalFree(h); }
LPVOID LocalLock(HLOCAL h) { return h; }
BOOL LocalUnlock(HLOCAL) { return TRUE; }
SIZE_T LocalSize(HLOCAL h) { return GlobalSize(h); }
LPVOID VirtualAlloc(LPVOID addr, SIZE_T size, DWORD, DWORD)
{
	void* p = mmap(addr, size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
	return p == MAP_FAILED ? nullptr : p;
}
BOOL VirtualFree(LPVOID addr, SIZE_T size, DWORD) { if (addr) munmap(addr, size?size:4096); return TRUE; }
BOOL VirtualProtect(LPVOID, SIZE_T, DWORD, PDWORD old) { if(old)*old=PAGE_READWRITE; return TRUE; }
SIZE_T VirtualQuery(LPCVOID, PMEMORY_BASIC_INFORMATION info, SIZE_T len)
{
	if (info && len >= sizeof(*info)) { memset(info,0,sizeof(*info)); info->Protect = PAGE_READWRITE; info->State = MEM_COMMIT; }
	return sizeof(*info);
}

void GetLocalTime(LPSYSTEMTIME st)
{
	time_t t = time(nullptr);
	struct tm tm{}; localtime_r(&t, &tm);
	st->wYear = tm.tm_year+1900; st->wMonth=tm.tm_mon+1; st->wDay=tm.tm_mday;
	st->wHour=tm.tm_hour; st->wMinute=tm.tm_min; st->wSecond=tm.tm_sec; st->wMilliseconds=0;
	st->wDayOfWeek=tm.tm_wday;
}
void GetSystemTime(LPSYSTEMTIME st)
{
	time_t t = time(nullptr);
	struct tm tm{}; gmtime_r(&t, &tm);
	st->wYear = tm.tm_year+1900; st->wMonth=tm.tm_mon+1; st->wDay=tm.tm_mday;
	st->wHour=tm.tm_hour; st->wMinute=tm.tm_min; st->wSecond=tm.tm_sec; st->wMilliseconds=0;
	st->wDayOfWeek=tm.tm_wday;
}
BOOL SystemTimeToFileTime(const SYSTEMTIME* st, LPFILETIME ft)
{
	struct tm tm{};
	tm.tm_year=st->wYear-1900; tm.tm_mon=st->wMonth-1; tm.tm_mday=st->wDay;
	tm.tm_hour=st->wHour; tm.tm_min=st->wMinute; tm.tm_sec=st->wSecond;
	unixToFileTime(timegm(&tm), ft);
	return TRUE;
}
BOOL FileTimeToSystemTime(const FILETIME* ft, LPSYSTEMTIME st)
{
	ULONGLONG v = ((ULONGLONG)ft->dwHighDateTime<<32) | ft->dwLowDateTime;
	time_t t = (time_t)(v / 10000000ULL - 11644473600ULL);
	struct tm tm{}; gmtime_r(&t, &tm);
	st->wYear=tm.tm_year+1900; st->wMonth=tm.tm_mon+1; st->wDay=tm.tm_mday;
	st->wHour=tm.tm_hour; st->wMinute=tm.tm_min; st->wSecond=tm.tm_sec; st->wMilliseconds=0;
	st->wDayOfWeek=tm.tm_wday;
	return TRUE;
}
BOOL FileTimeToLocalFileTime(const FILETIME* ft, LPFILETIME lft) { *lft=*ft; return TRUE; }
BOOL LocalFileTimeToFileTime(const FILETIME* lft, LPFILETIME ft) { *ft=*lft; return TRUE; }
DWORD GetTimeZoneInformation(LPTIME_ZONE_INFORMATION tz)
{
	if (tz) memset(tz, 0, sizeof(*tz));
	return TIME_ZONE_ID_UNKNOWN;
}
int GetDateFormatW(LCID, DWORD, const SYSTEMTIME* st, LPCWSTR, LPWSTR buf, int cch)
{
	SYSTEMTIME local;
	if (!st) { GetLocalTime(&local); st=&local; }
	wchar_t tmp[64];
	swprintf(tmp, 64, L"%04d-%02d-%02d", st->wYear, st->wMonth, st->wDay);
	if (!buf || cch==0) return (int)wcslen(tmp)+1;
	wcsncpy(buf, tmp, cch-1); return (int)wcslen(buf);
}
int GetTimeFormatW(LCID, DWORD, const SYSTEMTIME* st, LPCWSTR, LPWSTR buf, int cch)
{
	SYSTEMTIME local;
	if (!st) { GetLocalTime(&local); st=&local; }
	wchar_t tmp[64];
	swprintf(tmp, 64, L"%02d:%02d:%02d", st->wHour, st->wMinute, st->wSecond);
	if (!buf || cch==0) return (int)wcslen(tmp)+1;
	wcsncpy(buf, tmp, cch-1); return (int)wcslen(buf);
}
LONG CompareFileTime(const FILETIME* a, const FILETIME* b)
{
	if (!a||!b) return 0;
	if (a->dwHighDateTime != b->dwHighDateTime) return a->dwHighDateTime < b->dwHighDateTime ? -1 : 1;
	if (a->dwLowDateTime != b->dwLowDateTime) return a->dwLowDateTime < b->dwLowDateTime ? -1 : 1;
	return 0;
}
BOOL DosDateTimeToFileTime(WORD, WORD, LPFILETIME ft) { if(ft) ft->dwLowDateTime=ft->dwHighDateTime=0; return TRUE; }
BOOL FileTimeToDosDateTime(const FILETIME*, LPWORD d, LPWORD t) { if(d)*d=0; if(t)*t=0; return TRUE; }
void GetSystemTimeAsFileTime(LPFILETIME ft) { SYSTEMTIME st; GetSystemTime(&st); SystemTimeToFileTime(&st, ft); }
BOOL SystemTimeToTzSpecificLocalTime(const TIME_ZONE_INFORMATION*, const SYSTEMTIME* st, LPSYSTEMTIME local)
{ if(local&&st)*local=*st; return TRUE; }

DWORD GetLogicalDrives(void) { return 4; } // C:
UINT GetDriveTypeW(LPCWSTR) { return DRIVE_FIXED; }
BOOL GetVolumeInformationW(LPCWSTR, LPWSTR vol, DWORD volCch, LPDWORD serial, LPDWORD maxComp, LPDWORD flags, LPWSTR fs, DWORD fsCch)
{
	if (vol && volCch) wcsncpy(vol, L"/", volCch);
	if (serial) *serial = 0;
	if (maxComp) *maxComp = 255;
	if (flags) *flags = 0;
	if (fs && fsCch) wcsncpy(fs, L"ext4", fsCch);
	return TRUE;
}
BOOL GetDiskFreeSpaceExW(LPCWSTR, PULARGE_INTEGER freeCaller, PULARGE_INTEGER total, PULARGE_INTEGER free)
{
	if (freeCaller) freeCaller->QuadPart = 1ULL<<40;
	if (total) total->QuadPart = 1ULL<<40;
	if (free) free->QuadPart = 1ULL<<40;
	return TRUE;
}
DWORD GetLongPathNameW(LPCWSTR s, LPWSTR d, DWORD n) { return GetFullPathNameW(s,n,d,nullptr); }
DWORD GetShortPathNameW(LPCWSTR s, LPWSTR d, DWORD n) { return GetFullPathNameW(s,n,d,nullptr); }
BOOL SetEndOfFile(HANDLE h)
{
	auto* f = (FileHandle*)h;
	if (!f || f->fd<0) return FALSE;
	off_t pos = lseek(f->fd, 0, SEEK_CUR);
	return ftruncate(f->fd, pos)==0;
}
BOOL DeviceIoControl(HANDLE, DWORD, LPVOID, DWORD, LPVOID, DWORD, LPDWORD, LPOVERLAPPED) { return FALSE; }
BOOL LockFile(HANDLE, DWORD, DWORD, DWORD, DWORD) { return TRUE; }
BOOL UnlockFile(HANDLE, DWORD, DWORD, DWORD, DWORD) { return TRUE; }
BOOL LockFileEx(HANDLE, DWORD, DWORD, DWORD, DWORD, LPOVERLAPPED) { return TRUE; }
BOOL UnlockFileEx(HANDLE, DWORD, DWORD, DWORD, LPOVERLAPPED) { return TRUE; }
BOOL ReadFileEx(HANDLE h, LPVOID b, DWORD n, LPOVERLAPPED, LPOVERLAPPED_COMPLETION_ROUTINE) { DWORD r; return ReadFile(h,b,n,&r,nullptr); }
BOOL WriteFileEx(HANDLE h, LPCVOID b, DWORD n, LPOVERLAPPED, LPOVERLAPPED_COMPLETION_ROUTINE) { DWORD r; return WriteFile(h,b,n,&r,nullptr); }
HANDLE CreateFileMappingW(HANDLE, LPSECURITY_ATTRIBUTES, DWORD, DWORD, DWORD maxLow, LPCWSTR)
{
	return (HANDLE)(uintptr_t)maxLow;
}
LPVOID MapViewOfFile(HANDLE, DWORD, DWORD, DWORD, SIZE_T n) { return VirtualAlloc(nullptr, n?n:4096, 0, 0); }
BOOL UnmapViewOfFile(LPCVOID base) { return VirtualFree((LPVOID)base, 0, 0); }
BOOL FlushViewOfFile(LPCVOID, SIZE_T) { return TRUE; }

int wsprintfW(LPWSTR buf, LPCWSTR fmt, ...)
{
	va_list ap; va_start(ap, fmt);
	int r = vswprintf(buf, 4096, fmt, ap);
	va_end(ap);
	return r;
}
int wvsprintfW(LPWSTR buf, LPCWSTR fmt, va_list args) { return vswprintf(buf, 4096, fmt, args); }
int wsprintfA(LPSTR buf, LPCSTR fmt, ...)
{
	va_list ap; va_start(ap, fmt);
	int r = vsprintf(buf, fmt, ap);
	va_end(ap);
	return r;
}
int wvsprintfA(LPSTR buf, LPCSTR fmt, va_list args) { return vsprintf(buf, fmt, args); }

DWORD timeGetTime(void) { return GetTickCount(); }
MMRESULT timeBeginPeriod(UINT) { return 0; }
MMRESULT timeEndPeriod(UINT) { return 0; }
BOOL GetProcessTimes(HANDLE, LPFILETIME a, LPFILETIME b, LPFILETIME c, LPFILETIME d)
{ FILETIME z{}; if(a)*a=z; if(b)*b=z; if(c)*c=z; if(d)*d=z; return TRUE; }
BOOL GetSystemTimes(LPFILETIME a, LPFILETIME b, LPFILETIME c) { FILETIME z{}; if(a)*a=z; if(b)*b=z; if(c)*c=z; return TRUE; }
BOOL GlobalMemoryStatusEx(LPMEMORYSTATUSEX buf)
{
	if (!buf) return FALSE;
	buf->dwMemoryLoad = 50;
	buf->ullTotalPhys = 8ULL<<30;
	buf->ullAvailPhys = 4ULL<<30;
	buf->ullTotalVirtual = 1ULL<<40;
	buf->ullAvailVirtual = 1ULL<<40;
	return TRUE;
}
UINT SetErrorMode(UINT) { return 0; }
DWORD GetFileType(HANDLE) { return FILE_TYPE_DISK; }
BOOL PeekNamedPipe(HANDLE, LPVOID, DWORD, LPDWORD, LPDWORD avail, LPDWORD) { if(avail)*avail=0; return TRUE; }
BOOL CreatePipe(PHANDLE read, PHANDLE write, LPSECURITY_ATTRIBUTES, DWORD)
{
	int fds[2];
	if (pipe(fds)!=0) return FALSE;
	auto* r = new FileHandle(); r->fd = fds[0];
	auto* w = new FileHandle(); w->fd = fds[1];
	if (read) *read = r;
	if (write) *write = w;
	return TRUE;
}
DWORD SearchPathW(LPCWSTR, LPCWSTR file, LPCWSTR, DWORD bufLen, LPWSTR buf, LPWSTR*)
{
	return GetFullPathNameW(file, bufLen, buf, nullptr);
}
DWORD GetDllDirectoryW(DWORD n, LPWSTR buf) { if(buf&&n) buf[0]=0; return 0; }
BOOL SetDllDirectoryW(LPCWSTR) { return TRUE; }
BOOL IsWow64Process(HANDLE, PBOOL wow64) { if(wow64)*wow64=FALSE; return TRUE; }
BOOL IsDebuggerPresent(void) { return FALSE; }
void DebugBreak(void) {}
void OutputDebugStringW(LPCWSTR s) { if (s) { std::string u=nppWideToUtf8(s); fputs(u.c_str(), stderr); } }
void OutputDebugStringA(LPCSTR s) { if (s) fputs(s, stderr); }
LPTOP_LEVEL_EXCEPTION_FILTER SetUnhandledExceptionFilter(LPTOP_LEVEL_EXCEPTION_FILTER) { return nullptr; }
LONG UnhandledExceptionFilter(EXCEPTION_POINTERS*) { return 0; }
void RaiseException(DWORD, DWORD, DWORD, const ULONG_PTR*) {}
PVOID AddVectoredExceptionHandler(ULONG, PVECTORED_EXCEPTION_HANDLER) { return (PVOID)1; }
ULONG RemoveVectoredExceptionHandler(PVOID) { return 1; }
DWORD GetPriorityClass(HANDLE) { return 0x20; }
BOOL SetPriorityClass(HANDLE, DWORD) { return TRUE; }
BOOL GetBinaryTypeW(LPCWSTR, LPDWORD type) { if(type)*type=0; return FALSE; }
BOOL VerifyVersionInfoW(LPOSVERSIONINFOEXW, DWORD, DWORDLONG) { return TRUE; }
ULONGLONG VerSetConditionMask(ULONGLONG mask, DWORD, BYTE) { return mask; }
BOOL GetProcessMemoryInfo(HANDLE, PPROCESS_MEMORY_COUNTERS p, DWORD)
{ if(p) memset(p,0,sizeof(*p)); return TRUE; }
BOOL MiniDumpWriteDump(HANDLE, DWORD, HANDLE, MINIDUMP_TYPE, PMINIDUMP_EXCEPTION_INFORMATION, PMINIDUMP_USER_STREAM_INFORMATION, PMINIDUMP_CALLBACK_INFORMATION) { return FALSE; }
UINT SendInput(UINT, LPINPUT, int) { return 0; }
void keybd_event(BYTE, BYTE, DWORD, ULONG_PTR) {}
void mouse_event(DWORD, DWORD, DWORD, DWORD, ULONG_PTR) {}
int GetKeyNameTextW(LONG, LPWSTR buf, int cch) { if(buf&&cch) buf[0]=0; return 0; }
BOOL AttachThreadInput(DWORD, DWORD, BOOL) { return TRUE; }
HWND GetShellWindow(void) { return nullptr; }
BOOL SetProcessWorkingSetSize(HANDLE, SIZE_T, SIZE_T) { return TRUE; }
BOOL IsWindows10OrGreater(void) { return TRUE; }
BOOL IsWindows11OrGreater(void) { return FALSE; }
int MessageBoxTimeoutW(HWND h, LPCWSTR t, LPCWSTR c, UINT type, WORD, DWORD) { return MessageBoxW(h,t,c,type); }
BOOL GetFileVersionInfoW(LPCWSTR, DWORD, DWORD, LPVOID) { return FALSE; }
DWORD GetFileVersionInfoSizeW(LPCWSTR, LPDWORD h) { if(h)*h=0; return 0; }
BOOL VerQueryValueW(LPCVOID, LPCWSTR, LPVOID*, PUINT) { return FALSE; }
BOOL WinVerifyTrust(HWND, GUID*, LPVOID) { return 0; }
LONG WinVerifyTrustEx(HWND, GUID*, WINTRUST_DATA*) { return 0; }
BOOL InternetGetConnectedState(LPDWORD flags, DWORD) { if(flags)*flags=1; return TRUE; }
BOOL IsNetworkAlive(LPDWORD flags) { if(flags)*flags=1; return TRUE; }
DWORD MsgWaitForMultipleObjects(DWORD c, const HANDLE* h, BOOL a, DWORD ms, DWORD) { return WaitForMultipleObjects(c,h,a,ms); }
ATOM GlobalAddAtomW(LPCWSTR) { return 1; }
ATOM GlobalFindAtomW(LPCWSTR) { return 1; }
ATOM GlobalDeleteAtom(ATOM) { return 0; }
BOOL EnumResourceNamesW(HMODULE, LPCWSTR, ENUMRESNAMEPROCW, LONG_PTR) { return TRUE; }
BOOL EnumResourceTypesW(HMODULE, ENUMRESTYPEPROCW, LONG_PTR) { return TRUE; }
BOOL WinHelpW(HWND, LPCWSTR, UINT, ULONG_PTR) { return TRUE; }
HWND HtmlHelpW(HWND, LPCWSTR, UINT, DWORD_PTR) { return nullptr; }
int StrFromTimeIntervalW(LPWSTR buf, UINT cch, DWORD ms, int)
{
	if (!buf) return 0;
	swprintf(buf, cch, L"%u ms", ms);
	return (int)wcslen(buf);
}
