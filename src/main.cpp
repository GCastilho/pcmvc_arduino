#include <Arduino.h>
#include <SHA256.h>
#include "network.cpp"
#include "anem.cpp"

volatile unsigned int Anemometro::counter;
float windVelocity;
NetworkManager* connection;
Anemometro* anem;

const char apiKey[] = "alsdkhf9432ksledhfasidfaskdjhf";
const char RA[] = "2761234567890";
const char latitude[] = "-22.8044635";
const char longitude[] = "-47.3158102";
const float height = 2.0;

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
	Serial.print("Building message to send to server... ");
	char* postData;
	{
		// Calcula tamanho necessário para o char do postData
		int size;
		{
			int fixed_size = 1+13+1+5+1+6+1+6+1+6+1+7+1;
			int ra = strlen(RA);
			int lat = strlen(latitude);
			int lon = strlen(longitude);
			int hgt = 4;
			int wind = 4;
			int pipe = 1;
			int sha_size = 64;

			size = fixed_size+ra+lat+lon+hgt+wind+pipe+sha_size;
		}
		// Aloca espaço para o postData
		postData = (char*)malloc(sizeof(char)*(size+1));

		// Aponta o char array 'message' para o mesmo endereço que postData
		char* message = postData;

		// Cria o JSON object message
		strcpy(message, "{");						// 1 caractere

		strcat(message, "\"version\":0.4");			// 13
		strcat(message, ",");						// 1

		strcat(message, "\"RA\":");					// 5
		strcat(message, RA);						// strlen()
		strcat(message, ",");						// 1

		strcat(message, "\"lat\":");				// 6
		strcat(message, latitude);					// strlen()
		strcat(message, ",");						// 1

		strcat(message, "\"lon\":");				// 6
		strcat(message, longitude);					// strlen()
		strcat(message, ",");						// 1

		strcat(message, "\"hgt\":");				// 6
		{
			char buffer[5];							// Buffer big enough for 4-character float
			dtostrf(height, 4, 2, buffer);			// Leave room for too large numbers!
			strcat(message, buffer);				// 4
		}
		strcat(message, ",");						// 1

		strcat(message, "\"wind\":");				// 7
		{
			char buffer[5];							// Buffer big enough for 4-character float
			dtostrf(windVelocity, 4, 2, buffer);	// Leave room for too large numbers!
			strcat(message, buffer);				// 4
		}

		strcat(message, "}");						// 1

		// Cria o array signature; signature é um sha256 com hash_size de 32 bytes
		uint8_t signature[32];

		// Cria um sha256 da mensagem concatenada com a apiKey (signature)
		{
			class SHA256 sha256;

			sha256.update(message, strlen(message));
			sha256.update(apiKey, strlen(apiKey));

			sha256.finalize(signature, 32);
		}

		// Conctena o pipe para separar a mensagem da assinatura
		strcat(postData, "|");

		// Concatena a assinatura
		for (size_t i = 0; i < 32; i++) {
			char str[2];
			sprintf(str, "%02x", signature[i]);
			strcat(postData, str);
		}
	}
	Serial.println("Done");

	// Envia 'postData' para o servidor
	connection->post(postData);

	// 'postData' é liberado da memória em connection->post()
}