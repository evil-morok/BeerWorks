#pragma once

#include "Pin.h"

#define HW_UP_INT	2
#define HW_RIGHT_INT	3
#define HW_UP_REV	4
#define HW_RIGHT_REV	5

#define HW_BTN_OK	8
#define HW_BTN_CANCEL 9


class Hw {
public:
	typedef Pin<2, false> BtnUPInt;
	typedef Pin<3, false> BtnRightInt;
	typedef Pin<4, false> BtnUPRev;
	typedef Pin<5, false> BtnRightRev;
	typedef Pin<8, false> BtnOk;
	typedef Pin<9, false> BtnCancel;

	typedef Pin<7> Ds18x20;

	typedef Pin<10> Relay;
};