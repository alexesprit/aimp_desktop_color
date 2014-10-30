#include "stdafx.h"
#include "utils.h"

#pragma comment(lib, "dwmapi.lib")
HRESULT GetDwmColor(int* r, int* g, int* b) {
    DWORD pcrColorization = 0;
    BOOL pfOpaqueBlend = FALSE;
    if (DwmGetColorizationColor(&pcrColorization, &pfOpaqueBlend) == S_OK) {
        *r = pcrColorization >> 16 & 0xFF;
        *g = pcrColorization >> 8 & 0xFF;
        *b = pcrColorization & 0xFF;
        return S_OK;
    }
    return E_UNEXPECTED;
}

int ByteToPercent(byte value) {
    return value * 100 / 255;
}
