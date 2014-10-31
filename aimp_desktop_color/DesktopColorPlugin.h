#pragma once

#include "IUnknownImpl.h"
#include "apiPlugin.h"

class DesktopColorPlugin : public IUnknownImpl<IAIMPPlugin> {
public:
    virtual HRESULT WINAPI Initialize(IAIMPCore* core);
    virtual HRESULT WINAPI Finalize();

    virtual PWCHAR WINAPI InfoGet(int Index);
    virtual DWORD WINAPI InfoGetCategories();

    virtual void WINAPI SystemNotification(int NotifyID, IUnknown* Data);
private:
    void ChangeCurrentSkinColor();
    bool IsServiceAvailable(IUnknown* provider, REFIID serviceIid);

    IAIMPCore* aimpCore;
};

