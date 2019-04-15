#include <Arduino.h>
#include <SHA512.h>

char* getMessageSignature(char* message, const char* apiKey) {
	class SHA512 sha512;
	
	sha512.update(message, strlen(message));
	sha512.update(apiKey, strlen(apiKey));

	size_t hash_size = sha512.hashSize();
	uint8_t hash[hash_size];

	sha512.finalize(&hash, hash_size);

	char* string = (char*) malloc(sizeof(char)*hash_size+1);

	for (size_t i = 0; i < hash_size; i++) {
		char str[2];
		sprintf(str, "%02x", hash[i]);
		if (i == 0) {
			strcpy(string, str);
		} else {
			strcat(string, str);
		}
	}

	return string;
}