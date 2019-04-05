#include <Arduino.h>

void networkSetup();
void postToServer(char*);

void setup() {
	// Initialize serial if connected
	if(Serial) {
		Serial.begin(9600);
	}
	networkSetup();

	char postData[] = "{\"version\":0.1,\"RA\":2761234567890,\"telemetry\":[{\"timestamp\":1552146530,\"latitude\":-22.8044635,\"longitude\":-47.3158102,\"windVelocity\":64},{\"timestamp\":1552148940,\"latitude\":-22.8044635,\"longitude\":-47.3158102,\"windVelocity\":44}]}";
	postToServer(postData);
}

void loop() {
	// do nothing forevermore:
	while (true) {
		delay(1);
	}
}