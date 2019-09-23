#include <RCSwitch.h>

#define TRANSMITTERPIN 12
#define THERMISTORPIN A0         
#define THERMISTORNOMINAL 10000
#define TEMPERATURENOMINAL 25
#define NUMSAMPLES 10
#define BCOEFFICIENT 3950
#define SERIESRESISTOR 10000    

RCSwitch transmitter = RCSwitch();

int samples[NUMSAMPLES];

void setup(void) {
  Serial.begin(9600);
  analogReference(EXTERNAL);
  transmitter.enableTransmit(TRANSMITTERPIN);
}

void loop(void) {

  // collect temperature samples
  uint8_t i;
  float average;
  for (i = 0; i < NUMSAMPLES; i++) {
   samples[i] = analogRead(THERMISTORPIN);
   delay(10);
  }

  // average samples
  average = 0;
  for (i = 0; i < NUMSAMPLES; i++) {
     average += samples[i];
  }
  average /= NUMSAMPLES;
  average = 1023 / average - 1;
  average = SERIESRESISTOR / average;

  // calculate temperature
  float temperature;
  temperature = average / THERMISTORNOMINAL;
  temperature = log(temperature);
  temperature /= BCOEFFICIENT;
  temperature += 1.0 / (TEMPERATURENOMINAL + 273.15);
  temperature = 1.0 / temperature;
  temperature -= 273.15;

  // output temperature
  float message = 10000 + (temperature * 100);
  for (i = 0; i < 60; i++) {
    Serial.println(String(temperature) + " " + String(i) + "...");
    transmitter.send(message, 24);
    delay(1000);
  }
}
