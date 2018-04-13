#include "MenuControl.h"

void MenuControl::render() {
	MenuItem->render();
}

void MenuControl::event(uint16_t event_type)
{
	MenuItem->event(event_type);
	switch ((event_type_t)event_type)
	{
	case EVT_BTN_UP:
		MenuItem = MenuItem->next;
		break;
	case EVT_BTN_DN:
		MenuItem = MenuItem->previous;
		break;
	case EVT_BTN_OK:
		MenuItem = MenuItem->child;
		break;
	case EVT_BTN_CL:
		MenuItem = MenuItem->parent;
		break;
	default:
		break;
	}
}



MenuControl::~MenuControl()
{
}
