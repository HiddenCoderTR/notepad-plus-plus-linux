// Extra Win32 types, constants, and APIs required by Notepad++ sources.
#pragma once

#include <filesystem>

#ifndef ERROR_NO_ASSOCIATION
#define ERROR_NO_ASSOCIATION 1157
#endif

#ifndef LPTR
#define LPTR (LMEM_FIXED | LMEM_ZEROINIT)
#endif

#ifndef CLIPFORMAT
typedef WORD CLIPFORMAT;
#endif
#ifndef PCTSTR
typedef LPCTSTR PCTSTR;
#endif

#ifndef CLR_INVALID
#define CLR_INVALID 0xFFFFFFFF
#endif

#ifndef IMAGE_FILE_MACHINE_I386
#define IMAGE_FILE_MACHINE_UNKNOWN 0
#define IMAGE_FILE_MACHINE_I386    0x014c
#define IMAGE_FILE_MACHINE_AMD64   0x8664
#define IMAGE_FILE_MACHINE_ARM64   0xAA64
#endif

#ifndef APPCOMMAND_BROWSER_BACKWARD
#define APPCOMMAND_BROWSER_BACKWARD 1
#define APPCOMMAND_BROWSER_FORWARD  2
#define GET_APPCOMMAND_LPARAM(l) ((short)(HIWORD(l) & ~FAPPCOMMAND_MASK))
#ifndef FAPPCOMMAND_MASK
#define FAPPCOMMAND_MASK 0xF000
#endif
#endif

#ifndef MAKEPOINTS
#define MAKEPOINTS(l) (*((POINTS*)&(l)))
#endif
#ifndef POINTSTOPOINT
#define POINTSTOPOINT(pt, pts) ((pt).x = (LONG)(SHORT)LOWORD(*(LONG*)&(pts)), (pt).y = (LONG)(SHORT)HIWORD(*(LONG*)&(pts)))
#endif

#ifndef CWP_SKIPINVISIBLE
#define CWP_SKIPINVISIBLE 0x0001
#define CWP_SKIPDISABLED  0x0002
#define CWP_SKIPTRANSPARENT 0x0004
#endif

#ifndef BS_SPLITBUTTON
#define BS_SPLITBUTTON    0x0000000CL
#define BS_DEFSPLITBUTTON 0x0000000DL
#endif
#ifndef BST_HOT
#define BST_HOT 0x0200
#endif
#ifndef BCN_DROPDOWN
#define BCN_FIRST (0U-1250U)
#define BCN_DROPDOWN (BCN_FIRST + 0x0002)
#endif
#ifndef BCM_GETIDEALSIZE
#define BCM_FIRST 0x1600
#define BCM_GETIDEALSIZE (BCM_FIRST + 0x0001)
#endif
#ifndef Button_GetIdealSize
#define Button_GetIdealSize(hwnd, psize) \
	(BOOL)SNDMSG((hwnd), BCM_GETIDEALSIZE, 0, (LPARAM)(SIZE*)(psize))
#endif

#ifndef WC_LINK
#define WC_LINK L"SysLink"
#endif
#ifndef LM_SETITEM
#define LM_SETITEM (WM_USER + 0x302)
#endif
#ifndef LIF_ITEMINDEX
#define LIF_ITEMINDEX 0x00000001
#define LIF_STATE     0x00000002
#define LIS_DEFAULTCOLORS 0x0010
#endif

#ifndef TB_GETITEMDROPDOWNRECT
#define TB_GETITEMDROPDOWNRECT (WM_USER + 67)
#endif
#ifndef TB_SETCOLORSCHEME
#define TB_SETCOLORSCHEME CCM_SETCOLORSCHEME
#endif
#ifndef TBDDRET_DEFAULT
#define TBDDRET_DEFAULT   0
#define TBDDRET_NODEFAULT 1
#endif
#ifndef TBCDRF_NOBACKGROUND
#define TBCDRF_NOBACKGROUND 0x00000040
#define TBCDRF_USECDCOLORS  0x00000080
#endif
#ifndef RBN_CHEVRONPUSHED
#define RBN_FIRST (0U-831U)
#define RBN_HEIGHTCHANGE   (RBN_FIRST - 0)
#define RBN_CHEVRONPUSHED  (RBN_FIRST - 10)
#endif
#ifndef TTDT_INITIAL
#define TTDT_AUTOMATIC 0
#define TTDT_RESHOW    1
#define TTDT_AUTOPOP   2
#define TTDT_INITIAL   3
#endif

#ifndef MB_ICONMASK
#define MB_ICONMASK 0x000000F0L
#define MB_DEFMASK  0x00000F00L
#define MB_TYPEMASK 0x0000000FL
#endif

#ifndef IMR_RECONVERTSTRING
#define IMR_RECONVERTSTRING 0x0004
#endif

#ifndef WAIT_IO_COMPLETION
#define WAIT_IO_COMPLETION 0x000000C0L
#endif
#ifndef TOKEN_QUERY
#define TOKEN_QUERY 0x0008
#endif
#ifndef TokenIntegrityLevel
#define TokenIntegrityLevel 25
#endif
#ifndef SECURITY_MAX_SID_SIZE
#define SECURITY_MAX_SID_SIZE 68
#endif
#ifndef SECURITY_MANDATORY_MEDIUM_RID
#define SECURITY_MANDATORY_MEDIUM_RID 0x00002000
#endif
#ifndef PROCESS_QUERY_LIMITED_INFORMATION
#define PROCESS_QUERY_LIMITED_INFORMATION 0x1000
#endif

#ifndef ASSOCF_INIT_IGNOREUNKNOWN
#define ASSOCF_NONE 0x00000000
#define ASSOCF_INIT_NOREMAPCLSID 0x00000001
#define ASSOCF_INIT_BYEXENAME 0x00000002
#define ASSOCF_OPEN_BYEXENAME ASSOCF_INIT_BYEXENAME
#define ASSOCF_INIT_DEFAULTTOSTAR 0x00000004
#define ASSOCF_INIT_DEFAULTTOFOLDER 0x00000008
#define ASSOCF_NOUSERSETTINGS 0x00000010
#define ASSOCF_NOTRUNCATE 0x00000020
#define ASSOCF_VERIFY 0x00000040
#define ASSOCF_REMAPRUNDLL 0x00000080
#define ASSOCF_NOFIXUPS 0x00000100
#define ASSOCF_IGNOREUNKNOWN 0x00000400
#define ASSOCF_INIT_IGNOREUNKNOWN 0x00000400
#endif
#ifndef ASSOCSTR_COMMAND
#define ASSOCSTR_COMMAND 1
#endif

#ifndef STGM_READ
#define STGM_READ 0x00000000L
#endif
#ifndef SLGP_SHORTPATH
#define SLGP_SHORTPATH 0x1
#endif

#ifndef CERT_QUERY_OBJECT_FILE
#define CERT_QUERY_OBJECT_FILE 1
#define CERT_QUERY_CONTENT_FLAG_PKCS7_SIGNED_EMBED 0x00000400
#define CERT_QUERY_FORMAT_FLAG_BINARY 2
#define CERT_FIND_SUBJECT_CERT 0x000d0000
#define CERT_NAME_SIMPLE_DISPLAY_TYPE 4
#define CMSG_SIGNER_INFO_PARAM 6
#define PKCS_7_ASN_ENCODING 0x00010000
#define X509_ASN_ENCODING 0x00000001
#endif

#ifndef REPLACEFILE_IGNORE_MERGE_ERRORS
#define REPLACEFILE_IGNORE_MERGE_ERRORS 0x00000001
#define REPLACEFILE_IGNORE_ACL_ERRORS 0x00000002
#endif
#ifndef RRF_RT_REG_SZ
#define RRF_RT_REG_SZ    0x00000002
#define RRF_RT_REG_DWORD 0x00000010
#endif

#ifndef RESTART_NO_CRASH
#define RESTART_NO_CRASH 1
#define RESTART_NO_HANG  2
#define RESTART_NO_PATCH 4
#define RESTART_MAX_CMD_LINE 2048
#endif

#ifndef GetFileExInfoStandard
typedef enum _GET_FILEEX_INFO_LEVELS { GetFileExInfoStandard, GetFileExMaxInfoLevel } GET_FILEEX_INFO_LEVELS;
#endif

#ifndef PRODUCT_WEB_SERVER_CORE
#define PRODUCT_DATACENTER_SERVER_CORE 0x0000000C
#define PRODUCT_STANDARD_SERVER_CORE 0x0000000D
#define PRODUCT_ENTERPRISE_SERVER_CORE 0x0000000E
#define PRODUCT_WEB_SERVER_CORE 0x0000001D
#define PRODUCT_DATACENTER_SERVER_CORE_V 0x00000027
#define PRODUCT_STANDARD_SERVER_CORE_V 0x00000028
#define PRODUCT_ENTERPRISE_SERVER_CORE_V 0x00000029
#define PRODUCT_DATACENTER_A_SERVER_CORE 0x00000091
#define PRODUCT_STANDARD_A_SERVER_CORE 0x00000092
#define PRODUCT_SMALLBUSINESS_SERVER_PREMIUM_CORE 0x0000003F
#define PRODUCT_STANDARD_SERVER_SOLUTIONS_CORE 0x00000035
#define PRODUCT_STORAGE_ENTERPRISE_SERVER_CORE 0x0000002E
#define PRODUCT_STORAGE_EXPRESS_SERVER_CORE 0x0000002B
#define PRODUCT_STORAGE_STANDARD_SERVER_CORE 0x0000002C
#define PRODUCT_STORAGE_WORKGROUP_SERVER_CORE 0x0000002D
#endif

#ifndef VSCLASS_MENU
#define VSCLASS_MENU L"MENU"
#endif
#ifndef MENU_BARITEM
#define MENU_BARITEM 8
#endif
#ifndef MBI_NORMAL
#define MBI_NORMAL 1
#define MBI_HOT 2
#define MBI_PUSHED 3
#define MBI_DISABLED 4
#define MBI_DISABLEDHOT 5
#define MBI_DISABLEDPUSHED 6
#endif
#ifndef CBXSR_NORMAL
#define CBXSR_NORMAL 1
#define CBXSR_DISABLED 4
#endif
#ifndef CP_DROPDOWNBUTTONRIGHT
#define CP_DROPDOWNBUTTONRIGHT 6
#endif
#ifndef RBS_UNCHECKEDNORMAL
#define RBS_UNCHECKEDNORMAL 1
#define RBS_UNCHECKEDHOT 2
#define RBS_UNCHECKEDPRESSED 3
#define RBS_UNCHECKEDDISABLED 4
#endif
#ifndef PBFS_NORMAL
#define PBFS_NORMAL 1
#define PBFS_ERROR 2
#define PBFS_PAUSED 3
#endif
#ifndef PP_FILL
#define PP_FILL 5
#endif
#ifndef TBCD_CHANNEL
#define TBCD_CHANNEL 1
#define TBCD_THUMB 2
#endif
#ifndef BPAS_LINEAR
#define BPAS_LINEAR 1
#endif
#ifndef DTT_TEXTCOLOR
#define DTT_TEXTCOLOR (1UL << 0)
#define DTT_CALCRECT  (1UL << 9)
#endif
#ifndef TDN_DIALOG_CONSTRUCTED
#define TDN_DIALOG_CONSTRUCTED 5
#endif

typedef struct tagENUMLOGFONTEXW {
	LOGFONTW elfLogFont;
	WCHAR elfFullName[64];
	WCHAR elfStyle[32];
	WCHAR elfScript[32];
} ENUMLOGFONTEXW, ENUMLOGFONTEX, *LPENUMLOGFONTEXW, *LPENUMLOGFONTEX;

typedef struct tagCWPSTRUCT {
	LPARAM lParam;
	WPARAM wParam;
	UINT message;
	HWND hwnd;
} CWPSTRUCT, *PCWPSTRUCT, *LPCWPSTRUCT;

typedef struct tagTRACKMOUSEEVENT {
	DWORD cbSize;
	DWORD dwFlags;
	HWND hwndTrack;
	DWORD dwHoverTime;
} TRACKMOUSEEVENT, *LPTRACKMOUSEEVENT;

typedef struct tagMENUBARINFO {
	DWORD cbSize;
	RECT rcBar;
	HMENU hMenu;
	HWND hwndMenu;
	BOOL fBarFocused:1;
	BOOL fFocused:1;
} MENUBARINFO, *PMENUBARINFO, *LPMENUBARINFO;

typedef struct _DTTOPTS {
	DWORD dwSize;
	DWORD dwFlags;
	COLORREF crText;
	COLORREF crBorder;
	COLORREF crShadow;
	int iTextShadowType;
	POINT ptShadowOffset;
	int iBorderSize;
	int iFontPropId;
	int iColorPropId;
	int iStateId;
	BOOL fApplyOverlay;
	int iGlowSize;
	void* pfnDrawTextCallback;
	LPARAM lParam;
} DTTOPTS, *PDTTOPTS;

typedef struct _BP_ANIMATIONPARAMS {
	DWORD cbSize;
	DWORD dwFlags;
	DWORD style;
	DWORD dwDuration;
} BP_ANIMATIONPARAMS, *PBP_ANIMATIONPARAMS;
typedef HANDLE HANIMATIONBUFFER;

typedef struct tagCOLORSCHEME {
	DWORD dwSize;
	COLORREF clrBtnHighlight;
	COLORREF clrBtnShadow;
} COLORSCHEME, *LPCOLORSCHEME;

typedef struct tagREBARINFO {
	UINT cbSize;
	UINT fMask;
	HIMAGELIST himl;
} REBARINFO, *LPREBARINFO;

typedef struct tagNMREBARCHEVRON {
	NMHDR hdr;
	UINT uBand;
	UINT wID;
	LPARAM lParam;
	RECT rc;
	LPARAM lParamNM;
} NMREBARCHEVRON, *LPNMREBARCHEVRON;

typedef struct tagLITEM {
	UINT mask;
	int iLink;
	UINT state;
	UINT stateMask;
	WCHAR szID[48];
	WCHAR szUrl[2084];
} LITEM, *PLITEM;

typedef struct tagTOOLTIPTEXTW {
	NMHDR hdr;
	LPWSTR lpszText;
	WCHAR szText[80];
	HINSTANCE hinst;
	UINT uFlags;
} TOOLTIPTEXTW, TOOLTIPTEXT, *LPTOOLTIPTEXTW, *LPTOOLTIPTEXT;

#ifndef TCHITTESTINFO_TC_ALIAS
#define TCHITTESTINFO_TC_ALIAS
typedef TCHITTESTINFO TC_HITTESTINFO;
#endif

typedef struct tagRECONVERTSTRING {
	DWORD dwSize;
	DWORD dwVersion;
	DWORD dwStrLen;
	DWORD dwStrOffset;
	DWORD dwCompStrLen;
	DWORD dwCompStrOffset;
	DWORD dwTargetStrLen;
	DWORD dwTargetStrOffset;
} RECONVERTSTRING, *PRECONVERTSTRING;

typedef struct {
	DWORD dwStructSize;
	DWORD dwSchemeLength;
	DWORD nScheme;
	DWORD dwHostNameLength;
	DWORD nPort;
	DWORD dwUserNameLength;
	DWORD dwPasswordLength;
	DWORD dwUrlPathLength;
	DWORD dwExtraInfoLength;
	LPWSTR lpszScheme;
	LPWSTR lpszHostName;
	LPWSTR lpszUserName;
	LPWSTR lpszPassword;
	LPWSTR lpszUrlPath;
	LPWSTR lpszExtraInfo;
} URL_COMPONENTS, *LPURL_COMPONENTS;

typedef struct {
	DWORD dwSignature;
	DWORD dwStrucVersion;
	DWORD dwFileVersionMS;
	DWORD dwFileVersionLS;
	DWORD dwProductVersionMS;
	DWORD dwProductVersionLS;
	DWORD dwFileFlagsMask;
	DWORD dwFileFlags;
	DWORD dwFileOS;
	DWORD dwFileType;
	DWORD dwFileSubtype;
	DWORD dwFileDateMS;
	DWORD dwFileDateLS;
} VS_FIXEDFILEINFO;

#ifndef LPNMHEADER
#define LPNMHEADER LPNMHEADERW
#endif
#ifndef LPNMLVGETINFOTIP
#define LPNMLVGETINFOTIP LPNMLVGETINFOTIPW
#endif
#ifndef LPNMTREEVIEW
#define LPNMTREEVIEW LPNMTREEVIEWW
#endif
#ifndef LPNMTVDISPINFO
#define LPNMTVDISPINFO LPNMTVDISPINFOW
#endif

typedef struct tagNMLVKEYDOWN {
	NMHDR hdr;
	WORD wVKey;
	UINT flags;
} NMLVKEYDOWN, *LPNMLVKEYDOWN;

typedef struct tagNMITEMACTIVATE {
	NMHDR hdr;
	int iItem;
	int iSubItem;
	UINT uNewState;
	UINT uOldState;
	UINT uChanged;
	POINT ptAction;
	LPARAM lParam;
	UINT uKeyFlags;
} NMITEMACTIVATE, *LPNMITEMACTIVATE;

typedef struct tagNMTVKEYDOWN {
	NMHDR hdr;
	WORD wVKey;
	UINT flags;
} NMTVKEYDOWN, *LPNMTVKEYDOWN;

typedef struct tagNMTVGETINFOTIPW {
	NMHDR hdr;
	LPWSTR pszText;
	int cchTextMax;
	HTREEITEM hItem;
	LPARAM lParam;
} NMTVGETINFOTIPW, NMTVGETINFOTIP, *LPNMTVGETINFOTIPW, *LPNMTVGETINFOTIP;

typedef struct tagLVGROUP {
	UINT cbSize;
	UINT mask;
	LPWSTR pszHeader;
	int cchHeader;
	LPWSTR pszFooter;
	int cchFooter;
	int iGroupId;
	UINT stateMask;
	UINT state;
	UINT uAlign;
} LVGROUP, *PLVGROUP;

typedef struct _EDITBALLOONTIP {
	DWORD cbStruct;
	LPCWSTR pszTitle;
	LPCWSTR pszText;
	INT ttiIcon;
} EDITBALLOONTIP, *PEDITBALLOONTIP;

typedef struct _FILE_NOTIFY_INFORMATION {
	DWORD NextEntryOffset;
	DWORD Action;
	DWORD FileNameLength;
	WCHAR FileName[1];
} FILE_NOTIFY_INFORMATION, *PFILE_NOTIFY_INFORMATION;

typedef struct tagMSLLHOOKSTRUCT {
	POINT pt;
	DWORD mouseData;
	DWORD flags;
	DWORD time;
	ULONG_PTR dwExtraInfo;
} MSLLHOOKSTRUCT, *PMSLLHOOKSTRUCT, *LPMSLLHOOKSTRUCT;

typedef struct _IMAGE_FILE_HEADER {
	WORD Machine;
	WORD NumberOfSections;
	DWORD TimeDateStamp;
	DWORD PointerToSymbolTable;
	DWORD NumberOfSymbols;
	WORD SizeOfOptionalHeader;
	WORD Characteristics;
} IMAGE_FILE_HEADER, *PIMAGE_FILE_HEADER;

typedef struct _IMAGE_NT_HEADERS64 {
	DWORD Signature;
	IMAGE_FILE_HEADER FileHeader;
	BYTE OptionalHeader[1];
} IMAGE_NT_HEADERS64, IMAGE_NT_HEADERS, *PIMAGE_NT_HEADERS;

typedef void (CALLBACK *PAPCFUNC)(ULONG_PTR Parameter);

#ifndef EM_SHOWBALLOONTIP
#define EM_SHOWBALLOONTIP 0x1503
#define EM_HIDEBALLOONTIP 0x1504
#endif
#ifndef TTI_ERROR_LARGE
#define TTI_NONE 0
#define TTI_INFO 1
#define TTI_WARNING 2
#define TTI_ERROR 3
#define TTI_INFO_LARGE 4
#define TTI_WARNING_LARGE 5
#define TTI_ERROR_LARGE 6
#endif
#ifndef FILE_LIST_DIRECTORY
#define FILE_LIST_DIRECTORY 0x0001
#endif
#ifndef FILE_MAP_READ
#define FILE_MAP_READ 0x0004
#endif
#ifndef SEC_IMAGE
#define SEC_IMAGE 0x1000000
#endif
#ifndef GDI_ERROR
#define GDI_ERROR 0xFFFFFFFF
#endif
#ifndef OCR_HAND
#define OCR_HAND 32649
#endif
#ifndef TA_RTLREADING
#define TA_RTLREADING 256
#endif
#ifndef IS_TEXT_UNICODE_STATISTICS
#define IS_TEXT_UNICODE_STATISTICS 0x0002
#endif
#ifndef LINGUISTIC_IGNOREDIACRITIC
#define LINGUISTIC_IGNOREDIACRITIC 0x00000020
#endif
#ifndef SORT_DIGITSASNUMBERS
#define SORT_DIGITSASNUMBERS 0x00000008
#endif
#ifndef SP_GRIPPER
#define SP_GRIPPER 3
#endif
#ifndef LVCDI_GROUP
#define LVCDI_GROUP 0x00000001
#endif
#ifndef LVGF_HEADER
#define LVGF_HEADER  0x00000001
#define LVGF_STATE   0x00000004
#define LVGF_GROUPID 0x00000010
#endif
#ifndef LVGS_COLLAPSIBLE
#define LVGS_COLLAPSIBLE 0x00000008
#endif
#ifndef LVGGR_HEADER
#define LVGGR_HEADER 1
#endif
#ifndef LVSICF_NOINVALIDATEALL
#define LVSICF_NOINVALIDATEALL 0x00000001
#define LVSICF_NOSCROLL 0x00000002
#endif
#ifndef LVM_INSERTGROUP
#define LVM_INSERTGROUP (LVM_FIRST+145)
#define LVM_ENABLEGROUPVIEW (LVM_FIRST+157)
#define LVM_GETGROUPRECT (LVM_FIRST+98)
#define LVM_GETSTRINGWIDTHW (LVM_FIRST+17)
#define LVM_SETITEMCOUNTEX (LVM_FIRST+47)
#define LVM_SORTITEMSEX (LVM_FIRST+81)
#define LVM_SUBITEMHITTEST (LVM_FIRST+57)
#endif
#ifndef ListView_EnableGroupView
#define ListView_EnableGroupView(hwnd, f) (BOOL)SNDMSG((hwnd), LVM_ENABLEGROUPVIEW, (WPARAM)(f), 0)
#define ListView_InsertGroup(hwnd, i, pgrp) (int)SNDMSG((hwnd), LVM_INSERTGROUP, (WPARAM)(i), (LPARAM)(pgrp))
#define ListView_GetGroupRect(hwnd, i, type, prc) (BOOL)SNDMSG((hwnd), LVM_GETGROUPRECT, (WPARAM)(i), ((prc) ? (((RECT*)(prc))->top = (type), (LPARAM)(prc)) : 0))
#define ListView_GetStringWidth(hwnd, psz) (int)SNDMSG((hwnd), LVM_GETSTRINGWIDTHW, 0, (LPARAM)(psz))
#define ListView_SetItemCountEx(hwnd, c, f) (void)SNDMSG((hwnd), LVM_SETITEMCOUNTEX, (WPARAM)(c), (LPARAM)(f))
#define ListView_SortItemsEx(hwnd, fn, lp) (BOOL)SNDMSG((hwnd), LVM_SORTITEMSEX, (WPARAM)(lp), (LPARAM)(fn))
#define ListView_SubItemHitTest(hwnd, pinfo) (int)SNDMSG((hwnd), LVM_SUBITEMHITTEST, 0, (LPARAM)(pinfo))
#endif
#ifndef TVM_GETITEMRECT
#define TVM_GETITEMRECT (TV_FIRST+4)
#define TVM_GETITEMSTATE (TV_FIRST+39)
#endif
#ifndef TreeView_GetItemRect
#define TreeView_GetItemRect(hwnd, hitem, prc, code) \
	(*(HTREEITEM*)(prc) = (hitem), (BOOL)SNDMSG((hwnd), TVM_GETITEMRECT, (WPARAM)(code), (LPARAM)(RECT*)(prc)))
#define TreeView_GetItemState(hwnd, hitem, mask) \
	(UINT)SNDMSG((hwnd), TVM_GETITEMSTATE, (WPARAM)(hitem), (LPARAM)(mask))
#endif

typedef int (CALLBACK *PFNTVCOMPARE)(LPARAM, LPARAM, LPARAM);
typedef LRESULT (CALLBACK *SUBCLASSPROC)(HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);

#ifndef HCERTSTORE
typedef PVOID HCERTSTORE;
#endif
#ifndef HCRYPTMSG
typedef PVOID HCRYPTMSG;
#endif
typedef struct _SID_IDENTIFIER_AUTHORITY { BYTE Value[6]; } SID_IDENTIFIER_AUTHORITY, *PSID_IDENTIFIER_AUTHORITY;
typedef struct _TOKEN_MANDATORY_LABEL { struct { PSID Sid; DWORD Attributes; } Label; } TOKEN_MANDATORY_LABEL, *PTOKEN_MANDATORY_LABEL;

#ifndef LPOLESTR
typedef WCHAR* LPOLESTR;
#endif
typedef const WCHAR* LPCOLESTR;
#ifndef PCUITEMID_CHILD_ARRAY
typedef const void* PCUITEMID_CHILD_ARRAY;
#endif

struct IPersistFile : public IUnknown {
	virtual HRESULT GetClassID(CLSID*) = 0;
	virtual HRESULT IsDirty() = 0;
	virtual HRESULT Load(LPCOLESTR, DWORD) = 0;
	virtual HRESULT Save(LPCOLESTR, BOOL) = 0;
	virtual HRESULT SaveCompleted(LPCOLESTR) = 0;
	virtual HRESULT GetCurFile(LPOLESTR*) = 0;
};
struct IShellLinkW : public IUnknown {
	virtual HRESULT GetPath(LPWSTR, int, WIN32_FIND_DATAW*, DWORD) = 0;
	virtual HRESULT GetIDList(PIDLIST_ABSOLUTE*) = 0;
	virtual HRESULT SetIDList(PCIDLIST_ABSOLUTE) = 0;
	virtual HRESULT GetDescription(LPWSTR, int) = 0;
	virtual HRESULT SetDescription(LPCWSTR) = 0;
	virtual HRESULT GetWorkingDirectory(LPWSTR, int) = 0;
	virtual HRESULT SetWorkingDirectory(LPCWSTR) = 0;
	virtual HRESULT GetArguments(LPWSTR, int) = 0;
	virtual HRESULT SetArguments(LPCWSTR) = 0;
	virtual HRESULT GetHotkey(WORD*) = 0;
	virtual HRESULT SetHotkey(WORD) = 0;
	virtual HRESULT GetShowCmd(int*) = 0;
	virtual HRESULT SetShowCmd(int) = 0;
	virtual HRESULT GetIconLocation(LPWSTR, int, int*) = 0;
	virtual HRESULT SetIconLocation(LPCWSTR, int) = 0;
	virtual HRESULT SetRelativePath(LPCWSTR, DWORD) = 0;
	virtual HRESULT Resolve(HWND, DWORD) = 0;
	virtual HRESULT SetPath(LPCWSTR) = 0;
};
#ifndef IShellLink
#define IShellLink IShellLinkW
#endif

#ifdef __cplusplus
extern "C" {
extern const GUID CLSID_ShellLink;
extern const GUID IID_IShellLink;
extern const GUID IID_IPersistFile;
}
#endif

#ifdef __cplusplus
extern "C" {
#endif

HWND ChildWindowFromPointEx(HWND parent, POINT pt, UINT flags);
BOOL GetClassNameA(HWND h, LPSTR buf, int max);
BOOL SetDlgItemTextA(HWND dlg, int id, LPCSTR text);
int lstrcmpiA(LPCSTR a, LPCSTR b);
int lstrcpynA(LPSTR dst, LPCSTR src, int max);
BOOL GetTextExtentPointA(HDC hdc, LPCSTR s, int c, LPSIZE sz);
BOOL GetTextExtentPointW(HDC hdc, LPCWSTR s, int c, LPSIZE sz);
#ifndef GetTextExtentPoint
#define GetTextExtentPoint GetTextExtentPointW
#endif
BOOL OffsetWindowOrgEx(HDC hdc, int x, int y, LPPOINT pt);
BOOL RectVisible(HDC hdc, const RECT* rc);
BOOL GetMenuBarInfo(HWND h, LONG idObject, LONG idItem, PMENUBARINFO pmbi);
BOOL SetMenuItemBitmaps(HMENU m, UINT pos, UINT flags, HBITMAP unchecked, HBITMAP checked);
DWORD GetWindowThreadProcessId(HWND h, LPDWORD pid);

BOOL GetFileAttributesExW(LPCWSTR name, GET_FILEEX_INFO_LEVELS level, LPVOID info);
#ifndef GetFileAttributesEx
#define GetFileAttributesEx GetFileAttributesExW
#endif
BOOL CopyFileExW(LPCWSTR exist, LPCWSTR newFile, LPVOID progress, LPVOID data, LPBOOL cancel, DWORD flags);
#ifndef CopyFileEx
#define CopyFileEx CopyFileExW
#endif
BOOL ReplaceFileW(LPCWSTR replaced, LPCWSTR replacement, LPCWSTR backup, DWORD flags, LPVOID, LPVOID);
#ifndef ReplaceFile
#define ReplaceFile ReplaceFileW
#endif

int GetDateFormatEx(LPCWSTR locale, DWORD flags, const SYSTEMTIME* st, LPCWSTR fmt, LPWSTR buf, int cch, LPCWSTR cal);
int GetTimeFormatEx(LPCWSTR locale, DWORD flags, const SYSTEMTIME* st, LPCWSTR fmt, LPWSTR buf, int cch);
COLORREF ColorAdjustLuma(COLORREF clr, int n, BOOL scale);
HRESULT AssocQueryStringW(DWORD flags, DWORD str, LPCWSTR assoc, LPCWSTR extra, LPWSTR psz, DWORD* pcch);
#ifndef AssocQueryString
#define AssocQueryString AssocQueryStringW
#endif
BOOL PathIsNetworkPathW(LPCWSTR path);
#ifndef PathIsNetworkPath
#define PathIsNetworkPath PathIsNetworkPathW
#endif
int SHCreateDirectory(HWND hwnd, LPCWSTR path);
HRESULT SHOpenFolderAndSelectItems(PCIDLIST_ABSOLUTE pidl, UINT cidl, PCUITEMID_CHILD_ARRAY, DWORD flags);

BOOL ImageList_GetImageInfo(HIMAGELIST himl, int i, IMAGEINFO* pImageInfo);

BOOL SetWindowSubclass(HWND h, SUBCLASSPROC proc, UINT_PTR uid, DWORD_PTR data);
BOOL GetWindowSubclass(HWND h, SUBCLASSPROC proc, UINT_PTR uid, DWORD_PTR* data);
BOOL RemoveWindowSubclass(HWND h, SUBCLASSPROC proc, UINT_PTR uid);
LRESULT DefSubclassProc(HWND h, UINT msg, WPARAM w, LPARAM l);

BOOL TrackMouseEvent(LPTRACKMOUSEEVENT tme);
BOOL _TrackMouseEvent(LPTRACKMOUSEEVENT tme);

HRESULT DrawThemeTextEx(HTHEME theme, HDC hdc, int part, int state, LPCWSTR text, int cch, DWORD flags, LPRECT rc, const DTTOPTS* opts);
HRESULT DrawThemeParentBackground(HWND h, HDC hdc, const RECT* rc);
HRESULT GetThemeBackgroundContentRect(HTHEME, HDC, int, int, LPCRECT, LPRECT);
HRESULT GetThemeFont(HTHEME, HDC, int, int, int, LOGFONTW*);
HRESULT GetThemePartSize(HTHEME, HDC, int, int, LPCRECT, int, SIZE*);
HRESULT GetThemeTransitionDuration(HTHEME, int, int, int, int, DWORD*);
HRESULT DwmGetColorizationColor(DWORD* color, BOOL* opaque);
HANIMATIONBUFFER BeginBufferedAnimation(HWND, HDC, const RECT*, BP_BUFFERFORMAT, BP_PAINTPARAMS*, BP_ANIMATIONPARAMS*, HDC*, HDC*);
BOOL EndBufferedAnimation(HANIMATIONBUFFER, BOOL);
BOOL BufferedPaintRenderAnimation(HWND, HDC);
BOOL BufferedPaintStopAllAnimations(HWND);

HANDLE OpenProcess(DWORD access, BOOL inherit, DWORD pid);
BOOL OpenProcessToken(HANDLE proc, DWORD access, PHANDLE token);
BOOL GetTokenInformation(HANDLE token, int cls, LPVOID info, DWORD len, PDWORD ret);
BOOL AllocateAndInitializeSid(PSID_IDENTIFIER_AUTHORITY a, BYTE n, DWORD s0, DWORD s1, DWORD s2, DWORD s3, DWORD s4, DWORD s5, DWORD s6, DWORD s7, PSID* sid);
BOOL CheckTokenMembership(HANDLE token, PSID sid, PBOOL member);
PUCHAR GetSidSubAuthorityCount(PSID sid);
PDWORD GetSidSubAuthority(PSID sid, DWORD i);
PVOID FreeSid(PSID sid);
BOOL QueryFullProcessImageNameW(HANDLE proc, DWORD flags, LPWSTR buf, PDWORD size);
BOOL TerminateThread(HANDLE thread, DWORD code);
BOOL GetProductInfo(DWORD maj, DWORD min, DWORD spmaj, DWORD spmin, PDWORD type);
HRESULT RegisterApplicationRestart(PCWSTR cmd, DWORD flags);
HRESULT UnregisterApplicationRestart(void);
HRESULT GetApplicationRestartSettings(HANDLE proc, PWSTR cmd, PDWORD size, PDWORD flags);

BOOL CryptQueryObject(DWORD objType, const void* obj, DWORD content, DWORD format, DWORD flags, DWORD* enc, DWORD* contentOut, DWORD* formatOut, HCERTSTORE* store, HCRYPTMSG* msg, const void** ctx);
BOOL CryptMsgGetParam(HCRYPTMSG msg, DWORD param, DWORD index, void* data, DWORD* size);
BOOL CryptMsgClose(HCRYPTMSG msg);
PCCERT_CONTEXT CertFindCertificateInStore(HCERTSTORE store, DWORD enc, DWORD findFlags, DWORD findType, const void* para, PCCERT_CONTEXT prev);
DWORD CertGetNameStringW(PCCERT_CONTEXT ctx, DWORD type, DWORD flags, void* para, LPWSTR buf, DWORD cch);
#ifndef CertGetNameString
#define CertGetNameString CertGetNameStringW
#endif
BOOL CertFreeCertificateContext(PCCERT_CONTEXT ctx);
BOOL CertCloseStore(HCERTSTORE store, DWORD flags);

BOOL InternetCrackUrlW(LPCWSTR url, DWORD len, DWORD flags, LPURL_COMPONENTS c);
#ifndef InternetCrackUrl
#define InternetCrackUrl InternetCrackUrlW
#endif

LONG RegGetValueW(HKEY k, LPCWSTR sub, LPCWSTR value, DWORD flags, LPDWORD type, PVOID data, LPDWORD size);

int GetROP2(HDC hdc);
BOOL SetBrushOrgEx(HDC hdc, int x, int y, LPPOINT pt);
BOOL ImageList_BeginDrag(HIMAGELIST himl, int i, int dx, int dy);
BOOL ImageList_DragEnter(HWND hwnd, int x, int y);
BOOL ImageList_DragLeave(HWND hwnd);
BOOL ImageList_DragMove(int x, int y);
BOOL ImageList_DragShowNolock(BOOL show);
void ImageList_EndDrag(void);
HRESULT LoadIconWithScaleDown(HINSTANCE inst, PCWSTR name, int cx, int cy, HICON* icon);
BOOL Shell_NotifyIconW(DWORD dwMessage, NOTIFYICONDATAW* data);
#ifndef Shell_NotifyIcon
#define Shell_NotifyIcon Shell_NotifyIconW
#endif
BOOL CancelIo(HANDLE h);
DWORD QueueUserAPC(PAPCFUNC pfn, HANDLE thread, ULONG_PTR data);
BOOL ReadDirectoryChangesW(HANDLE h, LPVOID buf, DWORD n, BOOL subtree, DWORD filter, LPDWORD ret, LPOVERLAPPED ov, LPOVERLAPPED_COMPLETION_ROUTINE cb);
DWORD WaitForSingleObjectEx(HANDLE h, DWORD ms, BOOL alertable);
int LCMapStringEx(LPCWSTR locale, DWORD flags, LPCWSTR src, int cchSrc, LPWSTR dst, int cchDst, LPVOID, LPARAM, HANDLE);
BOOL IsTextUnicode(const void* buf, int len, LPINT tests);
int ToAscii(UINT vk, UINT scan, const BYTE* keystate, LPWORD out, UINT flags);
PIMAGE_NT_HEADERS ImageNtHeader(PVOID base);

#ifdef __cplusplus
}

inline HANDLE CreateFileW(const char* name, DWORD access, DWORD share, LPSECURITY_ATTRIBUTES sa,
	DWORD disp, DWORD flags, HANDLE templateFile)
{
	return CreateFileW(std::filesystem::path(name ? name : "").wstring().c_str(), access, share, sa, disp, flags, templateFile);
}
inline BOOL GetFileAttributesExW(const char* name, GET_FILEEX_INFO_LEVELS level, LPVOID info)
{
	return GetFileAttributesExW(std::filesystem::path(name ? name : "").wstring().c_str(), level, info);
}
inline HINSTANCE ShellExecuteW(HWND hwnd, LPCWSTR verb, const char* file, LPCWSTR params, LPCWSTR dir, INT show)
{
	std::wstring w = std::filesystem::path(file ? file : "").wstring();
	return ShellExecuteW(hwnd, verb, w.c_str(), params, dir, show);
}
inline BOOL PathFileExistsW(const char* path)
{
	return PathFileExistsW(std::filesystem::path(path ? path : "").wstring().c_str());
}
#endif
