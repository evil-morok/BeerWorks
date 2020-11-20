#pragma once

#define REQUEST_CYCLES_DELAY 1000
#include <OneWire.h>


enum Ds18x20Sequences
{
	Ds18x20SequencesSearch,
	Ds18x20SequencesStart,
	Ds18x20SequencesRead,
	Ds18x20SequencesStop
};

template<class pin>
class Ds18x20 : public Sequencer< Ds18x20Sequences, Ds18x20<pin>, Ds18x20SequencesStop, Ds18x20SequencesSearch, Ds18x20SequencesStop>{

private:
	bool _Inited;
	uint8_t _Addr[8];
	uint8_t _Data[12];
	OneWire * _ThermalSensor;
	bool _Valid;
	uint32_t _Cycles;
	float _Temperature;
	uint8_t _Type_s;

public:
	void Run() {
		if (!_Inited) return;
		if (_Cycles >= REQUEST_CYCLES_DELAY) {
			_Cycles = 0;
			Next();
		}
		else {
			_Cycles++;
		}

	}

	Ds18x20() {
		_Cycles = 0;
		AddSequence(Ds18x20SequencesSearch, Ds18x20SequencesStart, &Ds18x20<pin>::_SequenceSearch);
		AddSequence(Ds18x20SequencesStart, Ds18x20SequencesRead, &Ds18x20<pin>::_SequenceStart);
		AddSequence(Ds18x20SequencesRead, Ds18x20SequencesStart, &Ds18x20<pin>::_SequenceRead);
		_ThermalSensor = new OneWire(pin::GetPinNumber());
		Serial.println("------------");
		_Inited = true;
		_Valid = false;
	}
	bool GetTemp(float *temperature) {
		*temperature = _Temperature;
		return _Valid;
	}
private:
	void _SequenceSearch() {
		Serial.println("Search sensor");
		_ThermalSensor->reset_search();

		if (_ThermalSensor->search(_Addr)) {
			for (uint8_t i = 0; i < 8; i++) {
				Serial.write(' ');
				Serial.print(_Addr[i], HEX);
			}
			uint8_t crc = OneWire::crc8(_Addr, 7);
			Serial.print(" Crc: ");
			Serial.println(crc, HEX);
			if (crc == _Addr[7]) {
				switch (_Addr[0]) {
				case 0x10:
					Serial.println("  Chip = DS18S20");  // or old DS1820
					_Type_s = 1;
					break;
				case 0x28:
					Serial.println("  Chip = DS18B20");
					_Type_s = 0;
					break;
				case 0x22:
					Serial.println("  Chip = DS1822");
					_Type_s = 0;
					break;
				default:
					Serial.println("Device is not a DS18x20 family device.");
					return;
				}
			}
			else {
				SetSequence(Ds18x20SequencesSearch);
			}
		}
	}

	void _SequenceStart() {
		_ThermalSensor->reset();
		_ThermalSensor->select(_Addr);
		_ThermalSensor->write(0x44, 1);
	}

	void _SequenceRead() {
		_ThermalSensor->reset();
		_ThermalSensor->select(_Addr);
		_ThermalSensor->write(0xBE);
		_Valid = false;
		for (uint8_t i = 0; i < 9; i++) {           // we need 9 bytes
			_Data[i] = _ThermalSensor->read();
		}
		if (OneWire::crc8(_Data, 8) == _Data[8]) {
			_Valid = true;
			int16_t raw = (_Data[1] << 8) | _Data[0];
			if (_Type_s) {
				raw = raw << 3; // 9 bit resolution default
				if (_Data[7] == 0x10) {
					// "count remain" gives full 12 bit resolution
					raw = (raw & 0xFFF0) + 12 - _Data[6];
				}
			}
			else {
				byte cfg = (_Data[4] & 0x60);
				// at lower res, the low bits are undefined, so let's zero them
				if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
				else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
				else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
				//// default is 12 bit resolution, 750 ms conversion time
			}
			_Temperature = (float)raw / 16.0;
		}
		if (!_Valid) {
			SetSequence(Ds18x20SequencesSearch);
		}
	}


};

