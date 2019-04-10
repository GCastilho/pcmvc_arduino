#include <Ethernet.h>

class NetworkManager {
	private:
		// Enter a MAC address for your controller below.
		// Newer Ethernet shields have a MAC address printed on a sticker on the shield
		byte mac[6] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

		// Name address for Server 
		char server[15] = "192.168.97.164";

		// Initialize the Ethernet client library
		// with the IP address and port of the server
		// that you want to connect to (port 80 is default for HTTP):
		class EthernetClient client;

		// Variables to measure the speed
		unsigned long beginMicros, endMicros;
		unsigned long byteCount = 0;
		bool printWebData = true;	// Set to true to process and print server response
	
	public: NetworkManager() {
		// Set the static IP address to use if the DHCP fails to assign
		class IPAddress ip(192, 168, 0, 177);
		class IPAddress myDns(8, 8, 8, 8);

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
			// try to configure using IP address instead of DHCP:
			Serial.println("Warning: trying to configure internet without DHCP");
			Ethernet.begin(mac, ip, myDns);
		}
	}

	public: void post(char *postData) {
		Serial.print("connecting to ");
		Serial.print(server);
		Serial.println("...");

		// Try to connect to the server
		if (client.connect(server, 80)) {
			Serial.print("connected to ");
			Serial.println(client.remoteIP());

			// Make a HTTP POST request:
			Serial.println("Sending POST data...");
			Serial.println();
			client.println("POST /telemetryProject/server/telemetry.php HTTP/1.1");
			client.print("Host: ");
			client.println(server);
			client.println("User-Agent: Arduino/1.0");
			client.println("Connection: close");
			client.print("Content-Length: ");
			client.println(strlen(postData));
			client.println();
			client.println(postData);
			beginMicros = micros();
		} else {
			Serial.println("connection failed");
			return;
		}

		// Read and print incoming bytes available from the server, if any
		if (printWebData) {
			Serial.println("Printing server response...");
			while(client.connected()){
				int len = client.available();
				if (len > 0) {
					byte buffer[80];
					if (len > 80) len = 80;
					client.read(buffer, len);
					Serial.write(buffer, len);
					byteCount = byteCount + len;
				}
			}
			//Print approximate connection velocity
			endMicros = micros();
			Serial.println("\n");
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
		} else {
			Serial.println("Printing response disabled, skipping");
			Serial.println();
		}
		Serial.println("disconnecting...");
		client.stop();
	}
};