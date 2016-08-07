#pragma once

#include "IUnknownImpl.h"
#include "apiMessages.h"

class CoreMessageListener : public IUnknownImpl<IAIMPMessageHook> {
public:
    CoreMessageListener(CoreMessageCallback callback);
    virtual void WINAPI CoreMessage(DWORD message, int wparam, void *lparam, HRESULT *result);
private:
    CoreMessageCallback callback;
};
