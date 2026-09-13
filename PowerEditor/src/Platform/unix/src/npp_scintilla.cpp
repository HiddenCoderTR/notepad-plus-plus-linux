#include "npp_platform.h"

#define GTK
#include "Scintilla.h"
#include "ScintillaWidget.h"

#include <gtk/gtk.h>

static sptr_t nppSciDirect(sptr_t ptr, unsigned int iMessage, uptr_t wParam, sptr_t lParam)
{
	return scintilla_send_message(reinterpret_cast<ScintillaObject*>(ptr), iMessage, wParam, lParam);
}

struct SciData {
	GtkWidget* sci = nullptr;
};

static SciData* sciOf(WindowImpl* w)
{
	if (!w->controlData) w->controlData = new SciData();
	return (SciData*)w->controlData;
}

static void onSciNotify(GtkWidget*, gint, SCNotification* scn, gpointer data)
{
	auto* w = (WindowImpl*)data;
	if (!w) return;
	NMHDR* hdr = &scn->nmhdr;
	hdr->hwndFrom = hwndFrom(w);
	hdr->idFrom = w->id;
	auto* parent = implFrom(w->parent);
	if (parent)
		nppDispatch(parent, WM_NOTIFY, (WPARAM)w->id, (LPARAM)scn);
}

LRESULT nppScintillaSend(WindowImpl* w, UINT msg, WPARAM wp, LPARAM lp)
{
	auto* sd = sciOf(w);
	if (msg == WM_CREATE || msg == WM_NCCREATE)
	{
		if (!sd->sci)
		{
			sd->sci = scintilla_new();
			w->native = sd->sci;
			g_signal_connect(sd->sci, SCINTILLA_NOTIFY, G_CALLBACK(onSciNotify), w);
			if (w->widget && GTK_IS_CONTAINER(w->widget))
			{
				gtk_container_add(GTK_CONTAINER(w->widget), sd->sci);
				gtk_widget_show(sd->sci);
			}
			else if (w->client && GTK_IS_CONTAINER(w->client))
			{
				gtk_container_add(GTK_CONTAINER(w->client), sd->sci);
				gtk_widget_show(sd->sci);
			}
		}
		return TRUE;
	}
	if (!sd->sci) return 0;
	if (msg == SCI_GETDIRECTFUNCTION)
		return (LRESULT)nppSciDirect;
	if (msg == SCI_GETDIRECTPOINTER)
		return (LRESULT)SCINTILLA(sd->sci);
	return scintilla_send_message(SCINTILLA(sd->sci), msg, wp, lp);
}

extern "C" int Scintilla_RegisterClasses(void*)
{
	WNDCLASSEXW wc{};
	wc.cbSize = sizeof(wc);
	wc.lpfnWndProc = DefWindowProcW;
	wc.lpszClassName = L"Scintilla";
	nppRegisterClass(L"Scintilla", wc);
	return TRUE;
}
extern "C" int Scintilla_ReleaseResources(void) { return TRUE; }
