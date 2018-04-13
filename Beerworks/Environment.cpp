#include "Environment.h"
#include <YetAnotherPcInt.h>

#include "MenuControl.h"

Environment env;

void Environment::Start()
{
	pinMode(HW_BTN_CANCEL, INPUT_PULLUP);
	pinMode(HW_BTN_OK, INPUT_PULLUP);
	pinMode(HW_RIGHT_INT, INPUT_PULLUP);
	pinMode(HW_RIGHT_REV, INPUT_PULLUP);
	pinMode(HW_UP_INT, INPUT_PULLUP);
	pinMode(HW_UP_REV, INPUT_PULLUP);
	attachInterrupt(0, Environment::up_int_handler, FALLING);
	attachInterrupt(1, Environment::right_int_handler, FALLING);
	PcInt::attachInterrupt(HW_BTN_OK, Environment::btn_ok_int_handler, FALLING);
	PcInt::attachInterrupt(HW_BTN_CANCEL, Environment::btn_cancel_int_handler, FALLING);
	lcd = new LiquidCrystal_I2C(0x3f, 16, 2);
	lcd->init();
	lcd->backlight();
	lcd->setCursor(0, 0);
	lcd->print("Init...");
	lcd->clear();

	auto item0 = new MenuItem(Display.Buffer, "Item0", "format0");
	auto item1 = new MenuItem(Display.Buffer, "Item1", "format1");
	auto item2 = new MenuItem(Display.Buffer, "Item2", "format2");
	auto item01 = new MenuItem(Display.Buffer, "Item01", "format01");
	item0->child = item01; item0->next = item1; item0->previous = item2;
	item1->next = item2; item1->previous = item0;
	item2->next = item0; item2->previous = item1;
	auto mc = new MenuControl(Display.Buffer);
	mc->MenuItem = item0;

	this->Root = mc;

	Serial.begin(115200);
	Serial.println("Start");
}

void Environment::Log(String line)
{
}

void Environment::Main()
{
	Root->render();
	Redraw(false);
}

void Environment::Redraw(bool force)
{
	if (force) {
		display_redraw();
	}
	else {
		uint16_t sum = 0;
		for (int i = 0; i < 34; i++) {
			sum += this->Display.Buffer[i];
			if (this->display_sum != sum) {
				display_redraw();
				display_sum = sum;
			}
		}
	}
}

void Environment::display_redraw()
{
	lcd->setCursor(0, 0);
	lcd->print(Display.Lines.Line0);
	lcd->setCursor(0, 1);
	lcd->print(Display.Lines.Line1);
}

void Environment::rising_event(event_type_t event_type)
{
	Root->event(event_type);
}

#pragma region HW_BUTTON_INTERRUPT_HANDLERS

int readB(uint8_t pin) {
	int prev = digitalRead(pin);
	int data;
	for (int i = 0; i < 5; data = digitalRead(pin)) {
		if (data == prev) {
			i++;
		}
		else {
			i = 0;
		}
		prev = data;
	}
	return data;
}

void Environment::btn_ok_int_handler() {
#ifdef DEBUGBTNS
	Serial.println("BTN!");
#endif
	env.rising_event(EVT_BTN_OK);
}

void Environment::btn_cancel_int_handler() {
#ifdef DEBUGBTNS
	Serial.println("cancel!");
#endif
	env.rising_event(EVT_BTN_CL);
}

void Environment::up_int_handler() {
	if (!digitalRead(HW_UP_INT)) {
		if (!readB(HW_UP_REV)) {
#ifdef DEBUGBTNS
			Serial.println("UP!");
#endif
			env.rising_event(EVT_BTN_UP);
		}
		else {
#ifdef DEBUGBTNS
			Serial.println("DOWN!");
#endif
			env.rising_event(EVT_BTN_DN);
		}
	}
}

void Environment::right_int_handler() {
	if (!digitalRead(HW_RIGHT_INT)) {
		if (!readB(HW_RIGHT_REV)) {
#ifdef DEBUGBTNS
			Serial.println("Right!");
#endif
			env.rising_event(EVT_BTN_RI);
		}
		else {
#ifdef DEBUGBTNS
			Serial.println("Left!");
#endif
			env.rising_event(EVT_BTN_LF);
		}
	}
}

#pragma endregion
