#pragma once

#include "hw.h"
#include "WString.h"
#include "Control.h"
#include <LiquidCrystal_I2C.h>

#define DEBUGBTNS


#define LONG_PRESS_CYCLES 10000


class Environment
{
public:
	void Start();
	void Log(String line);
	void Main();
	void Redraw(bool force);
	display_t Display;
	Control *Root;
private:
	void display_redraw();
	void rising_event(event_type_t event_type);
	static void btn_ok_int_handler(bool pinstate);
	static void btn_cancel_int_handler(bool pinstate);
	static void up_int_handler();
	static void right_int_handler();
	LiquidCrystal_I2C *lcd;
	uint16_t display_sum = 0;
	static bool _okIsPressed;
	static bool _cancelIsPressed;
	static uint32_t _okPressedCycles;
	static uint32_t _cancelPressedCycles;
};

extern Environment env;
