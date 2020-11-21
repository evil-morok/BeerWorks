#pragma once

#include "Control.h"
#include "HwThermo.h"
#include <EEPROM.h>

#define BOILING_OFFSET 0
#define BOILING_MAXPOWER BOILING_OFFSET + sizeof(uint8_t)
#define BOILING_MAXTEMP BOILING_MAXPOWER + sizeof(uint8_t)
#define BOILING_EEPROM_MAGIC 0x33

class BoilingControl : public Control {
	typedef Thermo<Hw::Relay> thermo;
public:
	BoilingControl(char * display, Control * parent, Control ** root, float * Temperature) :
		Control((display_t *)display, parent), root(root), _temperature(Temperature),
		_maxPower(100), _currentPower(0), _SecondsToStop(60 * 60), _TargetTemperature(98), _TempNoTime(false), _EepromParamsChanged(false){}

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
		float tt = *_temperature <= 99 ? *_temperature : 99;
		dtostrf(tt, 3, 1, _ctStr);
		dtostrf(_TargetTemperature, 3, 1, _ttStr);
		_currentPower = thermo::GetCurrentPower();
		snprintf(&(Display->Lines.Line0[0]), 17, "m:%3i%% %3i%% %s", _maxPower, _currentPower, str);
		uint8_t hr = _SecondsToStop / 60;
		uint8_t min = _SecondsToStop % 60;
		if (_TempNoTime) {
			snprintf(&(Display->Lines.Line1[0]), 17, "%s >%s %02i:%02i", _ctStr, _ttStr, hr, min);
		}
		else {
			snprintf(&(Display->Lines.Line1[0]), 17, "%s %s >%02i:%02i", _ctStr, _ttStr, hr, min);
		}
		
	}

	virtual void event(uint16_t event_type) {
		uint16_t sent_event = event_type;
		
		switch ((event_type_t)event_type)
		{
		case EVT_MENU_ENTER:
			{
				thermo::Enable(true);
				uint8_t ok = EEPROM.read(BOILING_OFFSET);
				_EepromParamsChanged = false;
				if (ok == BOILING_EEPROM_MAGIC) {
					EEPROM.get(BOILING_MAXTEMP, _TargetTemperature);
					EEPROM.get(BOILING_MAXPOWER, _maxPower);
				}
				else {
					EEPROM.put(BOILING_MAXTEMP, _TargetTemperature);
					EEPROM.put(BOILING_MAXPOWER, _maxPower);
					EEPROM.write(BOILING_OFFSET, BOILING_EEPROM_MAGIC);
				}
				thermo::SetTemperature(100);
				thermo::SetMaxPower(_maxPower);
			}
			break;
		case EVT_BTN_CL_LO:
			_Stop();
			break;
		case EVT_BTN_RI:
			if (_maxPower < 100) {
				_maxPower += 10;
			}
			else {
				_maxPower = 100;
			}
			thermo::SetMaxPower(_maxPower);
			_EepromParamsChanged = true;
			break;
		case EVT_BTN_LF:
			if (_maxPower > 0) {
				_maxPower -= 10;
			}
			else {
				_maxPower = 0;
			}
			thermo::SetMaxPower(_maxPower);
			_EepromParamsChanged = true;
			break;
		case EVT_BTN_UP:
			if (_TempNoTime) {
				if (_TargetTemperature < 99.8f) {
					_TargetTemperature += .1;
				}
				_EepromParamsChanged = true;
			}
			else {
				if (_SecondsToStop < 60 * 60 * 3) {
					_SecondsToStop += 60;
				}
			}
			break;
		case EVT_BTN_DN:
			if (_TempNoTime) {
				if (_TargetTemperature > 90.1f) {
					_TargetTemperature -= .1;
				}
				_EepromParamsChanged = true;
			}
			else {
				if (_SecondsToStop > 60) {
					_SecondsToStop -= 60;
				}
			}
			_EepromParamsChanged = true;
			break;
		case EVT_BTN_OK:
			_TempNoTime = !_TempNoTime;
			break;
		}
	};
	virtual void SecondInterval() {
		thermo::UpdateTemp(*_temperature);
		if (_SecondsToStop > 0) {
			if (_TargetTemperature < *_temperature)
				_SecondsToStop--;
		}
		else {
			_Stop();
		}
	}
private:
	void _Stop() {
		Hw::Relay::Off();
		thermo::Enable(false);
		*root = parent;
		if (_EepromParamsChanged) {
			EEPROM.put(BOILING_MAXTEMP, _TargetTemperature);
			EEPROM.put(BOILING_MAXPOWER, _maxPower);
			_EepromParamsChanged = false;
		}
	}
	Control ** root;
	float * _temperature;
	float _TargetTemperature;
	uint16_t _SecondsToStop;

	uint8_t _maxPower;
	uint8_t _currentPower;
	bool _TempNoTime;
	bool _EepromParamsChanged;
};