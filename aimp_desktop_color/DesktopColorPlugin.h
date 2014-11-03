#pragma once

#include "CoreMessageListener.h"
#include "IUnknownImpl.h"
#include "apiPlugin.h"
#include "apiMenu.h"

class DesktopColorPlugin : public IUnknownImpl<IAIMPPlugin> {
    friend class MenuItemEvent;
public:
    virtual HRESULT WINAPI Initialize(IAIMPCore* core);
    virtual HRESULT WINAPI Finalize();

    virtual PWCHAR WINAPI InfoGet(int Index);
    virtual DWORD WINAPI InfoGetCategories();

    virtual void WINAPI SystemNotification(int NotifyID, IUnknown* Data);

    void OnMenuItemPressed();
    bool OnCoreMessage(DWORD message, int wParam, void* lParam);
private:
    bool ChangeCurrentSkinColor();
    void AddItemToUtilsMenu();
    void UpdateMenuItemNames();

    void RegisterMessageListener();
    void UnregisterMessageListener();

    bool CreateObject(REFIID iid, void** object);
    bool GetService(REFIID iid, void** service);
    bool IsServiceAvailable(IUnknown* provider, REFIID serviceIid);

    HRESULT UpdateMenuItemName(IAIMPString* menuId, IAIMPString* newName);

    IAIMPString* MakeString(PWCHAR strSeq);
    HRESULT MakeString(PWCHAR strSeq, IAIMPString** string);

    HRESULT LangLoadString(PWCHAR keyPath, IAIMPString** out);

    IAIMPCore* aimpCore;
    CoreMessageListener* coreMessageListener;
};

