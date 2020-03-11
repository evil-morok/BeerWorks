#pragma once


class ActionControl : Control {
public:
	ActionControl(char * disp) :
		Control(disp, 34),
		Display((display_t *)disp) {}


	virtual void render() {
		snprintf(&(Display->Lines.Line0[0]), 16, "1");
		snprintf(&(Display->Lines.Line1[0]), 16, "2");
	}

	virtual void event(uint16_t event_type) {

	};
private:
	display_t *Display;
};