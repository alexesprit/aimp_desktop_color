#include "stdafx.h"

#include "CoreMessageListener.h"

CoreMessageListener::CoreMessageListener(CoreMessageCallback callback) {
    this->callback = callback;
}

void WINAPI CoreMessageListener::CoreMessage(DWORD message, int wparam, void *lparam, HRESULT *result) {
    if (callback(message, wparam, lparam)) {
        *result = S_OK;
    }
}