#pragma once

#include <Wire.h>
#include <RTCx.h>
#include "Sequencer.h"

enum RTCSequences {
	RtcSequenceStart,
	RtcSequenceInit,
	RtcSequenceRead,
	RtcSequenceStop
};

#define RTC_CYCLES_DELAY_MSEC 100

template <class functorType>
class RTC : public Sequencer<RTCSequences, RTC<functorType>, RtcSequenceStop, RtcSequenceStart, RtcSequenceStop> {
public:
	typedef void (functorType::*Delegate)();
private:
	
	bool _Inited = false;
	uint32_t _LastMsec;
	Delegate _OnSecondDelegate;
	uint8_t _LastSecond;
	functorType *_Functor;

public:
	struct RTCx::tm Time;

	RTC(functorType *functor, Delegate onSecond) {
		AddSequence(RtcSequenceStart, RtcSequenceInit);
		AddSequence(RtcSequenceInit, RtcSequenceRead, &RTC::_RtcSequenceInit);
		AddSequence(RtcSequenceRead, RtcSequenceStop, &RTC::_RtcSequenceRead);
		_Inited = true;
		_LastMsec = millis();
		_OnSecondDelegate = onSecond;
		_Functor = functor;
	}

	void Run() {
		if (!_Inited) return;
		if (millis() - _LastMsec > RTC_CYCLES_DELAY_MSEC) {
			_LastMsec = millis();
			Next();
			if (_LastSecond != Time.tm_sec) {
				_LastSecond = Time.tm_sec;
				(_Functor->*_OnSecondDelegate)();
			}
		}
	}
private:
	void _RtcSequenceInit() {
		Serial.println("Search for RTC");
		if (!rtc.autoprobe()) {
			SetSequence(RtcSequenceInit);
		}
		else {
			Serial.print("Autoprobe found ");
			Serial.print(rtc.getDeviceName());
			Serial.print(" at 0x");
			Serial.println(rtc.getAddress(), HEX);
			rtc.enableBatteryBackup();
			rtc.startClock();
			rtc.setSQW(RTCx::freq4096Hz);
		}
	}

	void _RtcSequenceRead() {
		rtc.readClock(Time);

		//RTCx::printIsotime(Serial, tm).println();
		//RTCx::time_t t = RTCx::mktime(&tm);

		//Serial.print("unixtime = ");
		//Serial.println(t);
		//Serial.println("-----");
	}
};