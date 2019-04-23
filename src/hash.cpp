#include <Arduino.h>
#include <SHA512.h>

class HashHandler {
	private:
		class SHA512 sha512;
		size_t msg_size;
		String signature;
		//char buffer[hash_size+1];

	public: String signMessage(String message, const char* apiKey) {
		char msg[msg_size];
		message.toCharArray(msg, msg_size);
		
		sha512.update(msg, strlen(msg));
		sha512.update(apiKey, strlen(apiKey));

		size_t hash_size = sha512.hashSize();
		uint8_t hash[hash_size];

		sha512.finalize(&hash, hash_size);

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