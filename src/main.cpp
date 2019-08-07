#include <Arduino.h>
#include <SHA256.h>
#include "network.cpp"
#include "anem.cpp"

volatile unsigned int Anemometro::counter;
float windVelocity;
NetworkManager* connection;
Anemometro* anem;

const char apiKey[] = "edhfasidfaskdjalsdkhf9432kslhf";
const char matricula[] = "2760000000000";
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

	char* message;
	{
		// Calcula tamanho necessário para o char do message
		int size;
		{
			int fixed_size = 1+9+1+12+1+6+1+6+1+6+1+7+1;
			int ra = strlen(matricula);
			int lat = strlen(latitude);
			int lon = strlen(longitude);
			int hgt = 5;		// Suporta até 99.99 metros
			int wind = 5;		// Suporta até 99.99 m/s
			int pipe = 1;

			size = fixed_size+ra+lat+lon+hgt+wind+pipe;
		}
		// Aloca espaço para o message
		message = (char*)malloc(sizeof(char)*(size+1));

		// Cria o JSON object message
		strcpy(message, "{");						// 1 caractere

		strcat(message, "\"ver\":1.0");				// 9
		strcat(message, ",");						// 1

		strcat(message, "\"matricula\":");			// 12
		strcat(message, matricula);					// strlen()
		strcat(message, ",");						// 1

		strcat(message, "\"lat\":");				// 6
		strcat(message, latitude);					// strlen()
		strcat(message, ",");						// 1

		strcat(message, "\"lon\":");				// 6
		strcat(message, longitude);					// strlen()
		strcat(message, ",");						// 1

		strcat(message, "\"hgt\":");				// 6
		{
			char buffer[6];							// Buffer big enough for 5-character float
			dtostrf(height, 5, 2, buffer);			// Leave room for too large numbers!
			strcat(message, buffer);				// hgt: 5
		}
		strcat(message, ",");						// 1

		strcat(message, "\"wind\":");				// 7
		{
			char buffer[6];							// Buffer big enough for 6-character float
			dtostrf(windVelocity, 5, 2, buffer);	// Leave room for too large numbers!
			strcat(message, buffer);				// wind: 5
		}

		strcat(message, "}");						// 1
	}

	char* signature;
	{
		// Aloca espaço para o signature
		// Um sha256 ocupa 64 caracteres ao ser convertido para string
		signature = (char*)malloc(sizeof(char)*(64+1));

		// Cria o array signature; signature é um sha256 com hash_size de 32 bytes
		uint8_t raw_signature[32];

		// Cria um sha256 da mensagem concatenada com a apiKey (signature)
		{
			class SHA256 sha256;

			sha256.update(message, strlen(message));
			sha256.update(apiKey, strlen(apiKey));

			sha256.finalize(raw_signature, 32);
		}

		// Concatena a assinatura
		for (size_t i = 0; i < 32; i++) {
			char str[2];
			sprintf(str, "%02x", raw_signature[i]);
			strcat(signature, str);
		}
	}

	Serial.println("Done");

	Serial.print("Message: ");
	Serial.print(message);

	Serial.print("Signature: ");
	Serial.print(signature);

	// Envia 'message' e 'signature' para o servidor
	connection->post(message, signature);

	// Libera 'signature' e 'message' da memória
	free(signature);
	free(message);
}
