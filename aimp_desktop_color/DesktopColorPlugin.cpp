#include "stdafx.h"
#include "apiSkins.h"
#include "DesktopColorPlugin.h"
#include "utils.h"

HRESULT WINAPI DesktopColorPlugin::Initialize(IAIMPCore* core) {
    if (IsWindowsVistaOrGreater() &&
        IsServiceAvailable(core, IID_IAIMPServiceSkinsManager)) {
        aimpCore = core;
        aimpCore->AddRef();

        ChangeCurrentSkinColor();
        return S_OK;
    }
    return E_ABORT;
}

HRESULT WINAPI DesktopColorPlugin::Finalize() {
    aimpCore->Release();
    aimpCore = nullptr;
    return S_OK;
}

PWCHAR WINAPI DesktopColorPlugin::InfoGet(int index) {
    switch (index) {
        case AIMP_PLUGIN_INFO_NAME:
            return L"Desktop Color";
        case AIMP_PLUGIN_INFO_AUTHOR:
            return L"esprit";
        case AIMP_PLUGIN_INFO_SHORT_DESCRIPTION:
            return L"Set the player color scheme according to desktop color.";
        case AIMP_PLUGIN_INFO_FULL_DESCRIPTION:
            return L"Works only on Windows Vista and above.";
        default:
            return nullptr;
    }
}

DWORD WINAPI DesktopColorPlugin::InfoGetCategories() {
    return AIMP_PLUGIN_CATEGORY_ADDONS;
}

void WINAPI DesktopColorPlugin::SystemNotification(int NotifyID, IUnknown* Data) {

}

void DesktopColorPlugin::ChangeCurrentSkinColor() {
    int dwmR = 0, dwmG = 0, dwmB = 0;
    if (GetDwmColor(&dwmR, &dwmG, &dwmB) == S_OK) {
        IAIMPServiceSkinsManager* skinManager;
        if (SUCCEEDED(aimpCore->QueryInterface(IID_IAIMPServiceSkinsManager, (void**)&skinManager))) {
            IAIMPPropertyList* skinPropertyList;
            if (SUCCEEDED(skinManager->QueryInterface(IID_IAIMPPropertyList, (void**)&skinPropertyList))) {
                int oldH = 0, oldS = 0, oldL = 0;
                skinPropertyList->GetValueAsInt32(AIMP_SERVICE_SKINSMAN_PROPID_HUE, &oldH);
                skinPropertyList->GetValueAsInt32(AIMP_SERVICE_SKINSMAN_PROPID_HUE_INTENSITY, &oldS);

                byte dwmH = 0, dwmS = 0, dwmL = 0;
                skinManager->RGBToHSL(dwmR, dwmG, dwmB, &dwmH, &dwmS, &dwmL);
                dwmS = ByteToPercent(dwmS);

                if (oldS != dwmS || oldH != dwmH) {
                    skinPropertyList->BeginUpdate();
                    skinPropertyList->SetValueAsInt32(AIMP_SERVICE_SKINSMAN_PROPID_HUE, dwmH);
                    skinPropertyList->SetValueAsInt32(AIMP_SERVICE_SKINSMAN_PROPID_HUE_INTENSITY, dwmS);
                    skinPropertyList->EndUpdate();
                    skinManager->Select(NULL);
                }

                skinPropertyList->Release();
            }

            skinManager->Release();
        }
    }
}

bool DesktopColorPlugin::IsServiceAvailable(IUnknown* provider, REFIID serviceIid) {
    IUnknown* dummyService;
    if (SUCCEEDED(provider->QueryInterface(serviceIid, (void**)&dummyService))) {
        dummyService->Release();
        return true;
    }
    return false;
}
