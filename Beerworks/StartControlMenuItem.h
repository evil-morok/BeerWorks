#pragma once

#include "MenuItem.h"

class StartControlMenuItem : public MenuItem {

public:
	StartControlMenuItem(char * disp, const char * n, const char * f, Control * MenuControl, Control * NewControl, Control ** root) :
		MenuItem(disp, n, f, MenuControl), rootControl( root ), targetControl(NewControl) {}

	virtual void render() {
		snprintf(&(Display->Lines.Line0[0]), 16, name);
		snprintf(&(Display->Lines.Line1[0]), 16, format);
	}

	virtual void event(uint16_t event_type) {
		uint16_t sent_event = event_type;
		Serial.println(event_type);
		switch ((event_type_t)event_type)
		{
		case EVT_BTN_OK_LO:
			*rootControl = targetControl;
			targetControl->event(EVT_MENU_ENTER);
			break;
		}
	};
private:
	Control** rootControl;
	Control* targetControl;
};