#include "stdafx.h"

#include "MenuItemEvent.h"

MenuItemEvent::MenuItemEvent(EventCallback callback) {
    this->callback = callback;
}

MenuItemEvent::~MenuItemEvent() {
}

void WINAPI MenuItemEvent::OnExecute(IUnknown* data) {
    callback();
}
