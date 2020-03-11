#pragma once

#include "Control.h"
#include "MenuItem.h"

class MenuControl : public Control
{
public:
	MenuControl(char * disp) : 
		Control((display_t *)disp)
	{};
	~MenuControl() {}
	void render() {
		MenuItem->render();
	}
	void event(uint16_t event_type) {
		uint16_t sent_event = event_type;
		switch ((event_type_t)event_type)
		{
		case EVT_BTN_UP:
			if (MenuItem->next) {
				MenuItem->event(EVT_MENU_EXIT);
				MenuItem->next->parentMenu = MenuItem->parentMenu;
				MenuItem = MenuItem->next;
				sent_event = EVT_MENU_ENTER;
			}
			break;
		case EVT_BTN_DN:
			if (MenuItem->previous) {
				MenuItem->event(EVT_MENU_EXIT);
				MenuItem->next->parentMenu = MenuItem->parentMenu;
				MenuItem = MenuItem->previous;
				sent_event = EVT_MENU_ENTER;
			}
			break;
		case EVT_BTN_OK:
			if (MenuItem->child) {
				MenuItem->event(EVT_MENU_EXIT);
				MenuItem->child->parentMenu = MenuItem;
				MenuItem = MenuItem->child;
				sent_event = EVT_MENU_ENTER;
			}
			break;
		case EVT_BTN_CL:
			if (MenuItem->parentMenu) {
				MenuItem->event(EVT_MENU_EXIT);
				MenuItem = MenuItem->parentMenu;
				sent_event = EVT_MENU_ENTER;
			}
			break;
		default:
			break;
		}
		MenuItem->event(sent_event);
	}
	MenuItem *MenuItem;

};


