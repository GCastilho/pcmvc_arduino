#include <Arduino.h>
#include "telemetryProject.h"
#include "network.cpp"

char* buildMessage(const char*, const char*, const char*, struct telemetry[], size_t);
char* getMessageSignature(char*, const char*);
char* buildPostData(char*, char*);

const char apiKey[] = "alsdkhf9432ksledhfasidfaskdjhf";
const char RA[] = "2761234567890";
const char latitude[] = "-22.8044635";
const char longitude[] = "-47.3158102";

void setup() {
	// Initialize serial if connected
	if(Serial) {
		Serial.begin(9600);
	}
	Serial.println("START");
	//class NetworkManager connection;

	struct telemetry telemetryArray[2];
	telemetryArray[0].timestamp = 1552146530;
	telemetryArray[0].windVelocity = 64;
	telemetryArray[1].timestamp = 1552148940;
	telemetryArray[1].windVelocity = 44;

	//TODO: Fixed array size
	char* message = buildMessage(RA, latitude, longitude, telemetryArray, sizeof(telemetryArray) / sizeof(*telemetryArray));
	char* signature = getMessageSignature(message, apiKey);
	char* postData = buildPostData(message, signature);
	free(message);
	free(signature);

	Serial.println(postData);

	free(postData);
	//connection.post(message);
	Serial.println("END");
}

void loop() {
	// do nothing forevermore:
	while (true) {
		delay(1);
	}
}