#include "skse/PluginAPI.h"		// super
#include "skse/skse_version.h"	// What version of SKSE is running?

#include <shlobj.h>				// CSIDL_MYCODUMENTS

#include "SplitItemStacks.h" //my stuff
//#include "UIExtensions.h"

static PluginHandle					g_pluginHandle = kPluginHandle_Invalid;
static SKSEPapyrusInterface         * g_papyrus = NULL;
static SKSEScaleformInterface		* g_scaleform = NULL;

// ###***### TESTS
static SKSESerializationInterface	* g_serialization = NULL;

extern "C"	{

	bool SKSEPlugin_Query(const SKSEInterface * skse, PluginInfo * info) // Called by SKSE to learn about this plugin and check that it's safe to load it
	{	
		
		gLog.OpenRelative(CSIDL_MYDOCUMENTS, "\\My Games\\Skyrim\\SKSE\\spis_plugin.log");
		gLog.SetPrintLevel(IDebugLog::kLevel_Error);
		gLog.SetLogLevel(IDebugLog::kLevel_DebugMessage);

		_MESSAGE("SplitItemStacks");

		// populate info structure
		info->infoVersion = PluginInfo::kInfoVersion;
		info->name = "SplitItemStacks";
		info->version = 1;

		// store plugin handle so we can identify ourselves later
		g_pluginHandle = skse->GetPluginHandle();

		if (skse->isEditor)
		{
			_MESSAGE("loaded in editor, marking as incompatible");

			return false;
		}
		else if (skse->runtimeVersion != RUNTIME_VERSION_1_9_32_0)
		{
			_MESSAGE("unsupported runtime version %08X", skse->runtimeVersion);

			return false;
		}

		//scaleform things? I saw this was done in the example function
		g_scaleform = (SKSEScaleformInterface *)skse->QueryInterface(kInterface_Scaleform);

		// ### do not do anything else in this callback
		// ### only fill out PluginInfo and return true/false

		// supported runtime version

		// ###***### TESTS
		g_serialization = (SKSESerializationInterface *)skse->QueryInterface(kInterface_Serialization);
		if (!g_serialization)
		{
			_MESSAGE("couldn't get serialization interface");

			return false;
		}

		if (g_serialization->version < SKSESerializationInterface::kVersion)
		{
			_MESSAGE("serialization interface too old (%d expected %d)", g_serialization->version, SKSESerializationInterface::kVersion);

			return false;
		}
		//ENDTESTS

		return true;
	}

	bool SKSEPlugin_Load(const SKSEInterface * skse)	{	// Called by SKSE to load this plugin
		_MESSAGE("SplitItemStacks loaded");

		g_papyrus = (SKSEPapyrusInterface *)skse->QueryInterface(kInterface_Papyrus);

		//Check if the function registration was a success...
		bool btest = g_papyrus->Register(plugin_spis::RegisterFuncsPapyrus);
		bool ctest = g_scaleform->Register("spis_plugin_scaleform", plugin_spis::RegisterFuncsScaleform);
		bool dtest = plugin_spis::RegisterSerializationCallbacks(g_serialization, g_pluginHandle);

		if (btest && ctest) {
			_MESSAGE("Papyrus register succeeded");
			_MESSAGE("Scaleform register succeeded");



		}

		return true;
	}

};