#include "MenuControl.h"

void MenuControl::render() {
	MenuItem->render();
}

void MenuControl::event(uint16_t event_type)
{
	switch ((event_type_t)event_type)
	{
	case EVT_BTN_UP:
		if (MenuItem->next) {
			MenuItem->next->parent = MenuItem->parent;
			MenuItem = MenuItem->next;
		}
		break;
	case EVT_BTN_DN:
		if (MenuItem->previous) {
			MenuItem->next->parent = MenuItem->parent;
			MenuItem = MenuItem->previous;
		}
		break;
	case EVT_BTN_OK:
		if (MenuItem->child) {
			MenuItem->child->parent = MenuItem;
			MenuItem = MenuItem->child;
		}
		break;
	case EVT_BTN_CL:
		if (MenuItem->parent) {
			MenuItem = MenuItem->parent;
		}
		break;
	default:
		break;
	}
	MenuItem->event(event_type);
}



MenuControl::~MenuControl()
{
}
