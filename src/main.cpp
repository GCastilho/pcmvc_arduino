#include <Arduino.h>
#include "network.cpp"
#include "anem.cpp"
#include "jsonHandler.cpp"
#include "hash.cpp"

volatile unsigned int Anemometro::counter;
float windVelocity;
NetworkManager* connection;
Anemometro* anem;

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
	connection = new NetworkManager();
	anem = new Anemometro();
	Serial.println("END SETUP");
}

void loop() {
	Serial.print("Starting measurement...");
	windVelocity = anem->getWindVelocity();
	Serial.println(" finished.");
	Serial.print("Wind speed: ");
	Serial.print(windVelocity);
	Serial.println(" [m/s]");

	// Cria o json para enviar ao servidor
	String postData;
	{
		String postBuffer[2];
		postBuffer[0] = JSON::buildMessage(RA, latitude, longitude, windVelocity);
		Serial.print("message: ");
		Serial.println(postBuffer[0]);

		postBuffer[1] = HashHandler::signMessage(postBuffer[0], apiKey);
		Serial.print("signature: ");
		Serial.println(postBuffer[1]);

		postData = JSON::buildPostData(&postBuffer[0], &postBuffer[1]);
	}
	Serial.println(postData);

	//Envia o json para o servidor
	//connection->post(&postData);
}