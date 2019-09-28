#include "Environment.h"
#include <YetAnotherPcInt.h>

#include "MenuControl.h"

Environment env;

bool Environment::_cancelIsPressed = false;
bool Environment::_okIsPressed = false;

uint32_t Environment::_cancelPressedCycles = 0;
uint32_t Environment::_okPressedCycles = 0;

void Environment::Start()
{
	Hw::BtnCancel::Mode(INPUT_PULLUP);
	Hw::BtnOk::Mode(INPUT_PULLUP);
	Hw::BtnRightInt::Mode(INPUT_PULLUP);
	Hw::BtnRightRev::Mode(INPUT_PULLUP);
	Hw::BtnUPInt::Mode(INPUT_PULLUP);
	Hw::BtnUPRev::Mode(INPUT_PULLUP);

	attachInterrupt(0, Environment::up_int_handler, FALLING);
	attachInterrupt(1, Environment::right_int_handler, FALLING);

	lcd = new LiquidCrystal_I2C(0x3f, 16, 2);
	lcd->init();
	lcd->backlight();
	lcd->setCursor(0, 0);
	lcd->print("Init...");
	lcd->clear();

	auto item0 = new MenuItem(Display.Buffer, "1. Brewing", "");
	auto item01 = new MenuItem(Display.Buffer, "1.1 Preset 1", "Sweet");
	auto item02 = new MenuItem(Display.Buffer, "1.2 Preset 2", "Noname");
	auto item03 = new MenuItem(Display.Buffer, "1.3 Preset 3", "OneStep");

	auto item1 = new MenuItem(Display.Buffer, "2. Boiling", "");

	auto item2 = new MenuItem(Display.Buffer, "3. Settings", "");
	auto item21 = new MenuItem(Display.Buffer, "3.1 Preset 1", "111");
	auto item22 = new MenuItem(Display.Buffer, "3.2 Preset 2", "222");
	auto item23 = new MenuItem(Display.Buffer, "3.3 Preset 3", "333");

	item0->setNext(item1); {
		item0->child = item01;
		item01->setNext(item02);
		item02->setNext(item03);
		item03->setNext(item01);
	}
	item1->setNext(item2); {

	}
	item2->setNext(item0); {
		item2->child = item21;
		item21->setNext(item22);
		item22->setNext(item23);
		item22->setNext(item21);
	}
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
	static Hw::BtnOk btnOk;
	static Hw::BtnCancel btnCancel;
	btnOk.Attach(Hw::BtnOk::PcChanged, btn_ok_int_handler);
	btnCancel.Attach(Hw::BtnCancel::PcChanged, btn_cancel_int_handler);

	if (_okIsPressed) {
		_okPressedCycles++;
	}
	if (_cancelIsPressed) {
		_cancelPressedCycles++;
	}
	if (_okPressedCycles >= LONG_PRESS_CYCLES) {
		_okIsPressed = false;
		_okPressedCycles = 0;
		env.rising_event(EVT_BTN_OK_LO);
#ifdef DEBUGBTNS
		Serial.println("LONG OK!");
#endif
	}
	if (_cancelPressedCycles >= LONG_PRESS_CYCLES) {
		_cancelIsPressed = false;
		_cancelPressedCycles = 0;
		env.rising_event(EVT_BTN_CL_LO);
#ifdef DEBUGBTNS
		Serial.println("LONG cancel!");
#endif
	}

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
		}
		if (this->display_sum != sum) {
			display_redraw();
			display_sum = sum;
		}
	}
}

void Environment::display_redraw()
{
	lcd->clear();
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

void Environment::btn_ok_int_handler(bool pinstate) {
	noInterrupts();
#ifdef DEBUGBTNS
	Serial.println("BTN!");
#endif
	if (pinstate) {
		env.rising_event(EVT_BTN_OK); 
	}
	else {
		_okPressedCycles = 0;
	}
	_okIsPressed = pinstate;
	interrupts();
}

void Environment::btn_cancel_int_handler(bool pinstate) {
	noInterrupts();
#ifdef DEBUGBTNS
	Serial.println("cancel!");
#endif
	if (pinstate) {
		env.rising_event(EVT_BTN_CL);
	}
	else {
		_cancelIsPressed = 0;
	}
	_cancelIsPressed = pinstate;
	interrupts();
}

void Environment::up_int_handler() {
	noInterrupts();
	if (!digitalRead(HW_UP_INT)) {
		if (!digitalRead(HW_UP_REV)) {
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
	interrupts();
}

void Environment::right_int_handler() {
	noInterrupts();
	if (!digitalRead(HW_RIGHT_INT)) {
		if (!digitalRead(HW_RIGHT_REV)) {
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
	interrupts();
}

#pragma endregion
