#include "telemetryProject.h"
#include <Arduino.h>

char* buildJsonProperty(char* property, char* value) {
	const int property_len = strlen(property);
	const int value_len = strlen(value);
	int json_chars;

	bool onlyNumber = true;
	{
		for (int i = 0; i < value_len; ++i) {
			if (!isdigit(value[i]) && value[i] != '.' && value[i] != '-') {
				//Se começa com { ou [ presume q é um jArray ou jObj e pula a análise
				if (value[i] == '{' || value[i] == '[') break;
				onlyNumber = false;
				break;
			}
		}
		if (onlyNumber) {
			json_chars = 3;
		} else {
			json_chars = 5;
		}
	}

	char* json_property = (char*) malloc(sizeof(char) * (property_len+value_len+json_chars+1));

	strcpy(json_property, "\"");
	strcat(json_property, property);
	strcat(json_property, "\":");
	if (onlyNumber) {
		strcat(json_property, value);
	} else {
		strcat(json_property, "\"");
		strcat(json_property, value);
		strcat(json_property, "\"");
	}
	
	return json_property;
}

char* _buildJsonItem(char** data, size_t data_count, int item) {
	// item = 0: jsonObject; item != 0: jsonArray
	//Soma o length de todos os itens de data[] 
	size_t data_total_size = 0;
	for (size_t i = 0; i < data_count; i++) {
		data_total_size += strlen(data[i]);
	}
	size_t separators = data_count-1;
	size_t delimiters = 2;
	
	char* json_item = (char*) malloc(sizeof(char) * (data_total_size+separators+delimiters+1));

	if (item == 0) strcpy(json_item, "{"); else strcpy(json_item, "[");
	for (size_t i = 0; i < data_count; i++) {
		strcat(json_item, data[i]);
		if (i < data_count-1) {
			strcat(json_item, ",");
		}
	}
	if (item == 0) strcat(json_item, "}"); else strcat(json_item, "]");
	
	return json_item;
}

char* buildJsonObject(char** data, size_t data_count) {
	return _buildJsonItem(data, data_count, 0);
}

char* buildJsonArray(char** data, size_t data_count) {
	return _buildJsonItem(data, data_count, 1);
}

char* buildMessage(const char* RA, const char* latitude, const char* longitude, struct telemetry telemetryArray[], size_t arr_len) {
	char* data_array_buffer[arr_len];

	for (size_t i = 0; i < arr_len; i++) {
		char timestampBuff[10+1];
		sprintf(timestampBuff, "%lu", telemetryArray[i].timestamp);

		char* timestamp_prop = buildJsonProperty("timestamp", timestampBuff);

		char windBuff[10+1];
		dtostrf(telemetryArray[i].windVelocity, 6, 4, windBuff); 

		char* windVelocity_prop = buildJsonProperty("windVelocity", windBuff);
		char* buffer[] = { timestamp_prop, windVelocity_prop };

		data_array_buffer[i] = buildJsonObject(buffer, 2);

		free(timestamp_prop);
		free(windVelocity_prop);
	}
	
	char* data_array = buildJsonArray(data_array_buffer, arr_len);
	char* data_prop = buildJsonProperty("data", data_array);
	free(data_array);

	char* latitude_prop = buildJsonProperty("latitude", (char*)latitude);
	char* longitude_prop = buildJsonProperty("longitude", (char*)longitude);
	char* telemetry_buffer[] = { latitude_prop, longitude_prop, data_prop};

	char* telemetry_obj = buildJsonObject(telemetry_buffer, 3);
	free(latitude_prop);
	free(longitude_prop);
	free(data_prop);

	char* version_prop = buildJsonProperty("version", "0.2");
	char* ra_prop = buildJsonProperty("RA", (char*)RA);
	char* telemetry_prop = buildJsonProperty("telemetry", telemetry_obj);
	free(telemetry_obj);

	char* message_buffer[] = { version_prop, ra_prop, telemetry_prop };
	char* message_obj = buildJsonObject(message_buffer, 3);
	free(version_prop);
	free(ra_prop);
	free(telemetry_prop);

	return message_obj;
}

char* buildPostData(char* message, char* signature) {
	char* signature_prop = buildJsonProperty("signature", signature);
	char* message_prop = buildJsonProperty("message", message);
	char* post_buffer[] = { message_prop, signature_prop };

	char* post_obj = buildJsonObject(post_buffer, 2);
	free(message_prop);
	free(signature_prop);

	return post_obj;
}