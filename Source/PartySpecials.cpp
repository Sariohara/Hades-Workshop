#include "PartySpecials.h"

#define PARTY_SPECIAL_HWS_VERSION 1

#define MACRO_MAGICSWORD_IOFUNCTION(IO,SEEK,READ,PPF) \
	if (PPF) PPFInitScanStep(ffbin); \
	for (i=0;i<MAGIC_SWORD_AMOUNT;i++) \
		IO ## Char(ffbin,magic_sword_requirement[i]); \
	if (PPF) PPFEndScanStep();


void PartySpecialDataSet::Load(fstream& ffbin, ConfigurationSet& config) {
	unsigned int i;
	magic_sword[0] = 0x32;	magic_sword[1] = 0x34;	magic_sword[2] = 0x36;
	magic_sword[3] = 0x38;	magic_sword[4] = 0x39;	magic_sword[5] = 0x3B;
	magic_sword[6] = 0x3D;	magic_sword[7] = 0x3F;	magic_sword[8] = 0x41;
	magic_sword[9] = 0x4B;	magic_sword[10] = 0x4C;
	magic_sword[11] = 0xBD;	magic_sword[12] = 0xBF;
	if (GetGameType()==GAME_TYPE_PSX) {
		ffbin.seekg(config.party_magicsword_offset);
		MACRO_MAGICSWORD_IOFUNCTION(FFIXRead,FFIXSeek,true,false)
	} else {
		DllMetaData& dlldata = config.meta_dll;
		dlldata.dll_file.seekg(dlldata.GetStaticFieldOffset(config.dll_magicsword_field_id));
		for (i=0;i<MAGIC_SWORD_AMOUNT;i++) {
			SteamReadChar(dlldata.dll_file,magic_sword_requirement[i]);
			dlldata.dll_file.seekg(3,ios::cur);
		}
	}
}

DllMetaDataModification* PartySpecialDataSet::ComputeSteamMod(ConfigurationSet& config, unsigned int* modifamount) {
	DllMetaDataModification* res = new DllMetaDataModification[1];
	DllMetaData& dlldata = config.meta_dll;
	unsigned int i;
	res[0].position = dlldata.GetStaticFieldOffset(config.dll_magicsword_field_id);
	res[0].base_length = 4*MAGIC_SWORD_AMOUNT; // config.meta_dll.GetStaticFieldRange(config.dll_magicsword_field_id);
	res[0].new_length = 4*MAGIC_SWORD_AMOUNT;
	res[0].value = new uint8_t[res[0].new_length];
	BufferInitPosition();
	for (i=0;i<MAGIC_SWORD_AMOUNT;i++)
		BufferWriteLong(res[0].value,magic_sword_requirement[i]);
	*modifamount = 1;
	return res;
}

void PartySpecialDataSet::GenerateCSharp(vector<string>& buffer) {
	unsigned int i;
	stringstream mgcswddb;
	mgcswddb << "// Method: BattleHUD::SetAbilityMagic\n\n";
	mgcswddb << "\t// ...\n";
	mgcswddb << "\tint[] array2 = new int[] {\n";
	for (i = 0; i < MAGIC_SWORD_AMOUNT; i++)
		mgcswddb << "\t\t" << (int)magic_sword_requirement[i] << (i+1==MAGIC_SWORD_AMOUNT ? "" : ",") << "\n";
	mgcswddb << "\t};\n";
	mgcswddb << "\t// ...\n";
	buffer.push_back(mgcswddb.str());
}

void PartySpecialDataSet::Write(fstream& ffbin, ConfigurationSet& config) {
	unsigned int i;
	ffbin.seekg(config.party_magicsword_offset);
	MACRO_MAGICSWORD_IOFUNCTION(FFIXWrite,FFIXSeek,false,false)
}

void PartySpecialDataSet::WritePPF(fstream& ffbin, ConfigurationSet& config) {
	unsigned int i;
	ffbin.seekg(config.party_magicsword_offset);
	MACRO_MAGICSWORD_IOFUNCTION(PPFStepAdd,FFIXSeek,false,true)
}

void PartySpecialDataSet::LoadHWS(fstream& ffbin) {
	uint16_t version;
	unsigned int i;
	HWSReadShort(ffbin,version);
	MACRO_MAGICSWORD_IOFUNCTION(HWSRead,HWSSeek,true,false)
}

void PartySpecialDataSet::WriteHWS(fstream& ffbin) {
	unsigned int i;
	HWSWriteShort(ffbin,PARTY_SPECIAL_HWS_VERSION);
	MACRO_MAGICSWORD_IOFUNCTION(HWSWrite,HWSSeek,false,false)
}
