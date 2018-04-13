#include "Environment.h"
#include <YetAnotherPcInt.h>

Environment env;

void Environment::Start()
{
	pinMode(HW_BTN_CANCEL, INPUT_PULLUP);
	pinMode(HW_BTN_OK, INPUT_PULLUP);
	pinMode(HW_RIGHT_INT, INPUT_PULLUP);
	pinMode(HW_RIGHT_REV, INPUT_PULLUP);
	pinMode(HW_UP_INT, INPUT_PULLUP);
	pinMode(HW_UP_REV, INPUT_PULLUP);
	attachInterrupt(0, Environment::up_int_handler, CHANGE);
	attachInterrupt(1, Environment::right_int_handler, CHANGE);
	PcInt::attachInterrupt(HW_BTN_OK, Environment::btn_ok_int_handler, FALLING);
	PcInt::attachInterrupt(HW_BTN_CANCEL, Environment::btn_cancel_int_handler, FALLING);
	Serial.begin(115200);
	Serial.println("Start");
}

void Environment::rising_event(event_type_t event_type)
{
	
}

#pragma region HW_BUTTON_INTERRUPT_HANDLERS

void Environment::btn_ok_int_handler() {
#ifdef DEBUGBTNS
	Serial.println("BTN!");
#endif
	env.rising_event(EVT_BTN_OK);
}

void Environment::btn_cancel_int_handler() {
#ifdef DEBUGBTNS
	Serial.println("cancel!");
#endif
	env.rising_event(EVT_BTN_CL);
}

void Environment::up_int_handler() {
	if (!digitalRead(HW_UP_INT)) {
		if (!digitalRead(HW_UP_REV)) {
#ifdef DEBUGBTNS
			Serial.println("UP!");
#endif
			env.rising_event(EVT_BTN_UP);
		}
		else {
#ifdef DEBUGBTNS
			Serial.println("DOWN!");
#endif
			env.rising_event(EVT_BTN_DN);
		}
	}
}

void Environment::right_int_handler() {
	if (!digitalRead(HW_RIGHT_INT)) {
		if (!digitalRead(HW_RIGHT_REV)) {
#ifdef DEBUGBTNS
			Serial.println("Right!");
#endif
			env.rising_event(EVT_BTN_RI);
		}
		else {
#ifdef DEBUGBTNS
			Serial.println("Left!");
#endif
			env.rising_event(EVT_BTN_LF);
		}
	}
}

#pragma endregion
