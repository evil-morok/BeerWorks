/*
 Name:		Beerworks.ino
 Created:	13.04.2018 11:53:42
 Author:	RogovAk
*/


#include <EEPROM.h>
#include <RTCx.h>
#include <OneWire.h>
#include <YetAnotherPcInt.h>
#include <PinChangeInterruptBoards.h>
#include "hw.h"
#include "Environment.h"

// the setup function runs once when you press reset or power the board


void setup() {
	env.Start();
}



// the loop function runs over and over again until power down or reset
void loop() {
	env.Main();
}
