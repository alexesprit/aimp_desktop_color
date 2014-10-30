#include "stdafx.h"
#include "apiPlugin.h"
#include "DesktopColorPlugin.h"

bool APIENTRY DllMain(HMODULE hModule, DWORD  ulReasonForCall, LPVOID lpReserved) {
    switch (ulReasonForCall) {
        case DLL_PROCESS_ATTACH:
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
    }
	return true;
}

bool WINAPI AIMPPluginGetHeader(IAIMPPlugin **header) {
    DesktopColorPlugin* plugin = new DesktopColorPlugin();
    plugin->AddRef();
    *header = plugin;
    return true;
}
