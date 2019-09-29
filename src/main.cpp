#include <Arduino.h>
#include <RCSwitch.h>

// configuration
#define SENSOR_ID 2

// pins
#define TRANSMITTERPIN 12
#define THERMISTORPIN A0

// transmitter
#define RESEND_TIMES 12
#define RESEND_DELAY 5000

// thermistor
#define NUMSAMPLES 10
#define THERMISTORNOMINAL 10000
#define TEMPERATURENOMINAL 25
#define BCOEFFICIENT 3950
#define SERIESRESISTOR 10000

RCSwitch transmitter = RCSwitch();
int samples[NUMSAMPLES];

void setup(void) {
  Serial.begin(9600);
  analogReference(EXTERNAL);
  transmitter.enableTransmit(TRANSMITTERPIN);
}



int checkDigit(int message) {
  String characters = String(message);
  int odd = 0;
  int even = 0;
  for (uint8_t i = 0; i < characters.length(); i++) {
    String character = String(characters.charAt(i));
    int digit = character.toInt();
    if ((i + 1) % 2 == 0) {
      even += digit;
    } else {
      odd += digit;
    }
  }
  int total = (odd * 3) + even;
  int remainder = total % 10;
  int result = 10 - remainder;
  if (result == 10) {
    result = 0;
  }
  return result;
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
  int message = sensor_part + temperature_part;
  int check_digit = checkDigit(message);
  message = (message * 10) + check_digit;
  for (i = 0; i < RESEND_TIMES; i++) {
    Serial.println("Sending " + String(i + 1) + "/" + String(RESEND_TIMES) + ": " + String(message));
    transmitter.send(message, 24);
    delay(RESEND_DELAY);
  }
}
