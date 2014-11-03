#pragma once

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
private:
    void ChangeCurrentSkinColor();
    void AddItemToUtilsMenu();

    bool CreateObject(REFIID iid, void** object);
    bool GetService(REFIID iid, void** service);
    bool IsServiceAvailable(IUnknown* provider, REFIID serviceIid);

    IAIMPString* MakeString(PWCHAR strSeq);

    IAIMPCore* aimpCore;
};

