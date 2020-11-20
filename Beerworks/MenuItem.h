#pragma once
#include "Control.h"


class MenuItem :
	public Control
{
public:
	MenuItem(char * disp, const char * n, const char * f, Control * MenuControl) :
		Control((display_t *)disp), name(n), format(f) {
		this->parent = MenuControl;
	};
	~MenuItem() {}
	MenuItem *previous = NULL;
	MenuItem *next = NULL;
	MenuItem *parentMenu = NULL;
	MenuItem *child = NULL;

	virtual void render() {
		snprintf(&(Display->Lines.Line0[0]), 17, name);
		snprintf(&(Display->Lines.Line1[0]), 17, format);
	}

	virtual void event(uint16_t event_type) {

	};

	void setNext(MenuItem *next)
	{
		this->next = next;
		next->previous = this;
	}

protected:
	const char * name;
	const char * format;
	
};

