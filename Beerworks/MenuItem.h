#pragma once
#include "Control.h"


class MenuItem :
	public Control
{
public:
	MenuItem(char * disp, const char * n, const char * f) : Control(disp, 34), name(n), format(f), Display((display_t *)disp) {};
	~MenuItem();
	MenuItem *previous;
	MenuItem *next;
	MenuItem *parent;
	MenuItem *child;
	void render();
	void event(uint16_t event_type);
private:
	const char * name;
	const char * format;
	display_t * Display;
};

