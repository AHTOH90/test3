#include "common.h"
#include "platform.h"
#include "crossplatform.h"
#include "Renderer.h"
#include "Frontend.h"
#include "Font.h"
#include "Camera.h"
#include "main.h"
#include "MBlur.h"
#include "postfx.h"
#include "custompipes.h"
#include "RwHelper.h"
#include "Text.h"
#include "Streaming.h"
#include "FileLoader.h"
#include "Collision.h"
#include "ModelInfo.h"
#include "Pad.h"

// Menu screens array is at the bottom of the file.

#ifdef PC_MENU

#ifdef CUSTOM_FRONTEND_OPTIONS

#ifdef IMPROVED_VIDEOMODE
	#define VIDEOMODE_SELECTOR MENUACTION_CFO_SELECT, "FEM_SCF", { new CCFOSelect((int8*)&FrontEndMenuManager.m_nPrefsWindowed, nil, screenModes, 2, true, ScreenModeAfterChange) }, 0, 0, MENUALIGN_LEFT,
#else
	#define VIDEOMODE_SELECTOR
#endif

#ifdef MULTISAMPLING
	#define MULTISAMPLING_SELECTOR MENUACTION_CFO_DYNAMIC, "FED_AAS", { new CCFODynamic((int8*)&FrontEndMenuManager.m_nPrefsMSAALevel, "MultiSampling", MultiSamplingDraw, MultiSamplingButtonPress) }, 0, 0, MENUALIGN_LEFT,
#else
	#define MULTISAMPLING_SELECTOR
#endif

#ifdef CUTSCENE_BORDERS_SWITCH
	#define CUTSCENE_BORDERS_TOGGLE MENUACTION_CFO_SELECT, "FEM_CSB", { new CCFOSelect((int8 *)&FrontEndMenuManager.m_PrefsCutsceneBorders, "CutsceneBorders", off_on, 2, false, nil) }, 0, 0, MENUALIGN_LEFT,
#else
	#define CUTSCENE_BORDERS_TOGGLE
#endif

#ifdef FREE_CAM
	#define FREE_CAM_TOGGLE MENUACTION_CFO_SELECT, "FEC_FRC", { new CCFOSelect((int8*)&TheCamera.bFreeCam, "FreeCam", off_on, 2, false, nil) }, 0, 0, MENUALIGN_LEFT,
#else
	#define FREE_CAM_TOGGLE
#endif

#ifdef PS2_ALPHA_TEST
	#define DUALPASS_SELECTOR MENUACTION_CFO_SELECT, "FEM_2PR", { new CCFOSelect((int8*)&gPS2alphaTest, "PS2AlphaTest", off_on, 2, false, nil) }, 0, 0, MENUALIGN_LEFT,
#else
	#define DUALPASS_SELECTOR 
#endif

#ifdef NO_ISLAND_LOADING
	#define ISLAND_LOADING_SELECTOR MENUACTION_CFO_SELECT, "FEM_ISL", { new CCFOSelect((int8*)&FrontEndMenuManager.m_PrefsIslandLoading, "IslandLoading", islandLoadingOpts, ARRAY_SIZE(islandLoadingOpts), true, IslandLoadingAfterChange) }, 0, 0, MENUALIGN_LEFT,
#else
	#define ISLAND_LOADING_SELECTOR 
#endif

#ifdef EXTENDED_COLOURFILTER
	#define POSTFX_SELECTORS \
		MENUACTION_CFO_SELECT, "FED_CLF", { new CCFOSelect((int8*)&CPostFX::EffectSwitch, "ColourFilter", filterNames, ARRAY_SIZE(filterNames), false, nil) }, 0, 0, MENUALIGN_LEFT, \
		MENUACTION_CFO_SELECT, "FED_MBL", { new CCFOSelect((int8*)&CPostFX::MotionBlurOn, "MotionBlur", off_on, 2, false, nil) }, 0, 0, MENUALIGN_LEFT,
#else
	#define POSTFX_SELECTORS
#endif	

#ifdef INVERT_LOOK_FOR_PAD
	#define INVERT_PAD_SELECTOR MENUACTION_CFO_SELECT, "FEC_ILU", { new CCFOSelect((int8*)&CPad::bInvertLook4Pad, nil, off_on, 2, false, nil) }, 150, 0, MENUALIGN_LEFT,
#else
	#define INVERT_PAD_SELECTOR
#endif

const char *filterNames[] = { "FEM_NON", "FEM_SIM", "FEM_NRM", "FEM_MOB" };
const char *off_on[] = { "FEM_OFF", "FEM_ON" };

void RestoreDefGraphics(int8 action) {
	if (action != FEOPTION_ACTION_SELECT)
		return;

	#ifdef PS2_ALPHA_TEST
		gPS2alphaTest = false;
	#endif
	#ifdef MULTISAMPLING
		FrontEndMenuManager.m_nPrefsMSAALevel = FrontEndMenuManager.m_nDisplayMSAALevel = 0;
	#endif
	#ifdef NO_ISLAND_LOADING
	    	if (!FrontEndMenuManager.m_bGameNotLoaded) {
	    		FrontEndMenuManager.m_PrefsIslandLoading = FrontEndMenuManager.ISLAND_LOADING_LOW;
				CStreaming::RemoveUnusedBigBuildings(CGame::currLevel);
				CStreaming::RemoveUnusedBuildings(CGame::currLevel);
				CStreaming::RequestIslands(CGame::currLevel);
		        CStreaming::LoadAllRequestedModels(true);
	    	} else
	    		FrontEndMenuManager.m_PrefsIslandLoading = FrontEndMenuManager.ISLAND_LOADING_LOW;
	#endif
	#ifdef GRAPHICS_MENU_OPTIONS // otherwise Frontend will handle those
		FrontEndMenuManager.m_PrefsFrameLimiter = true;
		FrontEndMenuManager.m_PrefsVsyncDisp = true;
		#ifdef LEGACY_MENU_OPTIONS
			FrontEndMenuManager.m_PrefsVsync = true;
		#endif
		FrontEndMenuManager.m_PrefsUseWideScreen = false;
		FrontEndMenuManager.m_nDisplayVideoMode = FrontEndMenuManager.m_nPrefsVideoMode;
		CMBlur::BlurOn = false;
		FrontEndMenuManager.SaveSettings();
	#endif
}

void RestoreDefDisplay(int8 action) {
	if (action != FEOPTION_ACTION_SELECT)
		return;

	#ifdef CUTSCENE_BORDERS_SWITCH
		FrontEndMenuManager.m_PrefsCutsceneBorders = true;
	#endif
	#ifdef FREE_CAM
		TheCamera.bFreeCam = false;
	#endif
	#ifdef GRAPHICS_MENU_OPTIONS // otherwise Frontend will handle those
		FrontEndMenuManager.m_PrefsBrightness = 256;
		FrontEndMenuManager.m_PrefsLOD = 1.2f;
		CRenderer::ms_lodDistScale = 1.2f;
		FrontEndMenuManager.m_PrefsShowSubtitles = false;
		FrontEndMenuManager.m_PrefsShowLegends = true;
		FrontEndMenuManager.m_PrefsRadarMode = 0;
		FrontEndMenuManager.m_PrefsShowHud = true;
		FrontEndMenuManager.SaveSettings();
	#endif
}

#ifdef NO_ISLAND_LOADING
const char *islandLoadingOpts[] = { "FEM_LOW", "FEM_MED", "FEM_HIG" };
void IslandLoadingAfterChange(int8 before, int8 after) {
	if (!FrontEndMenuManager.m_bGameNotLoaded) {
		if (after > FrontEndMenuManager.ISLAND_LOADING_LOW) {
		    FrontEndMenuManager.m_PrefsIslandLoading = before; // calls below needs previous mode :shrug:
		    
		    if (after == FrontEndMenuManager.ISLAND_LOADING_HIGH) {
			    CStreaming::RemoveIslandsNotUsed(LEVEL_BEACH);
			    CStreaming::RemoveIslandsNotUsed(LEVEL_MAINLAND);
			}
		    if (before == FrontEndMenuManager.ISLAND_LOADING_LOW) {
			    FrontEndMenuManager.m_PrefsIslandLoading = after;
			    CStreaming::RequestBigBuildings(CGame::currLevel);
			    
		    } else if (before == FrontEndMenuManager.ISLAND_LOADING_HIGH) {
			    FrontEndMenuManager.m_PrefsIslandLoading = after;
			    CStreaming::RequestIslands(CGame::currLevel);
		    } else
		    	    FrontEndMenuManager.m_PrefsIslandLoading = after;
		    	    
		} else { // low
		    CStreaming::RemoveUnusedBigBuildings(CGame::currLevel);
		    CStreaming::RemoveUnusedBuildings(CGame::currLevel);
		    CStreaming::RequestIslands(CGame::currLevel);
		}

		CStreaming::LoadAllRequestedModels(true);
	}

	FrontEndMenuManager.SetHelperText(0);
}
#endif

#ifdef MORE_LANGUAGES
void LangPolSelect(int8 action)
{
	if (action == FEOPTION_ACTION_SELECT) {
		FrontEndMenuManager.m_PrefsLanguage = CMenuManager::LANGUAGE_POLISH;
		FrontEndMenuManager.m_bFrontEnd_ReloadObrTxtGxt = true;
		FrontEndMenuManager.InitialiseChangedLanguageSettings();
		FrontEndMenuManager.SaveSettings();
	}
}

void LangRusSelect(int8 action)
{
	if (action == FEOPTION_ACTION_SELECT) {
		FrontEndMenuManager.m_PrefsLanguage = CMenuManager::LANGUAGE_RUSSIAN;
		FrontEndMenuManager.m_bFrontEnd_ReloadObrTxtGxt = true;
		FrontEndMenuManager.InitialiseChangedLanguageSettings();
		FrontEndMenuManager.SaveSettings();
	}
}

void LangJapSelect(int8 action)
{
	if (action == FEOPTION_ACTION_SELECT) {
		FrontEndMenuManager.m_PrefsLanguage = CMenuManager::LANGUAGE_JAPANESE;
		FrontEndMenuManager.m_bFrontEnd_ReloadObrTxtGxt = true;
		FrontEndMenuManager.InitialiseChangedLanguageSettings();
		FrontEndMenuManager.SaveSettings();
	}
}
#endif

#ifndef MULTISAMPLING
void GraphicsGoBack() {
}
#else
void GraphicsGoBack() {
	FrontEndMenuManager.m_nDisplayMSAALevel = FrontEndMenuManager.m_nPrefsMSAALevel;
}

void MultiSamplingButtonPress(int8 action) {
	if (action == FEOPTION_ACTION_SELECT) {
		if (FrontEndMenuManager.m_nDisplayMSAALevel != FrontEndMenuManager.m_nPrefsMSAALevel) {
			FrontEndMenuManager.m_nPrefsMSAALevel = FrontEndMenuManager.m_nDisplayMSAALevel;
			_psSelectScreenVM(FrontEndMenuManager.m_nPrefsVideoMode);
			FrontEndMenuManager.SetHelperText(0);
			FrontEndMenuManager.SaveSettings();
		}
	} else if (action == FEOPTION_ACTION_LEFT || action == FEOPTION_ACTION_RIGHT) {
		if (FrontEndMenuManager.m_bGameNotLoaded) {
			FrontEndMenuManager.m_nDisplayMSAALevel += (action == FEOPTION_ACTION_RIGHT ? 1 : -1);

			int i = 0;
			int maxAA = RwD3D8EngineGetMaxMultiSamplingLevels();
			while (maxAA != 1) {
				i++;
				maxAA >>= 1;
			}

			if (FrontEndMenuManager.m_nDisplayMSAALevel < 0)
				FrontEndMenuManager.m_nDisplayMSAALevel = i;
			else if (FrontEndMenuManager.m_nDisplayMSAALevel > i)
				FrontEndMenuManager.m_nDisplayMSAALevel = 0;
		}
	} else if (action == FEOPTION_ACTION_FOCUSLOSS) {
		if (FrontEndMenuManager.m_nDisplayMSAALevel != FrontEndMenuManager.m_nPrefsMSAALevel) {
			FrontEndMenuManager.m_nDisplayMSAALevel = FrontEndMenuManager.m_nPrefsMSAALevel;
			FrontEndMenuManager.SetHelperText(3);
		}
	}
}

wchar* MultiSamplingDraw(bool *disabled, bool userHovering) {
	static wchar unicodeTemp[64];
	if (userHovering) {
		if (FrontEndMenuManager.m_nDisplayMSAALevel == FrontEndMenuManager.m_nPrefsMSAALevel) {
			if (FrontEndMenuManager.m_nHelperTextMsgId == 1) // Press enter to apply
				FrontEndMenuManager.ResetHelperText();
		} else {
			FrontEndMenuManager.SetHelperText(1);
		}
	} else {
		if (FrontEndMenuManager.m_nDisplayMSAALevel != FrontEndMenuManager.m_nPrefsMSAALevel) {
			FrontEndMenuManager.m_nDisplayMSAALevel = FrontEndMenuManager.m_nPrefsMSAALevel;
		}
	}

	if (!FrontEndMenuManager.m_bGameNotLoaded)
		*disabled = true;

	switch (FrontEndMenuManager.m_nDisplayMSAALevel) {
		case 0:
			return TheText.Get("FEM_OFF");
		default:
			sprintf(gString, "%iX", 1 << (FrontEndMenuManager.m_nDisplayMSAALevel));
			AsciiToUnicode(gString, unicodeTemp);
			return unicodeTemp;
	}
}
#endif

#ifdef IMPROVED_VIDEOMODE
const char* screenModes[] = { "FED_FLS", "FED_WND" };
void ScreenModeAfterChange(int8 before, int8 after)
{
	_psSelectScreenVM(FrontEndMenuManager.m_nPrefsVideoMode); // apply same resolution
	FrontEndMenuManager.SetHelperText(0);
}

#endif

#ifdef DONT_TRUST_RECOGNIZED_JOYSTICKS
wchar selectedJoystickUnicode[128];

wchar* DetectJoystickDraw(bool* disabled, bool userHovering) {
	int numButtons;
	int found = -1;
	const char *joyname;
	if (userHovering) {
		for (int i = 0; i <= GLFW_JOYSTICK_LAST; i++) {
			if ((joyname = glfwGetJoystickName(i))) {
				const uint8* buttons = glfwGetJoystickButtons(i, &numButtons);
				for (int j = 0; j < numButtons; j++) {
					if (buttons[j]) {
						found = i;
						break;
					}
				}
				if (found != -1)
					break;
			}
		}

		if (found != -1 && PSGLOBAL(joy1id) != found) {
			if (PSGLOBAL(joy1id) != -1 && PSGLOBAL(joy1id) != found)
				PSGLOBAL(joy2id) = PSGLOBAL(joy1id);
			else
				PSGLOBAL(joy2id) = -1;

			strcpy(gSelectedJoystickName, joyname);
			PSGLOBAL(joy1id) = found;
		}
	}
	if (PSGLOBAL(joy1id) == -1)
		AsciiToUnicode("Not found", selectedJoystickUnicode);
	else
		AsciiToUnicode(gSelectedJoystickName, selectedJoystickUnicode);

	return selectedJoystickUnicode;
}
#endif

CMenuScreenCustom aScreens[] = {
	// MENUPAGE_STATS = 0
	{ "FEH_STA", MENUPAGE_NONE, nil, nil,
		MENUACTION_GOBACK, "FEDS_TB", {nil, SAVESLOT_NONE, MENUPAGE_NONE}, 190, 320, MENUALIGN_RIGHT,
	},

	// MENUPAGE_NEW_GAME = 1
	{ "FEP_STG", MENUPAGE_NONE, nil, nil,
		MENUACTION_CHANGEMENU, "FES_NGA", {nil, SAVESLOT_NONE, MENUPAGE_NEW_GAME_RELOAD}, 320, 155, MENUALIGN_CENTER,
		MENUACTION_CHANGEMENU, "FES_LOA",  {nil, SAVESLOT_NONE, MENUPAGE_CHOOSE_LOAD_SLOT}, 0, 0, MENUALIGN_CENTER,
		MENUACTION_CHANGEMENU, "FES_DEL", {nil, SAVESLOT_NONE, MENUPAGE_CHOOSE_DELETE_SLOT}, 0, 0, MENUALIGN_CENTER,
		MENUACTION_GOBACK, "FEDS_TB", {nil, SAVESLOT_NONE, 0}, 0, 0, MENUALIGN_CENTER,
	},

	// MENUPAGE_BRIEFS = 2
	{ "FEH_BRI", MENUPAGE_NONE, nil, nil,
		MENUACTION_GOBACK, "FEDS_TB", {nil, SAVESLOT_NONE, MENUPAGE_NONE}, 190, 320, MENUALIGN_RIGHT,
	},

	// MENUPAGE_SOUND_SETTINGS = 3
	{ "FEH_AUD", MENUPAGE_OPTIONS, nil, nil,
		MENUACTION_MUSICVOLUME,		"FEA_MUS", {nil, SAVESLOT_NONE, MENUPAGE_SOUND_SETTINGS}, 40, 76, MENUALIGN_LEFT,
		MENUACTION_SFXVOLUME,		"FEA_SFX", {nil, SAVESLOT_NONE, MENUPAGE_SOUND_SETTINGS}, 0, 0, MENUALIGN_LEFT,
		MENUACTION_MP3VOLUMEBOOST,	"FEA_MPB", {nil, SAVESLOT_NONE, MENUPAGE_SOUND_SETTINGS}, 0, 0, MENUALIGN_LEFT,
		MENUACTION_AUDIOHW,			"FEA_3DH", {nil, SAVESLOT_NONE, MENUPAGE_SOUND_SETTINGS}, 0, 0, MENUALIGN_LEFT,
		MENUACTION_SPEAKERCONF,		"FEA_SPK", {nil, SAVESLOT_NONE, MENUPAGE_SOUND_SETTINGS}, 0, 0, MENUALIGN_LEFT,
		MENUACTION_DYNAMICACOUSTIC,	"FET_DAM", {nil, SAVESLOT_NONE, MENUPAGE_SOUND_SETTINGS}, 0, 0, MENUALIGN_LEFT,
		MENUACTION_RADIO,			"FEA_RSS", {nil, SAVESLOT_NONE, MENUPAGE_SOUND_SETTINGS}, 0, 0, MENUALIGN_LEFT,
		MENUACTION_RESTOREDEF,		"FET_DEF", {nil, SAVESLOT_NONE, MENUPAGE_SOUND_SETTINGS}, 320, 367, MENUALIGN_CENTER,
		MENUACTION_GOBACK,			"FEDS_TB", {nil, SAVESLOT_NONE, MENUPAGE_NONE}, 0, 0, MENUALIGN_CENTER,
	},

	// MENUPAGE_DISPLAY_SETTINGS = 4
#ifndef GRAPHICS_MENU_OPTIONS
	{ "FEH_DIS", MENUPAGE_OPTIONS, new CCustomScreenLayout({40, 78, 25, true}), nil,
		MENUACTION_BRIGHTNESS,	"FED_BRI", {nil, SAVESLOT_NONE, MENUPAGE_DISPLAY_SETTINGS}, 0, 0, MENUALIGN_LEFT,
		MENUACTION_DRAWDIST,	"FEM_LOD", {nil, SAVESLOT_NONE, MENUPAGE_DISPLAY_SETTINGS}, 0, 0, MENUALIGN_LEFT,
#ifdef LEGACY_MENU_OPTIONS
		MENUACTION_FRAMESYNC,	"FEM_VSC", {nil, SAVESLOT_NONE, MENUPAGE_DISPLAY_SETTINGS}, 0, 0, MENUALIGN_LEFT,
#endif
		MENUACTION_FRAMELIMIT,	"FEM_FRM", {nil, SAVESLOT_NONE, MENUPAGE_DISPLAY_SETTINGS}, 0, 0, MENUALIGN_LEFT,
#if defined LEGACY_MENU_OPTIONS && !defined EXTENDED_COLOURFILTER
		MENUACTION_TRAILS,		"FED_TRA", {nil, SAVESLOT_NONE, MENUPAGE_DISPLAY_SETTINGS}, 0, 0, MENUALIGN_LEFT,
#endif
		MENUACTION_SUBTITLES,	"FED_SUB", {nil, SAVESLOT_NONE, MENUPAGE_DISPLAY_SETTINGS}, 0, 0, MENUALIGN_LEFT,
		MENUACTION_WIDESCREEN,	"FED_WIS", {nil, SAVESLOT_NONE, MENUPAGE_DISPLAY_SETTINGS}, 0, 0, MENUALIGN_LEFT,
		MENUACTION_LEGENDS,		"MAP_LEG", {nil, SAVESLOT_NONE, MENUPAGE_DISPLAY_SETTINGS}, 0, 0, MENUALIGN_LEFT,
		MENUACTION_RADARMODE,	"FED_RDR", {nil, SAVESLOT_NONE, MENUPAGE_DISPLAY_SETTINGS}, 0, 0, MENUALIGN_LEFT,
		MENUACTION_HUD,			"FED_HUD", {nil, SAVESLOT_NONE, MENUPAGE_DISPLAY_SETTINGS}, 0, 0, MENUALIGN_LEFT,
		MENUACTION_SCREENRES,	"FED_RES", {nil, SAVESLOT_NONE, MENUPAGE_DISPLAY_SETTINGS}, 0, 0, MENUALIGN_LEFT,
		VIDEOMODE_SELECTOR
		MULTISAMPLING_SELECTOR
		ISLAND_LOADING_SELECTOR
		DUALPASS_SELECTOR
		CUTSCENE_BORDERS_TOGGLE
		FREE_CAM_TOGGLE
		POSTFX_SELECTORS
		// re3.cpp inserts here pipeline selectors if neo/neo.txd exists and EXTENDED_PIPELINES defined
		MENUACTION_RESTOREDEF,	"FET_DEF", {nil, SAVESLOT_NONE, MENUPAGE_DISPLAY_SETTINGS}, 320, 0, MENUALIGN_CENTER,
		MENUACTION_GOBACK,		"FEDS_TB", {nil, SAVESLOT_NONE, MENUPAGE_NONE}, 320, 0, MENUALIGN_CENTER,
	},
#else
	{ "FEH_DIS", MENUPAGE_OPTIONS, new CCustomScreenLayout({40, 78, 25, true}), nil,
		MENUACTION_BRIGHTNESS,	"FED_BRI", { nil, SAVESLOT_NONE, MENUPAGE_DISPLAY_SETTINGS }, 0, 0, MENUALIGN_LEFT,
		MENUACTION_DRAWDIST,	"FEM_LOD", { nil, SAVESLOT_NONE, MENUPAGE_DISPLAY_SETTINGS }, 0, 0, MENUALIGN_LEFT,
		CUTSCENE_BORDERS_TOGGLE
		FREE_CAM_TOGGLE
		MENUACTION_LEGENDS,		"MAP_LEG", { nil, SAVESLOT_NONE, MENUPAGE_DISPLAY_SETTINGS }, 0, 0, MENUALIGN_LEFT,
		MENUACTION_RADARMODE,	"FED_RDR", { nil, SAVESLOT_NONE, MENUPAGE_DISPLAY_SETTINGS }, 0, 0, MENUALIGN_LEFT,
		MENUACTION_HUD,			"FED_HUD", { nil, SAVESLOT_NONE, MENUPAGE_DISPLAY_SETTINGS }, 0, 0, MENUALIGN_LEFT,
		MENUACTION_SUBTITLES,	"FED_SUB", { nil, SAVESLOT_NONE, MENUPAGE_DISPLAY_SETTINGS }, 0, 0, MENUALIGN_LEFT,
		MENUACTION_CFO_DYNAMIC,	"FET_DEF", { new CCFODynamic(nil, nil, nil, RestoreDefDisplay) }, 320, 0, MENUALIGN_CENTER,
		MENUACTION_GOBACK,		"FEDS_TB", { nil, SAVESLOT_NONE, MENUPAGE_NONE}, 320, 0, MENUALIGN_CENTER,
	},
#endif

	// MENUPAGE_LANGUAGE_SETTINGS = 5
	{ "FEH_LAN", MENUPAGE_OPTIONS, nil, nil,
		MENUACTION_LANG_ENG,	"FEL_ENG", {nil, SAVESLOT_NONE, MENUPAGE_LANGUAGE_SETTINGS}, 320, 132, MENUALIGN_CENTER,
		MENUACTION_LANG_FRE,	"FEL_FRE", {nil, SAVESLOT_NONE, MENUPAGE_LANGUAGE_SETTINGS}, 0, 0, MENUALIGN_CENTER,
		MENUACTION_LANG_GER,	"FEL_GER", {nil, SAVESLOT_NONE, MENUPAGE_LANGUAGE_SETTINGS}, 0, 0, MENUALIGN_CENTER,
		MENUACTION_LANG_ITA,	"FEL_ITA", {nil, SAVESLOT_NONE, MENUPAGE_LANGUAGE_SETTINGS}, 0, 0, MENUALIGN_CENTER,
		MENUACTION_LANG_SPA,    "FEL_SPA", {nil, SAVESLOT_NONE, MENUPAGE_LANGUAGE_SETTINGS}, 0, 0, MENUALIGN_CENTER,
#ifdef MORE_LANGUAGES
		MENUACTION_CFO_DYNAMIC,    "FEL_POL", { new CCFODynamic(nil, nil, nil, LangPolSelect) }, 0, 0, MENUALIGN_CENTER,
		MENUACTION_CFO_DYNAMIC,    "FEL_RUS", { new CCFODynamic(nil, nil, nil, LangRusSelect) }, 0, 0, MENUALIGN_CENTER
		MENUACTION_CFO_DYNAMIC,    "FEL_JAP", { new CCFODynamic(nil, nil, nil, LangJapSelect) }, 0, 0, MENUALIGN_CENTER,
#endif
		MENUACTION_GOBACK,		"FEDS_TB", {nil, SAVESLOT_NONE, MENUPAGE_NONE}, 0, 0, MENUALIGN_CENTER,
	},

	// MENUPAGE_MAP = 6
	{ "FEH_MAP", MENUPAGE_NONE, nil, nil,
		 MENUACTION_GOBACK,	"FEDS_TB", {nil, SAVESLOT_NONE, MENUPAGE_NONE}, 70, 380, MENUALIGN_CENTER,
	},

	// MENUPAGE_NEW_GAME_RELOAD = 7
	{ "FES_NGA", MENUPAGE_NEW_GAME, nil, nil,
		MENUACTION_LABEL,		"FESZ_QR",	{nil, SAVESLOT_NONE,	0}, 0, 0, 0,
		MENUACTION_NO,			"FEM_NO",	{nil, SAVESLOT_NONE,	MENUPAGE_NEW_GAME}, 320, 200, MENUALIGN_CENTER,
		MENUACTION_NEWGAME,		"FEM_YES",	{nil, SAVESLOT_NONE,	MENUPAGE_NEW_GAME_RELOAD}, 320, 225, MENUALIGN_CENTER,
	},

	// MENUPAGE_CHOOSE_LOAD_SLOT = 8
	{ "FET_LG", MENUPAGE_NEW_GAME, nil, nil,
		MENUACTION_CHECKSAVE,	"FEM_SL1", {nil, SAVESLOT_1,		0}, 40, 90, MENUALIGN_LEFT,
		MENUACTION_CHECKSAVE,	"FEM_SL2", {nil, SAVESLOT_2,		0}, 0, 0, MENUALIGN_LEFT,
		MENUACTION_CHECKSAVE,	"FEM_SL3", {nil, SAVESLOT_3,		0}, 0, 0, MENUALIGN_LEFT,
		MENUACTION_CHECKSAVE,	"FEM_SL4", {nil, SAVESLOT_4,		0}, 0, 0, MENUALIGN_LEFT,
		MENUACTION_CHECKSAVE,	"FEM_SL5", {nil, SAVESLOT_5,		0}, 0, 0, MENUALIGN_LEFT,
		MENUACTION_CHECKSAVE,	"FEM_SL6", {nil, SAVESLOT_6,		0}, 0, 0, MENUALIGN_LEFT,
		MENUACTION_CHECKSAVE,	"FEM_SL7", {nil, SAVESLOT_7,		0}, 0, 0, MENUALIGN_LEFT,
		MENUACTION_CHECKSAVE,	"FEM_SL8", {nil, SAVESLOT_8,		0}, 0, 0, MENUALIGN_LEFT,
		MENUACTION_GOBACK,		"FEDS_TB", {nil, SAVESLOT_NONE,	0}, 320, 345, MENUALIGN_CENTER,
	},

	// MENUPAGE_CHOOSE_DELETE_SLOT = 9
	{ "FES_DEL", MENUPAGE_NEW_GAME, nil, nil,
		MENUACTION_CHECKSAVE,	"FEM_SL1",	{nil, SAVESLOT_1,		0}, 40, 90, MENUALIGN_LEFT,
		MENUACTION_CHECKSAVE,	"FEM_SL2",	{nil, SAVESLOT_2,		0}, 0, 0, MENUALIGN_LEFT,
		MENUACTION_CHECKSAVE,	"FEM_SL3",	{nil, SAVESLOT_3,		0}, 0, 0, MENUALIGN_LEFT,
		MENUACTION_CHECKSAVE,	"FEM_SL4",	{nil, SAVESLOT_4,		0}, 0, 0, MENUALIGN_LEFT,
		MENUACTION_CHECKSAVE,	"FEM_SL5",	{nil, SAVESLOT_5,		0}, 0, 0, MENUALIGN_LEFT,
		MENUACTION_CHECKSAVE,	"FEM_SL6",	{nil, SAVESLOT_6,		0}, 0, 0, MENUALIGN_LEFT,
		MENUACTION_CHECKSAVE,	"FEM_SL7",	{nil, SAVESLOT_7,		0}, 0, 0, MENUALIGN_LEFT,
		MENUACTION_CHECKSAVE,	"FEM_SL8",	{nil, SAVESLOT_8,		0}, 0, 0, MENUALIGN_LEFT,
		MENUACTION_GOBACK,		"FEDS_TB",	{nil, SAVESLOT_NONE,	0}, 320, 345, MENUALIGN_CENTER,
	},

	// MENUPAGE_LOAD_SLOT_CONFIRM = 10
	{ "FET_LG", MENUPAGE_CHOOSE_LOAD_SLOT, nil, nil,
		 MENUACTION_LABEL,		"FESZ_QL",	{nil, SAVESLOT_NONE,	0}, 0, 0, 0,
		 MENUACTION_NO,			"FEM_NO",	{nil, SAVESLOT_NONE,	MENUPAGE_CHOOSE_LOAD_SLOT}, 320, 200, MENUALIGN_CENTER,
		 MENUACTION_YES,		"FEM_YES",	{nil, SAVESLOT_NONE,	MENUPAGE_LOADING_IN_PROGRESS}, 320, 225, MENUALIGN_CENTER,
	},

	// MENUPAGE_DELETE_SLOT_CONFIRM = 11
	{ "FES_DEL", MENUPAGE_CHOOSE_DELETE_SLOT, nil, nil,
		 MENUACTION_LABEL,		"FESZ_QD",	{nil, SAVESLOT_NONE,  MENUPAGE_NONE}, 0, 0, 0,
		 MENUACTION_NO,			"FEM_NO",	{nil, SAVESLOT_NONE,  MENUPAGE_CHOOSE_DELETE_SLOT}, 320, 200, MENUALIGN_CENTER,
		 MENUACTION_YES,		"FEM_YES",	{nil, SAVESLOT_NONE,	MENUPAGE_DELETING_IN_PROGRESS}, 320, 225, MENUALIGN_CENTER,
	},

	// MENUPAGE_LOADING_IN_PROGRESS = 12
	{ "FET_LG", MENUPAGE_CHOOSE_LOAD_SLOT, nil, nil,
	},

	// MENUPAGE_DELETING_IN_PROGRESS = 13
	{ "FES_DEL", MENUPAGE_CHOOSE_DELETE_SLOT, nil, nil,
	},

	// MENUPAGE_DELETE_SUCCESSFUL = 14
	{ "FES_DEL", MENUPAGE_NEW_GAME, nil, nil,
		 MENUACTION_LABEL,		"FES_DSC",	{nil, SAVESLOT_NONE,	0}, 0, 0, 0,
		 MENUACTION_CHANGEMENU,	"FEM_OK",	{nil, SAVESLOT_NONE,	MENUPAGE_NEW_GAME}, 320, 225, MENUALIGN_CENTER,
	},

	// MENUPAGE_CHOOSE_SAVE_SLOT = 15
	{ "FET_SG", MENUPAGE_DISABLED, nil, nil,
		MENUACTION_SAVEGAME,			"FEM_SL1", {nil, SAVESLOT_1,		MENUPAGE_SAVE_OVERWRITE_CONFIRM}, 40, 90, MENUALIGN_LEFT,
		MENUACTION_SAVEGAME,			"FEM_SL2", {nil, SAVESLOT_2,		MENUPAGE_SAVE_OVERWRITE_CONFIRM}, 0, 0, MENUALIGN_LEFT,
		MENUACTION_SAVEGAME,			"FEM_SL3", {nil, SAVESLOT_3,		MENUPAGE_SAVE_OVERWRITE_CONFIRM}, 0, 0, MENUALIGN_LEFT,
		MENUACTION_SAVEGAME,			"FEM_SL4", {nil, SAVESLOT_4,		MENUPAGE_SAVE_OVERWRITE_CONFIRM}, 0, 0, MENUALIGN_LEFT,
		MENUACTION_SAVEGAME,			"FEM_SL5", {nil, SAVESLOT_5,		MENUPAGE_SAVE_OVERWRITE_CONFIRM}, 0, 0, MENUALIGN_LEFT,
		MENUACTION_SAVEGAME,			"FEM_SL6", {nil, SAVESLOT_6,		MENUPAGE_SAVE_OVERWRITE_CONFIRM}, 0, 0, MENUALIGN_LEFT,
		MENUACTION_SAVEGAME,			"FEM_SL7", {nil, SAVESLOT_7,		MENUPAGE_SAVE_OVERWRITE_CONFIRM}, 0, 0, MENUALIGN_LEFT,
		MENUACTION_SAVEGAME,			"FEM_SL8", {nil, SAVESLOT_8,		MENUPAGE_SAVE_OVERWRITE_CONFIRM}, 0, 0, MENUALIGN_LEFT,
		MENUACTION_RESUME_FROM_SAVEZONE,"FESZ_CA", {nil, SAVESLOT_NONE,	0}, 320, 345, MENUALIGN_CENTER,
	},

	// MENUPAGE_SAVE_OVERWRITE_CONFIRM = 16
	{ "FET_SG", MENUPAGE_CHOOSE_SAVE_SLOT, nil, nil,
		MENUACTION_LABEL,		"FESZ_QZ", {nil, SAVESLOT_NONE, MENUPAGE_NONE}, 0, 0, 0,
		MENUACTION_NO,			"FEM_NO", {nil, SAVESLOT_NONE, MENUPAGE_CHOOSE_SAVE_SLOT}, 320, 200, MENUALIGN_CENTER,
		MENUACTION_YES,			"FEM_YES",  {nil, SAVESLOT_NONE, MENUPAGE_SAVING_IN_PROGRESS}, 320, 225, MENUALIGN_CENTER,
	},

	// MENUPAGE_SAVING_IN_PROGRESS = 17
	{ "FET_SG", MENUPAGE_CHOOSE_SAVE_SLOT, nil, nil,
	},

	// MENUPAGE_SAVE_SUCCESSFUL = 18
	{ "FET_SG", MENUPAGE_CHOOSE_SAVE_SLOT, nil, nil,
		MENUACTION_LABEL,					"FES_SSC",	{nil, SAVESLOT_LABEL,	MENUPAGE_NONE}, 0, 0, 0,
		MENUACTION_RESUME_FROM_SAVEZONE,	"FEM_OK",	{nil, SAVESLOT_NONE,	MENUPAGE_CHOOSE_SAVE_SLOT}, 320, 225, MENUALIGN_CENTER,
	},

	// MENUPAGE_SAVE_CUSTOM_WARNING = 19
	{ "FET_SG", MENUPAGE_NONE, nil, nil,
		MENUACTION_LABEL,		"",			{nil, SAVESLOT_NONE, 0}, 0, 0, 0,
		MENUACTION_CHANGEMENU,	"FEM_OK",	{nil, SAVESLOT_NONE, MENUPAGE_CHOOSE_SAVE_SLOT}, 320, 225, MENUALIGN_CENTER,
	},

	// MENUPAGE_SAVE_CHEAT_WARNING = 20
	{ "FET_SG", MENUPAGE_NEW_GAME, nil, nil,
		MENUACTION_LABEL,		"FES_CHE",	{nil, SAVESLOT_NONE,	MENUPAGE_NONE}, 0, 0, 0,
		MENUACTION_CHANGEMENU,	"FEM_OK",	{nil, SAVESLOT_NONE,	MENUPAGE_CHOOSE_SAVE_SLOT}, 320, 225, MENUALIGN_CENTER,
	},

	// MENUPAGE_SKIN_SELECT = 21
	{ "FET_PS", MENUPAGE_OPTIONS, nil, nil,
		 MENUACTION_GOBACK,		"FEDS_TB",	{nil, SAVESLOT_NONE, MENUPAGE_OPTIONS}, 0, 0, 0,
	},

	// MENUPAGE_SAVE_UNUSED = 22
	{ "FET_SG", MENUPAGE_NEW_GAME, nil, nil,
		 MENUACTION_LABEL,		"FED_LWR",	{nil, SAVESLOT_NONE,	0}, 0, 0, 0,
		 MENUACTION_CHANGEMENU,	"FEC_OKK",	{nil, SAVESLOT_NONE,	MENUPAGE_CHOOSE_SAVE_SLOT}, 0, 0, 0,
	},

	// MENUPAGE_SAVE_FAILED = 23
	{ "FET_SG", MENUPAGE_CHOOSE_SAVE_SLOT, nil, nil,
		 MENUACTION_LABEL,		"FEC_SVU",	{nil, SAVESLOT_NONE,	0}, 0, 0, 0,
		 MENUACTION_CHANGEMENU,	"FEC_OKK",	{nil, SAVESLOT_NONE,	MENUPAGE_CHOOSE_SAVE_SLOT}, 0, 0, 0,
	},

	// MENUPAGE_SAVE_FAILED_2 = 24
	{ "FET_LG", MENUPAGE_CHOOSE_SAVE_SLOT, nil, nil,
		 MENUACTION_LABEL,		"FEC_SVU",	{nil, SAVESLOT_NONE,	0}, 0, 0, 0,
	},

	// MENUPAGE_LOAD_FAILED = 25
	{ "FET_LG", MENUPAGE_NEW_GAME, nil, nil,
		 MENUACTION_LABEL,		"FEC_LUN",	{nil, SAVESLOT_NONE,  0}, 0, 0, 0,
		 MENUACTION_GOBACK,		"FEDS_TB",	{nil, SAVESLOT_NONE,  MENUPAGE_NEW_GAME}, 0, 0, 0,
	},

	// MENUPAGE_CONTROLLER_PC = 26
	{ "FET_CTL", MENUPAGE_OPTIONS, new CCustomScreenLayout({0, 0, MENU_DEFAULT_LINE_HEIGHT, false, false, 150}), nil,
#ifdef PC_PLAYER_CONTROLS
		MENUACTION_CTRLMETHOD,	"FET_STI", {nil, SAVESLOT_NONE, MENUPAGE_CONTROLLER_PC}, 320, 150, MENUALIGN_CENTER,
#endif
		MENUACTION_KEYBOARDCTRLS,"FEC_RED", {nil, SAVESLOT_NONE, MENUPAGE_KEYBOARD_CONTROLS}, 0, 0, MENUALIGN_CENTER,
#ifdef DONT_TRUST_RECOGNIZED_JOYSTICKS
		MENUACTION_CHANGEMENU,	"FEC_JOD", {nil, SAVESLOT_NONE, MENUPAGE_DETECT_JOYSTICK}, 0, 0, MENUALIGN_CENTER,
#endif
		MENUACTION_CHANGEMENU,	"FEC_MOU", {nil, SAVESLOT_NONE, MENUPAGE_MOUSE_CONTROLS}, 0, 0, MENUALIGN_CENTER,
		INVERT_PAD_SELECTOR
		MENUACTION_RESTOREDEF,	"FET_DEF", {nil, SAVESLOT_NONE, MENUPAGE_CONTROLLER_PC}, 320, 0, MENUALIGN_CENTER,
		MENUACTION_GOBACK,		"FEDS_TB", {nil, SAVESLOT_NONE, 0}, 320, 0, MENUALIGN_CENTER,
   },

	// MENUPAGE_OPTIONS = 27
	{ "FET_OPT", MENUPAGE_NONE, nil, nil,
		 MENUACTION_CHANGEMENU,		"FEO_CON", {nil, SAVESLOT_NONE, MENUPAGE_CONTROLLER_PC}, 320, 132, MENUALIGN_CENTER,
		 MENUACTION_LOADRADIO,		"FEO_AUD", {nil, SAVESLOT_NONE, MENUPAGE_SOUND_SETTINGS}, 0, 0, MENUALIGN_CENTER,
		 MENUACTION_CHANGEMENU,		"FEO_DIS", {nil, SAVESLOT_NONE, MENUPAGE_DISPLAY_SETTINGS}, 0, 0, MENUALIGN_CENTER,
#ifdef GRAPHICS_MENU_OPTIONS
		 MENUACTION_CHANGEMENU,		"FET_GRA", {nil, SAVESLOT_NONE, MENUPAGE_GRAPHICS_SETTINGS}, 0, 0, MENUALIGN_CENTER,
#endif
		 MENUACTION_CHANGEMENU,		"FEO_LAN", {nil, SAVESLOT_NONE, MENUPAGE_LANGUAGE_SETTINGS}, 0, 0, MENUALIGN_CENTER,
		 MENUACTION_PLAYERSETUP,	"FET_PS", {nil, SAVESLOT_NONE, MENUPAGE_SKIN_SELECT}, 0, 0, MENUALIGN_CENTER,
		 MENUACTION_GOBACK,			"FEDS_TB", {nil, SAVESLOT_NONE, 0}, 0, 0, MENUALIGN_CENTER,
   },

	// MENUPAGE_EXIT = 28
	{ "FET_QG", MENUPAGE_NONE, nil, nil,
		MENUACTION_LABEL,		"FEQ_SRE",	{nil, SAVESLOT_NONE, 0}, 0, 0, 0,
		MENUACTION_DONTCANCEL,	"FEM_NO",	{nil, SAVESLOT_NONE, MENUPAGE_NONE}, 320, 200, MENUALIGN_CENTER,
		MENUACTION_CANCELGAME,	"FEM_YES",	{nil, SAVESLOT_NONE, MENUPAGE_NONE}, 320, 225, MENUALIGN_CENTER,
   },

	// MENUPAGE_START_MENU = 29
	{ "FEM_MM", MENUPAGE_DISABLED, nil, nil,
		  MENUACTION_CHANGEMENU,	"FEP_STG",	{nil, SAVESLOT_NONE,	MENUPAGE_NEW_GAME}, 320, 170, MENUALIGN_CENTER,
		  MENUACTION_CHANGEMENU,	"FEP_OPT",	{nil, SAVESLOT_NONE,	MENUPAGE_OPTIONS}, 0, 0, MENUALIGN_CENTER,
		  MENUACTION_CHANGEMENU,	"FEP_QUI",	{nil, SAVESLOT_NONE,	MENUPAGE_EXIT}, 0, 0, MENUALIGN_CENTER,
   },

	// MENUPAGE_KEYBOARD_CONTROLS = 30
	{ "FET_STI", MENUPAGE_CONTROLLER_PC, nil, nil,
   },

	// MENUPAGE_MOUSE_CONTROLS = 31
	{ "FEC_MOU", MENUPAGE_CONTROLLER_PC, nil, nil,
		MENUACTION_MOUSESENS,	"FEC_MSH",	{nil, SAVESLOT_NONE, MENUPAGE_MOUSE_CONTROLS}, 40, 170, MENUALIGN_LEFT,
		MENUACTION_INVVERT,		"FEC_IVV",	{nil, SAVESLOT_NONE, MENUPAGE_MOUSE_CONTROLS}, 0, 0, MENUALIGN_LEFT,
		MENUACTION_MOUSESTEER,	"FET_MST",	{nil, SAVESLOT_NONE, MENUPAGE_MOUSE_CONTROLS}, 0, 0, MENUALIGN_LEFT,
		MENUACTION_GOBACK,		"FEDS_TB",	{nil, SAVESLOT_NONE, 0}, 320, 260, MENUALIGN_CENTER,
   },

	// MENUPAGE_PAUSE_MENU = 32
	{ "FET_PAU", MENUPAGE_DISABLED, nil, nil,
		MENUACTION_RESUME,		"FEP_RES",	{nil, SAVESLOT_NONE, 0}, 320, 120, MENUALIGN_CENTER,
		MENUACTION_CHANGEMENU,	"FEH_SGA",	{nil, SAVESLOT_NONE, MENUPAGE_NEW_GAME}, 0, 0, MENUALIGN_CENTER,
		MENUACTION_CHANGEMENU,	"FEH_MAP",	{nil, SAVESLOT_NONE, MENUPAGE_MAP}, 0, 0, MENUALIGN_CENTER,
		MENUACTION_CHANGEMENU,	"FEP_STA",	{nil, SAVESLOT_NONE, MENUPAGE_STATS}, 0, 0, MENUALIGN_CENTER,
		MENUACTION_CHANGEMENU,	"FEH_BRI",	{nil, SAVESLOT_NONE, MENUPAGE_BRIEFS}, 0, 0, MENUALIGN_CENTER,
		MENUACTION_CHANGEMENU,	"FET_OPT",	{nil, SAVESLOT_NONE, MENUPAGE_OPTIONS}, 0, 0, MENUALIGN_CENTER,
		MENUACTION_CHANGEMENU,	"FEP_QUI",	{nil, SAVESLOT_NONE, MENUPAGE_EXIT}, 0, 0, MENUALIGN_CENTER,
   },

	// MENUPAGE_NONE = 33
	{ "", 0, nil, nil, },


#ifdef LEGACY_MENU_OPTIONS
	// MENUPAGE_CONTROLLER_SETTINGS = 4
	{ "FET_CON", MENUPAGE_OPTIONS, nil, nil,
		MENUACTION_CTRLCONFIG,		"FEC_CCF", {nil, SAVESLOT_NONE, MENUPAGE_CONTROLLER_SETTINGS}, 0, 0, 0,
		MENUACTION_CTRLVIBRATION,	"FEC_VIB", {nil, SAVESLOT_NONE, MENUPAGE_CONTROLLER_SETTINGS}, 0, 0, 0,
		MENUACTION_GOBACK,			"FEDS_TB", {nil, SAVESLOT_NONE, MENUPAGE_NONE}, 0, 0, 0,
   },

	// MENUPAGE_DEBUG_MENU = 18
	{ "FED_DBG", MENUPAGE_NONE, nil, nil,
		MENUACTION_RELOADIDE,	"FED_RID", {nil, SAVESLOT_NONE, MENUPAGE_NONE}, 0, 0, 0,
		MENUACTION_SETDBGFLAG,	"FED_DFL", {nil, SAVESLOT_NONE, MENUPAGE_NONE}, 0, 0, 0,
		MENUACTION_SWITCHBIGWHITEDEBUGLIGHT,	"FED_DLS", {nil, SAVESLOT_NONE, MENUPAGE_NONE}, 0, 0, 0,
		MENUACTION_COLLISIONPOLYS,	"FED_SCP", {nil, SAVESLOT_NONE, MENUPAGE_NONE}, 0, 0, 0,
		MENUACTION_GOBACK,		"FEDS_TB", {nil, SAVESLOT_NONE, MENUPAGE_NONE}, 0, 0, 0,
   },

	// MENUPAGE_CONTROLLER_PC_OLD1 = 36
	{ "FET_CTL", MENUPAGE_CONTROLLER_PC, nil, nil,
		MENUACTION_GETKEY,	"FEC_PLB", {nil, SAVESLOT_NONE, MENUPAGE_CONTROLLER_PC_OLD1}, 0, 0, 0,
		MENUACTION_GETKEY,	"FEC_CWL", {nil, SAVESLOT_NONE, MENUPAGE_CONTROLLER_PC_OLD1}, 0, 0, 0,
		MENUACTION_GETKEY,	"FEC_CWR", {nil, SAVESLOT_NONE, MENUPAGE_CONTROLLER_PC_OLD1}, 0, 0, 0,
		MENUACTION_GETKEY,	"FEC_LKT", {nil, SAVESLOT_NONE, MENUPAGE_CONTROLLER_PC_OLD1}, 0, 0, 0,
		MENUACTION_GETKEY,	"FEC_PJP", {nil, SAVESLOT_NONE, MENUPAGE_CONTROLLER_PC_OLD1}, 0, 0, 0,
		MENUACTION_GETKEY,	"FEC_PSP", {nil, SAVESLOT_NONE, MENUPAGE_CONTROLLER_PC_OLD1}, 0, 0, 0,
		MENUACTION_GETKEY,	"FEC_TLF", {nil, SAVESLOT_NONE, MENUPAGE_CONTROLLER_PC_OLD1}, 0, 0, 0,
		MENUACTION_GETKEY,	"FEC_TRG", {nil, SAVESLOT_NONE, MENUPAGE_CONTROLLER_PC_OLD1}, 0, 0, 0,
		MENUACTION_GETKEY,	"FEC_CCM", {nil, SAVESLOT_NONE, MENUPAGE_CONTROLLER_PC_OLD1}, 0, 0, 0,
		MENUACTION_GOBACK,		"FEDS_TB", {nil, SAVESLOT_NONE, MENUPAGE_NONE}, 0, 0, 0,
   },

	// MENUPAGE_CONTROLLER_PC_OLD2 = 37
	{ "FET_CTL", MENUPAGE_CONTROLLER_PC, nil, nil,

	},

	// MENUPAGE_CONTROLLER_PC_OLD3 = 38
	{ "FET_CTL", MENUPAGE_CONTROLLER_PC, nil, nil,
		 MENUACTION_GETKEY,	"FEC_LUP", {nil, SAVESLOT_NONE, MENUPAGE_CONTROLLER_PC_OLD3}, 0, 0, 0,
		 MENUACTION_GETKEY,	"FEC_LDN", {nil, SAVESLOT_NONE, MENUPAGE_CONTROLLER_PC_OLD3}, 0, 0, 0,
		 MENUACTION_GETKEY,	"FEC_SMS", {nil, SAVESLOT_NONE, MENUPAGE_CONTROLLER_PC_OLD3}, 0, 0, 0,
		 MENUACTION_SHOWHEADBOB,	"FEC_GSL", {nil, SAVESLOT_NONE, MENUPAGE_CONTROLLER_PC_OLD3}, 0, 0, 0,
		 MENUACTION_GOBACK,		"FEDS_TB", {nil, SAVESLOT_NONE, MENUPAGE_NONE}, 0, 0, 0,
   },

	// MENUPAGE_CONTROLLER_PC_OLD4 = 39
	{ "FET_CTL", MENUPAGE_CONTROLLER_PC, nil, nil,

	},

	// MENUPAGE_CONTROLLER_DEBUG = 40
	{ "FEC_DBG", MENUPAGE_CONTROLLER_PC, nil, nil,
		 MENUACTION_GETKEY,	"FEC_TGD",	{nil, SAVESLOT_NONE, MENUPAGE_CONTROLLER_DEBUG}, 0, 0, 0,
		 MENUACTION_GETKEY,	"FEC_TDO",	{nil, SAVESLOT_NONE, MENUPAGE_CONTROLLER_DEBUG}, 0, 0, 0,
		 MENUACTION_GETKEY,	"FEC_TSS",	{nil, SAVESLOT_NONE, MENUPAGE_CONTROLLER_DEBUG}, 0, 0, 0,
		 MENUACTION_GETKEY,	"FEC_SMS",	{nil, SAVESLOT_NONE, MENUPAGE_CONTROLLER_DEBUG}, 0, 0, 0,
		 MENUACTION_GOBACK,	"FEDS_TB",	{nil, SAVESLOT_NONE, MENUPAGE_NONE}, 0, 0, 0,
   },
#endif

#ifdef GRAPHICS_MENU_OPTIONS
	// MENUPAGE_GRAPHICS_SETTINGS
	{ "FET_GRA", MENUPAGE_OPTIONS, new CCustomScreenLayout({40, 78, 25, true, true}), GraphicsGoBack,

		MENUACTION_SCREENRES,	"FED_RES", { nil, SAVESLOT_NONE, MENUPAGE_GRAPHICS_SETTINGS }, 0, 0, MENUALIGN_LEFT,
		MENUACTION_WIDESCREEN,	"FED_WIS", { nil, SAVESLOT_NONE, MENUPAGE_GRAPHICS_SETTINGS }, 0, 0, MENUALIGN_LEFT,
		VIDEOMODE_SELECTOR
#ifdef LEGACY_MENU_OPTIONS
		MENUACTION_FRAMESYNC,	"FEM_VSC", {nil, SAVESLOT_NONE, MENUPAGE_GRAPHICS_SETTINGS}, 0, 0, MENUALIGN_LEFT,
#endif
		MENUACTION_FRAMELIMIT,	"FEM_FRM", { nil, SAVESLOT_NONE, MENUPAGE_GRAPHICS_SETTINGS }, 0, 0, MENUALIGN_LEFT,
		MULTISAMPLING_SELECTOR
		ISLAND_LOADING_SELECTOR
		DUALPASS_SELECTOR
#ifdef EXTENDED_COLOURFILTER
		POSTFX_SELECTORS
#elif defined LEGACY_MENU_OPTIONS
		MENUACTION_TRAILS,		"FED_TRA", { nil, SAVESLOT_NONE, MENUPAGE_GRAPHICS_SETTINGS }, 0, 0, MENUALIGN_LEFT,
#endif
		// re3.cpp inserts here pipeline selectors if neo/neo.txd exists and EXTENDED_PIPELINES defined
		MENUACTION_CFO_DYNAMIC,	"FET_DEF", { new CCFODynamic(nil, nil, nil, RestoreDefGraphics) }, 320, 0, MENUALIGN_CENTER,
		MENUACTION_GOBACK,		"FEDS_TB", {nil, SAVESLOT_NONE, MENUPAGE_NONE}, 320, 0, MENUALIGN_CENTER,
	},
#endif

#ifdef DONT_TRUST_RECOGNIZED_JOYSTICKS
	// MENUPAGE_DETECT_JOYSTICK
	{ "FEC_JOD", MENUPAGE_CONTROLLER_PC, new CCustomScreenLayout({0, 0, 0, false, false, 30}), nil,

		MENUACTION_LABEL,	"FEC_JPR", { nil, SAVESLOT_NONE, MENUPAGE_NONE }, 0, 0, 0,
		MENUACTION_CFO_DYNAMIC,	"FEC_JDE", { new CCFODynamic(nil, nil, DetectJoystickDraw, nil) }, 80, 200, MENUALIGN_LEFT,
		MENUACTION_GOBACK,		"FEDS_TB", {nil, SAVESLOT_NONE, MENUPAGE_NONE}, 320, 225, MENUALIGN_CENTER,
	},
#endif

	// MENUPAGE_OUTRO = 34
	{ "", 0, nil, nil, },
};

#endif
#endif