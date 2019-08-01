#ifndef _GUI_MANIPULATION_H
#define _GUI_MANIPULATION_H

#include "Configuration.h"
#include "Gui_Preferences.h"
#include "gui.h"

class CDDataStruct : public CDPanel {
public:
	string filename;
	ConfigurationSet& config;
	ClusterSet cluster;
	SaveSet saveset;
	UnusedSaveBackup backupset;
	GameType gametype;
	SpellDataSet spellset;
	bool spellloaded;
	bool spellmodified;
	SupportDataSet supportset;
	bool supportloaded;
	bool supportmodified;
	CommandDataSet cmdset;
	bool cmdloaded;
	bool cmdmodified;
	StatDataSet statset;
	bool statloaded;
	bool statmodified;
	PartySpecialDataSet partyspecialset;
	bool partyspecialloaded;
	bool partyspecialmodified;
	EnemyDataSet enemyset;
	bool enemyloaded;
	bool enemymodified;
	ItemDataSet itemset;
	bool itemloaded;
	bool itemmodified;
	ShopDataSet shopset;
	bool shoploaded;
	bool shopmodified;
	CardDataSet cardset;
	bool cardloaded;
	bool cardmodified;
	TextDataSet textset;
	bool textloaded;
	bool textmodified;
	WorldMapDataSet worldset;
	bool worldloaded;
	bool worldmodified;
	FieldDataSet fieldset;
	bool fieldloaded;
	bool fieldmodified;
	BattleSceneDataSet sceneset;
	bool sceneloaded;
	bool scenemodified;
	SpellAnimationDataSet spellanimset;
	bool spellanimloaded;
	bool spellanimmodified;
	MenuUIDataSet ffuiset;
	bool ffuiloaded;
	bool ffuimodified;
	MipsDataSet mipsset;
	bool mipsloaded;
	bool mipsmodified;
	CILDataSet cilset;
	bool cilloaded;
	bool cilmodified;
	int fieldselection;
	unsigned int* spellsorted;
	unsigned int* supportsorted;
	unsigned int* cmdsorted;
	unsigned int* enemysorted;
	unsigned int* cardsorted;
	unsigned int* itemsorted;
	unsigned int* keyitemsorted;
	unsigned int* textsorted;
	unsigned int* worldsorted;
	unsigned int* fieldsorted;
	unsigned int* scenesorted;
	unsigned int* spellanimsorted;
	unsigned int* cilsorted;
	
	CDDataStruct(wxWindow* parent, string fname, ConfigurationSet& cfg);
	void MarkDataSpellModified();
	void MarkDataSupportModified();
	void MarkDataCommandModified();
	void MarkDataStatModified();
	void MarkDataPartySpecialModified();
	void MarkDataEnemyModified(unsigned int battleid, Chunk_Type chunktype, bool alllang = false);
	void MarkDataItemModified();
	void MarkDataShopModified();
	void MarkDataCardModified();
	void MarkDataTextModified(unsigned int textid, Chunk_Type chunktype, unsigned int objectnum = 0);
	void MarkDataWorldMapModified(unsigned int worldid, Chunk_Type chunktype, unsigned int objectnum = 0);
	void MarkDataWorldMapScriptModified(unsigned int worldid, bool alllang = false);
	void MarkDataFieldModified(unsigned int fieldid, Chunk_Type chunktype, bool alllang = false);
	void MarkDataBattleSceneModified(unsigned int sceneid, Chunk_Type chunktype, unsigned int objectnum = 0);
	void MarkDataSpellAnimationModified(unsigned int spellanimid, Spell_Animation_Data_Type datatype);
	void MarkDataMenuUIModified();
	void MarkDataMipsModified(int datatype);
	void MarkDataCilModified();
	void ChangeFF9StringCharmap(wchar_t* chmapdef, wchar_t* chmapa, wchar_t* chmapb, ExtendedCharmap& chmapext);
	void ChangeFF9StringOpcodeChar(wchar_t newchar);
	void ChangeFF9StringSteamLanguage(SteamLanguage newlang);
	bool OverwriteBinary();
	bool ExportPPF();
	wstring* ReadHWS(const char* fname, bool* section, bool* sectext, bool* localsec);
	void InitSpell(void);
	void DisplaySpell(int spellid);
	void UpdateSpellName(unsigned int spellid);
	void UpdateSpellStatusName(unsigned int statusid);
	void InitSupport(void);
	void DisplaySupport(int supportid);
	void UpdateSupportName(unsigned int supportid);
	void InitCommand(void);
	void DisplayCommand(int cmdid);
	void UpdateCommandName(unsigned int cmdid);
	void InitStat(void);
	void DisplayLevelStat(void);
	void DisplayStatAbilityList(int abilsetid);
	void DisplayStat(int charid);
	void InitPartySpecial(void);
	void DisplayPartySpecial(int specialid);
	void InitEnemy(void);
	void DisplayEnemy(int battleid);
	void UpdateEnemyName(unsigned int battleid);
	void DisplayEnemyStat(int battleid, int statid);
	void DisplayEnemySpell(int battleid, int spellid);
	void DisplayEnemyGroup(int battleid, int groupid);
	void InitItem(void);
	void DisplayItem(int itemid);
	void DisplayKeyItem(int keyitemid);
	void UpdateItemName(unsigned int itemid);
	void DisplayItemStatIdHelp(void);
	void DisplayItemIcon(void);
	void InitShop(void);
	void DisplayShop(int shopid);
	void DisplaySynthesisShop(int synthshopid);
	void InitCard(void);
	void DisplayCard(int cardid);
	void DisplayCardDeck(int deckid);
	void UpdateCardName(unsigned int cardid);
	void DisplayCardPowerHelp(void);
	void DisplayCardSetHelp(void);
	void InitText(void);
	void DisplayText(int textid);
	void InitWorldMap(void);
	void DisplayWorldMap(int worldid);
	void DisplayWorldPlace(int placeid);
	void DisplayWorldBattle(int worldbattleid);
	int GetWorldBattleSetFromSpot(int spot, int spotversion = 0);
	void DisplayWorldBattleHelp(int spotversion, int whichbattle);
	void InitField(void);
	void DisplayField(int fieldid);
	void InitBattleScene(void);
	void DisplayBattleScene(int sceneid);
	void InitSpellAnimation(void);
	void DisplaySpellAnimation(int spellanimid);
	void InitMenuUI(void);
	void DisplaySpecialText(int textblockid);
	void InitMips(void);
	void DisplayMipsBattle(int mipsbattleid);
	void InitCil(void);
	void DisplayCilStruct(int cilstructid);
	void DisplayCilMacro(int cilmacroid);
	void DisplayCurrentData();
	
	void SpellDisplayNames(bool create=false);
	void SupportDisplayNames(bool create=false);
	void CommandDisplayNames(bool create=false);
	void StatDisplayNames(bool create=false);
	wxString GetEnemyBattleName(int battleid);
	void EnemyDisplayNames(bool create=false);
	void ItemDisplayNames(bool create=false);
	void KeyItemDisplayNames(bool create=false);
	void CardDisplayNames(bool create=false);
	void TextDisplayNames(bool create=false);
	void WorldMapDisplayNames(bool create=false);
	wxString GetFieldName(int fieldid);
	void FieldDisplayNames(bool create=false);
	void BattleSceneDisplayNames(bool create=false);
	void SpellAnimationDisplayNames(bool create=false);
	void CilDisplayNames(bool create=false);
	
private:
	uint16_t* scenetex;
	uint16_t* scenepal;
	int copyenemystat_battleid;
	int copyenemystat_statid;
	int copyenemyspell_battleid;
	int copyenemyspell_spellid;
	BattleSceneImportLinkTextureWindow* scenetexturelink;
	wxBitmap chartexpreview;
	wxBitmap worldchartexpreview;
	wxBitmap fieldtexpreview;
	wxBitmap scenetexpreview;
	wxBitmap scenetexlinkpreview;
	wxMenu* scenetexmenu;
	wxMenu* enemystatmenu;
	wxMenu* enemyspellmenu;
	wxMenu* enemygroupmenu;
	wxMenu* enemytextmenu;
	wxMenu* worldtextmenu;
	wxMenu* textmenu;
	wxMenu* specialtextmenu;
	wxMenuItem* enemystatmenupaste;
	wxMenuItem* enemyspellmenupaste;
	
	void TextReachLimit();
	void OnNotebookMain(wxNotebookEvent& event);
	void OnNotebookParty(wxNotebookEvent& event);
	void OnNotebookInventory(wxNotebookEvent& event);
	void OnNotebookCard(wxNotebookEvent& event);
	void OnNotebookEnvironment(wxNotebookEvent& event);
	void OnNotebookInterface(wxNotebookEvent& event);
	void OnNotebookMips(wxNotebookEvent& event);
	void OnNotebookCil(wxNotebookEvent& event);
	void OnNotebookNone(wxNotebookEvent& event) {} // Without this, wxNotebookEvent are passed to children notebooks for some reason
	void OnListBoxSpell(wxCommandEvent& event);
	void OnSpellChangeName(wxCommandEvent& event);
	void OnSpellChangeHelp(wxCommandEvent& event);
	void OnSpellChangeSpin(wxSpinEvent& event);
	void OnSpellChangeChoice(wxCommandEvent& event);
	void OnSpellChangeFlags(wxCommandEvent& event);
	void OnSpellChangeButton(wxCommandEvent& event);
	void OnButtonClickSpellModel(wxCommandEvent& event);
	void OnButtonClickSpellModelAlt(wxCommandEvent& event);
	void OnListBoxSupport(wxCommandEvent& event);
	void OnSupportChangeName(wxCommandEvent& event);
	void OnSupportChangeHelp(wxCommandEvent& event);
	void OnSupportChangeSpin(wxSpinEvent& event);
	void OnSupportChangeButton(wxCommandEvent& event);
	void OnListBoxCommand(wxCommandEvent& event);
	void OnCommandChangeName(wxCommandEvent& event);
	void OnCommandChangeHelp(wxCommandEvent& event);
	void OnCommandChangeChoice(wxCommandEvent& event);
	void OnCommandChangeButton(wxCommandEvent& event);
	void OnListBoxStat(wxCommandEvent& event);
	void OnStatChangeDefaultName(wxCommandEvent& event);
	void OnStatChangeSpin(wxSpinEvent& event);
	void OnStatChangeChoice(wxCommandEvent& event);
	void OnStatChangeButton(wxCommandEvent& event);
	void OnStatChangeList(wxCommandEvent& event);
	void OnListBoxPartySpecial(wxCommandEvent& event);
	void OnPartySpecialChangeChoice(wxCommandEvent& event);
	void OnPartySpecialChangeList(wxCommandEvent& event);
	void OnListBoxEnemy(wxCommandEvent& event);
	void OnListBoxEnemyStat(wxCommandEvent& event);
	void OnListBoxEnemySpell(wxCommandEvent& event);
	void OnListBoxEnemyGroup(wxCommandEvent& event);
	void OnListBoxEnemyText(wxCommandEvent& event);
	void OnEnemyStatChangeName(wxCommandEvent& event);
	void OnEnemySpellChangeName(wxCommandEvent& event);
	void OnEnemyChangeSpin(wxSpinEvent& event);
	void OnEnemyChangeChoice(wxCommandEvent& event);
	void OnEnemyChangeFlags(wxCommandEvent& event);
	void OnEnemyChangeButton(wxCommandEvent& event);
	void OnEnemyStatRightClick(wxMouseEvent& event);
	void OnEnemySpellRightClick(wxMouseEvent& event);
	void OnEnemyGroupRightClick(wxMouseEvent& event);
	void OnEnemyTextRightClick(wxMouseEvent& event);
	void OnEnemyStatRightClickMenu(wxCommandEvent& event);
	void OnEnemySpellRightClickMenu(wxCommandEvent& event);
	void OnEnemyGroupRightClickMenu(wxCommandEvent& event);
	void OnEnemyTextRightClickMenu(wxCommandEvent& event);
	void OnListBoxItem(wxCommandEvent& event);
	void OnListBoxKeyItem(wxCommandEvent& event);
	void OnItemChangeName(wxCommandEvent& event);
	void OnItemChangeHelp(wxCommandEvent& event);
	void OnItemChangeBattleHelp(wxCommandEvent& event);
	void OnKeyItemChangeName(wxCommandEvent& event);
	void OnKeyItemChangeHelp(wxCommandEvent& event);
	void OnKeyItemChangeDescription(wxCommandEvent& event);
	void OnItemChangeSpin(wxSpinEvent& event);
	void OnItemChangeChoice(wxCommandEvent& event);
	void OnItemChangeFlags(wxCommandEvent& event);
	void OnItemChangeButton(wxCommandEvent& event);
	void OnButtonClickItemModel(wxCommandEvent& event);
	void OnItemPositionListClick(wxMouseEvent& event);
	void OnListBoxShop(wxCommandEvent& event);
	void OnListBoxSynthesisShop(wxCommandEvent& event);
	void OnShopChangeSpin(wxSpinEvent& event);
	void OnShopChangeChoice(wxCommandEvent& event);
	void OnShopChangeFlags(wxCommandEvent& event);
	void OnShopChangeButton(wxCommandEvent& event);
	void OnListBoxCard(wxCommandEvent& event);
	void OnListBoxCardDeck(wxCommandEvent& event);
	void OnCardChangeName(wxCommandEvent& event);
	void OnCardChangeButton(wxCommandEvent& event);
	void OnCardChangeChoice(wxCommandEvent& event);
	void OnCardChangeSpin(wxSpinEvent& event);
	void OnListBoxText(wxCommandEvent& event);
	void OnTextEditText(wxCommandEvent& event);
	void OnTextExportText(wxCommandEvent& event);
	void OnTextCharmapListSelection(wxCommandEvent& event);
	void OnTextCharmapPaletteChoice(wxCommandEvent& event);
	void OnTextExportCharmap(wxCommandEvent& event);
	void OnTextManageCharmap(wxCommandEvent& event);
	void OnTextRightClick(wxMouseEvent& event);
	void OnTextRightClickMenu(wxCommandEvent& event);
	void OnTextCharmapPaint(wxPaintEvent& event);
	void OnListBoxWorldMap(wxCommandEvent& event);
	void OnListBoxWorldPlace(wxCommandEvent& event);
	void OnListBoxWorldBattle(wxCommandEvent& event);
	void OnWorldChangeName(wxCommandEvent& event);
	void OnWorldMapEditText(wxCommandEvent& event);
	void OnWorldMapExportText(wxCommandEvent& event);
	void OnWorldChangeButton(wxCommandEvent& event);
	void OnWorldChangeChoice(wxCommandEvent& event);
	void OnWorldChangeSpin(wxSpinEvent& event);
	void OnWorldTextRightClickMenu(wxCommandEvent& event);
	void OnListBoxField(wxCommandEvent& event);
	void OnFieldChangeName(wxCommandEvent& event);
	void OnFieldChangeChoice(wxCommandEvent& event);
	void OnFieldChangeButton(wxCommandEvent& event);
	void OnFieldTexturePaint(wxPaintEvent& event);
	void OnListBoxBattleScene(wxCommandEvent& event);
	void OnBattleSceneImportLinkTexturePaint(wxPaintEvent& event);
	void OnBattleSceneImportLinkTextureChoice(wxCommandEvent& event);
	void OnBattleSceneImportLinkTextureFocus(wxFocusEvent& event);
	void OnBattleSceneExportButton(wxCommandEvent& event);
	void OnBattleSceneImportButton(wxCommandEvent& event);
	void OnBattleSceneTextureListBox(wxCommandEvent& event);
	void OnBattleSceneManageTexturesButton(wxCommandEvent& event);
	void OnBattleSceneTextureChoice(wxCommandEvent& event);
	void OnBattleScenePaletteChoice(wxCommandEvent& event);
	void OnBattleSceneTextureSelectRightClickMenu(wxCommandEvent& event);
	void OnBattleSceneTextureRightClick(wxMouseEvent& event);
	void OnBattleSceneTexturePaint(wxPaintEvent& event);
	void OnListBoxSpellAnimation(wxCommandEvent& event);
	void OnSpellAnimationChangeButton(wxCommandEvent& event);
	void OnListBoxSpecialText(wxCommandEvent& event);
	void OnSpecialTextEditText(wxCommandEvent& event);
	void OnSpecialTextRightClick(wxMouseEvent& event);
	void OnSpecialTextRightClickMenu(wxCommandEvent& event);
	void OnListBoxMipsBattle(wxCommandEvent& event);
	void OnMipsBattleButton(wxCommandEvent& event);
	void OnListBoxCilStruct(wxCommandEvent& event);
	void OnListBoxCilMethod(wxCommandEvent& event);
	void OnListBoxCilMacro(wxCommandEvent& event);
	void OnCilMethodButton(wxCommandEvent& event);
	void OnCilMacroButton(wxCommandEvent& event);
	void OnCilParameterResolution(wxSpinEvent & event);
	
public:
	void DebugWrite();
};

class ExportPPFMessage : public ExportPPFWindow {
public:
	ExportPPFMessage(wxWindow* parent) : ExportPPFWindow(parent) {}
};

class IOHWSMessage : public IOHWSWindow {
public:
	bool save;
	bool* section;
	bool* sectext;
	wxCheckBox** section_box;
	wxCheckBox** section_text_box;
	
	IOHWSMessage(wxWindow* parent);
	void UpdateLoadableSection();
	void OnChangeFile(wxFileDirPickerEvent& event);
	void OnCheckSection(wxCommandEvent& event);
	int ShowModal(bool sv, bool* sct, bool* scttxt, bool* scton);
};

class LogDialog : public LogWindow {
public:
	LogDialog(wxWindow* parent, LogStruct log) : LogWindow(parent) {
		m_errornum->AppendText(wxString::Format(wxT("%i"),log.error_amount));
		m_warningnum->AppendText(wxString::Format(wxT("%i"),log.warning_amount));
		m_errorctrl->WriteText(_(log.error));
		m_warningctrl->WriteText(_(log.warning));
	}
};

#endif
