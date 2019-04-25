#include "telemetryProject.h"
#include <Arduino.h>

class JsonHandler {
	private:
		String buildJsonProperty(String property, String value) {
			String json_property;

			bool onlyNumber = true;
			for (uint8_t i = 0; i < value.length(); ++i) {
				if (!isdigit(value[i]) && value[i] != '.' && value[i] != '-') {
					//Se começa com { ou [ presume q é um jArray ou jObj e pula a análise
					if (value[i] == '{' || value[i] == '[') break;
					onlyNumber = false;
					break;
				}
			}

			json_property = ("\"" + property + "\":");
			if (onlyNumber) {
				json_property.concat(value);
			} else {
				json_property += ("\"" + value + "\"");
			}

			return json_property;
		}

		String buildJsonItem(String data[], uint8_t dataSize, uint8_t item) {
			// item = 0: jsonObject; item != 0: jsonArray
			String json_item;

			if (item == 0) json_item.concat("{"); else json_item.concat("[");
			for (uint8_t i = 0; i < dataSize; i++) {
				json_item.concat(data[i]);
				if (i < dataSize-1) {
					json_item.concat(",");
				}
			}
			if(item == 0) json_item.concat("}"); else json_item.concat("]");

			return json_item;
		}

		String buildJsonObject(String data[], uint8_t dataSize) {
			return buildJsonItem(data, dataSize, 0);
		}

		String buildJsonArray(String data[], uint8_t dataSize) {
			return buildJsonItem(data, dataSize, 1);
		}
	
	public:
		String buildMessage(const char* RA,
							const char* latitude,
							const char* longitude,
							struct telemetry telemetryArray[],
							size_t arr_len) {
			// Cria o objeto message
			String message_obj;
			{
				// Cria o objeto telemetria
				String telemetry_obj;
				{
					// Cria o data property
					String data_prop;
					{
						// Cria o data array
						String data_array;
						{
							String data_array_buffer[arr_len];
							for (uint8_t i = 0; i < arr_len; i++) {
								String strBuff;

								strBuff = String(telemetryArray[i].timestamp);
								String timestamp_prop = buildJsonProperty("timestamp", strBuff);

								strBuff = String(telemetryArray[i].windVelocity);
								String windVelocity_prop = buildJsonProperty("windVelocity", strBuff);

								String buffer[] = {timestamp_prop, windVelocity_prop};
								data_array_buffer[i] = buildJsonObject(buffer, 2);
							}
							data_array = buildJsonArray(data_array_buffer, arr_len);
						}
						data_prop = buildJsonProperty("data", data_array);
					}
					String latitude_prop = buildJsonProperty("latitude", latitude);
					String longitude_prop = buildJsonProperty("longitude", longitude);
					String buffer[] = { latitude_prop, longitude_prop, data_prop};

					telemetry_obj = buildJsonObject(buffer, 3);
				}
				String message_buffer[3];
				message_buffer[0] = buildJsonProperty("version", "0.2");
				message_buffer[1] = buildJsonProperty("RA", RA);
				message_buffer[2] = buildJsonProperty("telemetry", telemetry_obj);

				message_obj = buildJsonObject(message_buffer, 3);
			}

			return message_obj;
		}

		String buildPostData(String message, String signature) {
			String post_obj;
			{
				String prop_buffer[2];
				prop_buffer[0] = buildJsonProperty("message", message);
				prop_buffer[1] = buildJsonProperty("signature", signature);

				post_obj = buildJsonObject(prop_buffer, 2);
			}

			return post_obj;
		}
};