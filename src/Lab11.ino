#include "Particle.h"
#include "oled-wing-adafruit.h"
#include "LIS3DH.h"

// Example using position detection feature of the LIS3DH and SPI connection, like the AssetTracker
SYSTEM_MODE(MANUAL);
SYSTEM_THREAD(ENABLED);

const unsigned long PRINT_SAMPLE_PERIOD = 100;

void positionInterruptHandler();

// LIS3DH is connected as in the AssetTracker, to the primary SPI with A2 as the CS (SS) pin, and INT connected to WKP
LIS3DHSPI accel(SPI, D3, WKP);
OledWingAdafruit display;
unsigned long lastPrintSample = 0;

volatile bool positionInterrupt = false;
uint8_t lastPos = 0;

void setup() {
	Serial.begin(9600);
  if (!Serial.isConnected()) {}
  display.setup();

	attachInterrupt(WKP, positionInterruptHandler, RISING);

display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.display();
	
	// Initialize sensors
	LIS3DHConfig config;
	config.setPositionInterrupt(16);

	bool setupSuccess = accel.setup(config);
	Serial.printlnf("setupSuccess=%d", setupSuccess);
}

void loop() {
	LIS3DHSample sample;
display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
	if (millis() - lastPrintSample >= PRINT_SAMPLE_PERIOD) {
		lastPrintSample = millis();
		if (accel.getSample(sample)) {
			display.clearDisplay();
      display.println("X: " + String(sample.x));
      display.println("Y: " + String(sample.y));
      display.println("Z: " + String(sample.z));
		}
		else {
			display.println("no sample");
		}
	}
	if (positionInterrupt) {
		positionInterrupt = false;

		// Test the position interrupt support. Normal result is 5.
		// 5: normal position, with the accerometer facing up
		// 4: upside down
		// 1 - 3: other orientations
		uint8_t pos = accel.readPositionInterrupt();
		if (pos != 0 && pos != lastPos) {
			Serial.printlnf("pos=%d", pos);
			lastPos = pos;
		}
	}
  display.display();
}

void positionInterruptHandler() {
	positionInterrupt = true;
}

