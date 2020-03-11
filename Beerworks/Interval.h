#pragma once

#include <EEPROM.h>

#define OFFSET 10

class Interval {
private:
	uint8_t _number;
	uint8_t _defTemp;
	uint8_t _defMin;
	inline int _TempAddr() {
		return OFFSET + _number * 2;
	}
	inline int _MinAddr() {
		return OFFSET + _number * 2 + 1;
	}

public:
	Interval(uint8_t number, uint8_t DefTemp, uint8_t DefMin) {
		_number = number;
		_defMin = DefMin;
		_defTemp = DefTemp;
	}

	uint8_t Temperatire;
	uint8_t Minutes;
	
	void IncTemperature() {
		if (Temperatire < 99) 
			Temperatire++;
		else 
			Temperatire = 99;
	}
	void DecTemperature() {
		if (Temperatire > 20)
			Temperatire--;
		else 
			Temperatire = 20;
	}
	void IncMinutes() {
		if (Minutes < 99)
			Minutes++;
		else
			Minutes = 99;
	}
	void DecMinutes() {
		if (Minutes > 0) 
			Minutes--;
		else
			Minutes = 0;
	}

	void ReadInterval() {
		Temperatire = EEPROM.read(_TempAddr());
		Minutes = EEPROM.read(_MinAddr());
	}
	void UpdateInterval() {
		EEPROM.update(_TempAddr(), Temperatire);
		EEPROM.update(_MinAddr(), Minutes);
	}

	void SetDefault() {
		Temperatire = _defTemp;
		Minutes = _defMin;
		UpdateInterval();
	}

	static bool CheckEpromValid() {
		auto data = EEPROM.read(OFFSET - 1);
		Serial.println(data);
		return data == 0x5b;
	}
	static void UpdateEpromValid() {
		EEPROM.write(OFFSET - 1, 0x5b);
	}
};