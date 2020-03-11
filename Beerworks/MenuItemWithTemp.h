#pragma once

#include "MenuItem.h"

class MenuItemWithTemp : public MenuItem {
private:
	float * _temperature;
public:
	MenuItemWithTemp(char * disp, const char * n, const char * f, float * temperature) : MenuItem(disp, n, f) {

	}
};