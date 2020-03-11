#pragma once

template <class Pin>
class Thermo {

public:
	
	static void UpdateTemp(float currentTemperature) {
		_CurrentTemperature = currentTemperature;
		_PrevUpdate = 0;
		auto d = _TargetTemperature > _CurrentTemperature ? _TargetTemperature - _CurrentTemperature : 0;
		_PowerPercent = d < 1 ? 100 * d : 100;
		if (_PowerPercent > _MaxPower) _PowerPercent = _MaxPower;
	}

	static void SetTemperature(float Temperature) {
		_TargetTemperature = Temperature;
	}

	static void SetMaxPower(uint8_t MaxPower) {
		_MaxPower = MaxPower;
	}

	static bool Run() {
		uint32_t m = millis();
		uint32_t d;
		if (m >= _PrevMillis) {
			d = m - _PrevMillis;
		} else {
			d = m + (4294967295 - _PrevMillis);
		}
		_PrevMillis = m;
		_PrevUpdate += d;
		_IsOk = _PrevUpdate < MaxUpdateIntervalMsec;
		if (_IsOk) {
			return _PowerCycle(d);
		} else {
			Pin::Off();
			return false;
		}
	}
private:
	static bool _PowerCycle(uint16_t msec) {
		if (_PowerOnTimeMsec > msec) {
			Pin::On();
			_PowerOnTimeMsec -= msec;
			_PowerIsOn = true;
		}
		else {
			_PowerOnTimeMsec = 0;
			if (_PowerOffTimeMsec > msec) {
				Pin::Off();
				_PowerOffTimeMsec -= msec;
				_PowerIsOn = false;
			}
			else {
				_PowerOnTimeMsec = CyclePeriodMsec / 100 * _PowerPercent;
				_PowerOffTimeMsec = CyclePeriodMsec / 100 * (100 - _PowerPercent);
			}
		}
		return _PowerIsOn;
	}

private:
	static const uint16_t CyclePeriodMsec = 1 * 1000;
	static const uint16_t MaxUpdateIntervalMsec = 2 * 1000;
	static float _TargetTemperature;
	static float _CurrentTemperature;
	static uint32_t _PrevMillis;
	static uint16_t _PrevUpdate;
	static uint8_t _PowerPercent;
	static uint8_t _MaxPower;
	static uint16_t _PowerOnTimeMsec;
	static uint16_t _PowerOffTimeMsec;
	static bool _IsOk;
	static bool _PowerIsOn;
};

template<class Pin> float Thermo<Pin>::_TargetTemperature;
template<class Pin> float Thermo<Pin>::_CurrentTemperature;
template<class Pin> uint32_t Thermo<Pin>::_PrevMillis;
template<class Pin> uint16_t Thermo<Pin>::_PrevUpdate;
template<class Pin> uint16_t Thermo<Pin>::_PowerOffTimeMsec;
template<class Pin> uint16_t Thermo<Pin>::_PowerOnTimeMsec;
template<class Pin> uint8_t Thermo<Pin>::_PowerPercent;
template<class Pin> uint8_t Thermo<Pin>::_MaxPower = 100;
template<class Pin> bool Thermo<Pin>::_IsOk;
template<class Pin> bool Thermo<Pin>::_PowerIsOn;