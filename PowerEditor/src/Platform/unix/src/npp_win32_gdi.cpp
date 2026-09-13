#include "npp_platform.h"

#include <cairo.h>
#include <pango/pangocairo.h>
#include <vector>
#include <cmath>

struct GdiObj {
	enum Type { Pen, Brush, Font, Bitmap, Region, Dc } type;
	COLORREF color = 0;
	int width = 1;
	int style = 0;
	int weight = 400;
	bool italic = false;
	std::wstring face = L"Sans";
	int height = 12;
	cairo_surface_t* surface = nullptr;
	unsigned char* bits = nullptr;
	int bmpW = 0, bmpH = 0;
	RECT rgn{};
	bool stock = false;
};

struct DcImpl {
	cairo_t* cr = nullptr;
	cairo_surface_t* owned = nullptr;
	GdiObj* pen = nullptr;
	GdiObj* brush = nullptr;
	GdiObj* font = nullptr;
	GdiObj* bitmap = nullptr;
	COLORREF textColor = 0;
	COLORREF bkColor = RGB(255,255,255);
	int bkMode = TRANSPARENT;
	int rop2 = R2_COPYPEN;
	POINT pos{};
	WindowImpl* window = nullptr;
	int w = 1, h = 1;
	bool borrowedCr = false;
};

static std::vector<GdiObj*> g_stock;
static GdiObj* g_whiteBrush = nullptr;
static GdiObj* g_blackBrush = nullptr;
static GdiObj* g_nullBrush = nullptr;
static GdiObj* g_blackPen = nullptr;
static GdiObj* g_nullPen = nullptr;
static GdiObj* g_sysFont = nullptr;

static GdiObj* asObj(HGDIOBJ h) { return reinterpret_cast<GdiObj*>(h); }
static DcImpl* asDc(HDC h) { return reinterpret_cast<DcImpl*>(h); }

void nppInitGdi()
{
	auto makeBrush = [](COLORREF c) {
		auto* o = new GdiObj(); o->type = GdiObj::Brush; o->color = c; o->stock = true; return o;
	};
	g_whiteBrush = makeBrush(RGB(255,255,255));
	g_blackBrush = makeBrush(RGB(0,0,0));
	g_nullBrush = makeBrush(0); g_nullBrush->style = -1;
	g_blackPen = new GdiObj(); g_blackPen->type = GdiObj::Pen; g_blackPen->color = 0; g_blackPen->stock = true;
	g_nullPen = new GdiObj(); g_nullPen->type = GdiObj::Pen; g_nullPen->style = -1; g_nullPen->stock = true;
	g_sysFont = new GdiObj(); g_sysFont->type = GdiObj::Font; g_sysFont->stock = true; g_sysFont->height = 12;
}

HDC nppCreatePaintDC(WindowImpl* w, cairo_t* cr, int width, int height)
{
	auto* dc = new DcImpl();
	dc->cr = cr;
	dc->borrowedCr = true;
	dc->window = w;
	dc->w = width;
	dc->h = height;
	dc->pen = g_blackPen;
	dc->brush = g_whiteBrush;
	dc->font = g_sysFont;
	w->paintCr = cr;
	return (HDC)dc;
}
void nppReleasePaintDC(HDC hdc)
{
	auto* dc = asDc(hdc);
	if (!dc) return;
	dc->cr = nullptr;
	delete dc;
}

HDC GetDC(HWND h)
{
	auto* w = implFrom(h);
	auto* dc = new DcImpl();
	dc->w = 1; dc->h = 1;
	if (w && w->widget)
	{
		GtkAllocation a{};
		gtk_widget_get_allocation(w->widget, &a);
		dc->w = std::max(a.width, 1);
		dc->h = std::max(a.height, 1);
	}
	dc->owned = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, dc->w, dc->h);
	dc->cr = cairo_create(dc->owned);
	dc->window = w;
	dc->pen = g_blackPen;
	dc->brush = g_whiteBrush;
	dc->font = g_sysFont;
	return (HDC)dc;
}
HDC GetWindowDC(HWND h) { return GetDC(h); }
HDC GetDCEx(HWND h, HRGN, DWORD) { return GetDC(h); }
int ReleaseDC(HWND, HDC hdc)
{
	auto* dc = asDc(hdc);
	if (!dc) return 0;
	if (dc->cr && !dc->borrowedCr) cairo_destroy(dc->cr);
	if (dc->owned) cairo_surface_destroy(dc->owned);
	delete dc;
	return 1;
}
HDC BeginPaint(HWND h, LPPAINTSTRUCT ps)
{
	HDC dc = GetDC(h);
	if (ps)
	{
		ps->hdc = dc;
		ps->fErase = TRUE;
		GetClientRect(h, &ps->rcPaint);
	}
	return dc;
}
BOOL EndPaint(HWND, const PAINTSTRUCT* ps)
{
	if (ps) ReleaseDC(nullptr, ps->hdc);
	return TRUE;
}

HDC CreateCompatibleDC(HDC hdc)
{
	auto* src = asDc(hdc);
	auto* dc = new DcImpl();
	dc->w = src ? src->w : 1;
	dc->h = src ? src->h : 1;
	dc->pen = g_blackPen;
	dc->brush = g_whiteBrush;
	dc->font = g_sysFont;
	return (HDC)dc;
}
BOOL DeleteDC(HDC hdc) { return ReleaseDC(nullptr, hdc) != 0; }

static void ensureSurface(DcImpl* dc)
{
	if (dc->cr) return;
	if (dc->bitmap && dc->bitmap->surface)
	{
		dc->cr = cairo_create(dc->bitmap->surface);
		dc->w = dc->bitmap->bmpW;
		dc->h = dc->bitmap->bmpH;
		return;
	}
	dc->owned = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, std::max(dc->w,1), std::max(dc->h,1));
	dc->cr = cairo_create(dc->owned);
}

HBITMAP CreateCompatibleBitmap(HDC hdc, int cx, int cy)
{
	auto* o = new GdiObj();
	o->type = GdiObj::Bitmap;
	o->bmpW = std::max(cx, 1);
	o->bmpH = std::max(cy, 1);
	o->surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, o->bmpW, o->bmpH);
	return (HBITMAP)o;
}
HBITMAP CreateDIBSection(HDC, const BITMAPINFO* bmi, UINT, void** bits, HANDLE, DWORD)
{
	int w = bmi ? std::abs(bmi->bmiHeader.biWidth) : 1;
	int h = bmi ? std::abs(bmi->bmiHeader.biHeight) : 1;
	auto* o = new GdiObj();
	o->type = GdiObj::Bitmap;
	o->bmpW = std::max(w,1);
	o->bmpH = std::max(h,1);
	o->surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, o->bmpW, o->bmpH);
	o->bits = cairo_image_surface_get_data(o->surface);
	if (bits) *bits = o->bits;
	return (HBITMAP)o;
}
HBITMAP CreateBitmap(int w, int h, UINT, UINT, const void*)
{
	return CreateCompatibleBitmap(nullptr, w, h);
}
HBITMAP CreateBitmapIndirect(const BITMAP* pbm)
{
	return CreateCompatibleBitmap(nullptr, pbm ? pbm->bmWidth : 1, pbm ? pbm->bmHeight : 1);
}

HBRUSH CreateSolidBrush(COLORREF color)
{
	auto* o = new GdiObj(); o->type = GdiObj::Brush; o->color = color; return (HBRUSH)o;
}
HBRUSH CreatePatternBrush(HBITMAP) { return CreateSolidBrush(RGB(128,128,128)); }
HBRUSH CreateHatchBrush(int, COLORREF color) { return CreateSolidBrush(color); }
HPEN CreatePen(int style, int width, COLORREF color)
{
	auto* o = new GdiObj(); o->type = GdiObj::Pen; o->style = style; o->width = std::max(width,1); o->color = color; return (HPEN)o;
}
HPEN CreatePenIndirect(const LOGPEN* plp)
{
	if (!plp) return CreatePen(PS_SOLID, 1, 0);
	return CreatePen(plp->lopnStyle, plp->lopnWidth.x, plp->lopnColor);
}
HFONT CreateFontW(int h, int, int, int, int weight, DWORD italic, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, LPCWSTR face)
{
	auto* o = new GdiObj();
	o->type = GdiObj::Font;
	o->height = h ? std::abs(h) : 12;
	o->weight = weight;
	o->italic = italic != 0;
	if (face) o->face = face;
	return (HFONT)o;
}
HFONT CreateFontIndirectW(const LOGFONTW* lf)
{
	if (!lf) return CreateFontW(12,0,0,0,FW_NORMAL,0,0,0,0,0,0,0,0,L"Sans");
	return CreateFontW(lf->lfHeight, lf->lfWidth, 0, 0, lf->lfWeight, lf->lfItalic, lf->lfUnderline, lf->lfStrikeOut,
		lf->lfCharSet, 0, 0, 0, lf->lfPitchAndFamily, lf->lfFaceName);
}

HGDIOBJ SelectObject(HDC hdc, HGDIOBJ obj)
{
	auto* dc = asDc(hdc);
	auto* o = asObj(obj);
	if (!dc || !o) return nullptr;
	HGDIOBJ old = nullptr;
	switch (o->type)
	{
	case GdiObj::Pen: old = dc->pen; dc->pen = o; break;
	case GdiObj::Brush: old = dc->brush; dc->brush = o; break;
	case GdiObj::Font: old = dc->font; dc->font = o; break;
	case GdiObj::Bitmap:
		old = dc->bitmap; dc->bitmap = o;
		if (dc->cr && !dc->borrowedCr) { cairo_destroy(dc->cr); dc->cr = nullptr; }
		if (dc->owned) { cairo_surface_destroy(dc->owned); dc->owned = nullptr; }
		break;
	default: break;
	}
	return old;
}

BOOL DeleteObject(HGDIOBJ obj)
{
	auto* o = asObj(obj);
	if (!o || o->stock) return TRUE;
	if (o->surface) cairo_surface_destroy(o->surface);
	delete o;
	return TRUE;
}

HGDIOBJ GetStockObject(int i)
{
	switch (i)
	{
	case WHITE_BRUSH: return g_whiteBrush;
	case BLACK_BRUSH: return g_blackBrush;
	case NULL_BRUSH: case HOLLOW_BRUSH: return g_nullBrush;
	case BLACK_PEN: return g_blackPen;
	case NULL_PEN: return g_nullPen;
	case SYSTEM_FONT: case DEFAULT_GUI_FONT: case ANSI_VAR_FONT: return g_sysFont;
	default: return g_whiteBrush;
	}
}

static void setSource(cairo_t* cr, COLORREF c, double a = 1.0)
{
	cairo_set_source_rgba(cr, GetRValue(c)/255.0, GetGValue(c)/255.0, GetBValue(c)/255.0, a);
}

BOOL BitBlt(HDC dst, int x, int y, int cx, int cy, HDC src, int sx, int sy, DWORD)
{
	auto* d = asDc(dst); auto* s = asDc(src);
	if (!d) return FALSE;
	ensureSurface(d);
	if (s) ensureSurface(s);
	cairo_surface_t* ss = s && s->bitmap && s->bitmap->surface ? s->bitmap->surface : (s ? s->owned : nullptr);
	if (!ss) return TRUE;
	cairo_set_source_surface(d->cr, ss, x - sx, y - sy);
	cairo_rectangle(d->cr, x, y, cx, cy);
	cairo_fill(d->cr);
	return TRUE;
}
BOOL StretchBlt(HDC dst, int x, int y, int cx, int cy, HDC src, int sx, int sy, int scx, int scy, DWORD)
{
	auto* d = asDc(dst); auto* s = asDc(src);
	if (!d) return FALSE;
	ensureSurface(d);
	cairo_surface_t* ss = s && s->bitmap && s->bitmap->surface ? s->bitmap->surface : (s ? s->owned : nullptr);
	if (!ss) return TRUE;
	cairo_save(d->cr);
	cairo_translate(d->cr, x, y);
	cairo_scale(d->cr, scx ? (double)cx/scx : 1, scy ? (double)cy/scy : 1);
	cairo_set_source_surface(d->cr, ss, -sx, -sy);
	cairo_paint(d->cr);
	cairo_restore(d->cr);
	return TRUE;
}
BOOL PatBlt(HDC hdc, int x, int y, int w, int h, DWORD)
{
	auto* d = asDc(hdc); if (!d) return FALSE; ensureSurface(d);
	if (d->brush && d->brush->style != -1) setSource(d->cr, d->brush->color);
	cairo_rectangle(d->cr, x, y, w, h);
	cairo_fill(d->cr);
	return TRUE;
}
BOOL TransparentBlt(HDC d, int x, int y, int cx, int cy, HDC s, int sx, int sy, int scx, int scy, UINT)
{ return StretchBlt(d,x,y,cx,cy,s,sx,sy,scx,scy,SRCCOPY); }
BOOL AlphaBlend(HDC d, int x, int y, int cx, int cy, HDC s, int sx, int sy, int scx, int scy, BLENDFUNCTION)
{ return StretchBlt(d,x,y,cx,cy,s,sx,sy,scx,scy,SRCCOPY); }
BOOL GdiAlphaBlend(HDC d,int x,int y,int cx,int cy,HDC s,int sx,int sy,int scx,int scy,BLENDFUNCTION f)
{ return AlphaBlend(d,x,y,cx,cy,s,sx,sy,scx,scy,f); }
BOOL GdiTransparentBlt(HDC d,int x,int y,int cx,int cy,HDC s,int sx,int sy,int scx,int scy,UINT k)
{ return TransparentBlt(d,x,y,cx,cy,s,sx,sy,scx,scy,k); }
BOOL GradientFill(HDC hdc, PTRIVERTEX v, ULONG nVertex, PVOID, ULONG, ULONG)
{
	if (!v || nVertex < 2) return FALSE;
	auto* d = asDc(hdc); if (!d) return FALSE; ensureSurface(d);
	setSource(d->cr, RGB(v[0].Red>>8, v[0].Green>>8, v[0].Blue>>8));
	cairo_paint(d->cr);
	return TRUE;
}
BOOL GdiGradientFill(HDC h, PTRIVERTEX v, ULONG n, PVOID m, ULONG nm, ULONG mode) { return GradientFill(h,v,n,m,nm,mode); }
BOOL GdiFlush(void) { return TRUE; }

int SetBkMode(HDC hdc, int mode) { auto* d = asDc(hdc); if (!d) return 0; int o = d->bkMode; d->bkMode = mode; return o; }
COLORREF SetBkColor(HDC hdc, COLORREF c) { auto* d = asDc(hdc); if (!d) return 0; COLORREF o = d->bkColor; d->bkColor = c; return o; }
COLORREF SetTextColor(HDC hdc, COLORREF c) { auto* d = asDc(hdc); if (!d) return 0; COLORREF o = d->textColor; d->textColor = c; return o; }
COLORREF GetBkColor(HDC hdc) { auto* d = asDc(hdc); return d ? d->bkColor : 0; }
COLORREF GetTextColor(HDC hdc) { auto* d = asDc(hdc); return d ? d->textColor : 0; }
UINT SetTextAlign(HDC, UINT align) { return align; }

static PangoLayout* makeLayout(DcImpl* d, LPCWSTR s, int c)
{
	std::wstring tmp;
	if (c < 0) tmp = s ? s : L"";
	else tmp.assign(s ? s : L"", s ? c : 0);
	std::string u = nppWideToUtf8(tmp);
	PangoLayout* layout = pango_cairo_create_layout(d->cr);
	pango_layout_set_text(layout, u.c_str(), -1);
	char desc[128];
	int px = d->font ? d->font->height : 12;
	if (px < 0) px = -px;
	if (px > 48 && px > 96) px = px * 72 / 96; // lfHeight in pixels sometimes
	std::string face = d->font ? nppWideToUtf8(d->font->face) : "Sans";
	std::snprintf(desc, sizeof(desc), "%s %s %d", face.c_str(), (d->font && d->font->weight >= 700) ? "Bold" : "", std::max(px, 8));
	PangoFontDescription* fd = pango_font_description_from_string(desc);
	pango_layout_set_font_description(layout, fd);
	pango_font_description_free(fd);
	return layout;
}

BOOL TextOutW(HDC hdc, int x, int y, LPCWSTR s, int c)
{
	auto* d = asDc(hdc); if (!d) return FALSE; ensureSurface(d);
	PangoLayout* layout = makeLayout(d, s, c);
	setSource(d->cr, d->textColor);
	cairo_move_to(d->cr, x, y);
	pango_cairo_show_layout(d->cr, layout);
	g_object_unref(layout);
	return TRUE;
}
BOOL ExtTextOutW(HDC hdc, int x, int y, UINT, const RECT* rc, LPCWSTR s, UINT c, const INT*)
{
	if (rc) { /* clip ignored */ }
	return TextOutW(hdc, x, y, s, (int)c);
}
BOOL GetTextExtentPoint32W(HDC hdc, LPCWSTR s, int c, LPSIZE sz)
{
	auto* d = asDc(hdc); if (!sz) return FALSE;
	if (!d) { sz->cx = c * 8; sz->cy = 16; return TRUE; }
	ensureSurface(d);
	PangoLayout* layout = makeLayout(d, s, c);
	int w=0,h=0;
	pango_layout_get_pixel_size(layout, &w, &h);
	sz->cx = w; sz->cy = h;
	g_object_unref(layout);
	return TRUE;
}
BOOL GetTextExtentPointW(HDC h, LPCWSTR s, int c, LPSIZE sz) { return GetTextExtentPoint32W(h,s,c,sz); }
BOOL GetTextMetricsW(HDC hdc, LPTEXTMETRICW tm)
{
	if (!tm) return FALSE;
	memset(tm, 0, sizeof(*tm));
	SIZE sz{};
	GetTextExtentPoint32W(hdc, L"Mg", 2, &sz);
	tm->tmHeight = sz.cy ? sz.cy : 16;
	tm->tmAscent = tm->tmHeight * 4 / 5;
	tm->tmDescent = tm->tmHeight - tm->tmAscent;
	tm->tmAveCharWidth = 8;
	tm->tmMaxCharWidth = 16;
	return TRUE;
}

int DrawTextW(HDC hdc, LPCWSTR text, int count, LPRECT rc, UINT format)
{
	if (!rc) return 0;
	SIZE sz{};
	GetTextExtentPoint32W(hdc, text, count < 0 ? (int)wcslen(text?text:L"") : count, &sz);
	int x = rc->left, y = rc->top;
	if (format & DT_CENTER) x = rc->left + ((rc->right - rc->left) - sz.cx) / 2;
	if (format & DT_RIGHT) x = rc->right - sz.cx;
	if (format & DT_VCENTER) y = rc->top + ((rc->bottom - rc->top) - sz.cy) / 2;
	if (format & DT_BOTTOM) y = rc->bottom - sz.cy;
	if (!(format & DT_CALCRECT))
		TextOutW(hdc, x, y, text, count);
	else
	{
		rc->right = rc->left + sz.cx;
		rc->bottom = rc->top + sz.cy;
	}
	return sz.cy;
}
int DrawTextExW(HDC hdc, LPWSTR text, int count, LPRECT rc, UINT format, LPDRAWTEXTPARAMS)
{
	return DrawTextW(hdc, text, count, rc, format);
}

int FillRect(HDC hdc, const RECT* rc, HBRUSH br)
{
	auto* d = asDc(hdc); if (!d || !rc) return 0; ensureSurface(d);
	auto* b = asObj(br);
	if (b) setSource(d->cr, b->color);
	cairo_rectangle(d->cr, rc->left, rc->top, rc->right-rc->left, rc->bottom-rc->top);
	cairo_fill(d->cr);
	return 1;
}
int FrameRect(HDC hdc, const RECT* rc, HBRUSH br)
{
	auto* d = asDc(hdc); if (!d || !rc) return 0; ensureSurface(d);
	auto* b = asObj(br);
	if (b) setSource(d->cr, b->color);
	cairo_set_line_width(d->cr, 1);
	cairo_rectangle(d->cr, rc->left+0.5, rc->top+0.5, rc->right-rc->left-1, rc->bottom-rc->top-1);
	cairo_stroke(d->cr);
	return 1;
}
BOOL InvertRect(HDC hdc, const RECT* rc)
{
	auto* d = asDc(hdc); if (!d || !rc) return FALSE; ensureSurface(d);
	cairo_set_operator(d->cr, CAIRO_OPERATOR_DIFFERENCE);
	cairo_set_source_rgb(d->cr, 1,1,1);
	cairo_rectangle(d->cr, rc->left, rc->top, rc->right-rc->left, rc->bottom-rc->top);
	cairo_fill(d->cr);
	cairo_set_operator(d->cr, CAIRO_OPERATOR_OVER);
	return TRUE;
}
BOOL DrawFocusRect(HDC hdc, const RECT* rc)
{
	auto* d = asDc(hdc); if (!d || !rc) return FALSE; ensureSurface(d);
	static const double dashes[] = {1.0, 1.0};
	cairo_set_source_rgb(d->cr, 0,0,0);
	cairo_set_dash(d->cr, dashes, 2, 0);
	cairo_rectangle(d->cr, rc->left+0.5, rc->top+0.5, rc->right-rc->left-1, rc->bottom-rc->top-1);
	cairo_stroke(d->cr);
	cairo_set_dash(d->cr, nullptr, 0, 0);
	return TRUE;
}
BOOL DrawEdge(HDC hdc, LPRECT rc, UINT, UINT)
{
	return FrameRect(hdc, rc, GetStockObject(BLACK_BRUSH)) != 0;
}
BOOL DrawFrameControl(HDC hdc, LPRECT rc, UINT, UINT)
{
	return FrameRect(hdc, rc, GetStockObject(BLACK_BRUSH)) != 0;
}
BOOL DrawIcon(HDC hdc, int x, int y, HICON)
{
	RECT rc{x,y,x+16,y+16};
	return FrameRect(hdc, &rc, GetStockObject(BLACK_BRUSH)) != 0;
}
BOOL DrawIconEx(HDC hdc, int x, int y, HICON icon, int cx, int cy, UINT, HBRUSH, UINT)
{
	RECT rc{x,y,x+cx,y+cy};
	return DrawIcon(hdc, x, y, icon);
}
BOOL DrawStateW(HDC hdc, HBRUSH, DRAWSTATEPROC, LPARAM, WPARAM, int x, int y, int cx, int cy, UINT)
{
	RECT rc{x,y,x+cx,y+cy};
	return FillRect(hdc, &rc, GetStockObject(LTGRAY_BRUSH)) != 0;
}

BOOL MoveToEx(HDC hdc, int x, int y, LPPOINT pt)
{
	auto* d = asDc(hdc); if (!d) return FALSE;
	if (pt) *pt = d->pos;
	d->pos = {x,y};
	return TRUE;
}
BOOL LineTo(HDC hdc, int x, int y)
{
	auto* d = asDc(hdc); if (!d) return FALSE; ensureSurface(d);
	if (d->pen && d->pen->style != -1)
	{
		setSource(d->cr, d->pen->color);
		cairo_set_line_width(d->cr, d->pen->width);
		cairo_move_to(d->cr, d->pos.x + 0.5, d->pos.y + 0.5);
		cairo_line_to(d->cr, x + 0.5, y + 0.5);
		cairo_stroke(d->cr);
	}
	d->pos = {x,y};
	return TRUE;
}
static void fillStroke(DcImpl* d)
{
	if (d->brush && d->brush->style != -1) { setSource(d->cr, d->brush->color); cairo_fill_preserve(d->cr); }
	if (d->pen && d->pen->style != -1) { setSource(d->cr, d->pen->color); cairo_set_line_width(d->cr, d->pen->width); cairo_stroke(d->cr); }
	else cairo_new_path(d->cr);
}
BOOL Rectangle(HDC hdc, int l, int t, int r, int b)
{
	auto* d = asDc(hdc); if (!d) return FALSE; ensureSurface(d);
	cairo_rectangle(d->cr, l, t, r-l, b-t);
	fillStroke(d);
	return TRUE;
}
BOOL Ellipse(HDC hdc, int l, int t, int r, int b)
{
	auto* d = asDc(hdc); if (!d) return FALSE; ensureSurface(d);
	double cx=(l+r)/2.0, cy=(t+b)/2.0, rx=(r-l)/2.0, ry=(b-t)/2.0;
	cairo_save(d->cr); cairo_translate(d->cr, cx, cy); cairo_scale(d->cr, rx, ry); cairo_arc(d->cr, 0, 0, 1, 0, 2*M_PI); cairo_restore(d->cr);
	fillStroke(d);
	return TRUE;
}
BOOL RoundRect(HDC hdc, int l, int t, int r, int b, int, int) { return Rectangle(hdc,l,t,r,b); }
BOOL Polygon(HDC hdc, const POINT* pts, int n)
{
	auto* d = asDc(hdc); if (!d || !pts || n<2) return FALSE; ensureSurface(d);
	cairo_move_to(d->cr, pts[0].x, pts[0].y);
	for (int i=1;i<n;++i) cairo_line_to(d->cr, pts[i].x, pts[i].y);
	cairo_close_path(d->cr);
	fillStroke(d);
	return TRUE;
}
BOOL Polyline(HDC hdc, const POINT* pts, int n)
{
	auto* d = asDc(hdc); if (!d || !pts || n<2) return FALSE; ensureSurface(d);
	if (d->pen && d->pen->style != -1) { setSource(d->cr, d->pen->color); cairo_set_line_width(d->cr, d->pen->width); }
	cairo_move_to(d->cr, pts[0].x+0.5, pts[0].y+0.5);
	for (int i=1;i<n;++i) cairo_line_to(d->cr, pts[i].x+0.5, pts[i].y+0.5);
	cairo_stroke(d->cr);
	return TRUE;
}
BOOL PolyBezier(HDC hdc, const POINT* pts, DWORD n) { return Polyline(hdc, pts, (int)n); }
BOOL Arc(HDC hdc, int l, int t, int r, int b, int, int, int, int) { return Ellipse(hdc,l,t,r,b); }
BOOL Pie(HDC hdc, int l, int t, int r, int b, int, int, int, int) { return Ellipse(hdc,l,t,r,b); }
BOOL Chord(HDC hdc, int l, int t, int r, int b, int, int, int, int) { return Ellipse(hdc,l,t,r,b); }

COLORREF SetPixel(HDC hdc, int x, int y, COLORREF color)
{
	auto* d = asDc(hdc); if (!d) return 0; ensureSurface(d);
	setSource(d->cr, color);
	cairo_rectangle(d->cr, x, y, 1, 1);
	cairo_fill(d->cr);
	return color;
}
COLORREF GetPixel(HDC, int, int) { return 0; }
int GetDeviceCaps(HDC, int index)
{
	switch (index)
	{
	case LOGPIXELSX: case LOGPIXELSY: return 96;
	case HORZRES: return GetSystemMetrics(SM_CXSCREEN);
	case VERTRES: return GetSystemMetrics(SM_CYSCREEN);
	case BITSPIXEL: return 32;
	case PLANES: return 1;
	case NUMCOLORS: return -1;
	case TECHNOLOGY: return 1;
	default: return 0;
	}
}
int GetObjectW(HANDLE obj, int c, LPVOID buf)
{
	auto* o = asObj(obj);
	if (!o || !buf) return 0;
	if (o->type == GdiObj::Bitmap && c >= (int)sizeof(BITMAP))
	{
		BITMAP bm{};
		bm.bmWidth = o->bmpW; bm.bmHeight = o->bmpH; bm.bmPlanes = 1; bm.bmBitsPixel = 32;
		bm.bmWidthBytes = o->bmpW * 4; bm.bmBits = o->bits;
		memcpy(buf, &bm, sizeof(bm));
		return sizeof(bm);
	}
	if (o->type == GdiObj::Font && c >= (int)sizeof(LOGFONTW))
	{
		LOGFONTW lf{};
		lf.lfHeight = o->height; lf.lfWeight = o->weight; lf.lfItalic = o->italic;
		wcsncpy(lf.lfFaceName, o->face.c_str(), 31);
		memcpy(buf, &lf, sizeof(lf));
		return sizeof(lf);
	}
	return 0;
}

HRGN CreateRectRgn(int l, int t, int r, int b)
{
	auto* o = new GdiObj(); o->type = GdiObj::Region; o->rgn = {l,t,r,b}; return (HRGN)o;
}
HRGN CreateRectRgnIndirect(const RECT* rc) { return CreateRectRgn(rc->left, rc->top, rc->right, rc->bottom); }
HRGN CreateRoundRectRgn(int l,int t,int r,int b,int,int) { return CreateRectRgn(l,t,r,b); }
HRGN CreateEllipticRgn(int l,int t,int r,int b) { return CreateRectRgn(l,t,r,b); }
HRGN CreatePolygonRgn(const POINT*, int, int) { return CreateRectRgn(0,0,1,1); }
int CombineRgn(HRGN dst, HRGN a, HRGN, int)
{
	auto* d = asObj(dst); auto* s = asObj(a);
	if (d && s) d->rgn = s->rgn;
	return SIMPLEREGION;
}
int OffsetRgn(HRGN rgn, int x, int y)
{
	auto* o = asObj(rgn); if (!o) return ERROR;
	OffsetRect(&o->rgn, x, y);
	return SIMPLEREGION;
}
int GetClipBox(HDC hdc, LPRECT rc)
{
	auto* d = asDc(hdc);
	if (rc) { rc->left=0; rc->top=0; rc->right=d?d->w:1; rc->bottom=d?d->h:1; }
	return SIMPLEREGION;
}
int SelectClipRgn(HDC, HRGN) { return SIMPLEREGION; }
int IntersectClipRect(HDC, int, int, int, int) { return SIMPLEREGION; }
int ExcludeClipRect(HDC, int, int, int, int) { return SIMPLEREGION; }
int SaveDC(HDC hdc) { auto* d = asDc(hdc); if (d && d->cr) cairo_save(d->cr); return 1; }
BOOL RestoreDC(HDC hdc, int) { auto* d = asDc(hdc); if (d && d->cr) cairo_restore(d->cr); return TRUE; }
BOOL SetViewportOrgEx(HDC hdc, int x, int y, LPPOINT pt)
{
	auto* d = asDc(hdc); if (pt) *pt = d?d->pos:POINT{};
	if (d && d->cr) cairo_translate(d->cr, x, y);
	return TRUE;
}
BOOL GetViewportOrgEx(HDC, LPPOINT pt) { if (pt) pt->x=pt->y=0; return TRUE; }
BOOL SetWindowOrgEx(HDC h, int x, int y, LPPOINT p) { return SetViewportOrgEx(h,x,y,p); }
BOOL OffsetViewportOrgEx(HDC h, int x, int y, LPPOINT p) { return SetViewportOrgEx(h,x,y,p); }
BOOL SetViewportExtEx(HDC, int, int, LPSIZE) { return TRUE; }
BOOL SetWindowExtEx(HDC, int, int, LPSIZE) { return TRUE; }
int SetMapMode(HDC, int mode) { return mode; }
int SetROP2(HDC hdc, int rop) { auto* d=asDc(hdc); if(!d) return 0; int o=d->rop2; d->rop2=rop; return o; }
int SetStretchBltMode(HDC, int mode) { return mode; }
int SetPolyFillMode(HDC, int mode) { return mode; }
BOOL LPtoDP(HDC, LPPOINT, int) { return TRUE; }
BOOL DPtoLP(HDC, LPPOINT, int) { return TRUE; }
HGDIOBJ GetCurrentObject(HDC hdc, UINT type)
{
	auto* d = asDc(hdc); if (!d) return nullptr;
	if (type == OBJ_PEN) return d->pen;
	if (type == OBJ_BRUSH) return d->brush;
	if (type == OBJ_FONT) return d->font;
	if (type == OBJ_BITMAP) return d->bitmap;
	return nullptr;
}
BOOL GetDCOrgEx(HDC, LPPOINT pt) { if (pt) pt->x=pt->y=0; return TRUE; }
BOOL BeginPath(HDC hdc) { auto* d=asDc(hdc); if(d&&d->cr) cairo_new_path(d->cr); return TRUE; }
BOOL EndPath(HDC) { return TRUE; }
BOOL StrokePath(HDC hdc) { auto* d=asDc(hdc); if(d&&d->cr) cairo_stroke(d->cr); return TRUE; }
BOOL FillPath(HDC hdc) { auto* d=asDc(hdc); if(d&&d->cr) cairo_fill(d->cr); return TRUE; }
BOOL StrokeAndFillPath(HDC hdc) { auto* d=asDc(hdc); if(d&&d->cr){ cairo_fill_preserve(d->cr); cairo_stroke(d->cr);} return TRUE; }
int SetGraphicsMode(HDC, int mode) { return mode; }
BOOL SetWorldTransform(HDC, const XFORM*) { return TRUE; }
BOOL GetWorldTransform(HDC, XFORM* x) { if(x) memset(x,0,sizeof(*x)); return TRUE; }
BOOL ModifyWorldTransform(HDC, const XFORM*, DWORD) { return TRUE; }
int GetDIBits(HDC, HBITMAP, UINT, UINT, LPVOID, LPBITMAPINFO, UINT) { return 0; }
int SetDIBits(HDC, HBITMAP, UINT, UINT, const void*, const BITMAPINFO*, UINT) { return 0; }
int SetDIBitsToDevice(HDC,int,int,DWORD,DWORD,int,int,UINT,UINT,const void*,const BITMAPINFO*,UINT) { return 0; }
int StretchDIBits(HDC,int,int,int,int,int,int,int,int,const void*,const BITMAPINFO*,UINT,DWORD) { return 0; }
BOOL MaskBlt(HDC d,int x,int y,int cx,int cy,HDC s,int sx,int sy,HBITMAP,int,int,DWORD) { return BitBlt(d,x,y,cx,cy,s,sx,sy,SRCCOPY); }
HPALETTE CreatePalette(const LOGPALETTE*) { return (HPALETTE)g_blackPen; }
UINT RealizePalette(HDC) { return 0; }
HPALETTE SelectPalette(HDC, HPALETTE pal, BOOL) { return pal; }
COLORREF GetNearestColor(HDC, COLORREF c) { return c; }
UINT GetDIBColorTable(HDC, UINT, UINT, RGBQUAD*) { return 0; }
UINT SetDIBColorTable(HDC, UINT, UINT, const RGBQUAD*) { return 0; }
int EnumFontFamiliesExW(HDC, LPLOGFONTW, FONTENUMPROCW, LPARAM, DWORD) { return 1; }
DWORD GetGlyphIndicesW(HDC, LPCWSTR s, int c, LPWORD gi, DWORD)
{
	if (gi) for (int i=0;i<c;++i) gi[i]=(WORD)(s?s[i]:0);
	return c;
}
BOOL GetCharWidth32W(HDC hdc, UINT first, UINT last, LPINT buffer)
{
	if (!buffer) return FALSE;
	for (UINT i=first;i<=last;++i) buffer[i-first]=8;
	return TRUE;
}
BOOL GetTextExtentExPointW(HDC hdc, LPCWSTR s, int c, int, LPINT fit, LPINT, LPSIZE sz)
{
	if (fit) *fit = c;
	return GetTextExtentPoint32W(hdc,s,c,sz);
}
UINT GetOutlineTextMetricsW(HDC, UINT, LPOUTLINETEXTMETRICW) { return 0; }
DWORD GetFontData(HDC, DWORD, DWORD, LPVOID, DWORD) { return (DWORD)-1; }
int AddFontResourceExW(LPCWSTR, DWORD, PVOID) { return 1; }
BOOL RemoveFontResourceExW(LPCWSTR, DWORD, PVOID) { return TRUE; }
HANDLE AddFontMemResourceEx(PVOID, DWORD, PVOID, DWORD* num) { if(num)*num=1; return (HANDLE)1; }
int GetObjectType(HGDIOBJ obj) { auto* o=asObj(obj); return o?(int)o->type+1:0; }
BOOL PtInRegion(HRGN rgn, int x, int y) { auto* o=asObj(rgn); POINT p{x,y}; return o && PtInRect(&o->rgn,p); }
BOOL RectInRegion(HRGN rgn, const RECT* rc) { auto* o=asObj(rgn); return o && rc && IntersectRect(const_cast<RECT*>(rc), &o->rgn, rc); }
int GetRgnBox(HRGN rgn, LPRECT rc) { auto* o=asObj(rgn); if(o&&rc)*rc=o->rgn; return SIMPLEREGION; }
BOOL EqualRgn(HRGN a, HRGN b) { auto* oa=asObj(a); auto* ob=asObj(b); return oa&&ob&&EqualRect(&oa->rgn,&ob->rgn); }
DWORD GetRegionData(HRGN, DWORD, LPRGNDATA) { return 0; }
BOOL FillRgn(HDC hdc, HRGN rgn, HBRUSH br) { auto* o=asObj(rgn); return o && FillRect(hdc,&o->rgn,br); }
BOOL FrameRgn(HDC hdc, HRGN rgn, HBRUSH br, int, int) { auto* o=asObj(rgn); return o && FrameRect(hdc,&o->rgn,br); }
BOOL PaintRgn(HDC hdc, HRGN rgn) { return FillRgn(hdc,rgn,GetStockObject(WHITE_BRUSH)); }
BOOL InvertRgn(HDC hdc, HRGN rgn) { auto* o=asObj(rgn); return o && InvertRect(hdc,&o->rgn); }
int OffsetClipRgn(HDC, int, int) { return SIMPLEREGION; }
int GetClipRgn(HDC, HRGN) { return 0; }
HDC CreateDCW(LPCWSTR, LPCWSTR, LPCWSTR, const DEVMODEW*) { return CreateCompatibleDC(nullptr); }
HDC CreateICW(LPCWSTR a, LPCWSTR b, LPCWSTR c, const DEVMODEW* d) { return CreateDCW(a,b,c,d); }
int StartDocW(HDC, const DOCINFOW*) { return 1; }
int EndDoc(HDC) { return 1; }
int StartPage(HDC) { return 1; }
int EndPage(HDC) { return 1; }
int AbortDoc(HDC) { return 1; }
int SetAbortProc(HDC, ABORTPROC) { return 1; }
int Escape(HDC, int, int, LPCSTR, LPVOID) { return 0; }
int ExtEscape(HDC, int, int, LPCSTR, int, LPSTR) { return 0; }

BOOL SetRect(LPRECT rc, int l, int t, int r, int b) { if(!rc) return FALSE; rc->left=l; rc->top=t; rc->right=r; rc->bottom=b; return TRUE; }
BOOL SetRectEmpty(LPRECT rc) { return SetRect(rc,0,0,0,0); }
BOOL CopyRect(LPRECT dst, const RECT* src) { if(!dst||!src) return FALSE; *dst=*src; return TRUE; }
BOOL InflateRect(LPRECT rc, int dx, int dy) { if(!rc) return FALSE; rc->left-=dx; rc->right+=dx; rc->top-=dy; rc->bottom+=dy; return TRUE; }
BOOL OffsetRect(LPRECT rc, int dx, int dy) { if(!rc) return FALSE; rc->left+=dx; rc->right+=dx; rc->top+=dy; rc->bottom+=dy; return TRUE; }
BOOL IsRectEmpty(const RECT* rc) { return !rc || rc->left>=rc->right || rc->top>=rc->bottom; }
BOOL EqualRect(const RECT* a, const RECT* b) { return a&&b&&a->left==b->left&&a->top==b->top&&a->right==b->right&&a->bottom==b->bottom; }
BOOL IntersectRect(LPRECT dst, const RECT* a, const RECT* b)
{
	if(!dst||!a||!b) return FALSE;
	dst->left=std::max(a->left,b->left); dst->top=std::max(a->top,b->top);
	dst->right=std::min(a->right,b->right); dst->bottom=std::min(a->bottom,b->bottom);
	return !IsRectEmpty(dst);
}
BOOL UnionRect(LPRECT dst, const RECT* a, const RECT* b)
{
	if(!dst||!a||!b) return FALSE;
	dst->left=std::min(a->left,b->left); dst->top=std::min(a->top,b->top);
	dst->right=std::max(a->right,b->right); dst->bottom=std::max(a->bottom,b->bottom);
	return TRUE;
}
BOOL SubtractRect(LPRECT dst, const RECT* a, const RECT*) { if(dst&&a)*dst=*a; return TRUE; }
BOOL PtInRect(const RECT* rc, POINT pt) { return rc && pt.x>=rc->left && pt.x<rc->right && pt.y>=rc->top && pt.y<rc->bottom; }

HBRUSH GetSysColorBrush(int idx) { return CreateSolidBrush(GetSysColor(idx)); }
BOOL SetSysColors(int, const INT*, const COLORREF*) { return TRUE; }
UINT SetTextCharacterExtra(HDC, int extra) { return extra; }
int TabbedTextOutW(HDC hdc, int x, int y, LPCWSTR s, int count, int, const INT*, int) { TextOutW(hdc,x,y,s,count); return 0; }
DWORD GetTabbedTextExtentW(HDC hdc, LPCWSTR s, int count, int, const INT*) { SIZE sz{}; GetTextExtentPoint32W(hdc,s,count,&sz); return MAKELONG(sz.cx,sz.cy); }
BOOL GrayStringW(HDC hdc, HBRUSH, GRAYSTRINGPROC, LPARAM l, int cch, int x, int y, int, int)
{ return TextOutW(hdc,x,y,(LPCWSTR)l,cch); }
HICON CopyIcon(HICON icon) { return icon; }
BOOL DestroyIcon(HICON) { return TRUE; }
BOOL DestroyCursor(HCURSOR) { return TRUE; }
HICON CreateIconIndirect(PICONINFO) { return (HICON)1; }
BOOL GetIconInfo(HICON, PICONINFO p)
{
	if (p) { p->fIcon=TRUE; p->xHotspot=p->yHotspot=0; p->hbmMask=p->hbmColor=nullptr; }
	return TRUE;
}
HBITMAP CopyImage(HANDLE h, UINT, int, int, UINT) { return (HBITMAP)h; }
HCURSOR LoadCursorFromFileW(LPCWSTR) { return LoadCursorW(nullptr, IDC_ARROW); }
BOOL ClipCursor(const RECT*) { return TRUE; }
BOOL GetClipCursor(LPRECT rc) { if(rc) SetRect(rc,0,0,GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN)); return TRUE; }
BOOL GetCursorInfo(PCURSORINFO pci) { if(pci){ pci->flags=1; pci->hCursor=GetCursor(); GetCursorPos(&pci->ptScreenPos);} return TRUE; }
BOOL CreateCaret(HWND, HBITMAP, int, int) { return TRUE; }
BOOL DestroyCaret(void) { return TRUE; }
BOOL ShowCaret(HWND) { return TRUE; }
BOOL HideCaret(HWND) { return TRUE; }
BOOL SetCaretPos(int, int) { return TRUE; }
BOOL GetCaretPos(LPPOINT pt) { if(pt) pt->x=pt->y=0; return TRUE; }
