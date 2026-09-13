#include "CustomFileDialog.h"

#include <gtk/gtk.h>
#include "npp_platform.h"

class CustomFileDialog::Impl
{
public:
	explicit Impl(HWND hwnd) : _owner(hwnd) {}
	HWND _owner = nullptr;
	std::wstring _title;
	std::wstring _folder;
	std::wstring _defName;
	std::wstring _defExt;
	std::vector<std::pair<std::wstring, std::wstring>> _filters;
	int _filterIndex = 0;
	std::wstring _checkboxText;
	bool _checkboxActive = false;
	bool _checkboxState = false;
	std::wstring _fileTypeText;
	bool _fileTypeValue = false;
	bool _saveAsCopy = false;
	bool _openCopy = false;
	bool _readOnly = false;

	std::wstring run(GtkFileChooserAction action, bool multi, std::vector<std::wstring>* many)
	{
		GtkWindow* parent = nullptr;
		if (auto* w = implFrom(_owner); w && w->widget && GTK_IS_WINDOW(w->widget))
			parent = GTK_WINDOW(w->widget);
		const char* accept = (action == GTK_FILE_CHOOSER_ACTION_SAVE) ? "Save" : "Open";
		GtkWidget* dlg = gtk_file_chooser_dialog_new(
			nppWideToUtf8(_title.empty() ? L"Notepad++" : _title).c_str(),
			parent, action, "Cancel", GTK_RESPONSE_CANCEL, accept, GTK_RESPONSE_ACCEPT, nullptr);
		if (!_folder.empty())
			gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dlg), nppWideToUtf8(_folder).c_str());
		if (!_defName.empty() && action == GTK_FILE_CHOOSER_ACTION_SAVE)
			gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dlg), nppWideToUtf8(_defName).c_str());
		if (multi)
			gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(dlg), TRUE);
		for (auto& f : _filters)
		{
			GtkFileFilter* gf = gtk_file_filter_new();
			gtk_file_filter_set_name(gf, nppWideToUtf8(f.first).c_str());
			std::wstring spec = f.second;
			size_t start = 0;
			while (start < spec.size())
			{
				size_t sc = spec.find(L';', start);
				std::wstring pat = spec.substr(start, sc == std::wstring::npos ? std::wstring::npos : sc - start);
				gtk_file_filter_add_pattern(gf, nppWideToUtf8(pat).c_str());
				if (sc == std::wstring::npos) break;
				start = sc + 1;
			}
			gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dlg), gf);
		}
		std::wstring result;
		if (gtk_dialog_run(GTK_DIALOG(dlg)) == GTK_RESPONSE_ACCEPT)
		{
			if (many && multi)
			{
				GSList* list = gtk_file_chooser_get_filenames(GTK_FILE_CHOOSER(dlg));
				for (GSList* p = list; p; p = p->next)
				{
					many->push_back(nppUtf8ToWide((char*)p->data));
					g_free(p->data);
				}
				g_slist_free(list);
				if (!many->empty()) result = many->front();
			}
			else
			{
				char* fn = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dlg));
				if (fn) { result = nppUtf8ToWide(fn); g_free(fn); }
			}
		}
		gtk_widget_destroy(dlg);
		return result;
	}
};

CustomFileDialog::CustomFileDialog(HWND hwnd) : _impl(std::make_unique<Impl>(hwnd)) {}
CustomFileDialog::~CustomFileDialog() = default;
void CustomFileDialog::setTitle(const wchar_t* title) { _impl->_title = title ? title : L""; }
void CustomFileDialog::setExtFilter(const wchar_t* text, const wchar_t* ext)
{
	_impl->_filters.emplace_back(text ? text : L"", ext ? ext : L"*");
}
void CustomFileDialog::setExtFilter(const wchar_t* text, std::initializer_list<const wchar_t*> exts)
{
	std::wstring joined;
	for (auto* e : exts) { if (!joined.empty()) joined += L";"; joined += e ? e : L"*"; }
	setExtFilter(text, joined.c_str());
}
void CustomFileDialog::setDefExt(const wchar_t* ext) { _impl->_defExt = ext ? ext : L""; }
void CustomFileDialog::setDefFileName(const wchar_t* fn) { _impl->_defName = fn ? fn : L""; }
void CustomFileDialog::setFolder(const wchar_t* folder) { _impl->_folder = folder ? folder : L""; }
void CustomFileDialog::setCheckbox(const wchar_t* text, bool isActive)
{
	_impl->_checkboxText = text ? text : L"";
	_impl->_checkboxActive = isActive;
	_impl->_checkboxState = isActive;
}
void CustomFileDialog::setExtIndex(int extTypeIndex) { _impl->_filterIndex = extTypeIndex; }
void CustomFileDialog::setSaveAsCopy(bool isSavingAsCopy) { _impl->_saveAsCopy = isSavingAsCopy; }
bool CustomFileDialog::getOpenTheCopyAfterSaveAsCopy() { return _impl->_openCopy; }
void CustomFileDialog::enableFileTypeCheckbox(const std::wstring& text, bool value)
{
	_impl->_fileTypeText = text;
	_impl->_fileTypeValue = value;
}
bool CustomFileDialog::getFileTypeCheckboxValue() const { return _impl->_fileTypeValue; }
std::wstring CustomFileDialog::doSaveDlg() { return _impl->run(GTK_FILE_CHOOSER_ACTION_SAVE, false, nullptr); }
std::wstring CustomFileDialog::pickFolder() { return _impl->run(GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER, false, nullptr); }
std::wstring CustomFileDialog::doOpenSingleFileDlg() { return _impl->run(GTK_FILE_CHOOSER_ACTION_OPEN, false, nullptr); }
std::vector<std::wstring> CustomFileDialog::doOpenMultiFilesDlg()
{
	std::vector<std::wstring> many;
	_impl->run(GTK_FILE_CHOOSER_ACTION_OPEN, true, &many);
	return many;
}
bool CustomFileDialog::getCheckboxState() const { return _impl->_checkboxState; }
bool CustomFileDialog::isReadOnly() const { return _impl->_readOnly; }
