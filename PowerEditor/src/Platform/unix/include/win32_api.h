// Win32 API declarations for the Unix compatibility layer.
#pragma once

#ifndef NPP_WIN32_API_H
#define NPP_WIN32_API_H

#ifdef __cplusplus
extern "C" {
#endif

// ---- error / last-error ----
DWORD GetLastError(void);
void SetLastError(DWORD err);
DWORD FormatMessageA(DWORD flags, LPCVOID src, DWORD id, DWORD lang, LPSTR buf, DWORD size, va_list* args);
DWORD FormatMessageW(DWORD flags, LPCVOID src, DWORD id, DWORD lang, LPWSTR buf, DWORD size, va_list* args);

// ---- windows / classes ----
ATOM RegisterClassW(const WNDCLASSW* wc);
ATOM RegisterClassExW(const WNDCLASSEXW* wc);
BOOL UnregisterClassW(LPCWSTR name, HINSTANCE inst);
HWND CreateWindowExW(DWORD exStyle, LPCWSTR cls, LPCWSTR title, DWORD style,
	int x, int y, int w, int h, HWND parent, HMENU menu, HINSTANCE inst, LPVOID param);
HWND CreateWindowW(LPCWSTR cls, LPCWSTR title, DWORD style,
	int x, int y, int w, int h, HWND parent, HMENU menu, HINSTANCE inst, LPVOID param);
BOOL DestroyWindow(HWND h);
BOOL ShowWindow(HWND h, int cmd);
BOOL UpdateWindow(HWND h);
BOOL MoveWindow(HWND hwnd, int x, int y, int w, int height, BOOL repaint);
BOOL SetWindowPos(HWND h, HWND after, int x, int y, int cx, int cy, UINT flags);
BOOL GetWindowRect(HWND h, LPRECT rc);
BOOL GetClientRect(HWND h, LPRECT rc);
BOOL ClientToScreen(HWND h, LPPOINT pt);
BOOL ScreenToClient(HWND h, LPPOINT pt);
int MapWindowPoints(HWND from, HWND to, LPPOINT pts, UINT count);
LONG_PTR GetWindowLongPtrW(HWND h, int idx);
LONG_PTR SetWindowLongPtrW(HWND h, int idx, LONG_PTR val);
LONG GetWindowLongW(HWND h, int idx);
LONG SetWindowLongW(HWND h, int idx, LONG val);
ULONG_PTR GetClassLongPtrW(HWND h, int idx);
ULONG_PTR SetClassLongPtrW(HWND h, int idx, LONG_PTR val);
HWND GetParent(HWND h);
HWND SetParent(HWND h, HWND parent);
HWND GetDlgItem(HWND dlg, int id);
BOOL SetDlgItemTextW(HWND dlg, int id, LPCWSTR text);
UINT GetDlgItemTextW(HWND dlg, int id, LPWSTR buf, int max);
UINT GetDlgItemTextA(HWND dlg, int id, LPSTR buf, int max);
BOOL SetDlgItemInt(HWND dlg, int id, UINT value, BOOL signedVal);
UINT GetDlgItemInt(HWND dlg, int id, BOOL* translated, BOOL signedVal);
BOOL EnableWindow(HWND h, BOOL enable);
BOOL IsWindow(HWND h);
BOOL IsWindowVisible(HWND h);
BOOL IsWindowEnabled(HWND h);
BOOL IsIconic(HWND h);
BOOL IsZoomed(HWND h);
BOOL IsChild(HWND parent, HWND child);
HWND GetForegroundWindow(void);
BOOL SetForegroundWindow(HWND h);
HWND GetFocus(void);
HWND SetFocus(HWND h);
HWND GetActiveWindow(void);
HWND SetActiveWindow(HWND h);
HWND GetCapture(void);
HWND SetCapture(HWND h);
BOOL ReleaseCapture(void);
HWND GetDesktopWindow(void);
HWND FindWindowW(LPCWSTR cls, LPCWSTR title);
HWND FindWindowExW(HWND parent, HWND after, LPCWSTR cls, LPCWSTR title);
BOOL EnumWindows(WNDENUMPROC cb, LPARAM lp);
BOOL EnumChildWindows(HWND parent, WNDENUMPROC cb, LPARAM lp);
BOOL EnumThreadWindows(DWORD tid, WNDENUMPROC cb, LPARAM lp);
HWND GetWindow(HWND h, UINT cmd);
HWND GetTopWindow(HWND h);
HWND GetAncestor(HWND h, UINT flags);
int GetWindowTextW(HWND h, LPWSTR buf, int max);
int GetWindowTextLengthW(HWND h);
BOOL SetWindowTextW(HWND h, LPCWSTR text);
BOOL InvalidateRect(HWND h, const RECT* rc, BOOL erase);
BOOL ValidateRect(HWND h, const RECT* rc);
BOOL RedrawWindow(HWND h, const RECT* rc, HRGN rgn, UINT flags);
BOOL GetUpdateRect(HWND h, LPRECT rc, BOOL erase);
HDC GetDC(HWND h);
HDC GetWindowDC(HWND h);
HDC GetDCEx(HWND h, HRGN clip, DWORD flags);
int ReleaseDC(HWND h, HDC hdc);
HDC BeginPaint(HWND h, LPPAINTSTRUCT ps);
BOOL EndPaint(HWND h, const PAINTSTRUCT* ps);
int GetSystemMetrics(int idx);
BOOL SystemParametersInfoW(UINT action, UINT uiParam, PVOID pvParam, UINT fWinIni);
int MessageBoxW(HWND owner, LPCWSTR text, LPCWSTR caption, UINT type);
int MessageBoxA(HWND owner, LPCSTR text, LPCSTR caption, UINT type);
BOOL MessageBeep(UINT type);
LRESULT SendMessageW(HWND h, UINT msg, WPARAM w, LPARAM l);
LRESULT SendMessageA(HWND h, UINT msg, WPARAM w, LPARAM l);
LRESULT SendMessageTimeoutW(HWND h, UINT msg, WPARAM w, LPARAM l, UINT flags, UINT timeout, PDWORD_PTR result);
BOOL PostMessageW(HWND h, UINT msg, WPARAM w, LPARAM l);
BOOL PostThreadMessageW(DWORD tid, UINT msg, WPARAM w, LPARAM l);
void PostQuitMessage(int code);
BOOL GetMessageW(LPMSG msg, HWND h, UINT min, UINT max);
BOOL PeekMessageW(LPMSG msg, HWND h, UINT min, UINT max, UINT remove);
BOOL TranslateMessage(const MSG* msg);
LRESULT DispatchMessageW(const MSG* msg);
BOOL WaitMessage(void);
DWORD MsgWaitForMultipleObjects(DWORD count, const HANDLE* handles, BOOL waitAll, DWORD ms, DWORD wake);
int TranslateAcceleratorW(HWND h, HACCEL acc, LPMSG msg);
BOOL IsDialogMessageW(HWND dlg, LPMSG msg);
LRESULT CallWindowProcW(WNDPROC proc, HWND h, UINT msg, WPARAM w, LPARAM l);
LRESULT DefWindowProcW(HWND h, UINT msg, WPARAM w, LPARAM l);
INT_PTR DefDlgProcW(HWND h, UINT msg, WPARAM w, LPARAM l);
int GetClassNameW(HWND h, LPWSTR buf, int max);
int SetWindowRgn(HWND h, HRGN rgn, BOOL redraw);
int GetWindowRgn(HWND h, HRGN rgn);
BOOL AdjustWindowRect(LPRECT rc, DWORD style, BOOL menu);
BOOL AdjustWindowRectEx(LPRECT rc, DWORD style, BOOL menu, DWORD exStyle);
BOOL GetWindowPlacement(HWND h, WINDOWPLACEMENT* wp);
BOOL SetWindowPlacement(HWND h, const WINDOWPLACEMENT* wp);
BOOL ShowScrollBar(HWND h, int bar, BOOL show);
int SetScrollInfo(HWND h, int bar, LPCSCROLLINFO si, BOOL redraw);
BOOL GetScrollInfo(HWND h, int bar, LPSCROLLINFO si);
int SetScrollPos(HWND h, int bar, int pos, BOOL redraw);
int GetScrollPos(HWND h, int bar);
int SetScrollRange(HWND h, int bar, int min, int max, BOOL redraw);
BOOL GetScrollRange(HWND h, int bar, LPINT min, LPINT max);
BOOL ScrollWindow(HWND h, int dx, int dy, const RECT* scroll, const RECT* clip);
int ScrollWindowEx(HWND h, int dx, int dy, const RECT* scroll, const RECT* clip, HRGN update, LPRECT updateRc, UINT flags);
BOOL GetCursorPos(LPPOINT pt);
BOOL SetCursorPos(int x, int y);
HCURSOR LoadCursorW(HINSTANCE inst, LPCWSTR name);
HCURSOR SetCursor(HCURSOR cur);
int ShowCursor(BOOL show);
HCURSOR GetCursor(void);
HICON LoadIconW(HINSTANCE inst, LPCWSTR name);
HANDLE LoadImageW(HINSTANCE inst, LPCWSTR name, UINT type, int cx, int cy, UINT fuLoad);
HBITMAP LoadBitmapW(HINSTANCE inst, LPCWSTR name);
HMENU LoadMenuW(HINSTANCE inst, LPCWSTR name);
HMENU GetSubMenu(HMENU menu, int pos);
BOOL TrackPopupMenu(HMENU menu, UINT flags, int x, int y, int reserved, HWND owner, const RECT* rc);
BOOL TrackPopupMenuEx(HMENU menu, UINT flags, int x, int y, HWND owner, LPTPMPARAMS params);
BOOL DestroyMenu(HMENU menu);
HMENU CreatePopupMenu(void);
HMENU CreateMenu(void);
BOOL AppendMenuW(HMENU menu, UINT flags, UINT_PTR id, LPCWSTR item);
BOOL InsertMenuW(HMENU menu, UINT pos, UINT flags, UINT_PTR id, LPCWSTR item);
BOOL InsertMenuItemW(HMENU menu, UINT item, BOOL byPos, LPCMENUITEMINFOW mii);
BOOL DeleteMenu(HMENU menu, UINT pos, UINT flags);
BOOL RemoveMenu(HMENU menu, UINT pos, UINT flags);
DWORD CheckMenuItem(HMENU menu, UINT id, UINT check);
BOOL EnableMenuItem(HMENU menu, UINT id, UINT enable);
HMENU GetMenu(HWND h);
BOOL SetMenu(HWND h, HMENU menu);
BOOL DrawMenuBar(HWND h);
int GetMenuItemCount(HMENU menu);
UINT GetMenuItemID(HMENU menu, int pos);
int GetMenuStringW(HMENU menu, UINT id, LPWSTR buf, int max, UINT flags);
UINT GetMenuState(HMENU menu, UINT id, UINT flags);
BOOL GetMenuItemInfoW(HMENU menu, UINT item, BOOL byPos, LPMENUITEMINFOW mii);
BOOL SetMenuItemInfoW(HMENU menu, UINT item, BOOL byPos, LPCMENUITEMINFOW mii);
BOOL ModifyMenuW(HMENU menu, UINT pos, UINT flags, UINT_PTR id, LPCWSTR item);
BOOL CheckMenuRadioItem(HMENU menu, UINT first, UINT last, UINT check, UINT flags);
HACCEL CreateAcceleratorTableW(LPACCEL accels, int count);
BOOL DestroyAcceleratorTable(HACCEL acc);
int CopyAcceleratorTableW(HACCEL acc, LPACCEL out, int count);
HACCEL LoadAcceleratorsW(HINSTANCE inst, LPCWSTR name);
HWND GetNextDlgTabItem(HWND dlg, HWND ctl, BOOL prev);
HWND GetNextDlgGroupItem(HWND dlg, HWND ctl, BOOL prev);
BOOL SetPropW(HWND h, LPCWSTR name, HANDLE data);
HANDLE GetPropW(HWND h, LPCWSTR name);
HANDLE RemovePropW(HWND h, LPCWSTR name);
UINT_PTR SetTimer(HWND h, UINT_PTR id, UINT elapse, TIMERPROC proc);
BOOL KillTimer(HWND h, UINT_PTR id);
BOOL FlashWindow(HWND h, BOOL invert);
BOOL FlashWindowEx(PFLASHWINFO pfwi);
BOOL OpenIcon(HWND h);
BOOL CloseWindow(HWND h);
BOOL BringWindowToTop(HWND h);
HWND WindowFromPoint(POINT pt);
HWND ChildWindowFromPoint(HWND parent, POINT pt);
BOOL DragDetect(HWND h, POINT pt);
int GetWindowRgnBox(HWND h, LPRECT rc);
BOOL SetLayeredWindowAttributes(HWND h, COLORREF key, BYTE alpha, DWORD flags);
BOOL GetLayeredWindowAttributes(HWND h, COLORREF* key, BYTE* alpha, DWORD* flags);
BOOL AnimateWindow(HWND h, DWORD time, DWORD flags);
BOOL DrawAnimatedRects(HWND h, int idAni, const RECT* from, const RECT* to);
int GetDlgCtrlID(HWND h);
HWND GetLastActivePopup(HWND h);
BOOL AnyPopup(void);
void SwitchToThisWindow(HWND h, BOOL fAltTab);
BOOL LockWindowUpdate(HWND h);
BOOL RedrawWindow(HWND h, const RECT* lprcUpdate, HRGN hrgnUpdate, UINT flags);
HDWP BeginDeferWindowPos(int n);
HDWP DeferWindowPos(HDWP hdwp, HWND h, HWND after, int x, int y, int cx, int cy, UINT flags);
BOOL EndDeferWindowPos(HDWP hdwp);
BOOL GetGUIThreadInfo(DWORD id, PGUITHREADINFO info);
UINT GetDoubleClickTime(void);
BOOL SetDoubleClickTime(UINT ms);
int GetKeyboardType(int typeFlag);
SHORT GetKeyState(int vk);
SHORT GetAsyncKeyState(int vk);
BOOL GetKeyboardState(PBYTE keys);
BOOL SetKeyboardState(LPBYTE keys);
UINT MapVirtualKeyW(UINT code, UINT mapType);
UINT MapVirtualKeyExW(UINT code, UINT mapType, HKL hkl);
int ToUnicode(UINT vk, UINT scan, const BYTE* keystate, LPWSTR buf, int cch, UINT flags);
int ToUnicodeEx(UINT vk, UINT scan, const BYTE* keystate, LPWSTR buf, int cch, UINT flags, HKL hkl);
HKL GetKeyboardLayout(DWORD tid);
int GetKeyboardLayoutList(int n, HKL* layouts);
BOOL GetKeyboardLayoutNameW(LPWSTR pwszKLID);
UINT GetKBCodePage(void);
int LoadStringW(HINSTANCE inst, UINT id, LPWSTR buf, int max);
int DrawTextW(HDC hdc, LPCWSTR text, int count, LPRECT rc, UINT format);
int DrawTextExW(HDC hdc, LPWSTR text, int count, LPRECT rc, UINT format, LPDRAWTEXTPARAMS params);
BOOL DrawEdge(HDC hdc, LPRECT rc, UINT edge, UINT flags);
BOOL DrawFocusRect(HDC hdc, const RECT* rc);
BOOL DrawFrameControl(HDC hdc, LPRECT rc, UINT type, UINT state);
BOOL DrawIcon(HDC hdc, int x, int y, HICON icon);
BOOL DrawIconEx(HDC hdc, int x, int y, HICON icon, int cx, int cy, UINT istep, HBRUSH flicker, UINT flags);
BOOL DrawStateW(HDC hdc, HBRUSH br, DRAWSTATEPROC proc, LPARAM l, WPARAM w, int x, int y, int cx, int cy, UINT flags);
int FillRect(HDC hdc, const RECT* rc, HBRUSH br);
int FrameRect(HDC hdc, const RECT* rc, HBRUSH br);
BOOL InvertRect(HDC hdc, const RECT* rc);
int TabbedTextOutW(HDC hdc, int x, int y, LPCWSTR s, int count, int nTab, const INT* tabs, int origin);
DWORD GetTabbedTextExtentW(HDC hdc, LPCWSTR s, int count, int nTab, const INT* tabs);
BOOL GrayStringW(HDC hdc, HBRUSH br, GRAYSTRINGPROC proc, LPARAM l, int cch, int x, int y, int cx, int cy);
HICON CopyIcon(HICON icon);
BOOL DestroyIcon(HICON icon);
BOOL DestroyCursor(HCURSOR cur);
HICON CreateIconIndirect(PICONINFO piconinfo);
BOOL GetIconInfo(HICON icon, PICONINFO piconinfo);
HBITMAP CopyImage(HANDLE h, UINT type, int cx, int cy, UINT flags);
BOOL GetMonitorInfoW(HMONITOR mon, LPMONITORINFO mi);
HMONITOR MonitorFromWindow(HWND h, DWORD flags);
HMONITOR MonitorFromRect(LPCRECT rc, DWORD flags);
HMONITOR MonitorFromPoint(POINT pt, DWORD flags);
BOOL EnumDisplayMonitors(HDC hdc, LPCRECT clip, MONITORENUMPROC proc, LPARAM data);
BOOL EnumDisplayDevicesW(LPCWSTR device, DWORD devNum, PDISPLAY_DEVICEW dd, DWORD flags);
BOOL EnumDisplaySettingsW(LPCWSTR device, DWORD mode, LPDEVMODEW dm);
int GetWindowTextA(HWND h, LPSTR buf, int max);
BOOL SetWindowTextA(HWND h, LPCSTR text);
HWND GetWindow(HWND h, UINT cmd);
BOOL IsHungAppWindow(HWND h);
BOOL ChangeWindowMessageFilter(UINT message, DWORD flag);
BOOL ChangeWindowMessageFilterEx(HWND h, UINT message, DWORD action, PCHANGEFILTERSTRUCT pChangeFilterStruct);
BOOL RegisterHotKey(HWND h, int id, UINT fsModifiers, UINT vk);
BOOL UnregisterHotKey(HWND h, int id);
ATOM GlobalAddAtomW(LPCWSTR name);
ATOM GlobalFindAtomW(LPCWSTR name);
ATOM GlobalDeleteAtom(ATOM atom);
UINT RegisterWindowMessageW(LPCWSTR name);
UINT RegisterClipboardFormatW(LPCWSTR name);
HWND SetClipboardViewer(HWND h);
BOOL ChangeClipboardChain(HWND h, HWND next);
HWND GetClipboardOwner(void);
HWND GetOpenClipboardWindow(void);
DWORD GetClipboardSequenceNumber(void);
BOOL OpenClipboard(HWND h);
BOOL CloseClipboard(void);
BOOL EmptyClipboard(void);
HANDLE SetClipboardData(UINT fmt, HANDLE mem);
HANDLE GetClipboardData(UINT fmt);
BOOL IsClipboardFormatAvailable(UINT fmt);
int CountClipboardFormats(void);
UINT EnumClipboardFormats(UINT fmt);
int GetClipboardFormatNameW(UINT fmt, LPWSTR buf, int cch);
BOOL AddClipboardFormatListener(HWND h);
BOOL RemoveClipboardFormatListener(HWND h);
int MessageBoxTimeoutW(HWND h, LPCWSTR text, LPCWSTR caption, UINT type, WORD lang, DWORD timeout);

// ---- dialogs ----
INT_PTR DialogBoxParamW(HINSTANCE inst, LPCWSTR tmpl, HWND parent, DLGPROC proc, LPARAM lp);
INT_PTR DialogBoxIndirectParamW(HINSTANCE inst, LPCDLGTEMPLATE tmpl, HWND parent, DLGPROC proc, LPARAM lp);
HWND CreateDialogParamW(HINSTANCE inst, LPCWSTR tmpl, HWND parent, DLGPROC proc, LPARAM lp);
HWND CreateDialogIndirectParamW(HINSTANCE inst, LPCDLGTEMPLATE tmpl, HWND parent, DLGPROC proc, LPARAM lp);
BOOL EndDialog(HWND dlg, INT_PTR result);
LONG GetDialogBaseUnits(void);
BOOL MapDialogRect(HWND dlg, LPRECT rc);
LRESULT SendDlgItemMessageW(HWND dlg, int id, UINT msg, WPARAM w, LPARAM l);
HWND GetDlgItem(HWND dlg, int id);
BOOL CheckDlgButton(HWND dlg, int id, UINT check);
UINT IsDlgButtonChecked(HWND dlg, int id);
BOOL CheckRadioButton(HWND dlg, int first, int last, int check);

BOOL GetOpenFileNameW(LPOPENFILENAMEW ofn);
BOOL GetSaveFileNameW(LPOPENFILENAMEW ofn);
BOOL ChooseColorW(LPCHOOSECOLORW cc);
BOOL ChooseFontW(LPCHOOSEFONTW cf);
HWND FindTextW(LPFINDREPLACEW fr);
HWND ReplaceTextW(LPFINDREPLACEW fr);
DWORD CommDlgExtendedError(void);
BOOL PrintDlgW(LPPRINTDLGW pd);
BOOL PageSetupDlgW(LPPAGESETUPDLGW psd);

HRESULT TaskDialog(HWND owner, HINSTANCE inst, PCWSTR title, PCWSTR main, PCWSTR content,
	TASKDIALOG_COMMON_BUTTON_FLAGS buttons, PCWSTR icon, int* button);
HRESULT TaskDialogIndirect(const TASKDIALOGCONFIG* cfg, int* button, int* radio, BOOL* verification);

// ---- GDI ----
HDC CreateCompatibleDC(HDC hdc);
HBITMAP CreateCompatibleBitmap(HDC hdc, int cx, int cy);
HBITMAP CreateDIBSection(HDC hdc, const BITMAPINFO* bmi, UINT usage, void** bits, HANDLE section, DWORD offset);
HBITMAP CreateBitmap(int w, int h, UINT planes, UINT bitCount, const void* bits);
HBITMAP CreateBitmapIndirect(const BITMAP* pbm);
HBRUSH CreateSolidBrush(COLORREF color);
HBRUSH CreatePatternBrush(HBITMAP bmp);
HBRUSH CreateHatchBrush(int hatch, COLORREF color);
HPEN CreatePen(int style, int width, COLORREF color);
HPEN CreatePenIndirect(const LOGPEN* plp);
HFONT CreateFontW(int h, int w, int esc, int orient, int weight, DWORD italic, DWORD underline, DWORD strike,
	DWORD charset, DWORD outPrec, DWORD clipPrec, DWORD quality, DWORD pitch, LPCWSTR face);
HFONT CreateFontIndirectW(const LOGFONTW* lf);
HGDIOBJ SelectObject(HDC hdc, HGDIOBJ obj);
BOOL DeleteObject(HGDIOBJ obj);
BOOL DeleteDC(HDC hdc);
BOOL BitBlt(HDC dst, int x, int y, int cx, int cy, HDC src, int sx, int sy, DWORD rop);
BOOL StretchBlt(HDC dst, int x, int y, int cx, int cy, HDC src, int sx, int sy, int scx, int scy, DWORD rop);
BOOL PatBlt(HDC hdc, int x, int y, int w, int h, DWORD rop);
BOOL TransparentBlt(HDC dst, int x, int y, int cx, int cy, HDC src, int sx, int sy, int scx, int scy, UINT key);
BOOL AlphaBlend(HDC dst, int x, int y, int cx, int cy, HDC src, int sx, int sy, int scx, int scy, BLENDFUNCTION fn);
BOOL GradientFill(HDC hdc, PTRIVERTEX v, ULONG nVertex, PVOID mesh, ULONG nMesh, ULONG mode);
BOOL GdiFlush(void);
int SetBkMode(HDC hdc, int mode);
COLORREF SetBkColor(HDC hdc, COLORREF color);
COLORREF SetTextColor(HDC hdc, COLORREF color);
COLORREF GetBkColor(HDC hdc);
COLORREF GetTextColor(HDC hdc);
UINT SetTextAlign(HDC hdc, UINT align);
BOOL TextOutW(HDC hdc, int x, int y, LPCWSTR s, int c);
BOOL ExtTextOutW(HDC hdc, int x, int y, UINT options, const RECT* rc, LPCWSTR s, UINT c, const INT* dx);
BOOL GetTextExtentPoint32W(HDC hdc, LPCWSTR s, int c, LPSIZE sz);
BOOL GetTextMetricsW(HDC hdc, LPTEXTMETRICW tm);
int GetDeviceCaps(HDC hdc, int index);
int GetObjectW(HANDLE obj, int c, LPVOID buf);
HGDIOBJ GetStockObject(int i);
COLORREF SetPixel(HDC hdc, int x, int y, COLORREF color);
COLORREF GetPixel(HDC hdc, int x, int y);
BOOL MoveToEx(HDC hdc, int x, int y, LPPOINT pt);
BOOL LineTo(HDC hdc, int x, int y);
BOOL Rectangle(HDC hdc, int l, int t, int r, int b);
BOOL Ellipse(HDC hdc, int l, int t, int r, int b);
BOOL RoundRect(HDC hdc, int l, int t, int r, int b, int w, int h);
BOOL Polygon(HDC hdc, const POINT* pts, int n);
BOOL Polyline(HDC hdc, const POINT* pts, int n);
BOOL PolyBezier(HDC hdc, const POINT* pts, DWORD n);
BOOL Arc(HDC hdc, int l, int t, int r, int b, int x0, int y0, int x1, int y1);
BOOL Pie(HDC hdc, int l, int t, int r, int b, int x0, int y0, int x1, int y1);
BOOL Chord(HDC hdc, int l, int t, int r, int b, int x0, int y0, int x1, int y1);
HRGN CreateRectRgn(int l, int t, int r, int b);
HRGN CreateRectRgnIndirect(const RECT* rc);
HRGN CreateRoundRectRgn(int l, int t, int r, int b, int w, int h);
HRGN CreateEllipticRgn(int l, int t, int r, int b);
HRGN CreatePolygonRgn(const POINT* pts, int n, int mode);
int CombineRgn(HRGN dst, HRGN a, HRGN b, int mode);
int OffsetRgn(HRGN rgn, int x, int y);
int GetClipBox(HDC hdc, LPRECT rc);
int SelectClipRgn(HDC hdc, HRGN rgn);
int IntersectClipRect(HDC hdc, int l, int t, int r, int b);
int ExcludeClipRect(HDC hdc, int l, int t, int r, int b);
int SaveDC(HDC hdc);
BOOL RestoreDC(HDC hdc, int saved);
BOOL SetViewportOrgEx(HDC hdc, int x, int y, LPPOINT pt);
BOOL GetViewportOrgEx(HDC hdc, LPPOINT pt);
BOOL SetWindowOrgEx(HDC hdc, int x, int y, LPPOINT pt);
BOOL OffsetViewportOrgEx(HDC hdc, int x, int y, LPPOINT pt);
BOOL SetViewportExtEx(HDC hdc, int x, int y, LPSIZE sz);
BOOL SetWindowExtEx(HDC hdc, int x, int y, LPSIZE sz);
int SetMapMode(HDC hdc, int mode);
int SetROP2(HDC hdc, int rop);
int SetStretchBltMode(HDC hdc, int mode);
int SetPolyFillMode(HDC hdc, int mode);
BOOL LPtoDP(HDC hdc, LPPOINT pts, int n);
BOOL DPtoLP(HDC hdc, LPPOINT pts, int n);
HGDIOBJ GetCurrentObject(HDC hdc, UINT type);
BOOL GetDCOrgEx(HDC hdc, LPPOINT pt);
BOOL BeginPath(HDC hdc);
BOOL EndPath(HDC hdc);
BOOL StrokePath(HDC hdc);
BOOL FillPath(HDC hdc);
BOOL StrokeAndFillPath(HDC hdc);
int SetGraphicsMode(HDC hdc, int mode);
BOOL SetWorldTransform(HDC hdc, const XFORM* xform);
BOOL GetWorldTransform(HDC hdc, XFORM* xform);
BOOL ModifyWorldTransform(HDC hdc, const XFORM* xform, DWORD mode);
int GetDIBits(HDC hdc, HBITMAP bmp, UINT start, UINT lines, LPVOID bits, LPBITMAPINFO bmi, UINT usage);
int SetDIBits(HDC hdc, HBITMAP bmp, UINT start, UINT lines, const void* bits, const BITMAPINFO* bmi, UINT usage);
int SetDIBitsToDevice(HDC hdc, int x, int y, DWORD w, DWORD h, int sx, int sy, UINT start, UINT lines,
	const void* bits, const BITMAPINFO* bmi, UINT colorUse);
int StretchDIBits(HDC hdc, int x, int y, int destW, int destH, int sx, int sy, int srcW, int srcH,
	const void* bits, const BITMAPINFO* bmi, UINT usage, DWORD rop);
BOOL MaskBlt(HDC dst, int x, int y, int cx, int cy, HDC src, int sx, int sy, HBITMAP mask, int mx, int my, DWORD rop);
HPALETTE CreatePalette(const LOGPALETTE* pal);
UINT RealizePalette(HDC hdc);
HPALETTE SelectPalette(HDC hdc, HPALETTE pal, BOOL forceBkg);
COLORREF GetNearestColor(HDC hdc, COLORREF color);
UINT GetDIBColorTable(HDC hdc, UINT start, UINT entries, RGBQUAD* colors);
UINT SetDIBColorTable(HDC hdc, UINT start, UINT entries, const RGBQUAD* colors);
int EnumFontFamiliesExW(HDC hdc, LPLOGFONTW lf, FONTENUMPROCW proc, LPARAM l, DWORD flags);
DWORD GetGlyphIndicesW(HDC hdc, LPCWSTR s, int c, LPWORD gi, DWORD flags);
BOOL GetCharWidth32W(HDC hdc, UINT first, UINT last, LPINT buffer);
BOOL GetTextExtentExPointW(HDC hdc, LPCWSTR s, int c, int maxExtent, LPINT fit, LPINT dx, LPSIZE sz);
UINT GetOutlineTextMetricsW(HDC hdc, UINT cb, LPOUTLINETEXTMETRICW otm);
DWORD GetFontData(HDC hdc, DWORD table, DWORD offset, LPVOID buffer, DWORD cb);
int AddFontResourceExW(LPCWSTR name, DWORD flags, PVOID res);
BOOL RemoveFontResourceExW(LPCWSTR name, DWORD flags, PVOID res);
HANDLE AddFontMemResourceEx(PVOID data, DWORD size, PVOID res, DWORD* num);
BOOL GdiAlphaBlend(HDC dst, int x, int y, int cx, int cy, HDC src, int sx, int sy, int scx, int scy, BLENDFUNCTION fn);
BOOL GdiTransparentBlt(HDC dst, int x, int y, int cx, int cy, HDC src, int sx, int sy, int scx, int scy, UINT key);
BOOL GdiGradientFill(HDC hdc, PTRIVERTEX v, ULONG nVertex, PVOID mesh, ULONG nMesh, ULONG mode);
int GetObjectType(HGDIOBJ obj);
BOOL PtInRegion(HRGN rgn, int x, int y);
BOOL RectInRegion(HRGN rgn, const RECT* rc);
int GetRgnBox(HRGN rgn, LPRECT rc);
BOOL EqualRgn(HRGN a, HRGN b);
DWORD GetRegionData(HRGN rgn, DWORD count, LPRGNDATA data);
BOOL FillRgn(HDC hdc, HRGN rgn, HBRUSH br);
BOOL FrameRgn(HDC hdc, HRGN rgn, HBRUSH br, int w, int h);
BOOL PaintRgn(HDC hdc, HRGN rgn);
BOOL InvertRgn(HDC hdc, HRGN rgn);
int OffsetClipRgn(HDC hdc, int x, int y);
int GetClipRgn(HDC hdc, HRGN rgn);
HDC CreateDCW(LPCWSTR driver, LPCWSTR device, LPCWSTR output, const DEVMODEW* init);
HDC CreateICW(LPCWSTR driver, LPCWSTR device, LPCWSTR output, const DEVMODEW* init);
int StartDocW(HDC hdc, const DOCINFOW* di);
int EndDoc(HDC hdc);
int StartPage(HDC hdc);
int EndPage(HDC hdc);
int AbortDoc(HDC hdc);
int SetAbortProc(HDC hdc, ABORTPROC proc);
int Escape(HDC hdc, int esc, int inCount, LPCSTR in, LPVOID out);
int ExtEscape(HDC hdc, int esc, int inCount, LPCSTR in, int outCount, LPSTR out);

BOOL SetRect(LPRECT rc, int l, int t, int r, int b);
BOOL SetRectEmpty(LPRECT rc);
BOOL CopyRect(LPRECT dst, const RECT* src);
BOOL InflateRect(LPRECT rc, int dx, int dy);
BOOL OffsetRect(LPRECT rc, int dx, int dy);
BOOL IsRectEmpty(const RECT* rc);
BOOL EqualRect(const RECT* a, const RECT* b);
BOOL IntersectRect(LPRECT dst, const RECT* a, const RECT* b);
BOOL UnionRect(LPRECT dst, const RECT* a, const RECT* b);
BOOL SubtractRect(LPRECT dst, const RECT* a, const RECT* b);
BOOL PtInRect(const RECT* rc, POINT pt);
BOOL UnionRect(LPRECT dst, const RECT* a, const RECT* b);

// ---- kernel / files ----
HANDLE CreateFileW(LPCWSTR name, DWORD access, DWORD share, LPSECURITY_ATTRIBUTES sa,
	DWORD disp, DWORD flags, HANDLE templateFile);
BOOL ReadFile(HANDLE h, LPVOID buf, DWORD n, LPDWORD read, LPOVERLAPPED ov);
BOOL WriteFile(HANDLE h, LPCVOID buf, DWORD n, LPDWORD written, LPOVERLAPPED ov);
BOOL CloseHandle(HANDLE h);
DWORD SetFilePointer(HANDLE h, LONG dist, PLONG distHigh, DWORD method);
BOOL SetFilePointerEx(HANDLE h, LARGE_INTEGER dist, PLARGE_INTEGER newPos, DWORD method);
DWORD GetFileSize(HANDLE h, LPDWORD high);
BOOL GetFileSizeEx(HANDLE h, PLARGE_INTEGER size);
DWORD GetFileAttributesW(LPCWSTR name);
BOOL SetFileAttributesW(LPCWSTR name, DWORD attr);
BOOL GetFileTime(HANDLE h, LPFILETIME c, LPFILETIME a, LPFILETIME w);
BOOL SetFileTime(HANDLE h, const FILETIME* c, const FILETIME* a, const FILETIME* w);
BOOL FlushFileBuffers(HANDLE h);
BOOL GetFileInformationByHandle(HANDLE h, LPBY_HANDLE_FILE_INFORMATION info);
DWORD GetFinalPathNameByHandleW(HANDLE h, LPWSTR buf, DWORD cch, DWORD flags);
HANDLE FindFirstFileW(LPCWSTR name, LPWIN32_FIND_DATAW data);
BOOL FindNextFileW(HANDLE h, LPWIN32_FIND_DATAW data);
BOOL FindClose(HANDLE h);
HANDLE FindFirstStreamW(LPCWSTR name, STREAM_INFO_LEVELS level, LPVOID data, DWORD flags);
BOOL FindNextStreamW(HANDLE h, LPVOID data);
BOOL DeleteFileW(LPCWSTR name);
BOOL MoveFileW(LPCWSTR from, LPCWSTR to);
BOOL MoveFileExW(LPCWSTR from, LPCWSTR to, DWORD flags);
BOOL CopyFileW(LPCWSTR from, LPCWSTR to, BOOL failIfExists);
BOOL CreateDirectoryW(LPCWSTR name, LPSECURITY_ATTRIBUTES sa);
BOOL RemoveDirectoryW(LPCWSTR name);
DWORD GetCurrentDirectoryW(DWORD n, LPWSTR buf);
BOOL SetCurrentDirectoryW(LPCWSTR path);
DWORD GetFullPathNameW(LPCWSTR file, DWORD n, LPWSTR buf, LPWSTR* filePart);
DWORD GetTempPathW(DWORD n, LPWSTR buf);
UINT GetTempFileNameW(LPCWSTR path, LPCWSTR prefix, UINT unique, LPWSTR buf);
DWORD GetModuleFileNameW(HMODULE mod, LPWSTR buf, DWORD n);
DWORD GetModuleFileNameA(HMODULE mod, LPSTR buf, DWORD n);
HMODULE GetModuleHandleW(LPCWSTR name);
HMODULE GetModuleHandleA(LPCSTR name);
BOOL GetModuleHandleExW(DWORD flags, LPCWSTR name, HMODULE* mod);
HMODULE LoadLibraryW(LPCWSTR name);
HMODULE LoadLibraryA(LPCSTR name);
HMODULE LoadLibraryExW(LPCWSTR name, HANDLE file, DWORD flags);
BOOL FreeLibrary(HMODULE mod);
FARPROC GetProcAddress(HMODULE mod, LPCSTR name);
DWORD GetTickCount(void);
ULONGLONG GetTickCount64(void);
void Sleep(DWORD ms);
DWORD SleepEx(DWORD ms, BOOL alertable);
int MultiByteToWideChar(UINT cp, DWORD flags, LPCSTR src, int cb, LPWSTR dst, int cch);
int WideCharToMultiByte(UINT cp, DWORD flags, LPCWSTR src, int cch, LPSTR dst, int cb, LPCSTR def, LPBOOL used);
int lstrlenW(LPCWSTR s);
int lstrlenA(LPCSTR s);
LPWSTR lstrcpyW(LPWSTR dst, LPCWSTR src);
LPWSTR lstrcpynW(LPWSTR dst, LPCWSTR src, int max);
LPWSTR lstrcatW(LPWSTR dst, LPCWSTR src);
int lstrcmpW(LPCWSTR a, LPCWSTR b);
int lstrcmpiW(LPCWSTR a, LPCWSTR b);
int CompareStringW(LCID lcid, DWORD flags, LPCWSTR a, int na, LPCWSTR b, int nb);
int CompareStringOrdinal(LPCWCH a, int na, LPCWCH b, int nb, BOOL ignoreCase);
LPWSTR CharUpperW(LPWSTR s);
LPWSTR CharLowerW(LPWSTR s);
DWORD CharUpperBuffW(LPWSTR s, DWORD n);
DWORD CharLowerBuffW(LPWSTR s, DWORD n);
BOOL IsCharAlphaW(WCHAR ch);
BOOL IsCharAlphaNumericW(WCHAR ch);
BOOL IsCharUpperW(WCHAR ch);
BOOL IsCharLowerW(WCHAR ch);
UINT GetACP(void);
UINT GetOEMCP(void);
BOOL GetCPInfo(UINT cp, LPCPINFO info);
BOOL IsDBCSLeadByte(BYTE ch);
BOOL IsDBCSLeadByteEx(UINT cp, BYTE ch);
int GetLocaleInfoW(LCID lcid, LCTYPE type, LPWSTR buf, int cch);
int GetLocaleInfoEx(LPCWSTR localeName, LCTYPE type, LPWSTR buf, int cch);
LCID GetUserDefaultLCID(void);
LANGID GetUserDefaultLangID(void);
LANGID GetSystemDefaultLangID(void);
LCID GetSystemDefaultLCID(void);
int LCMapStringW(LCID lcid, DWORD flags, LPCWSTR src, int cchSrc, LPWSTR dst, int cchDst);
BOOL IsValidCodePage(UINT cp);
BOOL IsValidLocale(LCID lcid, DWORD flags);
int GetNumberFormatW(LCID lcid, DWORD flags, LPCWSTR value, const NUMBERFMTW* fmt, LPWSTR buf, int cch);
int GetCurrencyFormatW(LCID lcid, DWORD flags, LPCWSTR value, const CURRENCYFMTW* fmt, LPWSTR buf, int cch);

HANDLE CreateThread(LPSECURITY_ATTRIBUTES sa, SIZE_T stack, LPTHREAD_START_ROUTINE start, LPVOID param, DWORD flags, LPDWORD id);
void ExitThread(DWORD code);
HANDLE GetCurrentThread(void);
DWORD GetCurrentThreadId(void);
HANDLE GetCurrentProcess(void);
DWORD GetCurrentProcessId(void);
BOOL TerminateProcess(HANDLE proc, UINT code);
BOOL GetExitCodeProcess(HANDLE proc, LPDWORD code);
BOOL GetExitCodeThread(HANDLE thread, LPDWORD code);
DWORD WaitForSingleObject(HANDLE h, DWORD ms);
DWORD WaitForMultipleObjects(DWORD count, const HANDLE* handles, BOOL waitAll, DWORD ms);
HANDLE CreateEventW(LPSECURITY_ATTRIBUTES sa, BOOL manual, BOOL initial, LPCWSTR name);
BOOL SetEvent(HANDLE h);
BOOL ResetEvent(HANDLE h);
BOOL PulseEvent(HANDLE h);
HANDLE CreateMutexW(LPSECURITY_ATTRIBUTES sa, BOOL owner, LPCWSTR name);
BOOL ReleaseMutex(HANDLE h);
HANDLE CreateSemaphoreW(LPSECURITY_ATTRIBUTES sa, LONG initial, LONG max, LPCWSTR name);
BOOL ReleaseSemaphore(HANDLE h, LONG release, LPLONG prev);
void InitializeCriticalSection(LPCRITICAL_SECTION cs);
BOOL InitializeCriticalSectionAndSpinCount(LPCRITICAL_SECTION cs, DWORD spin);
void EnterCriticalSection(LPCRITICAL_SECTION cs);
void LeaveCriticalSection(LPCRITICAL_SECTION cs);
void DeleteCriticalSection(LPCRITICAL_SECTION cs);
BOOL TryEnterCriticalSection(LPCRITICAL_SECTION cs);
DWORD TlsAlloc(void);
BOOL TlsFree(DWORD idx);
LPVOID TlsGetValue(DWORD idx);
BOOL TlsSetValue(DWORD idx, LPVOID val);
BOOL DuplicateHandle(HANDLE srcProc, HANDLE src, HANDLE dstProc, LPHANDLE dst, DWORD access, BOOL inherit, DWORD options);
BOOL SetHandleInformation(HANDLE h, DWORD mask, DWORD flags);
BOOL GetHandleInformation(HANDLE h, LPDWORD flags);
DWORD ResumeThread(HANDLE h);
DWORD SuspendThread(HANDLE h);
BOOL SetThreadPriority(HANDLE h, int prio);
int GetThreadPriority(HANDLE h);
void ExitProcess(UINT code);
BOOL QueryPerformanceCounter(LARGE_INTEGER* li);
BOOL QueryPerformanceFrequency(LARGE_INTEGER* li);
BOOL GetVersionExW(LPOSVERSIONINFOW vi);
DWORD GetVersion(void);
void GetSystemInfo(LPSYSTEM_INFO si);
BOOL GetComputerNameW(LPWSTR buf, LPDWORD n);
BOOL GetUserNameW(LPWSTR buf, LPDWORD n);
UINT GetWindowsDirectoryW(LPWSTR buf, UINT n);
UINT GetSystemDirectoryW(LPWSTR buf, UINT n);
DWORD GetEnvironmentVariableW(LPCWSTR name, LPWSTR buf, DWORD n);
BOOL SetEnvironmentVariableW(LPCWSTR name, LPCWSTR value);
LPWSTR GetEnvironmentStringsW(void);
BOOL FreeEnvironmentStringsW(LPWSTR env);
DWORD ExpandEnvironmentStringsW(LPCWSTR src, LPWSTR dst, DWORD n);
BOOL CreateProcessW(LPCWSTR app, LPWSTR cmd, LPSECURITY_ATTRIBUTES pa, LPSECURITY_ATTRIBUTES ta,
	BOOL inherit, DWORD flags, LPVOID env, LPCWSTR dir, LPSTARTUPINFOW si, LPPROCESS_INFORMATION pi);
HINSTANCE ShellExecuteW(HWND hwnd, LPCWSTR verb, LPCWSTR file, LPCWSTR params, LPCWSTR dir, INT show);
BOOL ShellExecuteExW(SHELLEXECUTEINFOW* info);
BOOL GetBinaryTypeW(LPCWSTR app, LPDWORD type);
DWORD GetPriorityClass(HANDLE proc);
BOOL SetPriorityClass(HANDLE proc, DWORD cls);
BOOL IsWow64Process(HANDLE proc, PBOOL wow64);
BOOL IsDebuggerPresent(void);
void DebugBreak(void);
void OutputDebugStringW(LPCWSTR s);
void OutputDebugStringA(LPCSTR s);
LPTOP_LEVEL_EXCEPTION_FILTER SetUnhandledExceptionFilter(LPTOP_LEVEL_EXCEPTION_FILTER filter);
LONG UnhandledExceptionFilter(EXCEPTION_POINTERS* info);
void RaiseException(DWORD code, DWORD flags, DWORD nArgs, const ULONG_PTR* args);
PVOID AddVectoredExceptionHandler(ULONG first, PVECTORED_EXCEPTION_HANDLER handler);
ULONG RemoveVectoredExceptionHandler(PVOID handle);
void SetLastError(DWORD err);

HANDLE HeapCreate(DWORD opts, SIZE_T init, SIZE_T max);
BOOL HeapDestroy(HANDLE heap);
LPVOID HeapAlloc(HANDLE heap, DWORD flags, SIZE_T bytes);
LPVOID HeapReAlloc(HANDLE heap, DWORD flags, LPVOID mem, SIZE_T bytes);
BOOL HeapFree(HANDLE heap, DWORD flags, LPVOID mem);
SIZE_T HeapSize(HANDLE heap, DWORD flags, LPCVOID mem);
HANDLE GetProcessHeap(void);
HGLOBAL GlobalAlloc(UINT flags, SIZE_T bytes);
HGLOBAL GlobalReAlloc(HGLOBAL h, SIZE_T bytes, UINT flags);
HGLOBAL GlobalFree(HGLOBAL h);
LPVOID GlobalLock(HGLOBAL h);
BOOL GlobalUnlock(HGLOBAL h);
SIZE_T GlobalSize(HGLOBAL h);
UINT GlobalFlags(HGLOBAL h);
HLOCAL LocalAlloc(UINT flags, SIZE_T bytes);
HLOCAL LocalReAlloc(HLOCAL h, SIZE_T bytes, UINT flags);
HLOCAL LocalFree(HLOCAL h);
LPVOID LocalLock(HLOCAL h);
BOOL LocalUnlock(HLOCAL h);
SIZE_T LocalSize(HLOCAL h);
LPVOID VirtualAlloc(LPVOID addr, SIZE_T size, DWORD type, DWORD protect);
BOOL VirtualFree(LPVOID addr, SIZE_T size, DWORD type);
BOOL VirtualProtect(LPVOID addr, SIZE_T size, DWORD newProt, PDWORD oldProt);
SIZE_T VirtualQuery(LPCVOID addr, PMEMORY_BASIC_INFORMATION info, SIZE_T len);

HRSRC FindResourceW(HMODULE mod, LPCWSTR name, LPCWSTR type);
HRSRC FindResourceExW(HMODULE mod, LPCWSTR type, LPCWSTR name, WORD lang);
HGLOBAL LoadResource(HMODULE mod, HRSRC res);
LPVOID LockResource(HGLOBAL res);
DWORD SizeofResource(HMODULE mod, HRSRC res);
BOOL FreeResource(HGLOBAL res);
int LoadStringW(HINSTANCE inst, UINT id, LPWSTR buf, int max);

void GetLocalTime(LPSYSTEMTIME st);
void GetSystemTime(LPSYSTEMTIME st);
BOOL SystemTimeToFileTime(const SYSTEMTIME* st, LPFILETIME ft);
BOOL FileTimeToSystemTime(const FILETIME* ft, LPSYSTEMTIME st);
BOOL FileTimeToLocalFileTime(const FILETIME* ft, LPFILETIME lft);
BOOL LocalFileTimeToFileTime(const FILETIME* lft, LPFILETIME ft);
DWORD GetTimeZoneInformation(LPTIME_ZONE_INFORMATION tz);
int GetDateFormatW(LCID lcid, DWORD flags, const SYSTEMTIME* st, LPCWSTR fmt, LPWSTR buf, int cch);
int GetTimeFormatW(LCID lcid, DWORD flags, const SYSTEMTIME* st, LPCWSTR fmt, LPWSTR buf, int cch);
LONG CompareFileTime(const FILETIME* a, const FILETIME* b);
BOOL DosDateTimeToFileTime(WORD fatDate, WORD fatTime, LPFILETIME ft);
BOOL FileTimeToDosDateTime(const FILETIME* ft, LPWORD fatDate, LPWORD fatTime);
void GetSystemTimeAsFileTime(LPFILETIME ft);
BOOL SystemTimeToTzSpecificLocalTime(const TIME_ZONE_INFORMATION* tz, const SYSTEMTIME* st, LPSYSTEMTIME local);
ULONGLONG GetTickCount64(void);

DWORD GetLogicalDrives(void);
UINT GetDriveTypeW(LPCWSTR root);
BOOL GetVolumeInformationW(LPCWSTR root, LPWSTR vol, DWORD volCch, LPDWORD serial, LPDWORD maxComp,
	LPDWORD flags, LPWSTR fs, DWORD fsCch);
BOOL GetDiskFreeSpaceExW(LPCWSTR dir, PULARGE_INTEGER freeCaller, PULARGE_INTEGER total, PULARGE_INTEGER free);
DWORD GetLongPathNameW(LPCWSTR shortPath, LPWSTR buf, DWORD cch);
DWORD GetShortPathNameW(LPCWSTR longPath, LPWSTR buf, DWORD cch);
BOOL SetEndOfFile(HANDLE h);
BOOL DeviceIoControl(HANDLE h, DWORD code, LPVOID in, DWORD inSize, LPVOID out, DWORD outSize, LPDWORD ret, LPOVERLAPPED ov);
BOOL LockFile(HANDLE h, DWORD offLow, DWORD offHigh, DWORD lockLow, DWORD lockHigh);
BOOL UnlockFile(HANDLE h, DWORD offLow, DWORD offHigh, DWORD lockLow, DWORD lockHigh);
BOOL LockFileEx(HANDLE h, DWORD flags, DWORD reserved, DWORD lockLow, DWORD lockHigh, LPOVERLAPPED ov);
BOOL UnlockFileEx(HANDLE h, DWORD reserved, DWORD lockLow, DWORD lockHigh, LPOVERLAPPED ov);
BOOL ReadFileEx(HANDLE h, LPVOID buf, DWORD n, LPOVERLAPPED ov, LPOVERLAPPED_COMPLETION_ROUTINE cb);
BOOL WriteFileEx(HANDLE h, LPCVOID buf, DWORD n, LPOVERLAPPED ov, LPOVERLAPPED_COMPLETION_ROUTINE cb);
HANDLE CreateFileMappingW(HANDLE file, LPSECURITY_ATTRIBUTES sa, DWORD protect, DWORD maxHigh, DWORD maxLow, LPCWSTR name);
LPVOID MapViewOfFile(HANDLE mapping, DWORD access, DWORD offHigh, DWORD offLow, SIZE_T n);
BOOL UnmapViewOfFile(LPCVOID base);
BOOL FlushViewOfFile(LPCVOID base, SIZE_T n);

// ---- registry ----
LSTATUS RegOpenKeyExW(HKEY key, LPCWSTR sub, DWORD options, REGSAM sam, PHKEY result);
LSTATUS RegOpenKeyExA(HKEY key, LPCSTR sub, DWORD options, REGSAM sam, PHKEY result);
LSTATUS RegCreateKeyExW(HKEY key, LPCWSTR sub, DWORD reserved, LPWSTR cls, DWORD options, REGSAM sam,
	const LPSECURITY_ATTRIBUTES sa, PHKEY result, LPDWORD disp);
LSTATUS RegCloseKey(HKEY key);
LSTATUS RegQueryValueExW(HKEY key, LPCWSTR name, LPDWORD reserved, LPDWORD type, LPBYTE data, LPDWORD cb);
LSTATUS RegQueryValueExA(HKEY key, LPCSTR name, LPDWORD reserved, LPDWORD type, LPBYTE data, LPDWORD cb);
LSTATUS RegSetValueExW(HKEY key, LPCWSTR name, DWORD reserved, DWORD type, const BYTE* data, DWORD cb);
LSTATUS RegDeleteValueW(HKEY key, LPCWSTR name);
LSTATUS RegDeleteKeyW(HKEY key, LPCWSTR sub);
LSTATUS RegEnumKeyExW(HKEY key, DWORD index, LPWSTR name, LPDWORD cchName, LPDWORD reserved, LPWSTR cls, LPDWORD cchCls, PFILETIME lastWrite);
LSTATUS RegEnumValueW(HKEY key, DWORD index, LPWSTR name, LPDWORD cchName, LPDWORD reserved, LPDWORD type, LPBYTE data, LPDWORD cb);
LSTATUS RegQueryInfoKeyW(HKEY key, LPWSTR cls, LPDWORD cchCls, LPDWORD reserved, LPDWORD subKeys, LPDWORD maxSub,
	LPDWORD maxCls, LPDWORD values, LPDWORD maxValue, LPDWORD maxData, LPDWORD sec, PFILETIME lastWrite);
LSTATUS SHDeleteKeyW(HKEY key, LPCWSTR sub);
LSTATUS SHGetValueW(HKEY key, LPCWSTR sub, LPCWSTR value, DWORD* type, void* data, DWORD* cb);
LSTATUS SHSetValueW(HKEY key, LPCWSTR sub, LPCWSTR value, DWORD type, LPCVOID data, DWORD cb);

// ---- shlwapi paths ----
BOOL PathFileExistsW(LPCWSTR path);
BOOL PathIsDirectoryW(LPCWSTR path);
BOOL PathIsRelativeW(LPCWSTR path);
BOOL PathIsUNCW(LPCWSTR path);
BOOL PathIsURLW(LPCWSTR path);
LPWSTR PathFindFileNameW(LPCWSTR path);
LPWSTR PathFindExtensionW(LPCWSTR path);
BOOL PathRemoveFileSpecW(LPWSTR path);
void PathRemoveExtensionW(LPWSTR path);
void PathStripPathW(LPWSTR path);
LPWSTR PathAddBackslashW(LPWSTR path);
BOOL PathAppendW(LPWSTR path, LPCWSTR more);
LPWSTR PathCombineW(LPWSTR dest, LPCWSTR dir, LPCWSTR file);
BOOL PathCanonicalizeW(LPWSTR dest, LPCWSTR src);
BOOL PathQuoteSpacesW(LPWSTR path);
void PathUnquoteSpacesW(LPWSTR path);
BOOL PathMatchSpecW(LPCWSTR file, LPCWSTR spec);
BOOL PathIsPrefixW(LPCWSTR prefix, LPCWSTR path);
BOOL PathRelativePathToW(LPWSTR path, LPCWSTR from, DWORD fromAttr, LPCWSTR to, DWORD toAttr);
void PathRemoveBlanksW(LPWSTR path);
LPCWSTR PathFindNextComponentW(LPCWSTR path);
LPCWSTR PathSkipRootW(LPCWSTR path);
BOOL PathStripToRootW(LPWSTR path);
int PathCommonPrefixW(LPCWSTR a, LPCWSTR b, LPWSTR prefix);
BOOL PathCompactPathW(HDC hdc, LPWSTR path, UINT dx);
BOOL PathCompactPathExW(LPWSTR out, LPCWSTR src, UINT cch, DWORD flags);
int PathGetDriveNumberW(LPCWSTR path);
BOOL PathIsFileSpecW(LPCWSTR path);
BOOL PathIsRootW(LPCWSTR path);
BOOL PathIsSameRootW(LPCWSTR a, LPCWSTR b);
BOOL PathRenameExtensionW(LPWSTR path, LPCWSTR ext);
void PathSetDlgItemPathW(HWND dlg, int id, LPCWSTR path);
LPWSTR StrCpyNW(LPWSTR dst, LPCWSTR src, int max);
LPWSTR StrCatW(LPWSTR dst, LPCWSTR src);
LPWSTR StrStrW(LPCWSTR hay, LPCWSTR needle);
LPWSTR StrStrIW(LPCWSTR hay, LPCWSTR needle);
int StrCmpW(LPCWSTR a, LPCWSTR b);
int StrCmpIW(LPCWSTR a, LPCWSTR b);
int StrCmpNW(LPCWSTR a, LPCWSTR b, int n);
int StrCmpNIW(LPCWSTR a, LPCWSTR b, int n);
BOOL StrTrimW(LPWSTR s, LPCWSTR trim);
int StrToIntW(LPCWSTR s);
BOOL StrToInt64ExW(LPCWSTR s, DWORD flags, LONGLONG* ret);
LPWSTR StrDupW(LPCWSTR s);
LPWSTR StrRChrW(LPCWSTR start, LPCWSTR end, WCHAR ch);
LPWSTR StrChrW(LPCWSTR s, WCHAR ch);
LPWSTR StrRStrIW(LPCWSTR hay, LPCWSTR end, LPCWSTR needle);
int wvnsprintfW(LPWSTR buf, int cch, LPCWSTR fmt, va_list args);
int wnsprintfW(LPWSTR buf, int cch, LPCWSTR fmt, ...);
HRESULT StringCchCopyW(STRSAFE_LPWSTR dest, size_t cch, STRSAFE_LPCWSTR src);
HRESULT StringCchCatW(STRSAFE_LPWSTR dest, size_t cch, STRSAFE_LPCWSTR src);
HRESULT StringCchPrintfW(STRSAFE_LPWSTR dest, size_t cch, STRSAFE_LPCWSTR fmt, ...);
HRESULT StringCchVPrintfW(STRSAFE_LPWSTR dest, size_t cch, STRSAFE_LPCWSTR fmt, va_list args);
HRESULT StringCchLengthW(STRSAFE_LPCWSTR psz, size_t cchMax, size_t* length);

// ---- shell ----
HRESULT SHGetFolderPathW(HWND hwnd, int csidl, HANDLE token, DWORD flags, LPWSTR path);
BOOL SHGetSpecialFolderPathW(HWND hwnd, LPWSTR path, int csidl, BOOL create);
HRESULT SHGetKnownFolderPath(REFKNOWNFOLDERID rfid, DWORD flags, HANDLE token, PWSTR* path);
LPITEMIDLIST SHBrowseForFolderW(LPBROWSEINFOW bi);
BOOL SHGetPathFromIDListW(LPCITEMIDLIST pidl, LPWSTR path);
int SHFileOperationW(LPSHFILEOPSTRUCTW op);
DWORD_PTR SHGetFileInfoW(LPCWSTR path, DWORD attrs, SHFILEINFOW* info, UINT size, UINT flags);
UINT DragQueryFileW(HDROP drop, UINT i, LPWSTR buf, UINT cch);
void DragFinish(HDROP drop);
void DragAcceptFiles(HWND h, BOOL accept);
HRESULT SHCreateItemFromParsingName(PCWSTR path, IBindCtx* ctx, REFIID riid, void** ppv);
LPWSTR* CommandLineToArgvW(LPCWSTR cmd, int* argc);
UINT SHAppBarMessage(DWORD dwMessage, PAPPBARDATA data);
UINT ExtractIconExW(LPCWSTR file, int index, HICON* large, HICON* small, UINT n);
int SHCreateDirectoryExW(HWND hwnd, LPCWSTR path, const SECURITY_ATTRIBUTES* sa);
HRESULT SHParseDisplayName(PCWSTR name, IBindCtx* ctx, PIDLIST_ABSOLUTE* pidl, SFGAOF attrs, SFGAOF* out);
void CoTaskMemFree(LPVOID pv);
LPVOID CoTaskMemAlloc(SIZE_T cb);
LPVOID CoTaskMemRealloc(LPVOID pv, SIZE_T cb);
HRESULT CoInitialize(LPVOID reserved);
HRESULT CoInitializeEx(LPVOID reserved, DWORD coInit);
void CoUninitialize(void);
HRESULT OleInitialize(LPVOID reserved);
void OleUninitialize(void);
HRESULT CoCreateInstance(REFCLSID clsid, LPUNKNOWN outer, DWORD ctx, REFIID iid, LPVOID* ppv);
void CoTaskMemFree(LPVOID pv);

// ---- common controls ----
void InitCommonControls(void);
BOOL InitCommonControlsEx(const INITCOMMONCONTROLSEX* icc);
HIMAGELIST ImageList_Create(int cx, int cy, UINT flags, int initial, int grow);
BOOL ImageList_Destroy(HIMAGELIST himl);
int ImageList_Add(HIMAGELIST himl, HBITMAP bmp, HBITMAP mask);
int ImageList_AddMasked(HIMAGELIST himl, HBITMAP bmp, COLORREF mask);
int ImageList_AddIcon(HIMAGELIST himl, HICON icon);
int ImageList_ReplaceIcon(HIMAGELIST himl, int i, HICON icon);
BOOL ImageList_Remove(HIMAGELIST himl, int i);
#ifndef ImageList_RemoveAll
#define ImageList_RemoveAll(himl) ImageList_Remove((himl), -1)
#endif
BOOL ImageList_Draw(HIMAGELIST himl, int i, HDC hdc, int x, int y, UINT style);
BOOL ImageList_DrawEx(HIMAGELIST himl, int i, HDC hdc, int x, int y, int dx, int dy, COLORREF bk, COLORREF fg, UINT style);
HICON ImageList_GetIcon(HIMAGELIST himl, int i, UINT flags);
int ImageList_GetImageCount(HIMAGELIST himl);
BOOL ImageList_GetIconSize(HIMAGELIST himl, int* cx, int* cy);
BOOL ImageList_SetIconSize(HIMAGELIST himl, int cx, int cy);
COLORREF ImageList_SetBkColor(HIMAGELIST himl, COLORREF bk);
COLORREF ImageList_GetBkColor(HIMAGELIST himl);
HIMAGELIST ImageList_Duplicate(HIMAGELIST himl);
HIMAGELIST ImageList_LoadImageW(HINSTANCE inst, LPCWSTR name, int cx, int grow, COLORREF mask, UINT type, UINT flags);
HWND CreateStatusWindowW(LONG style, LPCWSTR text, HWND parent, UINT id);
HWND CreateToolbarEx(HWND parent, DWORD style, UINT id, int nBitmaps, HINSTANCE inst, UINT_PTR bmID,
	LPCTBBUTTON btns, int nButtons, int dxButton, int dyButton, int dxBitmap, int dyBitmap, UINT structSize);

// ---- theme / dwm / dpi ----
HTHEME OpenThemeData(HWND h, LPCWSTR classList);
HRESULT CloseThemeData(HTHEME theme);
HRESULT DrawThemeBackground(HTHEME theme, HDC hdc, int part, int state, LPCRECT rc, LPCRECT clip);
HRESULT DrawThemeText(HTHEME theme, HDC hdc, int part, int state, LPCWSTR text, int cch, DWORD flags, DWORD flags2, LPCRECT rc);
HRESULT GetThemeColor(HTHEME theme, int part, int state, int prop, COLORREF* color);
HRESULT SetWindowTheme(HWND h, LPCWSTR subApp, LPCWSTR idList);
BOOL IsAppThemed(void);
BOOL IsThemeActive(void);
HRESULT BufferedPaintInit(void);
HRESULT BufferedPaintUnInit(void);
HPAINTBUFFER BeginBufferedPaint(HDC target, const RECT* rc, BP_BUFFERFORMAT fmt, BP_PAINTPARAMS* params, HDC* hdc);
HRESULT EndBufferedPaint(HPAINTBUFFER buf, BOOL update);
HRESULT GetBufferedPaintBits(HPAINTBUFFER buf, RGBQUAD** bits, int* row);
UINT GetDpiForWindow(HWND h);
UINT GetDpiForSystem(void);
BOOL SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT value);
HRESULT SetProcessDpiAwareness(PROCESS_DPI_AWARENESS value);
BOOL SetProcessDPIAware(void);
int GetSystemMetricsForDpi(int idx, UINT dpi);
BOOL AdjustWindowRectExForDpi(LPRECT rc, DWORD style, BOOL menu, DWORD exStyle, UINT dpi);
BOOL EnableNonClientDpiScaling(HWND h);
int GetDpiForMonitor(HMONITOR mon, MONITOR_DPI_TYPE type, UINT* dx, UINT* dy);
BOOL SystemParametersInfoForDpi(UINT action, UINT uiParam, PVOID pvParam, UINT fWinIni, UINT dpi);

HRESULT DwmSetWindowAttribute(HWND h, DWORD attr, LPCVOID data, DWORD cb);
HRESULT DwmGetWindowAttribute(HWND h, DWORD attr, PVOID data, DWORD cb);
HRESULT DwmExtendFrameIntoClientArea(HWND h, const MARGINS* margins);
HRESULT DwmIsCompositionEnabled(BOOL* enabled);
HRESULT DwmFlush(void);
BOOL IsCompositionActive(void);

// ---- crypto / bcrypt ----
BOOL CryptAcquireContextW(HCRYPTPROV* prov, LPCWSTR container, LPCWSTR provider, DWORD provType, DWORD flags);
BOOL CryptCreateHash(HCRYPTPROV prov, ALG_ID alg, HCRYPTKEY key, DWORD flags, HCRYPTHASH* hash);
BOOL CryptHashData(HCRYPTHASH hash, const BYTE* data, DWORD len, DWORD flags);
BOOL CryptGetHashParam(HCRYPTHASH hash, DWORD param, BYTE* data, DWORD* len, DWORD flags);
BOOL CryptDestroyHash(HCRYPTHASH hash);
BOOL CryptReleaseContext(HCRYPTPROV prov, DWORD flags);
NTSTATUS BCryptOpenAlgorithmProvider(BCRYPT_ALG_HANDLE* alg, LPCWSTR id, LPCWSTR impl, ULONG flags);
NTSTATUS BCryptGetProperty(BCRYPT_HANDLE obj, LPCWSTR prop, PUCHAR out, ULONG cb, ULONG* result, ULONG flags);
NTSTATUS BCryptCreateHash(BCRYPT_ALG_HANDLE alg, BCRYPT_HASH_HANDLE* hash, PUCHAR obj, ULONG cbObj, PUCHAR secret, ULONG cbSecret, ULONG flags);
NTSTATUS BCryptHashData(BCRYPT_HASH_HANDLE hash, PUCHAR data, ULONG cb, ULONG flags);
NTSTATUS BCryptFinishHash(BCRYPT_HASH_HANDLE hash, PUCHAR out, ULONG cb, ULONG flags);
NTSTATUS BCryptDestroyHash(BCRYPT_HASH_HANDLE hash);
NTSTATUS BCryptCloseAlgorithmProvider(BCRYPT_ALG_HANDLE alg, ULONG flags);

// ---- imm ----
HIMC ImmGetContext(HWND h);
BOOL ImmReleaseContext(HWND h, HIMC imc);
LONG ImmGetCompositionStringW(HIMC imc, DWORD index, LPVOID buf, DWORD bufLen);
BOOL ImmSetCompositionWindow(HIMC imc, LPCOMPOSITIONFORM form);
BOOL ImmSetCandidateWindow(HIMC imc, LPCANDIDATEFORM form);
BOOL ImmNotifyIME(HIMC imc, DWORD action, DWORD index, DWORD value);

// ---- inet / version / misc ----
BOOL InternetGetConnectedState(LPDWORD flags, DWORD reserved);
BOOL IsNetworkAlive(LPDWORD flags);
BOOL GetFileVersionInfoW(LPCWSTR path, DWORD handle, DWORD len, LPVOID data);
DWORD GetFileVersionInfoSizeW(LPCWSTR path, LPDWORD handle);
BOOL VerQueryValueW(LPCVOID block, LPCWSTR sub, LPVOID* buf, PUINT len);
BOOL WinVerifyTrust(HWND hwnd, GUID* action, LPVOID data);
LONG WinVerifyTrustEx(HWND hwnd, GUID* action, WINTRUST_DATA* data);

DWORD GetSysColor(int idx);
HBRUSH GetSysColorBrush(int idx);
BOOL SetSysColors(int c, const INT* idx, const COLORREF* colors);
int GetSystemMetricsForDpi(int idx, UINT dpi);

BOOL GetCursorInfo(PCURSORINFO pci);
HCURSOR LoadCursorFromFileW(LPCWSTR name);
BOOL ClipCursor(const RECT* rc);
BOOL GetClipCursor(LPRECT rc);
int ShowCursor(BOOL show);

UINT DragQueryFileW(HDROP drop, UINT i, LPWSTR buf, UINT cch);
BOOL DragQueryPoint(HDROP drop, LPPOINT pt);

BOOL GetTextExtentPointW(HDC hdc, LPCWSTR s, int c, LPSIZE sz);
UINT SetTextCharacterExtra(HDC hdc, int extra);

BOOL WinHelpW(HWND h, LPCWSTR help, UINT cmd, ULONG_PTR data);
HWND HtmlHelpW(HWND h, LPCWSTR file, UINT cmd, DWORD_PTR data);

int MulDiv(int n, int num, int den);
void DebugBreak(void);

DWORD timeGetTime(void);
MMRESULT timeBeginPeriod(UINT period);
MMRESULT timeEndPeriod(UINT period);

BOOL GetProcessTimes(HANDLE proc, LPFILETIME create, LPFILETIME exit, LPFILETIME kernel, LPFILETIME user);
BOOL GetSystemTimes(LPFILETIME idle, LPFILETIME kernel, LPFILETIME user);
BOOL GlobalMemoryStatusEx(LPMEMORYSTATUSEX buf);

UINT SetErrorMode(UINT mode);
DWORD GetFileType(HANDLE h);

BOOL PeekNamedPipe(HANDLE pipe, LPVOID buf, DWORD n, LPDWORD read, LPDWORD avail, LPDWORD left);
BOOL CreatePipe(PHANDLE read, PHANDLE write, LPSECURITY_ATTRIBUTES sa, DWORD size);

int wvsprintfW(LPWSTR buf, LPCWSTR fmt, va_list args);
int wsprintfW(LPWSTR buf, LPCWSTR fmt, ...);
int wvsprintfA(LPSTR buf, LPCSTR fmt, va_list args);
int wsprintfA(LPSTR buf, LPCSTR fmt, ...);

BOOL GetWindowInfo(HWND h, PWINDOWINFO pwi);
BOOL GetTitleBarInfo(HWND h, PTITLEBARINFO pti);
BOOL GetComboBoxInfo(HWND h, PCOMBOBOXINFO pcbi);
UINT GetCaretBlinkTime(void);
BOOL SetCaretBlinkTime(UINT ms);
BOOL CreateCaret(HWND hwnd, HBITMAP bmp, int w, int height);
BOOL DestroyCaret(void);
BOOL ShowCaret(HWND h);
BOOL HideCaret(HWND h);
BOOL SetCaretPos(int x, int y);
BOOL GetCaretPos(LPPOINT pt);

int CopyAcceleratorTableW(HACCEL acc, LPACCEL out, int count);

BOOL EnumResourceNamesW(HMODULE mod, LPCWSTR type, ENUMRESNAMEPROCW proc, LONG_PTR param);
BOOL EnumResourceTypesW(HMODULE mod, ENUMRESTYPEPROCW proc, LONG_PTR param);

DWORD SearchPathW(LPCWSTR path, LPCWSTR file, LPCWSTR ext, DWORD bufLen, LPWSTR buf, LPWSTR* filePart);
DWORD GetDllDirectoryW(DWORD n, LPWSTR buf);
BOOL SetDllDirectoryW(LPCWSTR path);

BOOL GetProcessMemoryInfo(HANDLE proc, PPROCESS_MEMORY_COUNTERS ppsmemCounters, DWORD cb);
BOOL MiniDumpWriteDump(HANDLE proc, DWORD pid, HANDLE file, MINIDUMP_TYPE type,
	PMINIDUMP_EXCEPTION_INFORMATION ex, PMINIDUMP_USER_STREAM_INFORMATION user, PMINIDUMP_CALLBACK_INFORMATION cb);

BOOL VerifyVersionInfoW(LPOSVERSIONINFOEXW vi, DWORD typeMask, DWORDLONG cond);
ULONGLONG VerSetConditionMask(ULONGLONG mask, DWORD type, BYTE op);

COLORREF GetThemeSysColor(HTHEME theme, int idx);
BOOL EnableThemeDialogTexture(HWND h, DWORD flags);

int GetKeyNameTextW(LONG lParam, LPWSTR buf, int cch);
UINT SendInput(UINT n, LPINPUT pInputs, int cb);
void keybd_event(BYTE vk, BYTE scan, DWORD flags, ULONG_PTR extra);
void mouse_event(DWORD flags, DWORD dx, DWORD dy, DWORD data, ULONG_PTR extra);
HWND GetMessagePosHwnd(void);
DWORD GetMessagePos(void);
LONG GetMessageTime(void);
LPARAM GetMessageExtraInfo(void);

BOOL AttachThreadInput(DWORD attach, DWORD attachTo, BOOL fAttach);
HWND GetShellWindow(void);
BOOL SetProcessWorkingSetSize(HANDLE proc, SIZE_T min, SIZE_T max);

HRESULT SHGetStockIconInfo(SHSTOCKICONID siid, UINT flags, SHSTOCKICONINFO* info);
int StrFromTimeIntervalW(LPWSTR buf, UINT cch, DWORD ms, int digits);

BOOL IsWindows10OrGreater(void);
BOOL IsWindows11OrGreater(void);

HHOOK SetWindowsHookExW(int idHook, HOOKPROC proc, HINSTANCE inst, DWORD tid);
HHOOK SetWindowsHookExA(int idHook, HOOKPROC proc, HINSTANCE inst, DWORD tid);
BOOL UnhookWindowsHookEx(HHOOK hook);
LRESULT CallNextHookEx(HHOOK hook, int code, WPARAM wParam, LPARAM lParam);

void ColorRGBToHLS(COLORREF clrRGB, WORD* hue, WORD* luminance, WORD* saturation);
COLORREF ColorHLSToRGB(WORD hue, WORD luminance, WORD saturation);

#ifdef __cplusplus
}

// C++ helpers that Windows headers provide as macros/inlines
inline HWND CreateWindowEx(DWORD ex, LPCWSTR cls, LPCWSTR title, DWORD style,
	int x, int y, int w, int h, HWND parent, HMENU menu, HINSTANCE inst, LPVOID param)
{
	return CreateWindowExW(ex, cls, title, style, x, y, w, h, parent, menu, inst, param);
}

inline LONG InterlockedIncrement(LONG volatile* p)
{
	return __atomic_add_fetch(p, 1, __ATOMIC_SEQ_CST);
}
inline LONG InterlockedDecrement(LONG volatile* p)
{
	return __atomic_sub_fetch(p, 1, __ATOMIC_SEQ_CST);
}
inline LONG InterlockedExchange(LONG volatile* p, LONG v)
{
	LONG old;
	__atomic_exchange(p, &v, &old, __ATOMIC_SEQ_CST);
	return old;
}
inline LONG InterlockedCompareExchange(LONG volatile* p, LONG n, LONG c)
{
	__atomic_compare_exchange_n(p, &c, n, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
	return c;
}
inline LONG InterlockedExchangeAdd(LONG volatile* p, LONG v)
{
	return __atomic_fetch_add(p, v, __ATOMIC_SEQ_CST);
}

#endif // __cplusplus

// UNICODE aliases
#define RegisterClass RegisterClassW
#define RegisterClassEx RegisterClassExW
#define UnregisterClass UnregisterClassW
#define CreateWindowEx CreateWindowExW
#define CreateWindow CreateWindowW
#define GetWindowLongPtr GetWindowLongPtrW
#define SetWindowLongPtr SetWindowLongPtrW
#define GetWindowLong GetWindowLongW
#define SetWindowLong SetWindowLongW
#define GetClassLongPtr GetClassLongPtrW
#define SetClassLongPtr SetClassLongPtrW
#define SetDlgItemText SetDlgItemTextW
#define GetDlgItemText GetDlgItemTextW
#define SetWindowsHookEx SetWindowsHookExW
#define FindWindow FindWindowW
#define FindWindowEx FindWindowExW
#define GetWindowText GetWindowTextW
#define GetWindowTextLength GetWindowTextLengthW
#define SetWindowText SetWindowTextW
#define SystemParametersInfo SystemParametersInfoW
#define MessageBox MessageBoxW
#define SendMessage SendMessageW
#define SendMessageTimeout SendMessageTimeoutW
#define PostMessage PostMessageW
#define PostThreadMessage PostThreadMessageW
#define GetMessage GetMessageW
#define PeekMessage PeekMessageW
#define DispatchMessage DispatchMessageW
#define TranslateAccelerator TranslateAcceleratorW
#define IsDialogMessage IsDialogMessageW
#define CallWindowProc CallWindowProcW
#define DefWindowProc DefWindowProcW
#define DefDlgProc DefDlgProcW
#define GetClassName GetClassNameW
#define LoadCursor LoadCursorW
#define LoadIcon LoadIconW
#define LoadImage LoadImageW
#define LoadBitmap LoadBitmapW
#define LoadMenu LoadMenuW
#define AppendMenu AppendMenuW
#define InsertMenu InsertMenuW
#define InsertMenuItem InsertMenuItemW
#define GetMenuString GetMenuStringW
#define GetMenuItemInfo GetMenuItemInfoW
#define SetMenuItemInfo SetMenuItemInfoW
#define ModifyMenu ModifyMenuW
#define CreateAcceleratorTable CreateAcceleratorTableW
#define CopyAcceleratorTable CopyAcceleratorTableW
#define LoadAccelerators LoadAcceleratorsW
#define SetProp SetPropW
#define GetProp GetPropW
#define RemoveProp RemovePropW
#define LoadString LoadStringW
#define DrawText DrawTextW
#define DrawTextEx DrawTextExW
#define DrawState DrawStateW
#define GetMonitorInfo GetMonitorInfoW
#define EnumDisplayDevices EnumDisplayDevicesW
#define EnumDisplaySettings EnumDisplaySettingsW
#define RegisterWindowMessage RegisterWindowMessageW
#define RegisterClipboardFormat RegisterClipboardFormatW
#define GetClipboardFormatName GetClipboardFormatNameW
#define DialogBoxParam DialogBoxParamW
#define DialogBoxIndirectParam DialogBoxIndirectParamW
#define CreateDialogParam CreateDialogParamW
#define CreateDialogIndirectParam CreateDialogIndirectParamW
#define SendDlgItemMessage SendDlgItemMessageW
#define GetOpenFileName GetOpenFileNameW
#define GetSaveFileName GetSaveFileNameW
#define ChooseColor ChooseColorW
#define ChooseFont ChooseFontW
#define FindText FindTextW
#define ReplaceText ReplaceTextW
#define PrintDlg PrintDlgW
#define PageSetupDlg PageSetupDlgW
#define CreateFont CreateFontW
#define CreateFontIndirect CreateFontIndirectW
#define TextOut TextOutW
#define ExtTextOut ExtTextOutW
#define GetTextExtentPoint32 GetTextExtentPoint32W
#define GetTextMetrics GetTextMetricsW
#define GetObject GetObjectW
#define EnumFontFamiliesEx EnumFontFamiliesExW
#define GetGlyphIndices GetGlyphIndicesW
#define GetCharWidth32 GetCharWidth32W
#define GetTextExtentExPoint GetTextExtentExPointW
#define GetOutlineTextMetrics GetOutlineTextMetricsW
#define AddFontResourceEx AddFontResourceExW
#define RemoveFontResourceEx RemoveFontResourceExW
#define CreateDC CreateDCW
#define CreateIC CreateICW
#define StartDoc StartDocW
#define CreateFile CreateFileW
#define GetFileAttributes GetFileAttributesW
#define SetFileAttributes SetFileAttributesW
#define GetFinalPathNameByHandle GetFinalPathNameByHandleW
#define FindFirstFile FindFirstFileW
#define FindNextFile FindNextFileW
#define DeleteFile DeleteFileW
#define MoveFile MoveFileW
#define MoveFileEx MoveFileExW
#define CopyFile CopyFileW
#define CreateDirectory CreateDirectoryW
#define RemoveDirectory RemoveDirectoryW
#define GetCurrentDirectory GetCurrentDirectoryW
#define SetCurrentDirectory SetCurrentDirectoryW
#define GetFullPathName GetFullPathNameW
#define GetTempPath GetTempPathW
#define GetTempFileName GetTempFileNameW
#define GetModuleFileName GetModuleFileNameW
#define GetModuleHandle GetModuleHandleW
#define GetModuleHandleEx GetModuleHandleExW
#define LoadLibrary LoadLibraryW
#define LoadLibraryEx LoadLibraryExW
#define FormatMessage FormatMessageW
#define lstrlen lstrlenW
#define lstrcpy lstrcpyW
#define lstrcpyn lstrcpynW
#define lstrcat lstrcatW
#define lstrcmp lstrcmpW
#define lstrcmpi lstrcmpiW
#define CompareString CompareStringW
#define CharUpper CharUpperW
#define CharLower CharLowerW
#define CharUpperBuff CharUpperBuffW
#define CharLowerBuff CharLowerBuffW
#define IsCharAlpha IsCharAlphaW
#define IsCharAlphaNumeric IsCharAlphaNumericW
#define IsCharUpper IsCharUpperW
#define IsCharLower IsCharLowerW
#define GetLocaleInfo GetLocaleInfoW
#define LCMapString LCMapStringW
#define GetNumberFormat GetNumberFormatW
#define GetCurrencyFormat GetCurrencyFormatW
#define CreateEvent CreateEventW
#define CreateMutex CreateMutexW
#define CreateSemaphore CreateSemaphoreW
#define GetVersionEx GetVersionExW
#define GetComputerName GetComputerNameW
#define GetUserName GetUserNameW
#define GetWindowsDirectory GetWindowsDirectoryW
#define GetSystemDirectory GetSystemDirectoryW
#define GetEnvironmentVariable GetEnvironmentVariableW
#define SetEnvironmentVariable SetEnvironmentVariableW
#define GetEnvironmentStrings GetEnvironmentStringsW
#define FreeEnvironmentStrings FreeEnvironmentStringsW
#define ExpandEnvironmentStrings ExpandEnvironmentStringsW
#define CreateProcess CreateProcessW
#define ShellExecute ShellExecuteW
#define ShellExecuteEx ShellExecuteExW
#define GetBinaryType GetBinaryTypeW
#define OutputDebugString OutputDebugStringW
#define FindResource FindResourceW
#define FindResourceEx FindResourceExW
#define GetDateFormat GetDateFormatW
#define GetTimeFormat GetTimeFormatW
#define GetDriveType GetDriveTypeW
#define GetVolumeInformation GetVolumeInformationW
#define GetDiskFreeSpaceEx GetDiskFreeSpaceExW
#define GetLongPathName GetLongPathNameW
#define GetShortPathName GetShortPathNameW
#define CreateFileMapping CreateFileMappingW
#define RegOpenKeyEx RegOpenKeyExW
#define RegCreateKeyEx RegCreateKeyExW
#define RegQueryValueEx RegQueryValueExW
#define RegSetValueEx RegSetValueExW
#define RegDeleteValue RegDeleteValueW
#define RegDeleteKey RegDeleteKeyW
#define RegEnumKeyEx RegEnumKeyExW
#define RegEnumValue RegEnumValueW
#define RegQueryInfoKey RegQueryInfoKeyW
#define SHDeleteKey SHDeleteKeyW
#define SHGetValue SHGetValueW
#define SHSetValue SHSetValueW
#define PathFileExists PathFileExistsW
#define PathIsDirectory PathIsDirectoryW
#define PathIsRelative PathIsRelativeW
#define PathIsUNC PathIsUNCW
#define PathIsURL PathIsURLW
#define PathFindFileName PathFindFileNameW
#define PathFindExtension PathFindExtensionW
#define PathRemoveFileSpec PathRemoveFileSpecW
#define PathRemoveExtension PathRemoveExtensionW
#define PathStripPath PathStripPathW
#define PathAddBackslash PathAddBackslashW
#define PathAppend PathAppendW
#define PathCombine PathCombineW
#define PathCanonicalize PathCanonicalizeW
#define PathQuoteSpaces PathQuoteSpacesW
#define PathUnquoteSpaces PathUnquoteSpacesW
#define PathMatchSpec PathMatchSpecW
#define PathIsPrefix PathIsPrefixW
#define PathRelativePathTo PathRelativePathToW
#define PathRemoveBlanks PathRemoveBlanksW
#define PathFindNextComponent PathFindNextComponentW
#define PathSkipRoot PathSkipRootW
#define PathStripToRoot PathStripToRootW
#define PathCommonPrefix PathCommonPrefixW
#define PathCompactPath PathCompactPathW
#define PathCompactPathEx PathCompactPathExW
#define PathGetDriveNumber PathGetDriveNumberW
#define PathIsFileSpec PathIsFileSpecW
#define PathIsRoot PathIsRootW
#define PathIsSameRoot PathIsSameRootW
#define PathRenameExtension PathRenameExtensionW
#define PathSetDlgItemPath PathSetDlgItemPathW
#define StrCpyN StrCpyNW
#define StrCat StrCatW
#define StrStr StrStrW
#define StrStrI StrStrIW
#define StrCmp StrCmpW
#define StrCmpI StrCmpIW
#define StrCmpN StrCmpNW
#define StrCmpNI StrCmpNIW
#define StrTrim StrTrimW
#define StrToInt StrToIntW
#define StrToInt64Ex StrToInt64ExW
#define StrDup StrDupW
#define StrRChr StrRChrW
#define StrChr StrChrW
#define SHGetFolderPath SHGetFolderPathW
#define SHGetSpecialFolderPath SHGetSpecialFolderPathW
#define SHBrowseForFolder SHBrowseForFolderW
#define SHGetPathFromIDList SHGetPathFromIDListW
#define SHFileOperation SHFileOperationW
#define SHGetFileInfo SHGetFileInfoW
#define DragQueryFile DragQueryFileW
#define ExtractIconEx ExtractIconExW
#define SHCreateDirectoryEx SHCreateDirectoryExW
#define CryptAcquireContext CryptAcquireContextW
#define ImmGetCompositionString ImmGetCompositionStringW
#define GetFileVersionInfo GetFileVersionInfoW
#define GetFileVersionInfoSize GetFileVersionInfoSizeW
#define VerQueryValue VerQueryValueW
#define LoadCursorFromFile LoadCursorFromFileW
#define WinHelp WinHelpW
#define HtmlHelp HtmlHelpW
#define wvsprintf wvsprintfW
#define wsprintf wsprintfW
#define EnumResourceNames EnumResourceNamesW
#define EnumResourceTypes EnumResourceTypesW
#define SearchPath SearchPathW
#define GetDllDirectory GetDllDirectoryW
#define SetDllDirectory SetDllDirectoryW
#define VerifyVersionInfo VerifyVersionInfoW
#define GetKeyNameText GetKeyNameTextW
#define ImageList_LoadImage ImageList_LoadImageW
#define CreateStatusWindow CreateStatusWindowW
#define StringCchCopy StringCchCopyW
#define StringCchCat StringCchCatW
#define StringCchPrintf StringCchPrintfW
#define StringCchVPrintf StringCchVPrintfW
#define StringCchLength StringCchLengthW
#define GlobalAddAtom GlobalAddAtomW
#define GlobalFindAtom GlobalFindAtomW

#ifndef CreateWindow
#define CreateWindowA(cls, title, style, x, y, w, h, parent, menu, inst, param) \
	CreateWindowExA(0, cls, title, style, x, y, w, h, parent, menu, inst, param)
#define CreateWindowW(cls, title, style, x, y, w, h, parent, menu, inst, param) \
	CreateWindowExW(0, cls, title, style, x, y, w, h, parent, menu, inst, param)
#endif

#define DialogBox(inst, tmpl, parent, proc) DialogBoxParam(inst, tmpl, parent, proc, 0)
#define CreateDialog(inst, tmpl, parent, proc) CreateDialogParam(inst, tmpl, parent, proc, 0)

#endif // NPP_WIN32_API_H
