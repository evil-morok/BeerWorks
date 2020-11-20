#pragma once


#define D_T_TICKS 30
#define LEMP_LAG_SEC 120

template <class Pin>
class Thermo {

public:
	
	static void Enable(bool enable) {
		_Enabled = enable;
		if (enable) {
			_FirstUpdate = true;
			_WarmingVelocityCount = 1;
			_WarmingVelocitySum = 50.0f / 60.0f;
		}
		else {
			Pin::Off();
		}
	}

	static inline float dTAvgSec() {
		return _tAvgSum / D_T_TICKS;
	}

	static void UpdateTemp(float currentTemperature) {

		if (!_Enabled) return;
		_CurrentTemperature = currentTemperature;
		_PrevUpdate = 0;

		auto dt = _TargetTemperature - _CurrentTemperature;
		if (_FirstUpdate) {
			_PrevTemp = currentTemperature;
			_tAvgSum = 0;
			_FirstUpdate = false;
		}
		else {
			_tAvgSum -= dTAvgSec();
			_tAvgSum += currentTemperature - _PrevTemp;
			if (dTAvgSec() * 60 >= 2) {
				_WarmingVelocitySum += dTAvgSec();
				_WarmingVelocityCount++;
			}
//			Serial.print(dTAvgSec() * 600);
//			Serial.print(", ");
		}
		_PrevTemp = currentTemperature;
		
		//auto dtt = dTAvgSec() > 0 dTAvgSec() * 60 : 0;
		//auto t =  dt > 0 ?  dt / dtt;
		if (dt > 0) {
			float n = dTAvgSec() > 0 ? dt / dTAvgSec() : LEMP_LAG_SEC;
			float p = 0;
			if (n < LEMP_LAG_SEC) {
				p = 100.0f - (sqrt((LEMP_LAG_SEC - n) / LEMP_LAG_SEC) * 100.0f);
			}
			else {
				p = 100 * dt;
			}
			if (p > 100) { _PowerPercent = 100; }
			else if (p < 0) { _PowerPercent = 0; }
			else { _PowerPercent = p; }

			//_PowerPercent = t * 100 / 60;
		}
		else {
			_PowerPercent = 0;
		}
		
		if (_PowerPercent > _MaxPower) _PowerPercent = _MaxPower;
	}

	static void SetTemperature(float Temperature) {
		if (!_Enabled) return;
		_TargetTemperature = Temperature;
	}

	static void SetMaxPower(uint8_t MaxPower) {
		if (!_Enabled) return;
		_MaxPower = MaxPower;
	}

	static float GetTargetTemperature() {
		return _TargetTemperature;
	}

	static uint8_t GetCurrentPower() {
		if (!_Enabled) return 0;
		return _PowerPercent;
	}

	static bool Run() {
		if (!_Enabled) return false;
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
	static const uint16_t CyclePeriodMsec = .5 * 1000;
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
	static float _PrevTemp;
	static float _tAvgSum;
	static bool _Enabled;
	static bool _FirstUpdate;
	static float _WarmingVelocitySum;
	static uint32_t _WarmingVelocityCount;
};

template<class Pin> float Thermo<Pin>::_PrevTemp;
template<class Pin> float Thermo<Pin>::_tAvgSum;
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
template<class Pin> bool Thermo<Pin>::_Enabled;
template<class Pin> bool Thermo<Pin>::_FirstUpdate;
template<class Pin> float Thermo<Pin>::_WarmingVelocitySum;
template<class Pin> uint32_t Thermo<Pin>::_WarmingVelocityCount;