#ifndef _GUI_TEXTEDITOR_H
#define _GUI_TEXTEDITOR_H

class PreviewTextCtrl;
class TextEditDialogBase;
class TextEditDialog;
class TextSteamEditDialog;
class TextOpcodeDialog;
class TextExportDialog;

#include "gui.h"
#include "Charmaps.h"
#include "File_Manipulation.h"
using namespace std;

#define TEXT_STYLE_DEFAULT		0
#define TEXT_STYLE_HELP			1
#define TEXT_STYLE_DESCRIPTION	2

class PreviewTextCtrl : public wxRichTextCtrl {
public:
	TextEditDialog* parent_dialog;

	virtual void PaintBackground(wxDC& dc);
	PreviewTextCtrl(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& value = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxRE_MULTILINE);
};

class TextEditDialogBase {
public:
	FF9String text;
	wxTimer* timer;
	PreviewTextCtrl* preview_ctrl;
	wxColour text_colour;
	wstring::size_type str_pos;
	uint16_t choice_pos;
	uint16_t wait;
	int text_style;

	TextEditDialogBase(FF9String& str, wxTimer* tim, PreviewTextCtrl* richctrl, int style) : text(str), timer(tim), preview_ctrl(richctrl), text_style(style) {}

	virtual bool ProcessPreviewText() { return true; }
};

class TextEditDialog : public TextEditWindow, public TextEditDialogBase {
public:
	CharmapDataStruct* charmap;
	CharmapDataStruct* charmap_Ext;
	uint32_t max_length;
	uint16_t code_pos;
	uint16_t format_amount;
	TextFormatStruct* format_data;
	bool* format_removed;
	
	TextEditDialog(wxWindow* parent, FF9String& str, unsigned int extrasize, int style = TEXT_STYLE_DEFAULT, CharmapDataStruct* chmap = NULL, CharmapDataStruct* chmapext = NULL, int sizex = -1, int sizey = -1, uint16_t formatamount = 0, TextFormatStruct* format = NULL);
	~TextEditDialog();
	int ShowModal();
	bool ProcessPreviewText();
	void PreviewText();
	
private:
	TextOpcodeDialog* op_dial;
	
	void RefreshOpcodeList();
	void CalculateBestSize(bool x = true, bool y = true);
	
	void OnTextEdit(wxCommandEvent& event);
	void OnOpcodeEdit(wxCommandEvent& event);
	void OnButtonClick(wxCommandEvent& event);
	void OnTimer(wxTimerEvent& event);
};

class TextOpcodeDialog : public TextOpcodeWindow {
public:
	TextEditDialog* parent_dialog;
	FF9String text;
	uint8_t code[256];
	uint8_t length;
	uint32_t max_length;
	uint16_t code_pos;

	TextOpcodeDialog(TextEditDialog* parent);
	~TextOpcodeDialog();
	int ShowModal(int codeindex);

private:
	wxString str;
	wxString token_str;
	wxSpinCtrl* var_spin[10];
	uint8_t token_sel;

	void SetOpcodeSelection();
	void DisplayTokens(wxDC& dc);
	void OnOpcodeChange(wxCommandEvent& event);
	void OnArgumentChange(wxSpinEvent& event);
	void OnButtonClick(wxCommandEvent& event);
	void OnTokenMouse(wxMouseEvent& event);
	void OnTokenPaint(wxPaintEvent& event);
};

class TextSteamEditDialog : public TextSteamEditWindow, public TextEditDialogBase {
public:
	bool has_multilang;
	SteamLanguage multilang[STEAM_LANGUAGE_AMOUNT];
	wxStaticText* multilangname[STEAM_LANGUAGE_AMOUNT-1];
	wxTextCtrl* multilangctrl[STEAM_LANGUAGE_AMOUNT-1];
	wxButton* multilangbtn[STEAM_LANGUAGE_AMOUNT-1];

	TextSteamEditDialog(wxWindow* parent, FF9String& str, int style = TEXT_STYLE_DEFAULT);
	~TextSteamEditDialog();
	int ShowModal();
	bool ProcessPreviewText();
	void PreviewText();

private:
//	TextOpcodeDialog* op_dial;

	void CalculateBestSize();

	void OnButtonClick(wxCommandEvent& event);
	void OnTimer(wxTimerEvent& event);
};

class TextExportDialog : public TextExportWindow {
public:
	TextExportDialog(wxWindow* parent) : TextExportWindow(parent) {}

private:
	void OnButtonClick(wxCommandEvent& event);
};

#endif
