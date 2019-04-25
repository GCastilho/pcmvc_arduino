#include <Arduino.h>

class JSON {
	private: static String buildProperty(String property, String* value) {
		String json_property;

		bool onlyNumber = true;
		//Se começa com { presume q é um jObj e pula a análise
		if (value->operator[](0) != '{') {
			for (uint8_t i = 0; i < value->length(); ++i) {
				if (!isdigit(value->operator[](i))
							&& value->operator[](i) != '.'
							&& value->operator[](i) != '-') {
					onlyNumber = false;
					break;
				}
			}
		}

		json_property = ("\"" + property + "\":");
		if (onlyNumber) {
			json_property.concat(*value);
		} else {
			json_property += ("\"" + *value + "\"");
		}

		return json_property;
	}
	
	public:
		static String buildMessage(const char* RA,
							const char* latitude,
							const char* longitude,
							float windVelocity) {
			// Cria o objeto message
			String message_obj;
			message_obj = "{";
			message_obj.concat(buildProperty("version", &String("0.3")));
			message_obj.concat(",");
			message_obj.concat(buildProperty("RA", &String(RA)));
			message_obj.concat(",");
			message_obj.concat(buildProperty("latitude", &String(latitude)));
			message_obj.concat(",");
			message_obj.concat(buildProperty("longitude", &String(longitude)));
			message_obj.concat(",");
			message_obj.concat(buildProperty("windVelocity", &String(windVelocity)));
			message_obj.concat("}");
			//Serial.println(message_obj);

			return message_obj;
		}

		static String buildPostData(String* message, String* signature) {
			String post_obj;

			post_obj = "{";
			post_obj.concat(buildProperty("message", message));
			post_obj.concat(",");
			post_obj.concat(buildProperty("signature", signature));
			post_obj.concat("}");

			return post_obj;
		}
};