#include "MenuItem.h"

void MenuItem::render() {
	snprintf(&(Display->Lines.Line0[0]), 16, name);
	snprintf(&(Display->Lines.Line1[0]), 16, format);
}

void MenuItem::event(uint16_t event_type)
{
}

void MenuItem::setNext(MenuItem *next)
{
	this->next = next;
	next->previous = this;
}


MenuItem::~MenuItem()
{
}
