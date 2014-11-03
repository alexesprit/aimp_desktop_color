#pragma once

#include "apiActions.h"
#include "IUnknownImpl.h"

class MenuItemEvent : public IUnknownImpl<IAIMPActionEvent> {
public:
    MenuItemEvent(EventCallback callback);
    ~MenuItemEvent();
    virtual void WINAPI OnExecute(IUnknown* data);
private:
    std::function<void()> callback;
};
