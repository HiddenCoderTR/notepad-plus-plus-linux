#include "npp_platform.h"

#include <locale.h>
#include <vector>

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow);

int __argc = 0;
char** __argv = nullptr;
wchar_t** __wargv = nullptr;

static std::wstring g_cmdLine;
static std::vector<std::wstring> g_wargs;
static std::vector<wchar_t*> g_wargvPtrs;

int main(int argc, char** argv)
{
	setlocale(LC_ALL, "");
	nppInitPlatform();

	__argc = argc;
	__argv = argv;
	g_wargs.resize(argc);
	g_wargvPtrs.resize(argc + 1);
	for (int i = 0; i < argc; ++i)
	{
		g_wargs[i] = nppUtf8ToWide(argv[i] ? argv[i] : "");
		g_wargvPtrs[i] = g_wargs[i].data();
	}
	g_wargvPtrs[argc] = nullptr;
	__wargv = g_wargvPtrs.data();

	std::wstring cmd;
	for (int i = 1; i < argc; ++i)
	{
		if (i > 1) cmd += L" ";
		std::wstring a = g_wargs[i];
		if (a.find(L' ') != std::wstring::npos) cmd += L"\"" + a + L"\"";
		else cmd += a;
	}
	g_cmdLine = cmd;
	int show = SW_SHOW;
	return wWinMain((HINSTANCE)1, nullptr, g_cmdLine.empty() ? (LPWSTR)L"" : g_cmdLine.data(), show);
}
