#include <Arduino.h>
#define Hall sensor 2	//  Pino digital 2

class Anemometro {
	private:
		// Constants definitions
		const float pi = 3.14159265;	// Numero pi
		const int period = 5000;		// Tempo de medida(miliseconds)
		const int radius = 80;			// Raio do anemometro(mm)
		// Variable definitions
		static volatile unsigned int counter;	// magnet counter for sensor
		unsigned int RPM;			// Revolutions per minute
		float windSpeed;			// Wind speed (m/s)
	
	private: static void addcount() {
		counter++;
	}

	// Set anemometer pins
	public: Anemometro() {
		pinMode(2, INPUT);
		digitalWrite(2, HIGH);	//internall pull-up active
	}

	// Calcula a velocidade média dos últimos {period} milisegundos
	public: float getWindVelocity() {
		counter = 0;
		attachInterrupt(0, addcount, RISING);
		unsigned long startTime = millis();
		while(millis() < startTime + period)

		RPM = ((counter)*60)/(period/1000);
		windSpeed = ((4 * pi * radius * RPM)/60) / 1000;

		return windSpeed;
	}
};