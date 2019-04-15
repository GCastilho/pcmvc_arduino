#include "telemetryProject.h"
#include <ArduinoJson.h>

char* buildMessage(const char* RA, float latitude, float longitude, struct telemetry telemetryArray[], size_t arr_len) {

	// allocate the memory for the document  (a + b + c + d)
	const size_t CAPACITY = JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(3) + JSON_ARRAY_SIZE(30) + 30*JSON_OBJECT_SIZE(2);
	StaticJsonDocument<CAPACITY> message;

	// a ('telemetry' is a property of 'a')
	message["version"] = 0.2;
	message["RA"] = RA;

	// b ('data' is a property of 'b')
	JsonObject telemetry = message.createNestedObject("telemetry");
	telemetry["latitude"] = latitude;
	telemetry["longitude"] = longitude;

	//c
	JsonArray data = telemetry.createNestedArray("data");

	for (size_t i = 0; i < arr_len; i++) {
		JsonObject measurement = data.createNestedObject();
		// d
		measurement["timestamp"] = telemetryArray[i].timestamp;
		measurement["windVelocity"] = telemetryArray[i].windVelocity;
	}

	String serializedData;

	serializeJson(message, serializedData);

	// Length (with one extra character for the null terminator)
	int str_len = serializedData.length() + 1; 

	// Prepare the character array (the buffer) 
	char* serializedMessage = (char*) malloc(sizeof(char)*str_len);

	// Copy it over 
	serializedData.toCharArray(serializedMessage, str_len);

	return serializedMessage;
}