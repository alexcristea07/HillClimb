#include "DHT.h"  

#define AMB_PIN_1       14 // Cod GPIO 14 <-> Pin D5 pe NodeMCU
#define AMB_PIN_2       12 // Cod GPIO 12 <-> Pin D6 pe NodeMCU

DHT amb_temp1(AMB_PIN_1, DHT11);
DHT amb_temp2(AMB_PIN_2, DHT11);

void setup() {
  Serial.begin(115200);  // UART0

  Serial.println();

  amb_temp1.begin();
  amb_temp2.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("AMB0_temp: " + String((int)amb_temp1.readTemperature()));
  Serial.println("AMB1_temp: " + String((int)amb_temp2.readTemperature()));
  Serial.println();
  delay(1000);
}
