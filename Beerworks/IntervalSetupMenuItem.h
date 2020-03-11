#pragma once

#include "MenuItem.h"
#include "Interval.h"

class IntervalSetupMenuItem : public MenuItem {

public:
	IntervalSetupMenuItem(char * disp, const char * n, const char * f, Interval *i, Control * MenuControl) : MenuItem(disp, n, f, MenuControl) {
		this->_interval = i;
	}

	void render() {
		snprintf(&(Display->Lines.Line0[0]), 16, name);
		if (_setTemp) {
			snprintf(&(Display->Lines.Line1[0]), 16, "tmp:>%2.2i min: %2.2i", _interval->Temperatire, _interval->Minutes);
		}
		else {
			snprintf(&(Display->Lines.Line1[0]), 16, "tmp: %2.2i min:>%2.2i", _interval->Temperatire, _interval->Minutes);
		}
	}

	void event(uint16_t event_type) {
		switch ((event_type_t)event_type)
		{
		case EVT_BTN_OK:
			_setTemp = !_setTemp;
			break;
		case EVT_BTN_LF:
			if (_setTemp) {
				_interval->DecTemperature();
			}
			else {
				_interval->DecMinutes();
			}
			break;
		case EVT_BTN_RI:
			if (_setTemp) {
				_interval->IncTemperature();
			}
			else {
				_interval->IncMinutes();
			}
			break;
		case EVT_MENU_ENTER:
			_interval->ReadInterval();
			_setTemp = true;
			break;
		case EVT_MENU_EXIT:
			_interval->UpdateInterval();
			break;
		default:
			break;
		}
	};

private:
	Interval *_interval;
	bool _setTemp;
};