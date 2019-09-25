#include <Arduino.h>
#include <RCSwitch.h>

// configuration
#define SENSOR_ID 1

// pins
#define TRANSMITTERPIN 12
#define THERMISTORPIN A0

// transmitter
#define RESEND_TIMESPAN 60000

// thermistor
#define NUMSAMPLES 10
#define THERMISTORNOMINAL 10000
#define TEMPERATURENOMINAL 25
#define BCOEFFICIENT 3950
#define SERIESRESISTOR 10000

RCSwitch transmitter = RCSwitch();
int samples[NUMSAMPLES];

void setup(void) {
  delay(random(1000, 10000));
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

  // calculate average resistance
  average = 0;
  for (i = 0; i < NUMSAMPLES; i++) {
     average += samples[i];
  }
  average /= NUMSAMPLES;
  average = 1023 / average - 1;
  average = SERIESRESISTOR / average;

  // convert resistance to temperature
  float temperature;
  temperature = average / THERMISTORNOMINAL;
  temperature = log(temperature);
  temperature /= BCOEFFICIENT;
  temperature += 1.0 / (TEMPERATURENOMINAL + 273.15);
  temperature = 1.0 / temperature;
  temperature -= 273.15;
  Serial.println("New temperature is " + String(temperature));

  // transmit message
  int sensor_part = (SENSOR_ID * 10000);
  int temperature_part = (temperature * 100);
  Serial.println(sensor_part);
  Serial.println(temperature_part);
  int resend_delay = random(4000, 20000);
  int resend_times = RESEND_TIMESPAN / resend_delay;
  int message = sensor_part + temperature_part;
  for (i = 0; i < resend_times; i++) {
    Serial.println("Sending " + String(i + 1) + "/" + String(resend_times) + ": " + String(message));
    transmitter.send(message, 24);
    delay(resend_delay);
  }
}
