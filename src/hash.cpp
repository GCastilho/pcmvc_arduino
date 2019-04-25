#include <Arduino.h>
#include <SHA256.h>

class HashHandler {
	public: static String signMessage(String message, const char* apiKey) {
		class SHA256 sha256;

		size_t msg_size = message.length()+1;
		String signature;

		char msg[msg_size];
		message.toCharArray(msg, msg_size);
		
		sha256.update(msg, strlen(msg));
		sha256.update(apiKey, strlen(apiKey));

		size_t hash_size = sha256.hashSize();
		uint8_t hash[hash_size];

		sha256.finalize(&hash, hash_size);

		for (size_t i = 0; i < hash_size; i++) {
			char str[2];
			sprintf(str, "%02x", hash[i]);
			if (i == 0) {
				signature = String(str);
			} else {
				signature.concat(str);
			}
		}

		return signature;
	}
};