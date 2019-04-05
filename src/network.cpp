#include <SPI.h>
#include <Ethernet.h>

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
IPAddress server(192,168,97,164);	// numeric IP for Server (no DNS)
//char server[] = "www.google.com";	// name address for Server (using DNS)

// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 0, 177);
IPAddress myDns(192, 168, 0, 1);

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;

// Variables to measure the speed
unsigned long beginMicros, endMicros;
unsigned long byteCount = 0;
bool printWebData = true;  // set to false for better speed measurement

void networkSetup() {
	// start the Ethernet connection:
	Serial.println("Initializing Ethernet with DHCP:");
	if (Ethernet.begin(mac)) {
		Serial.print("  DHCP assigned IP ");
		Serial.println(Ethernet.localIP());
		delay(1000);	// give the Ethernet shield a second to initialize
	} else {
		Serial.println("Failed to configure Ethernet using DHCP");
		// Check for Ethernet hardware present
		if (Ethernet.hardwareStatus() == EthernetNoHardware) {
			Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
			while (true) {
				delay(1); // do nothing, no point running without Ethernet hardware
			}
		}
		if (Ethernet.linkStatus() == LinkOFF) {
			Serial.println("Ethernet cable is not connected.");
			while(true){
				delay(1); // do nothing
			}
		}
		// try to congifure using IP address instead of DHCP:
		Serial.println("Warning: trying to configure internet without DHCP");
		Ethernet.begin(mac, ip, myDns);
	}
}

void postToServer(String postData) {
	Serial.print("connecting to ");
	Serial.print(server);
	Serial.println("...");

	// if you get a connection, report back via serial:
	if (client.connect(server, 80)) {
		Serial.print("connected to ");
		Serial.println(client.remoteIP());
		Serial.println();

		// Make a HTTP POST request:
		client.println("POST /telemetryProject/server/telemetry.php HTTP/1.1");
		client.print("Host: ");
		client.println(server);
		client.println("User-Agent: Arduino/1.0");
		client.println("Connection: close");
		client.print("Content-Length: ");
		client.println(postData.length());
		client.println();
		client.println(postData);
	} else {
		// if you didn't get a connection to the server:
		Serial.println("connection failed");
	}
	beginMicros = micros();
}

void networkLoop() {
	// if there are incoming bytes available
	// from the server, read them and print them:
	int len = client.available();
	if (len > 0) {
		byte buffer[80];
		if (len > 80) len = 80;
		client.read(buffer, len);
		if (printWebData) {
			Serial.write(buffer, len); // show in the serial monitor (slows some boards)
		}
		byteCount = byteCount + len;
	}

	// if the server's disconnected, stop the client:
	if (!client.connected()) {
		endMicros = micros();
		Serial.println();
		Serial.println("disconnecting.");
		client.stop();
		Serial.print("Received ");
		Serial.print(byteCount);
		Serial.print(" bytes in ");
		float seconds = (float)(endMicros - beginMicros) / 1000000.0;
		Serial.print(seconds, 4);
		float rate = (float)byteCount / seconds / 1000.0;
		Serial.print(", rate = ");
		Serial.print(rate);
		Serial.print(" kbytes/second");
		Serial.println();

		// do nothing forevermore:
		while (true) {
			delay(1);
		}
	}
}