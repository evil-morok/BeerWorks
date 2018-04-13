#pragma once

#include "Control.h"
#include "MenuItem.h"

class MenuControl : public Control
{
public:
	MenuControl(char * disp) : 
		Control(disp, 34),
		Display((display_t *) disp)
	{};
	~MenuControl();
	void render();
	void event(uint16_t event_type);
	MenuItem *MenuItem;
private:
	display_t *Display;
};

