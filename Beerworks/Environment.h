#pragma once

#include "hw.h"
#include "WString.h"
#include "Control.h"
#include <LiquidCrystal_I2C.h>
#include "Sequencer.h"
#include "Ds18x20.h"
#include "RTC.h"
#include "IntervalSetupMenuItem.h"
#include "HwThermo.h"

//#define DEBUGBTNS


#define LONG_PRESS_CYCLES 5000
#define DEBOUNCE_FILTER_CYCLES 200


class Environment
{
public:
	typedef Thermo<Hw::Relay> thermo;
	void Start();
	void Log(String line);
	void Main();
	void Redraw(bool force);
	display_t Display;
	Control *Root;
	Interval *Intervals[3 * 5];
	unsigned int Environment::Hash(const char * str);
private:
	typedef Ds18x20<Hw::Ds18x20> TermalSensorType;
	typedef RTC<Environment> RtcType;
	void display_redraw();
	void rising_event(event_type_t event_type);
	static void btn_ok_int_handler(bool pinstate);
	static void btn_cancel_int_handler(bool pinstate);
	static void up_int_handler();
	static void right_int_handler();
	LiquidCrystal_I2C *lcd;
	TermalSensorType *_ThermalSensor;
	RtcType *_Rtc;
	unsigned int display_sum = 0;
	float _Temperature;
	char _TempStr[7];
	static bool _okIsPressed;
	static bool _cancelIsPressed;
	static uint32_t _okPressedCycles;
	static uint32_t _cancelPressedCycles;
	static uint16_t _debounceFilter;
	static uint16_t _upDownDebounce;
	static uint16_t _leftRightDebounce;
	void _OnSecondInterval();
	bool _TempIsValid;
};

extern Environment env;
