#include "Control.h"



Control::Control(char * display, uint8_t len)
{
	this->len = len;
	this->display = display;
}

Control::~Control()
{
}

void Control::render()
{
}

void Control::event(uint16_t event_type)
{
}

