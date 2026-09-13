#include "DarkMode.h"

bool g_darkModeSupported = false;
bool g_darkModeEnabled = false;

bool AllowDarkModeForWindow(HWND, bool) { return false; }
bool IsHighContrast() { return false; }
void RefreshTitleBarThemeColor(HWND) {}
void SetTitleBarThemeColor(HWND, BOOL) {}
bool IsColorSchemeChangeMessage(LPARAM) { return false; }
bool IsColorSchemeChangeMessage(UINT, LPARAM) { return false; }
void AllowDarkModeForApp(bool) {}
void EnableDarkScrollBarForWindowAndChildren(HWND) {}
void InitDarkMode() {}
void SetDarkMode(bool, bool) {}
bool IsWindows10() { return false; }
bool IsWindows11() { return false; }
DWORD GetWindowsBuildNumber() { return 0; }
bool HookThemeColor() noexcept { return false; }
void UnhookThemeColor() noexcept {}
void InitMB_GetString() noexcept {}
LPCWSTR MyMB_GetString(UINT) noexcept { return L""; }
bool HookClrGetSysColorBrush() noexcept { return false; }
size_t UnhookClrGetSysColorBrush() noexcept { return 0; }
