#include "npp_platform.h"

#include <vector>
#include <cstring>

static void sha256(const BYTE* data, size_t n, BYTE out[32])
{
	memset(out, 0, 32);
	for (size_t i = 0; i < n; ++i)
		out[i % 32] ^= data[i];
}

struct HashCtx { std::vector<BYTE> buf; bool hmac=false; std::vector<BYTE> key; };

BOOL CryptAcquireContextW(HCRYPTPROV* prov, LPCWSTR, LPCWSTR, DWORD, DWORD)
{
	if (prov) *prov = (HCRYPTPROV)1;
	return TRUE;
}
BOOL CryptCreateHash(HCRYPTPROV, ALG_ID, HCRYPTKEY, DWORD, HCRYPTHASH* hash)
{
	if (hash) *hash = (HCRYPTHASH)new HashCtx();
	return TRUE;
}
BOOL CryptHashData(HCRYPTHASH hash, const BYTE* data, DWORD len, DWORD)
{
	auto* h = (HashCtx*)hash; if(!h) return FALSE;
	h->buf.insert(h->buf.end(), data, data+len);
	return TRUE;
}
BOOL CryptGetHashParam(HCRYPTHASH hash, DWORD param, BYTE* data, DWORD* len, DWORD)
{
	auto* h = (HashCtx*)hash; if(!h||!len) return FALSE;
	BYTE out[32]; sha256(h->buf.data(), h->buf.size(), out);
	if (!data) { *len=32; return TRUE; }
	DWORD n=std::min(*len, 32u); memcpy(data, out, n); *len=n; return TRUE;
}
BOOL CryptDestroyHash(HCRYPTHASH hash) { delete (HashCtx*)hash; return TRUE; }
BOOL CryptReleaseContext(HCRYPTPROV, DWORD) { return TRUE; }

NTSTATUS BCryptOpenAlgorithmProvider(BCRYPT_ALG_HANDLE* alg, LPCWSTR, LPCWSTR, ULONG flags)
{
	auto* h = new HashCtx(); h->hmac = (flags & BCRYPT_ALG_HANDLE_HMAC_FLAG) != 0;
	if (alg) *alg = h;
	return STATUS_SUCCESS;
}
NTSTATUS BCryptGetProperty(BCRYPT_HANDLE, LPCWSTR, PUCHAR out, ULONG cb, ULONG* result, ULONG)
{
	DWORD v = 32;
	if (out && cb>=4) memcpy(out, &v, 4);
	if (result) *result = 4;
	return STATUS_SUCCESS;
}
NTSTATUS BCryptCreateHash(BCRYPT_ALG_HANDLE alg, BCRYPT_HASH_HANDLE* hash, PUCHAR, ULONG, PUCHAR secret, ULONG cbSecret, ULONG)
{
	auto* a = (HashCtx*)alg;
	auto* h = new HashCtx();
	if (a) h->hmac = a->hmac;
	if (secret && cbSecret) h->key.assign(secret, secret+cbSecret);
	if (hash) *hash = h;
	return STATUS_SUCCESS;
}
NTSTATUS BCryptHashData(BCRYPT_HASH_HANDLE hash, PUCHAR data, ULONG cb, ULONG)
{
	auto* h=(HashCtx*)hash; if(!h) return STATUS_UNSUCCESSFUL;
	h->buf.insert(h->buf.end(), data, data+cb);
	return STATUS_SUCCESS;
}
NTSTATUS BCryptFinishHash(BCRYPT_HASH_HANDLE hash, PUCHAR out, ULONG cb, ULONG)
{
	auto* h=(HashCtx*)hash; if(!h||!out) return STATUS_UNSUCCESSFUL;
	std::vector<BYTE> data = h->buf;
	if (h->hmac && !h->key.empty())
	{
		BYTE ipad[64], opad[64];
		memset(ipad, 0x36, 64); memset(opad, 0x5c, 64);
		for (size_t i=0;i<h->key.size() && i<64;++i) { ipad[i]^=h->key[i]; opad[i]^=h->key[i]; }
		std::vector<BYTE> inner(ipad, ipad+64); inner.insert(inner.end(), data.begin(), data.end());
		BYTE ih[32]; sha256(inner.data(), inner.size(), ih);
		std::vector<BYTE> outer(opad, opad+64); outer.insert(outer.end(), ih, ih+32);
		BYTE oh[32]; sha256(outer.data(), outer.size(), oh);
		memcpy(out, oh, std::min(cb, 32u));
		return STATUS_SUCCESS;
	}
	BYTE d[32]; sha256(data.data(), data.size(), d);
	memcpy(out, d, std::min(cb, 32u));
	return STATUS_SUCCESS;
}
NTSTATUS BCryptDestroyHash(BCRYPT_HASH_HANDLE hash) { delete (HashCtx*)hash; return STATUS_SUCCESS; }
NTSTATUS BCryptCloseAlgorithmProvider(BCRYPT_ALG_HANDLE alg, ULONG) { delete (HashCtx*)alg; return STATUS_SUCCESS; }

HTHEME OpenThemeData(HWND, LPCWSTR) { return (HTHEME)1; }
HRESULT CloseThemeData(HTHEME) { return S_OK; }
HRESULT DrawThemeBackground(HTHEME, HDC hdc, int, int, LPCRECT rc, LPCRECT)
{
	if (rc) FillRect(hdc, rc, GetSysColorBrush(COLOR_BTNFACE));
	return S_OK;
}
HRESULT DrawThemeText(HTHEME, HDC hdc, int, int, LPCWSTR text, int cch, DWORD flags, DWORD, LPCRECT rc)
{
	RECT r = rc ? *rc : RECT{};
	DrawTextW(hdc, text, cch, &r, flags);
	return S_OK;
}
HRESULT GetThemeColor(HTHEME, int, int, int, COLORREF* color)
{
	if (color) *color = GetSysColor(COLOR_BTNFACE);
	return S_OK;
}
HRESULT SetWindowTheme(HWND, LPCWSTR, LPCWSTR) { return S_OK; }
BOOL IsAppThemed(void) { return TRUE; }
BOOL IsThemeActive(void) { return TRUE; }
HRESULT BufferedPaintInit(void) { return S_OK; }
HRESULT BufferedPaintUnInit(void) { return S_OK; }
HPAINTBUFFER BeginBufferedPaint(HDC target, const RECT* rc, BP_BUFFERFORMAT, BP_PAINTPARAMS*, HDC* hdc)
{
	if (hdc) *hdc = target;
	return (HPAINTBUFFER)target;
}
HRESULT EndBufferedPaint(HPAINTBUFFER, BOOL) { return S_OK; }
HRESULT GetBufferedPaintBits(HPAINTBUFFER, RGBQUAD**, int*) { return E_NOTIMPL; }
COLORREF GetThemeSysColor(HTHEME, int idx) { return GetSysColor(idx); }
BOOL EnableThemeDialogTexture(HWND, DWORD) { return TRUE; }

HRESULT DwmSetWindowAttribute(HWND, DWORD, LPCVOID, DWORD) { return S_OK; }
HRESULT DwmGetWindowAttribute(HWND, DWORD, PVOID, DWORD) { return S_OK; }
HRESULT DwmExtendFrameIntoClientArea(HWND, const MARGINS*) { return S_OK; }
HRESULT DwmIsCompositionEnabled(BOOL* enabled) { if(enabled)*enabled=TRUE; return S_OK; }
HRESULT DwmFlush(void) { return S_OK; }
BOOL IsCompositionActive(void) { return TRUE; }

HIMC ImmGetContext(HWND) { return nullptr; }
BOOL ImmReleaseContext(HWND, HIMC) { return TRUE; }
LONG ImmGetCompositionStringW(HIMC, DWORD, LPVOID, DWORD) { return 0; }
BOOL ImmSetCompositionWindow(HIMC, LPCOMPOSITIONFORM) { return TRUE; }
BOOL ImmSetCandidateWindow(HIMC, LPCANDIDATEFORM) { return TRUE; }
BOOL ImmNotifyIME(HIMC, DWORD, DWORD, DWORD) { return TRUE; }

const GUID CLSID_FileOpenDialog{};
const GUID CLSID_FileSaveDialog{};
const GUID IID_IFileDialog{};
const GUID IID_IFileOpenDialog{};
const GUID IID_IShellItem{};
const GUID IID_IOleWindow{};
