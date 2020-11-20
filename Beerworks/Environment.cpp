#include "Environment.h"
#include <YetAnotherPcInt.h>

#include "MenuControl.h"
#include "StartControlMenuItem.h"
#include "BoilingControl.h"
#include "BrewingControl.h"

Environment env;

bool Environment::_cancelIsPressed = false;
bool Environment::_okIsPressed = false;

uint16_t Environment::_upDownDebounce = 0;
uint16_t Environment::_leftRightDebounce = 0;

uint32_t Environment::_cancelPressedCycles = 0;
uint32_t Environment::_okPressedCycles = 0;
uint16_t Environment::_debounceFilter = 0;

void Environment::Start()
{
	Hw::BtnCancel::Mode(INPUT_PULLUP);
	Hw::BtnOk::Mode(INPUT_PULLUP);
	Hw::BtnRightInt::Mode(INPUT_PULLUP);
	Hw::BtnRightRev::Mode(INPUT_PULLUP);
	Hw::BtnUPInt::Mode(INPUT_PULLUP);
	Hw::BtnUPRev::Mode(INPUT_PULLUP);

	Hw::Relay::Mode(OUTPUT);

	attachInterrupt(0, Environment::up_int_handler, FALLING);
	attachInterrupt(1, Environment::right_int_handler, FALLING);

	lcd = new LiquidCrystal_I2C(0x3f, 16, 2);
	lcd->init();
	lcd->backlight();
	lcd->setCursor(0, 0);
	lcd->print("Init...");
	lcd->clear();

	Serial.begin(115200);
	Serial.println("Start");

	Intervals[0] = new Interval(0, 40, 30, 100, 100);
	Intervals[1] = new Interval(1, 45, 15, 100, 100);
	Intervals[2] = new Interval(2, 57, 30, 50, 100);
	Intervals[3] = new Interval(3, 66, 90, 50, 100);
	Intervals[4] = new Interval(4, 77, 15, 100, 100);

	Intervals[5] = new Interval(5, 40, 30, 100, 100);
	Intervals[6] = new Interval(6, 45, 15, 100, 100);
	Intervals[7] = new Interval(7, 57, 30, 50, 100);
	Intervals[8] = new Interval(8, 70, 30, 50, 100);
	Intervals[9] = new Interval(9, 77, 15, 100, 100);

	Intervals[10] = new Interval(10, 35, 1, 100, 100);
	Intervals[11] = new Interval(11, 35, 40, 100, 100);
	Intervals[12] = new Interval(12, 52, 80, 100, 100);
	Intervals[13] = new Interval(13, 66, 70, 50, 100);
	Intervals[14] = new Interval(14, 75, 15, 100, 100);

	if (!Interval::CheckEpromValid()) {
		for (int i = 0; i < 15; i++) {
			Intervals[i]->SetDefault();
		}
		Interval::UpdateEpromValid();
	}

	// Menu control
	auto mc = new MenuControl(Display.Buffer);
	// Boiling control. 
	auto boilingControl = new BoilingControl(Display.Buffer, mc, &Root, &_Temperature);
	auto brewingControl = new BrewingControl(Display.Buffer, mc, &Root, &_Temperature, &_TempIsValid);

	// Menu items
	auto item0 = new MenuItem(Display.Buffer, "1. Brewing", _TempStr, mc);
	auto item01 = new StartControlMenuItem(Display.Buffer, "1.1 Preset 1", "(Dry)", mc, brewingControl, &this->Root, &Intervals[0]);
	auto item02 = new StartControlMenuItem(Display.Buffer, "1.2 Preset 2", "(Sweet)", mc, brewingControl, &this->Root, &Intervals[5]);
	auto item03 = new StartControlMenuItem(Display.Buffer, "1.3 Preset 3", "(Classic)", mc, brewingControl, &this->Root, &Intervals[10]);

	auto item1 = new StartControlMenuItem(Display.Buffer, "2. Boiling", _TempStr, mc, boilingControl, &this->Root);

	auto item2 = new MenuItem(Display.Buffer, "3. Settings", "", mc);
	auto item21 = new MenuItem(Display.Buffer, "3.1 Preset 1", "(Dry)", mc);
	auto item211 = new IntervalSetupMenuItem(Display.Buffer, "3.1.1 Int 1", "", Intervals[0], mc);
	auto item212 = new IntervalSetupMenuItem(Display.Buffer, "3.1.2 Int 2", "", Intervals[1], mc);
	auto item213 = new IntervalSetupMenuItem(Display.Buffer, "3.1.3 Int 3", "", Intervals[2], mc);
	auto item214 = new IntervalSetupMenuItem(Display.Buffer, "3.1.4 Int 4", "", Intervals[3], mc);
	auto item215 = new IntervalSetupMenuItem(Display.Buffer, "3.1.5 Int 5", "", Intervals[4], mc);

	auto item22 = new MenuItem(Display.Buffer, "3.2 Preset 2", "(Sweet)", mc);
	auto item221 = new IntervalSetupMenuItem(Display.Buffer, "3.2.1 Int 1", "", Intervals[5], mc);
	auto item222 = new IntervalSetupMenuItem(Display.Buffer, "3.2.2 Int 2", "", Intervals[6], mc);
	auto item223 = new IntervalSetupMenuItem(Display.Buffer, "3.2.3 Int 3", "", Intervals[7], mc);
	auto item224 = new IntervalSetupMenuItem(Display.Buffer, "3.2.4 Int 4", "", Intervals[8], mc);
	auto item225 = new IntervalSetupMenuItem(Display.Buffer, "3.2.5 Int 5", "", Intervals[9], mc);

	auto item23 = new MenuItem(Display.Buffer, "3.3 Preset 3", "(Classic)", mc);
	auto item231 = new IntervalSetupMenuItem(Display.Buffer, "3.3.1 Int 1", "", Intervals[10], mc);
	auto item232 = new IntervalSetupMenuItem(Display.Buffer, "3.3.2 Int 2", "", Intervals[11], mc);
	auto item233 = new IntervalSetupMenuItem(Display.Buffer, "3.3.3 Int 3", "", Intervals[12], mc);
	auto item234 = new IntervalSetupMenuItem(Display.Buffer, "3.3.4 Int 4", "", Intervals[13], mc);
	auto item235 = new IntervalSetupMenuItem(Display.Buffer, "3.3.5 Int 5", "", Intervals[14], mc);

	auto item3 = new MenuItem(Display.Buffer, "4. Thermostat", "", mc);
	auto item31 = new MenuItem(Display.Buffer, "4.1 Cool", "", mc);
	auto item32 = new MenuItem(Display.Buffer, "4.2 Heat", "", mc);

	// Describe menu structure
	item0->setNext(item1); {
		item0->child = item01;
		item01->setNext(item02);
		item02->setNext(item03);
		item03->setNext(item01);
	}
	item1->setNext(item2); {

	}
	item2->setNext(item3); {
		item2->child = item21;
		item21->setNext(item22); {
			item21->child = item211;
			item211->setNext(item212);
			item212->setNext(item213);
			item213->setNext(item214);
			item214->setNext(item215);
			item215->setNext(item211);
		}
		item22->setNext(item23); {
			item22->child = item221;
			item221->setNext(item222);
			item222->setNext(item223);
			item223->setNext(item224);
			item224->setNext(item225);
			item225->setNext(item221);
		}
		item23->setNext(item21); {
			item23->child = item231;
			item231->setNext(item232);
			item232->setNext(item233);
			item233->setNext(item234);
			item234->setNext(item235);
			item235->setNext(item231);
		}
	}
	item3->setNext(item0); {
		item3->child = item31;
		item31->setNext(item32);
		item32->setNext(item31);
	}

	mc->MenuItem = item0;

	// Set root control to MenuControl
	this->Root = mc;

	// Init perepherals
	_Rtc = new RtcType(this, &Environment::_OnSecondInterval);
	_ThermalSensor = new TermalSensorType();
}

void Environment::_OnSecondInterval() {
	this->Root->SecondInterval();
	if (_TempIsValid) {
		Serial.print("$");
		Serial.print(thermo::GetCurrentPower());
		Serial.print(" ");
		Serial.print(thermo::GetTargetTemperature());
		Serial.print(" ");
		Serial.print(_Temperature);
		Serial.println(";");
	}
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

	_ThermalSensor->Run();
	_Rtc->Run();
	_TempIsValid = _ThermalSensor->GetTemp(&_Temperature);
	if (_TempIsValid) {
		dtostrf(_Temperature, 4, 2, _TempStr);
	}
	else {
		strcpy(_TempStr, "--.--");
	}
	if (_upDownDebounce) {
		_upDownDebounce++;
		if (_upDownDebounce > DEBOUNCE_FILTER_CYCLES) {
			_upDownDebounce = 0;
		}
	}

	if (_leftRightDebounce) {
		_leftRightDebounce++;
		if (_leftRightDebounce > DEBOUNCE_FILTER_CYCLES) {
			_leftRightDebounce = 0;
		}
	}

	if (_debounceFilter) {
		_debounceFilter++;
		if (_debounceFilter > DEBOUNCE_FILTER_CYCLES) {
			_debounceFilter = 0;
		}
	}
	if (_okIsPressed) {
		_okPressedCycles++;
	}
	else {
		_okPressedCycles = 0;
	}
	if (_cancelIsPressed) {
		_cancelPressedCycles++;
	}
	else {
		_cancelPressedCycles = 0;
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
		unsigned int sum1 = Hash(this->Display.Lines.Line0); 
		unsigned int sum2 = Hash(this->Display.Lines.Line1);
		if (this->display_sum != sum1 + sum2) {
			display_redraw();
			display_sum = sum1 + sum2;
		}
	}
}

unsigned int Environment::Hash(const char * str)
{
	unsigned int hash = 0;

	for (; *str; str++)
	{
		hash += (unsigned char)(*str);
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}
	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);

	return hash;
}

void Environment::display_redraw()
{
	//lcd->clear();
	char l1[17] = { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 0x00 };
	char l2[17] = { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 0x00 };
	for (int i = 0; i < 17 && Display.Lines.Line0[i] != 0; i++) {
		l1[i] = Display.Lines.Line0[i];
	}
	for (int i = 0; i < 17 && Display.Lines.Line1[i] != 0; i++) {
		l2[i] = Display.Lines.Line1[i];
	}
	lcd->setCursor(0, 0);
	lcd->print(l1);
	lcd->setCursor(0, 1);
	lcd->print(l2);
}

void Environment::rising_event(event_type_t event_type)
{
	Root->event(event_type);
}

#pragma region HW_BUTTON_INTERRUPT_HANDLERS

void Environment::btn_ok_int_handler(bool pinstate) {
	noInterrupts();
	if (pinstate) {
		if (!_debounceFilter) {
#ifdef DEBUGBTNS
			Serial.println("BTN!");
#endif
			env.rising_event(EVT_BTN_OK);
			_debounceFilter++;
		}
	}
	
	_okIsPressed = pinstate;
	interrupts();
}

void Environment::btn_cancel_int_handler(bool pinstate) {
	noInterrupts();
	if (pinstate) {
		if (!_debounceFilter) {
#ifdef DEBUGBTNS
			Serial.println("cancel!");
#endif
			env.rising_event(EVT_BTN_CL);
			_debounceFilter++;
		}
	}
	_cancelIsPressed = pinstate;
	interrupts();
}

void Environment::up_int_handler() {
	noInterrupts();
	if (!digitalRead(HW_UP_INT)) {
		if (!digitalRead(HW_UP_REV)) {
			if (!_upDownDebounce) {
#ifdef DEBUGBTNS
				Serial.println("UP!");
#endif
				env.rising_event(EVT_BTN_UP);
				_upDownDebounce++;
			}
		}
		else {
			if (!_upDownDebounce) {
#ifdef DEBUGBTNS
				Serial.println("DOWN!");
#endif
				env.rising_event(EVT_BTN_DN);
				_upDownDebounce++;
			}
		}
	}
	interrupts();
}

void Environment::right_int_handler() {
	noInterrupts();
	if (!digitalRead(HW_RIGHT_INT)) {
		if (!digitalRead(HW_RIGHT_REV)) {
			if (!_leftRightDebounce) {
#ifdef DEBUGBTNS
				Serial.println("Right!");
#endif
				env.rising_event(EVT_BTN_RI);
				_leftRightDebounce++;
			}
		}
		else {

			if (!_leftRightDebounce) {
#ifdef DEBUGBTNS
				Serial.println("Left!");
#endif
				env.rising_event(EVT_BTN_LF);
				_leftRightDebounce++;
			}
		}
	}
	interrupts();
}

#pragma endregion
