#include <Arduino.h>
#include "telemetryProject.h"
#include "network.cpp"

char* buildMessage(const char*, float, float, struct telemetry[], size_t);
char* getMessageSignature(char*, const char*);
char* buildPostData(char*, char*);

const char apiKey[] = "alsdkhf9432ksledhfasidfaskdjhf";
const char RA[] = "2761234567890";
const float latitude = -22.8044635;
const float longitude = -47.3158102;

void setup() {
	// Initialize serial if connected
	if(Serial) {
		Serial.begin(9600);
	}
	Serial.println("START");

	struct telemetry telemetryArray[2];
	telemetryArray[0].timestamp = 1552146530;
	telemetryArray[0].windVelocity = 64;
	telemetryArray[1].timestamp = 1552148940;
	telemetryArray[1].windVelocity = 44;

	//TODO: Fixed array size of 30 (or less)
	char* message = buildMessage(RA, latitude, longitude, telemetryArray, sizeof(telemetryArray) / sizeof(*telemetryArray));
	Serial.println(message);
	free(message);

	char postData[] = "{\"version\": 0.1,\"RA\": 2761234567890,\"telemetry\": [{\"timestamp\": 1552146530,\"latitude\": -22.8044635,\"longitude\": -47.3158102,\"windVelocity\": 64},{\"timestamp\": 1552148940,\"latitude\": -22.8044635,\"longitude\": -47.3158102,\"windVelocity\": 44}]}";

	connection.post(postData);
	Serial.println("END");
}

void loop() {
	// do nothing forevermore:
	while (true) {
		delay(1);
	}
}