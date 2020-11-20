#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "Interval.h"

typedef enum {
	EVT_BTN_UP,
	EVT_BTN_DN,
	EVT_BTN_LF,
	EVT_BTN_RI,
	EVT_BTN_OK,
	EVT_BTN_CL,
	EVT_BTN_OK_LO,
	EVT_BTN_CL_LO,
	EVT_MENU_ENTER,
	EVT_MENU_EXIT
}event_type_t;

typedef struct display_t {
	union {
		char Buffer[34] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
		struct lines {
			char Line0[17];
			char Line1[17];
		} Lines;
	};
}display_t;

#define _display(col, row, ...) snprintf(&(Display->Lines.Line##row[col]), 16 - col, __VA_ARGS__)
class Control
{
public:
	Control(display_t * display, Control * parent = nullptr)
	{
		this->Display = display;
		this->parent = parent;
	}
	~Control() {}
	virtual void render() = 0;
	virtual void event(uint16_t event_type) = 0;
	virtual void SecondInterval() {}
	virtual void SetParam(void * param) {}
protected:
	Control * parent;
	display_t * Display;
};


