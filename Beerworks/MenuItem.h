#pragma once
#include "Control.h"


class MenuItem :
	public Control
{
public:
	MenuItem(char * disp, const char * n, const char * f) : Control(disp, 34), name(n), format(f), Display((display_t *)disp) {};
	~MenuItem();
	MenuItem *previous = NULL;
	MenuItem *next = NULL;
	MenuItem *parent = NULL;
	MenuItem *child = NULL;
	virtual void render();
	virtual void event(uint16_t event_type);
	void setNext(MenuItem *next);
private:
	const char * name;
	const char * format;
	display_t * Display;
};

