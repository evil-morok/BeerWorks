#pragma once

#include "hw.h"

#define DEBUGBTNS

extern Environment env;

typedef enum {
	EVT_BTN_UP,
	EVT_BTN_DN,
	EVT_BTN_LF,
	EVT_BTN_RI,
	EVT_BTN_OK,
	EVT_BTN_CL
}event_type_t;

class Environment
{
public:
	void Start();

private:
	void rising_event(event_type_t event_type);
	static void btn_ok_int_handler();
	static void btn_cancel_int_handler();
	static void up_int_handler();
	static void right_int_handler();
};

