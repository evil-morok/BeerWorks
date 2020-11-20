#pragma once

#include "Control.h"
#include "HwThermo.h"
#include "Sequencer.h"


#define TEMP_THRESHOLD .5f
#define SELECT_INT_TIMEOUT 5;


enum BrewingSequences
{
	BrewingGetInterval,
	BrewingPrepareWarmUp,
	BrewingWarmUp,
	BrewingPrepareDoInterval,
	BrewingDoInterval,
	BrewingPause,
	BrewingStop,
	BrewingSelectInterval,
	BrewingEnd
};


class BrewingControl : 
	public Control, 
	public Sequencer< BrewingSequences, BrewingControl, BrewingEnd, BrewingGetInterval, BrewingStop>
{
	typedef Thermo<Hw::Relay> thermo;
public:
	BrewingControl(char * display, Control * parent, Control ** root, float * Temperature, bool * TempIsValid) :
		Control((display_t *)display, parent), root(root), _temperature(Temperature), _TempIsValid(TempIsValid),
		_maxPower(100), _currentPower(0), _SecondsToStop(60 * 60), _TargetTemperature(98), _currentInterval(0) {
		AddSequence(BrewingGetInterval, BrewingPrepareWarmUp, &BrewingControl::_SequenceGetInterval);
		AddSequence(BrewingPrepareWarmUp, BrewingWarmUp, &BrewingControl::_SequencePrepareWarmUp);
		AddSequence(BrewingWarmUp, BrewingPrepareDoInterval, &BrewingControl::_SequenceWarmUp);
		AddSequence(BrewingPrepareDoInterval, BrewingDoInterval, &BrewingControl::_SequencePrepareDoInterval);
		AddSequence(BrewingDoInterval, BrewingGetInterval, &BrewingControl::_SequenceDoInterval);
		AddSequence(BrewingEnd, BrewingStop, &BrewingControl::_SequenceEnd);
		AddSequence(BrewingPause, BrewingPause, &BrewingControl::_SequencePause);
		AddSequence(BrewingSelectInterval, BrewingSelectInterval, &BrewingControl::_SequencePause);
	}

	virtual void render() {
		_currentPower = thermo::GetCurrentPower();
		uint8_t maxPower = _maxPower <= 99 ? _maxPower : 99;
		uint8_t curPower = _currentPower <= 99 ? _currentPower : 99;
		if (CurrentSequence == BrewingWarmUp || CurrentSequence == BrewingDoInterval || CurrentSequence == BrewingPause) {
			char* str;
			char* mode;
			bool RelayIsOn = thermo::Run();
			if (RelayIsOn) {
				str = ">";
			}
			else {
				str = " ";
			}
			if (CurrentSequence == BrewingWarmUp) {
				mode = "W";
			}
			else if (CurrentSequence == BrewingPause) {
				mode = "P";
			}
			else {
				mode = "I";
			}
			char _dTStr[10];

				if (thermo::dTAvgSec() != 0) {
					float ass = (_TargetTemperature - *_temperature) / (thermo::dTAvgSec() * 60 );
					if (ass > 0 && ass < 60) {
						if (ass > 1) {
							snprintf(_dTStr, 7, "%i min", (int)ass);
						}
						else {
							snprintf(_dTStr, 7, "%i sec", (int)(ass * 60));
						}
					}
					else {
						dtostrf(thermo::dTAvgSec() * 60, 6, 3, _dTStr);
					}
				}
				else {
					snprintf(_dTStr, 6, "0.00");
				}

			char _ctStr[5];
			char _ttStr[5];
			dtostrf(*_temperature, 4, 1, _ctStr);

			dtostrf(_TargetTemperature, 4, 1, _ttStr);
			
			snprintf(Display->Lines.Line0, 17, "%s%1i %2i/%2i%c %s", mode, _currentInterval + 1, curPower, maxPower, _EepromParamsChanged ? '?' : '%', _dTStr);
			uint8_t hr = _SecondsToStop / 60;
			uint8_t min = _SecondsToStop % 60;
			if (CurrentSequence != BrewingPause) {
				snprintf(Display->Lines.Line1, 17, "%s%s%s %02i:%02i", _ctStr, str, _ttStr, hr, min);
			}
			else {
				snprintf(Display->Lines.Line1, 17, "%s%s STOP %02i:%02i", _ctStr, str, hr, min);
			}
		}
		else if (CurrentSequence == BrewingSelectInterval) {
			char c[] = { ' ', ' ', ' ', ' ', ' ' };
			c[_currentInterval] = '^';
			snprintf(Display->Lines.Line0, 17, "Select 1 2 3 4 5");
			snprintf(Display->Lines.Line1, 17, "int.   %c %c %c %c %c", c[0], c[1], c[2], c[3], c[4]);
		}
		else {
			snprintf(Display->Lines.Line0, 17, "Wait");
			snprintf(Display->Lines.Line1, 17, "");
		}
	}

	virtual void event(uint16_t event_type) {
		uint16_t sent_event = event_type;

		switch ((event_type_t)event_type)
		{
			case EVT_MENU_ENTER:
			{
				thermo::Enable(true);
				_currentInterval = 0;
				NextSequence = BrewingGetInterval;
			}
			break;
			case EVT_BTN_CL_LO:
				_Stop();
				break;

			case EVT_BTN_OK_LO:
				_LastSequence = CurrentSequence;
				_BrewingSelectIntervalTimeout = SELECT_INT_TIMEOUT;
				NextSequence = BrewingSelectInterval;
				break;

			case EVT_BTN_UP:
				if (CurrentSequence == BrewingSelectInterval) {
					_BrewingSelectIntervalTimeout = SELECT_INT_TIMEOUT;
					_currentInterval++;
					if (_currentInterval > 4) {
						_currentInterval = 0;
					}
				}
				else {
					if (_SecondsToStop < 60 * 60 * 3) {
						_SecondsToStop += 60;
					}
				}
				break;

			case EVT_BTN_DN:
				if (CurrentSequence == BrewingSelectInterval) {
					_BrewingSelectIntervalTimeout = SELECT_INT_TIMEOUT;
					_currentInterval--;
					if (_currentInterval < 0) {
						_currentInterval = 4;
					}
				}
				else {
					if (_SecondsToStop > 60) {
						_SecondsToStop -= 60;
					}
				}
				break;
			case EVT_BTN_CL:
				if (CurrentSequence == BrewingPause) {
					NextSequence = _LastSequence;
					thermo::SetTemperature(_TargetTemperature);
				}
				else {
					_LastSequence = CurrentSequence;
					NextSequence = BrewingPause;
					thermo::SetTemperature(*_temperature);
				}
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
			case EVT_BTN_OK:
			{
				if (_EepromParamsChanged) {
					BrewingSequences s = CurrentSequence;
					if (CurrentSequence == BrewingPause) {
						s = _LastSequence;
					}
					switch (s)
					{
					case BrewingWarmUp:
						_int()->Power0 = _maxPower;
						_EepromParamsChanged = false;
						_int()->UpdateInterval();
						break;
					case BrewingDoInterval:
						_int()->Power1 = _maxPower;
						_EepromParamsChanged = false;
						_int()->UpdateInterval();
						break;
					default:
						break;
					}
				}
			}
			break;
		}
	};
	virtual void SecondInterval() {
		if (*_TempIsValid) {
			thermo::UpdateTemp(*_temperature);
			if (CurrentSequence == BrewingSelectInterval) {
				if (_BrewingSelectIntervalTimeout > 0) {
					_BrewingSelectIntervalTimeout--;
				}
				else {
					NextSequence = BrewingGetInterval;
				}
			}
			Next();
		}
	}

	virtual void SetParam(void * param) {
		_Preset = (Interval **)param;
	}

private:

	void _SequenceGetInterval() {
		_int()->ReadInterval();
		_TargetTemperature = _int()->Temperature;
		thermo::SetTemperature(_TargetTemperature);
		_SecondsToStop = _int()->Minutes * 60;
		_EepromParamsChanged = false;
	}

	void _SequencePrepareWarmUp() {
		_maxPower = _int()->Power0;
		thermo::SetMaxPower(_maxPower);
	}

	void _SequenceWarmUp() {
		float d = _TargetTemperature - (*_temperature);
		if (d > TEMP_THRESHOLD) {
			NextSequence = BrewingWarmUp;
		}
	}

	void _SequencePrepareDoInterval(){
		_maxPower = _int()->Power1;
		thermo::SetMaxPower(_maxPower);
		if (_currentInterval == 0) {
			_LastSequence = BrewingDoInterval;
			NextSequence = BrewingPause;
		}
	}

	void _SequenceDoInterval() {
		float d = _TargetTemperature - (*_temperature);

		if (d <= TEMP_THRESHOLD) {
			_SecondsToStop--;
		}
		if (_SecondsToStop > 0) {
			NextSequence = BrewingDoInterval;
		}
		else {
			if (_currentInterval < 4) {
				_currentInterval++;
			}
			else {
				NextSequence = BrewingEnd;
			}
		}
	}

	void _SequencePause() {

	}

	void _SequenceEnd() {
		_Stop();
	}

	void _Stop() {
		Hw::Relay::Off();
		thermo::Enable(false);
		*root = parent;

	}

	inline Interval * _int() {
		return _Preset[_currentInterval];
	}

	Control ** root;
	float * _temperature;
	bool * _TempIsValid;
	float _TargetTemperature;
	float _prevTemp;
	uint16_t _SecondsToStop;
	Interval **_Preset;
	uint8_t _maxPower;
	uint8_t _currentPower;
	int8_t _currentInterval;
	uint8_t _BrewingSelectIntervalTimeout;
	BrewingSequences _LastSequence;
	bool _EepromParamsChanged;
};