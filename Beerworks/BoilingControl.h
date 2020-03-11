#pragma once

#include "Control.h"
#include "HwThermo.h"

class BoilingControl : public Control {
	typedef Thermo<Hw::Relay> thermo;
public:
	BoilingControl(char * display, Control * parent, Control ** root, float * Temperature) :
		Control((display_t *)display, parent), root(root), temperature(Temperature){}

	virtual void render() {
		char* str;
		bool RelayIsOn = thermo::Run();
		if (RelayIsOn) {
			str = "On";
		}
		else {
			str = "Off";
		}
		char _ctStr[5];
		char _ttStr[5];
		dtostrf(*temperature, 3, 1, _ctStr);
		dtostrf(90.0f, 3, 1, _ttStr);
		snprintf(&(Display->Lines.Line0[0]), 17, "m:%.3i%% %.3i%% %s", maxPower, currentPower, str);
		snprintf(&(Display->Lines.Line1[0]), 17, "%s=>%s %.2i:%.2i", _ctStr, _ttStr, 59, 30);
	}

	virtual void event(uint16_t event_type) {
		uint16_t sent_event = event_type;
		
		switch ((event_type_t)event_type)
		{
		case EVT_MENU_ENTER:
			thermo::SetTemperature(30);
			break;
		case EVT_BTN_CL_LO:
			*root = parent;
			break;
		}
	};
	virtual void SecondInterval() {
		thermo::UpdateTemp(*temperature);
		
	}
private:
	Control ** root;
	float * temperature;

	uint8_t maxPower;
	uint8_t currentPower;
};