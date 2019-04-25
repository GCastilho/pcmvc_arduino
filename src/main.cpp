#include <Arduino.h>
#include "telemetryProject.h"
#include "network.cpp"
#include "anem.cpp"
#include "jsonHandler.cpp"
#include "hash.cpp"

volatile unsigned int Anemometro::counter;
void rtcSetup();
float getRtcTemperature();
uint32_t getEpoch32Time();
NetworkManager* connection;
Anemometro* anem;
JsonHandler* json;

const char apiKey[] = "alsdkhf9432ksledhfasidfaskdjhf";
const char RA[] = "2761234567890";
const char latitude[] = "-22.8044635";
const char longitude[] = "-47.3158102";
const uint8_t tArraySize = 1;

void setup() {
	// Initialize serial if connected
	if(Serial) {
		Serial.begin(9600);
	}
	Serial.println("START SETUP");
	rtcSetup();
	//connection = new NetworkManager();
	anem = new Anemometro();
	json = new JsonHandler();

	//connection.post(message);
	Serial.println("END SETUP");
}

void loop() {
	struct telemetry telemetryArray[tArraySize];
	for (size_t i = 0; i < tArraySize; i++) {
		Serial.print("Starting measurement...");

		telemetryArray[i].windVelocity = anem->getWindVelocity();
		telemetryArray[i].timestamp = getEpoch32Time();

		Serial.println(" finished.");
		Serial.print("Timestamp: ");
		Serial.print(telemetryArray[i].timestamp);
		Serial.print("; Wind speed: ");
		Serial.print(telemetryArray[i].windVelocity);
		Serial.println(" [m/s]");
	}
	// Cria o json para enviar ao servidor
	String postData;
	{
		String postBuffer[2];
		postBuffer[0] = json->buildMessage(RA, latitude, longitude, telemetryArray, tArraySize);
		Serial.print("message: ");
		Serial.println(postBuffer[0]);

		postBuffer[1] = HashHandler::signMessage(postBuffer[0], apiKey);
		Serial.print("signature: ");
		Serial.println(postBuffer[1]);

		postData = json->buildPostData(postBuffer[0], postBuffer[1]);
	}
	Serial.println(postData);
}