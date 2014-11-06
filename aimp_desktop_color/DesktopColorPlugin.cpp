#include "stdafx.h"

#include "apiSkins.h"
#include "apiMenu.h"
#include "apiMUI.h"

#include "DesktopColorPlugin.h"
#include "MenuItemEvent.h"
#include "utils.h"

#define MENU_CHANGE_COLOR_SCHEME_ID L"{db2dcb78-274a-4055-9bc2-01f89558b567}"

#define LNG_UPDATE_COLOR_SCHEME L"DesktopColor\\L1"
#define LNG_IS_ALREADY_UPDATED L"DesktopColor\\L2"

HRESULT WINAPI DesktopColorPlugin::Initialize(IAIMPCore* core) {
    if (IsWindowsVistaOrGreater() &&
        IsServiceAvailable(core, IID_IAIMPServiceSkinsManager)) {
        aimpCore = core;
        aimpCore->AddRef();

        RegisterMessageListener();
        AddItemToUtilsMenu();
        ChangeCurrentSkinColor();
        return S_OK;
    }
    return E_ABORT;
}

HRESULT WINAPI DesktopColorPlugin::Finalize() {
    UnregisterMessageListener();

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

void DesktopColorPlugin::OnMenuItemPressed() {
    if (!ChangeCurrentSkinColor()) {
        IAIMPStringPtr message;
        LangLoadString(LNG_IS_ALREADY_UPDATED, &message);
        MessageBox(0, message->GetData(), L"DesktopColor", MB_OK);
    }
}

bool DesktopColorPlugin::OnCoreMessage(DWORD message, int wParam, void* lParam) {
    switch (message) {
        case AIMP_MSG_EVENT_LANGUAGE:
            UpdateMenuItemNames();
            break;
    }
    return false;
}

bool DesktopColorPlugin::ChangeCurrentSkinColor() {
    int dwmR = 0, dwmG = 0, dwmB = 0;
    if (GetDwmColor(&dwmR, &dwmG, &dwmB) == S_OK) {
        IAIMPServiceSkinsManagerPtr skinManager;
        if (GetService(IID_IAIMPServiceSkinsManager, (void**)&skinManager)) {
            IAIMPSkinInfoPtr skinInfo;
            if (SUCCEEDED(skinManager->QueryInterface(IID_IAIMPPropertyList, (void**)&skinInfo))) {
                int oldH = 0, oldS = 0, oldL = 0;
                skinInfo->GetValueAsInt32(AIMP_SERVICE_SKINSMAN_PROPID_HUE, &oldH);
                skinInfo->GetValueAsInt32(AIMP_SERVICE_SKINSMAN_PROPID_HUE_INTENSITY, &oldS);

                byte dwmH = 0, dwmS = 0, dwmL = 0;
                skinManager->RGBToHSL(dwmR, dwmG, dwmB, &dwmH, &dwmS, &dwmL);
                dwmS = ByteToPercent(dwmS);

                if (oldS != dwmS || oldH != dwmH) {
                    skinInfo->BeginUpdate();
                    skinInfo->SetValueAsInt32(AIMP_SERVICE_SKINSMAN_PROPID_HUE, dwmH);
                    skinInfo->SetValueAsInt32(AIMP_SERVICE_SKINSMAN_PROPID_HUE_INTENSITY, dwmS);
                    skinInfo->EndUpdate();
                    skinManager->Select(nullptr);
                    return true;
                }
            }
        }
    }
    return false;
}

void DesktopColorPlugin::AddItemToUtilsMenu() {
    IAIMPMenuItemPtr menuItem;
    if (CreateObject(IID_IAIMPMenuItem, (void**)&menuItem)) {
        IAIMPServiceMenuManagerPtr menuManager;
        if (GetService(IID_IAIMPServiceMenuManager, (void**)&menuManager)) {
            IAIMPMenuItemPtr parentMenuItem;
            if (SUCCEEDED(menuManager->GetBuiltIn(AIMP_MENUID_PLAYER_MAIN_FUNCTIONS, &parentMenuItem))) {
                auto menuId = MakeString(MENU_CHANGE_COLOR_SCHEME_ID);
                IAIMPString* menuName;
                LangLoadString(LNG_UPDATE_COLOR_SCHEME, &menuName);

                auto callback = std::bind(&DesktopColorPlugin::OnMenuItemPressed, this);
                MenuItemEvent* itemEvent = new MenuItemEvent(callback);
                itemEvent->AddRef();

                menuItem->SetValueAsObject(AIMP_MENUITEM_PROPID_ID, menuId);
                menuItem->SetValueAsObject(AIMP_MENUITEM_PROPID_NAME, menuName);
                menuItem->SetValueAsObject(AIMP_MENUITEM_PROPID_PARENT, parentMenuItem);
                menuItem->SetValueAsObject(AIMP_MENUITEM_PROPID_EVENT, itemEvent);
                menuItem->SetValueAsInt32(AIMP_MENUITEM_PROPID_STYLE, AIMP_MENUITEM_STYLE_NORMAL);

                aimpCore->RegisterExtension(IID_IAIMPServiceMenuManager, menuItem);

                itemEvent->Release();
                menuName->Release();
                menuId->Release();
            }
        }
    }
}

void DesktopColorPlugin::UpdateMenuItemNames() {
    IAIMPStringPtr menuId;
    IAIMPStringPtr menuName;
    MakeString(MENU_CHANGE_COLOR_SCHEME_ID, &menuId);
    LangLoadString(LNG_UPDATE_COLOR_SCHEME, &menuName);
    UpdateMenuItemName(menuId, menuName);
}

void DesktopColorPlugin::RegisterMessageListener() {
    using namespace std::placeholders;

    IAIMPServiceMessageDispatcherPtr messageDispatcher;
    if (GetService(IID_IAIMPServiceMessageDispatcher, (void**)&messageDispatcher)) {
        auto coreMessageCallback = std::bind(&DesktopColorPlugin::OnCoreMessage, this, _1, _2, _3);
        coreMessageListener = new CoreMessageListener(coreMessageCallback);
        coreMessageListener->AddRef();
        messageDispatcher->Hook(coreMessageListener);
    }
}

void DesktopColorPlugin::UnregisterMessageListener() {
    IAIMPServiceMessageDispatcherPtr messageDispatcher;
    if (GetService(IID_IAIMPServiceMessageDispatcher, (void**)&messageDispatcher)) {
        messageDispatcher->Unhook(coreMessageListener);
    }
}

bool DesktopColorPlugin::CreateObject(REFIID iid, void** object) {
    return SUCCEEDED(aimpCore->CreateObject(iid, object));
}

bool DesktopColorPlugin::GetService(REFIID iid, void** service) {
    return SUCCEEDED(aimpCore->QueryInterface(iid, service));
}

bool DesktopColorPlugin::IsServiceAvailable(IUnknown* provider, REFIID serviceIid) {
    IUnknown* dummyService;
    if (SUCCEEDED(provider->QueryInterface(serviceIid, (void**)&dummyService))) {
        dummyService->Release();
        return true;
    }
    return false;
}

HRESULT DesktopColorPlugin::UpdateMenuItemName(IAIMPString* menuId, IAIMPString* newName) {
    IAIMPServiceMenuManagerPtr menuManager;
    if (GetService(IID_IAIMPServiceMenuManager, (void**)&menuManager)) {
        IAIMPMenuItemPtr menuItem;
        if (SUCCEEDED(menuManager->GetByID(menuId, &menuItem))) {
            return menuItem->SetValueAsObject(AIMP_MENUITEM_PROPID_NAME, newName);
        }
    }
    return E_UNEXPECTED;
}

IAIMPString* DesktopColorPlugin::MakeString(PWCHAR strSeq) {
    IAIMPString* string;
    CreateObject(IID_IAIMPString, (void**)&string);
    string->SetData(strSeq, wcslen(strSeq));
    return string;
}

HRESULT DesktopColorPlugin::MakeString(PWCHAR strSeq, IAIMPString** out) {
    auto result = CreateObject(IID_IAIMPString, (void**)out);
    if (result) {
        return (*out)->SetData(strSeq, wcslen(strSeq));
    }
    return result;
}

HRESULT DesktopColorPlugin::LangLoadString(PWCHAR keyPath, IAIMPString** out) {
    IAIMPServiceMUIPtr muiService;
    GetService(IID_IAIMPServiceMUI, (void**)&muiService); 
    IAIMPStringPtr sKeyPath;
    MakeString(keyPath, &sKeyPath);
    return muiService->GetValue(sKeyPath, out);
}
