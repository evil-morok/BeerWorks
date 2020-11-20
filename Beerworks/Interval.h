#pragma once

#include <EEPROM.h>

#define OFFSET 20
#define MAGIC 0x6a

class Interval {
private:
	uint8_t _number;
	uint8_t _defTemp;
	uint8_t _defMin;
	uint8_t _defPwr0;	
	uint8_t _defPwr1;
	inline int _TempAddr() {
		return OFFSET + _number * 4;
	}
	inline int _MinAddr() {
		return OFFSET + _number * 4 + 1;
	}
	inline int _PwrAddr0() {
		return OFFSET + _number * 4 + 2;
	}
	inline int _PwrAddr1() {
		return OFFSET + _number * 4 + 3;
	}

public:
	Interval(uint8_t number, uint8_t DefTemp, uint8_t DefMin, uint8_t DefPwr0, uint8_t DefPwr1) {
		_number = number;
		_defMin = DefMin;
		_defTemp = DefTemp;
		_defPwr0 = DefPwr0;
		_defPwr1 = DefPwr1;
	}

	uint8_t Temperature;
	uint8_t Minutes;
	uint8_t Power0;
	uint8_t Power1;
	
	void IncTemperature() {
		if (Temperature < 99) 
			Temperature++;
		else 
			Temperature = 99;
	}
	void DecTemperature() {
		if (Temperature > 20)
			Temperature--;
		else 
			Temperature = 20;
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
		Temperature = EEPROM.read(_TempAddr());
		Minutes = EEPROM.read(_MinAddr());
		Power0 = EEPROM.read(_PwrAddr0());
		Power1 = EEPROM.read(_PwrAddr1());
	}
	void UpdateInterval() {
		EEPROM.update(_TempAddr(), Temperature);
		EEPROM.update(_MinAddr(), Minutes);
		EEPROM.update(_PwrAddr0(), Power0);
		EEPROM.update(_PwrAddr1(), Power1);
	}

	void SetDefault() {
		Temperature = _defTemp;
		Minutes = _defMin;
		Power0 = _defPwr0;
		Power1 = _defPwr1;
		UpdateInterval();
	}

	static bool CheckEpromValid() {
		auto data = EEPROM.read(OFFSET - 1);
		return data == MAGIC;
	}
	static void UpdateEpromValid() {
		EEPROM.write(OFFSET - 1, MAGIC);
	}
};