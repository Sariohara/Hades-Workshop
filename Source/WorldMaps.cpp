#include "WorldMaps.h"

#include "main.h"
#include "Gui_LoadingDialog.h"
#include "Database_Resource.h"

#define WM_HWS_VERSION 1

// PSX
#define WM_DATA_CHUNK_ID		0x41B
#define WM_SECTION_NAME			0
#define WM_SECTION_BATTLES		3
#define WM_SECTION_FRIENDLIES	4 // Friendly monsters
#define WM_SECTION_TEXTURE_ANIM	6 // Animated textures (beach)
#define WM_SECTION_EVA			37 // Something to do with Memoria's entrance effect
#define WM_SECTION_SPS_START	41 // Start of the list of weather effects
#define WM_SECTION_SPS_END		52 // (12 of them, 52 is included)
#define WM_SECTION_BLOCK_HEADER	66 // Unknown

// Steam
#define WM_CHUNK_STEAM			1
#define WM_CHUNK_AMOUNT_STEAM	11

const vector<uint16_t> WorldMapDataStruct::friendly_battle_id[WORLD_MAP_FRIENDLY_AMOUNT] = {
	   { 251, 363, 364, 838 },
	   { 192, 193, 196, 197, 199 },
	   { 235, 239, 270, 682, 686, 687, 689, 841 },
	   { 216, 217, 652, 664, 668, 670, 751 },
	   { 188, 189, 268, 636, 637, 641, 647 },
	   { 631, 632 },
	   { 365, 367, 368, 595, 605, 606 },
	   { 723 },
	   { 920, 921 }
};

int WorldMapDataStruct::SetName(unsigned int placeid, wstring newvalue) {
	FF9String tmp(place_name[placeid]);
	tmp.SetValue(newvalue);
	int oldlen = place_name[placeid].length;
	int newlen = tmp.length;
	if (GetGameType() == GAME_TYPE_PSX) {
		if (newlen > oldlen + place_name_extra_size)
			return 1;
		place_name_extra_size += oldlen - newlen;
	} else {
		place_name_size += newlen - oldlen;
	}
	place_name[placeid].SetValue(newvalue);
	return 0;
}

int WorldMapDataStruct::SetName(unsigned int placeid, FF9String& newvalue) {
	int oldlen = place_name[placeid].length;
	int newlen = newvalue.length;
	if (GetGameType() == GAME_TYPE_PSX) {
		if (newlen > oldlen + place_name_extra_size)
			return 1;
		place_name_extra_size += oldlen - newlen;
	} else {
		place_name_size += newlen - oldlen;
	}
	place_name[placeid] = newvalue;
	return 0;
}

int WorldMapDataStruct::ChangeBattle(unsigned int groundid, unsigned int setid, uint16_t newid) {
	if (groundid >= WORLD_MAP_BATTLE_GROUND_AMOUNT || setid >= WORLD_MAP_BATTLE_SET_AMOUNT)
		return 2;
	int friendlyidold = -1;
	int friendlyidnew = -1;
	int i, j;
	for (i = 0; i < WORLD_MAP_FRIENDLY_AMOUNT; i++)
		for (j = 0; j < (int)WorldMapDataStruct::friendly_battle_id[i].size(); j++)
		{
			if (battle_id[groundid][setid] == WorldMapDataStruct::friendly_battle_id[i][j])
				friendlyidold = i;
			if (newid == WorldMapDataStruct::friendly_battle_id[i][j])
				friendlyidnew = i;
		}
	battle_id[groundid][setid] = newid;
	if (setid != 3)
		return friendlyidnew >= 0 ? 1 : 0;
	bool cantupdatefriendly = false;
	if (friendlyidold >= 0 && friendlyidold != friendlyidnew) {
		for (i = 0; i < WORLD_MAP_FRIENDLY_AREA_AMOUNT; i++)
			if (friendly_area[friendlyidold][i] == groundid)
				friendly_area[friendlyidold][i] = 0xFFFF;
	}
	if (friendlyidnew >= 0 && friendlyidold != friendlyidnew) {
		cantupdatefriendly = true;
		for (i = 0; i < WORLD_MAP_FRIENDLY_AREA_AMOUNT; i++)
			if (friendly_area[friendlyidnew][i] == 0xFFFF) {
				friendly_area[friendlyidnew][i] = groundid;
				cantupdatefriendly = false;
				break;
			}
	}
	return cantupdatefriendly ? 1 : 0;
}

#define MACRO_WORLDSTRUCT_IOFUNCTION(IO,SEEK,READ,PPF) \
	uint32_t headerpos = f.tellg(); \
	uint8_t zero8 = 0; \
	unsigned int i,j; \
	if (PPF) PPFInitScanStep(f); \
	IO ## Long(f,section_amount); \
	if (READ) { \
		section_offset = new uint32_t[section_amount]; \
		section_size = new uint32_t[section_amount]; \
		section_raw_data = new uint8_t*[section_amount]; \
	} \
	for (i=0;i<section_amount;i++) { \
		IO ## Long(f,section_offset[i]); \
		if (READ && i>0) \
			section_size[i-1] = section_offset[i]-section_offset[i-1]; \
	} \
	if (PPF) PPFEndScanStep(); \
	if (READ) section_size[section_amount-1] = size-section_offset[i-1]; \
	for (i=0;i<section_amount;i++) { \
		if (READ) section_raw_data[i] = new uint8_t[section_size[i]]; \
		SEEK(f,headerpos,section_offset[i]); \
		if (PPF) PPFInitScanStep(f); \
		for (j=0;j<section_size[i];j++) \
			IO ## Char(f,section_raw_data[i][j]); \
		if (PPF) PPFEndScanStep(); \
	}

#define MACRO_WORLDSTRUCT_PLACE(IO,SEEK,READ,PPF) \
	unsigned int k; \
	SEEK(f,headerpos,section_offset[WM_SECTION_NAME]); \
	j = 0; \
	for (i=0;i<WORLD_MAP_PLACE_AMOUNT;i++) { \
		if (PPF) PPFInitScanStep(f,true,place_name[i].length); \
		IO ## FF9String(f,place_name[i]); \
		if (PPF) PPFEndScanStep(); \
		if (READ) { \
			j += place_name[i].length+1; \
			k = 0; \
			while (section_raw_data[WM_SECTION_NAME][j+k]!=0) k++; \
			place_unknown_amount[i] = k; \
			place_unknown[i] = new uint8_t[k]; \
			k = 0; \
			for (k=0;k<place_unknown_amount[i];k++) place_unknown[i][k] = section_raw_data[WM_SECTION_NAME][j+k]; \
			j += k+1; \
			SEEK(f,headerpos,section_offset[WM_SECTION_NAME]+j); \
		} else { \
			if (PPF) PPFInitScanStep(f); \
			IO ## Char(f,zero8); \
			for (k=0;k<place_unknown_amount[i];k++) \
				IO ## Char(f,place_unknown[i][k]); \
			IO ## Char(f,zero8); \
			if (PPF) PPFEndScanStep(); \
		} \
	}

#define MACRO_WORLDSTRUCT_BATTLE(IO,SEEK,READ,PPF) \
	SEEK(f,headerpos,section_offset[WM_SECTION_BATTLES]); \
	if (PPF) PPFInitScanStep(f); \
	for (i=0;i<WORLD_MAP_BATTLE_GROUND_AMOUNT;i++) { \
		for (j=0;j<WORLD_MAP_BATTLE_SET_AMOUNT;j++) \
			IO ## Short(f,battle_id[i][j]); \
		IO ## Short(f,battle_flag[i]); \
	} \
	if (PPF) PPFEndScanStep();

#define MACRO_WORLDSTRUCT_FRIENDLY(IO,SEEK,READ,PPF) \
	SEEK(f,headerpos,section_offset[WM_SECTION_FRIENDLIES]); \
	if (PPF) PPFInitScanStep(f); \
	for (i=0;i<WORLD_MAP_FRIENDLY_AMOUNT;i++) \
		for (j=0;j<WORLD_MAP_FRIENDLY_AREA_AMOUNT;j++) \
			IO ## Short(f,friendly_area[i][j]); \
	if (PPF) PPFEndScanStep();


void WorldMapDataStruct::Read(fstream& f) {
	if (loaded)
		return;
	if (GetGameType()==GAME_TYPE_PSX) {
		MACRO_WORLDSTRUCT_IOFUNCTION(FFIXRead,FFIXSeek,true,false)
		MACRO_WORLDSTRUCT_PLACE(FFIXRead,FFIXSeek,true,false)
		MACRO_WORLDSTRUCT_BATTLE(FFIXRead,FFIXSeek,true,false)
		MACRO_WORLDSTRUCT_FRIENDLY(FFIXRead,FFIXSeek,true,false)
		place_name_extra_size = 0;
	} else {
		MACRO_WORLDSTRUCT_IOFUNCTION(SteamRead,SteamSeek,true,false)
//		MACRO_WORLDSTRUCT_PLACE(SteamRead,SteamSeek,true,false)
		MACRO_WORLDSTRUCT_BATTLE(SteamRead,SteamSeek,true,false)
		MACRO_WORLDSTRUCT_FRIENDLY(SteamRead,SteamSeek,true,false)
		place_name_extra_size = 0xFFFFFF;
	}
	loaded = true;
}

void WorldMapDataStruct::Write(fstream& f) {
	MACRO_WORLDSTRUCT_IOFUNCTION(FFIXWrite,FFIXSeek,false,false)
	MACRO_WORLDSTRUCT_PLACE(FFIXWrite,FFIXSeek,false,false)
	MACRO_WORLDSTRUCT_BATTLE(FFIXWrite,FFIXSeek,false,false)
	MACRO_WORLDSTRUCT_FRIENDLY(FFIXWrite,FFIXSeek,false,false)
	modified = false;
}

void WorldMapDataStruct::WritePPF(fstream& f) {
	MACRO_WORLDSTRUCT_IOFUNCTION(PPFStepAdd,FFIXSeek,false,true)
	MACRO_WORLDSTRUCT_PLACE(PPFStepAdd,FFIXSeek,false,true)
	MACRO_WORLDSTRUCT_BATTLE(PPFStepAdd,FFIXSeek,false,true)
	MACRO_WORLDSTRUCT_FRIENDLY(PPFStepAdd,FFIXSeek,false,true)
}

void WorldMapDataStruct::ReadHWS(fstream& f, bool usetext) {
	uint16_t version;
	HWSReadShort(f,version);
	MACRO_WORLDSTRUCT_IOFUNCTION(HWSRead,HWSSeek,true,false)
	if (usetext && GetHWSGameType() == GAME_TYPE_PSX) {
		MACRO_WORLDSTRUCT_PLACE(HWSRead,HWSSeek,true,false)
	}
	MACRO_WORLDSTRUCT_BATTLE(HWSRead,HWSSeek,true,false)
	if (version >= 1) {
		MACRO_WORLDSTRUCT_FRIENDLY(HWSRead,HWSSeek,true,false)
	}
	MarkDataModified();
}

void WorldMapDataStruct::WriteHWS(fstream& f, bool hwsformat, int steamdiscordiscmr) {
	if (hwsformat)
		HWSWriteShort(f,WM_HWS_VERSION);
	else if (steamdiscordiscmr>=0 && steamdiscordiscmr<2)
		HWSSeek(f,f.tellg(),steam_chunk_pos_disc[steamdiscordiscmr]);
	else if (steamdiscordiscmr>=2 && steamdiscordiscmr<4)
		HWSSeek(f,f.tellg(),steam_chunk_pos_discmr[steamdiscordiscmr-2]);
	MACRO_WORLDSTRUCT_IOFUNCTION(HWSWrite,HWSSeek,false,false)
	if (GetGameType()==GAME_TYPE_PSX) {
		MACRO_WORLDSTRUCT_PLACE(HWSWrite,HWSSeek,false,false)
	}
	MACRO_WORLDSTRUCT_BATTLE(HWSWrite,HWSSeek,false,false)
	MACRO_WORLDSTRUCT_FRIENDLY(HWSWrite,HWSSeek,false,false)
}

void WorldMapDataStruct::UpdateOffset() {
	// ToDo
}


void WorldMapDataSet::Load(fstream& ffbin, ClusterSet& clusset) {
	unsigned int i,j,k,l;
	amount = clusset.world_amount;
	struct_id.resize(amount);
	name.resize(amount);
	tim_amount.resize(amount);
	script.resize(amount);
	LoadingDialogInit(amount,_(L"Reading world maps..."));
	if (GetGameType()==GAME_TYPE_PSX) {
		cluster_id.resize(amount);
		text_data.resize(amount);
		charmap.resize(amount);
		chartim.resize(amount);
		preload.resize(amount);
		clusset.clus[clusset.world_map_shared_data_index].CreateChildren(ffbin);
		ChunkData& chunkworlddata = clusset.clus[clusset.world_map_shared_data_index].chunk[clusset.clus[clusset.world_map_shared_data_index].SearchChunkType(CHUNK_TYPE_VARIOUS)];
		world_data = (WorldMapDataStruct*)&chunkworlddata.GetObject(1);
		ffbin.seekg(chunkworlddata.object_offset[1]);
		world_data->Read(ffbin);
		j = 0;
		for (i=0;i<clusset.amount;i++) {
			if (clusset.clus_type[i]==CLUSTER_TYPE_WORLD_MAP) {
				clusset.clus[i].CreateChildren(ffbin);
				for (k=0;k<clusset.clus[i].chunk_amount;k++) {
					for (l=0;l<clusset.clus[i].chunk[k].object_amount;l++) {
						ffbin.seekg(clusset.clus[i].chunk[k].object_offset[l]);
						clusset.clus[i].chunk[k].GetObject(l).Read(ffbin);
					}
				}
				ClusterData& clustim = (ClusterData&)clusset.clus[i].chunk[clusset.clus[i].SearchChunkType(CHUNK_TYPE_CLUSTER_DATA)].GetObject(0);
				clustim.CreateChildren(ffbin);
				for (k=0;k<clustim.chunk_amount;k++) {
					for (l=0;l<clustim.chunk[k].object_amount;l++) {
						ffbin.seekg(clustim.chunk[k].object_offset[l]);
						clustim.chunk[k].GetObject(l).Read(ffbin);
					}
				}
				ClusterData& clus = (ClusterData&)clusset.clus[i].chunk[clusset.clus[i].SearchChunkType(CHUNK_TYPE_CLUSTER_DATA)].GetObject(1);
				cluster_id[j] = i;
				clus.CreateChildren(ffbin);
				for (k=0;k<clus.chunk_amount;k++) {
					for (l=0;l<clus.chunk[k].object_amount;l++) {
						ffbin.seekg(clus.chunk[k].object_offset[l]);
						clus.chunk[k].GetObject(l).Read(ffbin);
					}
				}
				ChunkData& chunkscript = clus.chunk[clus.SearchChunkType(CHUNK_TYPE_SCRIPT)];
				script[j] = (ScriptDataStruct*)&chunkscript.GetObject(0);
				struct_id[j] = script[j]->object_id;
				ChunkData& chunktxt = clus.chunk[clus.SearchChunkType(CHUNK_TYPE_TEXT)];
				text_data[j] = (TextDataStruct*)&chunktxt.GetObject(0);
				if (clus.SearchChunkType(CHUNK_TYPE_CHARMAP)>=0) {
					ChunkData& chmap = clus.chunk[clus.SearchChunkType(CHUNK_TYPE_CHARMAP)];
					charmap[j] = (CharmapDataStruct*)&chmap.GetObject(0);
					ChunkData& chtim = clus.chunk[clus.SearchChunkType(CHUNK_TYPE_TIM)];
					chartim[j] = (TIMImageDataStruct*)&chtim.GetObject(0);
					tim_amount[j] = chtim.object_amount;
					script[j]->related_charmap_id = charmap[j]->object_id;
				} else {
					tim_amount[j] = 0;
					charmap[j] = NULL;
					chartim[j] = NULL;
					script[j]->related_charmap_id = 0;
				}
				ChunkData& chunkimgmap = clusset.clus[i].chunk[clusset.clus[i].SearchChunkType(CHUNK_TYPE_IMAGE_MAP)];
				preload[j] = (ImageMapDataStruct*)&chunkimgmap.GetObject(0);
				for (k = 0; k < G_V_ELEMENTS(HADES_STRING_WORLD_BLOCK_NAME); k++)
					if (script[j]->object_id == HADES_STRING_WORLD_BLOCK_NAME[k].id) {
						name[j] = HADES_STRING_WORLD_BLOCK_NAME[k].label;
						break;
					}
				j++;
				LoadingDialogUpdate(j);
			}
		}
	} else {
		ClusterData** dummyclus = new ClusterData*[amount];
		ClusterData** dummyclusdata = new ClusterData*[1];
		ConfigurationSet& config = *clusset.config;
		string fname = config.steam_dir_data;
		SteamLanguage lang;
		uint16_t text_lang_amount[STEAM_LANGUAGE_AMOUNT];
		uint32_t fsize;
		char* buffer;
		fname += "resources.assets";
		ffbin.open(fname.c_str(),ios::in | ios::binary);
		text_data.resize(amount);
		text_data[0] = new TextDataStruct[1];
		text_data[0]->Init(true,CHUNK_TYPE_TEXT,STEAM_WORLD_MAP_TEXT_ID,&dummyclus[0],CLUSTER_TYPE_WORLD_MAP);
		text_data[0]->amount = 0;
		for (lang=0;lang<STEAM_LANGUAGE_AMOUNT;lang++) {
			if (hades::STEAM_SINGLE_LANGUAGE_MODE && lang!=GetSteamLanguage())
				continue;
			ffbin.seekg(config.meta_res.GetFileOffsetByIndex(config.world_text_file[lang]));
			fsize = config.meta_res.GetFileSizeByIndex(config.world_text_file[lang]);
			buffer = new char[fsize];
			ffbin.read(buffer,fsize);
			text_lang_amount[lang] = FF9String::CountSteamTextAmount(buffer,fsize);
			text_data[0]->amount = max(text_data[0]->amount,text_lang_amount[lang]);
			delete[] buffer;
		}
		text_data[0]->text = new FF9String[text_data[0]->amount];
//		text_data[0]->size = fsize;
		text_data[0]->loaded = true;
		for (lang=0;lang<STEAM_LANGUAGE_AMOUNT;lang++) {
			if (hades::STEAM_SINGLE_LANGUAGE_MODE && lang!=GetSteamLanguage())
				continue;
			ffbin.seekg(config.meta_res.GetFileOffsetByIndex(config.world_text_file[lang]));
			for (i=0;i<text_lang_amount[lang];i++)
				SteamReadFF9String(ffbin,text_data[0]->text[i],lang);
		}
		for (i=1;i<amount;i++)
			text_data[i] = text_data[0];
		world_data = new WorldMapDataStruct[1];
		world_data->Init(true,CHUNK_TYPE_VARIOUS,WM_DATA_CHUNK_ID,dummyclusdata,CLUSTER_TYPE_WORLD_MAP);
		for (lang=0;lang<STEAM_LANGUAGE_AMOUNT;lang++) {
			if (hades::STEAM_SINGLE_LANGUAGE_MODE && lang!=GetSteamLanguage())
				continue;
			ffbin.seekg(config.meta_res.GetFileOffsetByIndex(config.world_worldplace_name_file[lang]));
			for (i=0;i<WORLD_MAP_PLACE_AMOUNT;i++)
				SteamReadFF9String(ffbin,world_data->place_name[i],lang);
		}
//		world_data->place_name_size = config.meta_res.GetFileSizeByIndex(config.world_worldplace_name_file[GetSteamLanguage()]);
		ffbin.close();
		fname = config.steam_dir_assets + "p0data3.bin";
		ffbin.open(fname.c_str(),ios::in | ios::binary);
		ffbin.seekg(config.meta_world.GetFileOffsetByIndex(config.world_fx_file[0]));
		world_data->size = config.meta_world.GetFileSizeByIndex(config.world_fx_file[0]);
		world_data->Read(ffbin);
		ffbin.seekg(config.meta_world.GetFileOffsetByIndex(config.world_disc_file[0])+8*WM_CHUNK_STEAM);
		world_data->steam_chunk_pos_disc[0] = ReadLong(ffbin)*FILE_IGNORE_DATA_SIZE;
		ffbin.seekg(config.meta_world.GetFileOffsetByIndex(config.world_disc_file[1])+8*WM_CHUNK_STEAM);
		world_data->steam_chunk_pos_disc[1] = ReadLong(ffbin)*FILE_IGNORE_DATA_SIZE;
		ffbin.seekg(config.meta_world.GetFileOffsetByIndex(config.world_discmr_file[0])+8*WM_CHUNK_STEAM);
		world_data->steam_chunk_pos_discmr[0] = ReadLong(ffbin)*FILE_IGNORE_DATA_SIZE;
		ffbin.seekg(config.meta_world.GetFileOffsetByIndex(config.world_discmr_file[1])+8*WM_CHUNK_STEAM);
		world_data->steam_chunk_pos_discmr[1] = ReadLong(ffbin)*FILE_IGNORE_DATA_SIZE;
		ffbin.close();
		fname = config.steam_dir_assets + "p0data7.bin";
		ffbin.open(fname.c_str(),ios::in | ios::binary);
		for (i=0;i<amount;i++) {
			tim_amount[i] = 0;
			struct_id[i] = config.world_id[i];
			script[i] = new ScriptDataStruct[1];
			if (i==0)
				script[i]->Init(false,CHUNK_TYPE_SCRIPT,config.world_id[i],&dummyclus[i]);
			else
				script[i]->Init(true,CHUNK_TYPE_SCRIPT,config.world_id[i],&dummyclus[i],CLUSTER_TYPE_WORLD_MAP);
			for (lang=0;lang<STEAM_LANGUAGE_AMOUNT;lang++) {
				if (hades::STEAM_SINGLE_LANGUAGE_MODE && lang!=GetSteamLanguage())
					continue;
				ffbin.seekg(config.meta_script.GetFileOffsetByIndex(config.world_script_file[lang][i]));
				script[i]->Read(ffbin,lang);
			}
			script[i]->ChangeSteamLanguage(GetSteamLanguage());
			script[i]->size = config.meta_script.GetFileSizeByIndex(config.world_script_file[GetSteamLanguage()][i]);
			script[i]->related_charmap_id = 0;
			for (j = 0; j < G_V_ELEMENTS(HADES_STRING_WORLD_BLOCK_NAME); j++)
				if (script[i]->object_id == HADES_STRING_WORLD_BLOCK_NAME[j].id) {
					name[i] = HADES_STRING_WORLD_BLOCK_NAME[j].label;
					break;
				}
			script[i]->LinkSimilarLanguageScripts();
			LoadingDialogUpdate(i);
		}
		delete[] dummyclus;
		ffbin.close();
	}
	LoadingDialogEnd();
}

int WorldMapDataSet::GetSteamTextSize(unsigned int texttype, SteamLanguage lang) {
	if (texttype==0)
		return text_data[0]->GetDataSize(lang);
	unsigned int i;
	int res = 0;
	for (i=0;i<WORLD_MAP_PLACE_AMOUNT;i++)
		res += world_data->place_name[i].GetLength(lang);
	return res;
}

void WorldMapDataSet::WriteSteamText(fstream& ffbin, unsigned int texttype, SteamLanguage lang) {
	unsigned int i;
	if (texttype==0) {
		for (i=0;i<text_data[0]->amount;i++)
			SteamWriteFF9String(ffbin,text_data[0]->text[i],lang);
	} else {
		for (i=0;i<WORLD_MAP_PLACE_AMOUNT;i++)
			SteamWriteFF9String(ffbin,world_data->place_name[i],lang);
	}
}

void WorldMapDataSet::Write(fstream& ffbin, ClusterSet& clusset) {
	ffbin.seekg(world_data->parent_chunk->object_offset[1]);
	world_data->Write(ffbin);
	for (unsigned int i=0;i<amount;i++) {
		ClusterData& clus = clusset.clus[cluster_id[i]];
		ffbin.seekg(clus.offset);
		clus.Write(ffbin);
	}
}

void WorldMapDataSet::WritePPF(fstream& ffbin, ClusterSet& clusset) {
	ffbin.seekg(world_data->parent_chunk->object_offset[1]);
	world_data->WritePPF(ffbin);
	for (unsigned int i=0;i<amount;i++) {
		ClusterData& clus = clusset.clus[cluster_id[i]];
		ffbin.seekg(clus.offset);
		clus.WritePPF(ffbin);
	}
}

int* WorldMapDataSet::LoadHWS(fstream& ffhws, UnusedSaveBackupPart& backup, bool usetext, unsigned int localflag) {
	unsigned int i,j,k,l;
	uint32_t chunksize,clustersize,chunkpos,objectpos,objectsize;
	uint16_t nbmodified,objectid;
	SteamLanguage lang,sublang;
	uint8_t langcount;
	bool shouldread;
	uint8_t chunktype;
	ClusterData* clus;
	int* res = new int[5];
	res[0] = 0; res[1] = 0; res[2] = 0; res[3] = 0; res[4] = 0;
	bool loadmain = localflag & 1;
	bool loadlocal = localflag & 2;
	HWSReadShort(ffhws,nbmodified);
	for (i=0;i<nbmodified;i++) {
		objectpos = ffhws.tellg();
		HWSReadShort(ffhws,objectid);
		HWSReadLong(ffhws,clustersize);
		if (objectid==WM_DATA_CHUNK_ID) {
			clus = world_data->parent_cluster;
			if (clustersize<=clus->size+clus->extra_size) {
				HWSReadChar(ffhws,chunktype);
				while (chunktype!=0xFF) {
					HWSReadLong(ffhws,chunksize);
					chunkpos = ffhws.tellg();
					if (chunktype==1) {
						world_data->ReadHWS(ffhws,usetext);
						world_data->SetSize(chunksize);
					} else if (chunktype==2) {
						if (usetext) {
							HWSReadLong(ffhws,world_data->place_name_size);
							for (j=0;j<WORLD_MAP_PLACE_AMOUNT;j++) {
								SteamReadFF9String(ffhws,world_data->place_name[j]);
								if (GetGameType()==GAME_TYPE_PSX)
									world_data->place_name[j].SteamToPSX();
							}
						}
					} else if (chunktype == CHUNK_STEAM_TEXT_MULTILANG) {
						if (usetext) {
							uint32_t namesize;
							HWSReadChar(ffhws,lang);
							while (lang!=STEAM_LANGUAGE_NONE) {
								HWSReadLong(ffhws,namesize);
								if (GetGameType()==GAME_TYPE_PSX && lang!=GetSteamLanguage()) {
									ffhws.seekg(namesize,ios::cur);
									continue;
								}
								for (j=0;j<WORLD_MAP_PLACE_AMOUNT;j++) {
									SteamReadFF9String(ffhws,world_data->place_name[j],lang);
									if (GetGameType()==GAME_TYPE_PSX)
										world_data->place_name[j].SteamToPSX();
								}
								HWSReadChar(ffhws, lang);
							}
						}
					} else
						res[1]++;
					ffhws.seekg(chunkpos+chunksize);
					HWSReadChar(ffhws,chunktype);
				}
			} else {
				objectsize = 7;
				HWSReadChar(ffhws,chunktype);
				while (chunktype!=0xFF) {
					HWSReadLong(ffhws,chunksize);
					ffhws.seekg(chunksize,ios::cur);
					HWSReadChar(ffhws,chunktype);
					objectsize += chunksize+5;
				}
				ffhws.seekg(objectpos);
				backup.Add(ffhws,objectsize);
				res[0]++;
			}
		} else {
			for (j=0;j<amount;j++) {
				if (objectid==script[j]->object_id) {
					clus = script[j]->parent_cluster;
					if (clustersize<=clus->size+clus->extra_size) {
						HWSReadChar(ffhws,chunktype);
						while (chunktype!=0xFF) {
							HWSReadLong(ffhws,chunksize);
							chunkpos = ffhws.tellg();
							if (chunktype==CHUNK_TYPE_SCRIPT) {
								if (loadmain) {
									script[j]->ReadHWS(ffhws);
									script[j]->SetSize(chunksize);
								}
							} else if (chunktype==CHUNK_TYPE_TEXT) {
								if (usetext && loadmain) {
									if (j==0 || GetGameType()==GAME_TYPE_PSX) {
										text_data[j]->ReadHWS(ffhws);
										text_data[j]->SetSize(chunksize - (GetHWSGameType()==GAME_TYPE_PSX ? 0 : 2));
									} else
										res[2]++;
								}
							} else if (chunktype==CHUNK_TYPE_CHARMAP && charmap[j]) {
								if (loadmain) {
									charmap[j]->ReadHWS(ffhws);
									charmap[j]->SetSize(chunksize);
								}
							} else if (chunktype==CHUNK_TYPE_TIM && chartim[j]) {
								if (loadmain) {
									uint16_t timid;
									HWSReadShort(ffhws,timid);
									for (k=0;k<chartim[j]->parent_chunk->object_amount;k++)
										if (chartim[j][k].object_id==timid) {
											chartim[j][k].ReadHWS(ffhws);
											chartim[j][k].SetSize(chunksize-2);
										}
								}
							} else if (chunktype==CHUNK_TYPE_IMAGE_MAP) {
								if (loadmain) {
									uint32_t parentclussize;
									HWSReadLong(ffhws,parentclussize);
									if (GetHWSGameType()!=GetGameType()) {
										res[4]++;
									} else if (parentclussize<=preload[j]->parent_cluster->size+preload[j]->parent_cluster->extra_size) {
										preload[j]->ReadHWS(ffhws);
										preload[j]->SetSize(chunksize-4);
									} else
										res[3]++;
								}
							} else if (chunktype==CHUNK_SPECIAL_TYPE_LOCAL) {
								if (loadlocal)
									script[j]->ReadLocalHWS(ffhws);
							} else if (chunktype==CHUNK_STEAM_TEXT_MULTILANG) {
								if (usetext && loadmain)
									text_data[j]->ReadHWS(ffhws,true);
							} else if (chunktype==CHUNK_STEAM_SCRIPT_MULTILANG) {
								if (loadmain) {
									uint16_t langcorrcount;
									uint32_t langcorrpos;
									vector<uint16_t> corrlinkbase,corrlink;
									HWSReadChar(ffhws,lang);
									while (lang!=STEAM_LANGUAGE_NONE) {
										uint32_t langdatasize;
										shouldread = false;
										HWSReadChar(ffhws,langcount);
										langcorrpos = ffhws.tellg();
										for (k=0;k<langcount;k++) {
											HWSReadChar(ffhws,sublang);
											HWSReadLong(ffhws,langdatasize);
											if (hades::STEAM_SINGLE_LANGUAGE_MODE && sublang==GetSteamLanguage()) {
												shouldread = true;
												HWSReadShort(ffhws,langcorrcount);
												corrlinkbase.resize(langcorrcount);
												corrlink.resize(langcorrcount);
												for (l=0;l<langcorrcount;l++) {
													HWSReadShort(ffhws,corrlinkbase[l]);
													HWSReadShort(ffhws,corrlink[l]);
												}
											} else {
												ffhws.seekg((long long)ffhws.tellg()+langdatasize);
											}
										}
										HWSReadLong(ffhws,langdatasize);
										if (hades::STEAM_SINGLE_LANGUAGE_MODE && lang!=GetSteamLanguage()) {
											if (shouldread) {
												script[j]->ReadHWS(ffhws,false);
												script[j]->ApplyDialogLink(corrlink,corrlinkbase);
											} else {
												ffhws.seekg(langdatasize,ios::cur);
											}
										} else {
											script[j]->ReadHWS(ffhws,false,lang);
											if (script[j]->multi_lang_script!=NULL) {
												uint32_t endlangpos = ffhws.tellg();
												ffhws.seekg(langcorrpos);
												for (k=0;k<langcount;k++) {
													HWSReadChar(ffhws,sublang);
													HWSReadLong(ffhws,langdatasize);
													HWSReadShort(ffhws,langcorrcount);
													corrlinkbase.resize(langcorrcount);
													corrlink.resize(langcorrcount);
													for (l=0;l<langcorrcount;l++) {
														HWSReadShort(ffhws,corrlinkbase[l]);
														HWSReadShort(ffhws,corrlink[l]);
													}
													script[j]->LinkLanguageScripts(sublang,lang,corrlink,corrlinkbase);
												}
												ffhws.seekg(endlangpos);
											}
										}
										HWSReadChar(ffhws,lang);
									}
								}
							} else if (chunktype==CHUNK_SPECIAL_TYPE_LOCAL_MULTILANG) {
								if (loadlocal) {
									HWSReadChar(ffhws,lang);
									while (lang!=STEAM_LANGUAGE_NONE) {
										uint32_t langdatasize;
										shouldread = false;
										HWSReadChar(ffhws,langcount);
										for (k=0;k<langcount;k++) {
											HWSReadChar(ffhws,sublang);
											if (sublang==GetSteamLanguage())
												shouldread = true;
										}
										HWSReadLong(ffhws,langdatasize);
										if (hades::STEAM_SINGLE_LANGUAGE_MODE && lang!=GetSteamLanguage()) {
											if (shouldread)
												script[j]->ReadLocalHWS(ffhws);
											else
												ffhws.seekg(langdatasize,ios::cur);
										} else {
											script[j]->ReadLocalHWS(ffhws,lang);
										}
										HWSReadChar(ffhws,lang);
									}
								}
							} else
								res[1]++;
							ffhws.seekg(chunkpos+chunksize);
							HWSReadChar(ffhws,chunktype);
						}
					} else {
						objectsize = 7;
						HWSReadChar(ffhws,chunktype);
						while (chunktype!=0xFF) {
							HWSReadLong(ffhws,chunksize);
							ffhws.seekg(chunksize,ios::cur);
							HWSReadChar(ffhws,chunktype);
							objectsize += chunksize+5;
						}
						ffhws.seekg(objectpos);
						backup.Add(ffhws,objectsize);
						res[0]++;
					}
					j = amount;
				} else if (j+1==amount) {
					objectsize = 7;
					HWSReadChar(ffhws,chunktype);
					while (chunktype!=0xFF) {
						HWSReadLong(ffhws,chunksize);
						ffhws.seekg(chunksize,ios::cur);
						HWSReadChar(ffhws,chunktype);
						objectsize += chunksize+5;
					}
					ffhws.seekg(objectpos);
					backup.Add(ffhws,objectsize);
					res[2]++;
				}
			}
		}
	}
	return res;
}

void WorldMapDataSet::WriteHWS(fstream& ffhws, UnusedSaveBackupPart& backup, unsigned int localflag) {
	unsigned int i,j;
	uint16_t nbmodified = 0;
	uint32_t cluspos, clussize, chunkpos, chunksize, nboffset = ffhws.tellg();
	uint32_t aftlinkpos, linkpos;
	SteamLanguage lang, sublang;
	uint8_t nbscriptlink;
	ClusterData* clus;
	bool savemain = localflag & 1;
	bool savelocal = localflag & 2;
	HWSWriteShort(ffhws,nbmodified);
	if ((world_data->modified || world_data->parent_cluster->modified) && savemain) {
		world_data->parent_cluster->UpdateOffset();
		HWSWriteShort(ffhws,WM_DATA_CHUNK_ID);
		HWSWriteLong(ffhws,world_data->parent_cluster->size);
		cluspos = ffhws.tellg();
		if (world_data->modified) {
			HWSWriteChar(ffhws,1);
			HWSWriteLong(ffhws,world_data->size);
			chunkpos = ffhws.tellg();
			world_data->WriteHWS(ffhws);
			ffhws.seekg(chunkpos+world_data->size);
			if (GetGameType()!=GAME_TYPE_PSX) {
				HWSWriteChar(ffhws, CHUNK_STEAM_TEXT_MULTILANG);
				HWSWriteLong(ffhws, 0);
				chunkpos = ffhws.tellg();
				for (lang=0;lang<STEAM_LANGUAGE_AMOUNT;lang++)
					if (hades::STEAM_LANGUAGE_SAVE_LIST[lang]) {
						HWSWriteChar(ffhws, lang);
						HWSWriteLong(ffhws,GetSteamTextSize(1,lang));
						for (j=0;j<WORLD_MAP_PLACE_AMOUNT;j++)
							SteamWriteFF9String(ffhws,world_data->place_name[j],lang);
					}
				HWSWriteChar(ffhws,STEAM_LANGUAGE_NONE);
				chunksize = (long long)ffhws.tellg() - chunkpos;
				ffhws.seekg(chunkpos - 4);
				HWSWriteLong(ffhws, chunksize);
				ffhws.seekg(chunkpos + chunksize);
			}
		}
		HWSWriteChar(ffhws,0xFF);
		clussize = (long long)ffhws.tellg() - cluspos;
		ffhws.seekg(cluspos - 4);
		HWSWriteLong(ffhws, clussize);
		ffhws.seekg(cluspos + clussize);
		nbmodified++;
	}
	for (i=0;i<amount;i++) {
		clus = script[i]->parent_cluster;
		if (clus->modified || (clus->parent_cluster && clus->parent_cluster->modified)) {
			clus->UpdateOffset();
			HWSWriteShort(ffhws,script[i]->object_id);
			HWSWriteLong(ffhws,clus->size);
			cluspos = ffhws.tellg();
			if (GetGameType()==GAME_TYPE_PSX) {
				if (text_data[i]->modified && savemain) {
					HWSWriteChar(ffhws,CHUNK_TYPE_TEXT);
					HWSWriteLong(ffhws,text_data[i]->size+2);
					chunkpos = ffhws.tellg();
					text_data[i]->WriteHWS(ffhws);
					ffhws.seekg(chunkpos+text_data[i]->size+2);
				}
				if (charmap[i] && charmap[i]->modified && savemain) {
					HWSWriteChar(ffhws,CHUNK_TYPE_CHARMAP);
					HWSWriteLong(ffhws,charmap[i]->size);
					chunkpos = ffhws.tellg();
					charmap[i]->WriteHWS(ffhws);
					ffhws.seekg(chunkpos+charmap[i]->size);
				}
				if (chartim[i] && savemain) {
					for (j=0;j<chartim[i]->parent_chunk->object_amount;j++)
						if (chartim[i][j].modified) {
							HWSWriteChar(ffhws,CHUNK_TYPE_TIM);
							HWSWriteLong(ffhws,chartim[i][j].size+2);
							chunkpos = ffhws.tellg();
							HWSWriteShort(ffhws,chartim[i][j].object_id);
							chartim[i][j].WriteHWS(ffhws);
							ffhws.seekg(chunkpos+chartim[i][j].size+2);
						}
				}
				if (preload[i]->modified && savemain) {
					preload[i]->parent_cluster->UpdateOffset();
					HWSWriteChar(ffhws,CHUNK_TYPE_IMAGE_MAP);
					HWSWriteLong(ffhws,preload[i]->size+4);
					chunkpos = ffhws.tellg();
					HWSWriteLong(ffhws,preload[i]->parent_cluster->size);
					preload[i]->WriteHWS(ffhws);
					ffhws.seekg(chunkpos+preload[i]->size+4);
				}
				if (script[i]->modified && savemain) {
					HWSWriteChar(ffhws,CHUNK_TYPE_SCRIPT);
					HWSWriteLong(ffhws,script[i]->size);
					chunkpos = ffhws.tellg();
					script[i]->WriteHWS(ffhws);
					ffhws.seekg(chunkpos+script[i]->size);
				}
				if (savelocal) {
					uint32_t localsize = 0;
					HWSWriteChar(ffhws,CHUNK_SPECIAL_TYPE_LOCAL);
					HWSWriteLong(ffhws,localsize);
					chunkpos = ffhws.tellg();
					script[i]->WriteLocalHWS(ffhws);
					localsize = (long long)ffhws.tellg()-chunkpos;
					ffhws.seekg(chunkpos-4);
					HWSWriteLong(ffhws,localsize);
					ffhws.seekg(chunkpos+localsize);
				}
			} else {
				if (i==0 && text_data[i]->modified && savemain) {
					HWSWriteChar(ffhws,CHUNK_STEAM_TEXT_MULTILANG);
					HWSWriteLong(ffhws,0);
					chunkpos = ffhws.tellg();
					text_data[i]->WriteHWS(ffhws,true);
					chunksize = (long long)ffhws.tellg()-chunkpos;
					ffhws.seekg(chunkpos-4);
					HWSWriteLong(ffhws,chunksize);
					ffhws.seekg(chunkpos+chunksize);
				}
				for (lang=0;lang<STEAM_LANGUAGE_AMOUNT;lang++)
					if (savemain && hades::STEAM_LANGUAGE_SAVE_LIST[lang] && script[i]->IsDataModified(lang))
						break;
				if (lang<STEAM_LANGUAGE_AMOUNT) {
					HWSWriteChar(ffhws,CHUNK_STEAM_SCRIPT_MULTILANG);
					HWSWriteLong(ffhws,0);
					chunkpos = ffhws.tellg();
					for (lang=0;lang<STEAM_LANGUAGE_AMOUNT;lang++)
						if (hades::STEAM_LANGUAGE_SAVE_LIST[lang] && script[i]->IsDataModified(lang)) {
							if (script[i]->multi_lang_script!=NULL && script[i]->multi_lang_script->base_script_lang[lang]!=lang && hades::STEAM_LANGUAGE_SAVE_LIST[script[i]->multi_lang_script->base_script_lang[lang]])
								continue;
							HWSWriteChar(ffhws,lang);
							HWSWriteChar(ffhws,0);
							if (script[i]->multi_lang_script!=NULL && script[i]->multi_lang_script->base_script_lang[lang]==lang) {
								linkpos = ffhws.tellg();
								nbscriptlink = 0;
								for (sublang=0;sublang<STEAM_LANGUAGE_AMOUNT;sublang++)
									if (lang!=sublang && hades::STEAM_LANGUAGE_SAVE_LIST[sublang] && script[i]->multi_lang_script->base_script_lang[sublang]==lang) {
										uint16_t textcorresp = script[i]->multi_lang_script->base_script_text_id[sublang].size();
										HWSWriteChar(ffhws,sublang);
										HWSWriteLong(ffhws,2+4*textcorresp);
										HWSWriteShort(ffhws,textcorresp);
										for (j=0;j<textcorresp;j++) {
											HWSWriteShort(ffhws,script[i]->multi_lang_script->base_script_text_id[sublang][j]);
											HWSWriteShort(ffhws,script[i]->multi_lang_script->lang_script_text_id[sublang][j]);
										}
										nbscriptlink++;
									}
								aftlinkpos = ffhws.tellg();
								ffhws.seekg(linkpos-1);
								HWSWriteChar(ffhws,nbscriptlink);
								ffhws.seekg(aftlinkpos);
							}
							HWSWriteLong(ffhws,script[i]->GetDataSize(lang));
							script[i]->WriteHWS(ffhws,lang);
						}
					HWSWriteChar(ffhws,STEAM_LANGUAGE_NONE);
					chunksize = (long long)ffhws.tellg()-chunkpos;
					ffhws.seekg(chunkpos-4);
					HWSWriteLong(ffhws,chunksize);
					ffhws.seekg(chunkpos+chunksize);
				}
				for (lang=0;lang<STEAM_LANGUAGE_AMOUNT;lang++)
					if (savelocal && hades::STEAM_LANGUAGE_SAVE_LIST[lang])
						break;
				if (lang<STEAM_LANGUAGE_AMOUNT) {
					uint32_t langdatasize, langdatapos;
					HWSWriteChar(ffhws,CHUNK_SPECIAL_TYPE_LOCAL_MULTILANG);
					HWSWriteLong(ffhws,0);
					chunkpos = ffhws.tellg();
					for (lang=0;lang<STEAM_LANGUAGE_AMOUNT;lang++)
						if (hades::STEAM_LANGUAGE_SAVE_LIST[lang]) {
							if (script[i]->multi_lang_script!=NULL && script[i]->multi_lang_script->base_script_lang[lang]!=lang && hades::STEAM_LANGUAGE_SAVE_LIST[script[i]->multi_lang_script->base_script_lang[lang]])
								continue;
							HWSWriteChar(ffhws,lang);
							HWSWriteChar(ffhws,0);
							if (script[i]->multi_lang_script!=NULL && script[i]->multi_lang_script->base_script_lang[lang]==lang) {
								nbscriptlink = 0;
								for (sublang=0;sublang<STEAM_LANGUAGE_AMOUNT;sublang++)
									if (lang!=sublang && hades::STEAM_LANGUAGE_SAVE_LIST[sublang] && script[i]->multi_lang_script->base_script_lang[sublang]==lang) {
										HWSWriteChar(ffhws,sublang);
										nbscriptlink++;
									}
								ffhws.seekg((long long)ffhws.tellg()-nbscriptlink-1);
								HWSWriteChar(ffhws,nbscriptlink);
								ffhws.seekg((long long)ffhws.tellg()+nbscriptlink);
							}
							HWSWriteLong(ffhws,0);
							langdatapos = ffhws.tellg();
							script[i]->WriteLocalHWS(ffhws,lang);
							langdatasize = (long long)ffhws.tellg()-langdatapos;
							ffhws.seekg(langdatapos-4);
							HWSWriteLong(ffhws,langdatasize);
							ffhws.seekg(langdatapos+langdatasize);
						}
					HWSWriteChar(ffhws,STEAM_LANGUAGE_NONE);
					chunksize = (long long)ffhws.tellg()-chunkpos;
					ffhws.seekg(chunkpos-4);
					HWSWriteLong(ffhws,chunksize);
					ffhws.seekg(chunkpos+chunksize);
				}
			}
			HWSWriteChar(ffhws,0xFF);
			clussize = (long long)ffhws.tellg() - cluspos;
			ffhws.seekg(cluspos - 4);
			HWSWriteLong(ffhws, clussize);
			ffhws.seekg(cluspos + clussize);
			nbmodified++;
		}
	}
	for (i=0;i<backup.save_amount;i++)
		for (j=0;j<backup.save_size[i];j++)
			HWSWriteChar(ffhws,backup.save_data[i][j]);
	nbmodified += backup.save_amount;
	uint32_t endoffset = ffhws.tellg();
	ffhws.seekg(nboffset);
	HWSWriteShort(ffhws,nbmodified);
	ffhws.seekg(endoffset);
}

int WorldMapDataSet::GetIndexById(uint16_t worldid) {
	for (unsigned int i = 0; i < amount; i++)
		if (worldid == struct_id[i])
			return i;
	return -1;
}
