#pragma once
#include <stdbool.h>
#include <stdint.h>

#include <Arduino.h>

class delegates {
public:
	typedef void(*callback)(bool pinstate);
};

class ButtonState {
	enum {
		OnPress,
		OnRelease,
		OnLongPress
	};
};

template <uint8_t pin, bool UpLevel = true>
class Pin {
public:
	enum PinChange
	{
		PcTurnOn = 0,
		PcTurnOff = 1,
		PcChanged = 2,
		PcIsOn = 3,
		PcIsOff = 4
	};

	static void On() {
		if (UpLevel == true) digitalWrite(pin, HIGH);
		else digitalWrite(pin, LOW);
	}

	static void Off() {
		if (UpLevel == true) digitalWrite(pin, LOW);
		else digitalWrite(pin, HIGH);
	}

	static void Mode(uint8_t r) {
		pinMode(pin, r);
	}

	static bool IsOn() {
		if (UpLevel) return digitalRead(pin);
		else return !digitalRead(pin);
	}

	static bool IsOff() {
		return !IsOn();
	}

	Pin(bool def = false) {
		Prev = def;
	}

	static uint8_t GetPinNumber() {
		return pin;
	}

	bool Attach(PinChange ch, delegates::callback c = 0) {
		bool current = IsOn();
		bool rv = false;
		switch (ch)
		{
		case PcChanged:
			if (Prev != current) {
				if(c) c(current);
				rv = true;
			}
			break;

		case PcTurnOff:
			if (Prev != current && Prev) {
				if (c) c(current);
				rv = true;
			}
			break;

		case PcTurnOn:
			if (Prev != current && !Prev) {
				if (c) c(current);
				rv = true;
			}
			break;

		case PcIsOn:
			if (current) {
				if (c) c(current);
				rv = true;
			}
			break;

		case PcIsOff:
			if (!current) {
				if (c) c(current);
				rv = true;
			}
			break;

		default:
			break;
		}
		Prev = current;
		return rv;
	}
private:
	bool Prev;
};



