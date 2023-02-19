
#include <Wire.h> // I2C library
#include <SparkFunMLX90614.h> 
 #include "DHT.h"  

/*******************************************************/
/* Adrese I2C ale senzorilor de temperatura infra-rosu */
/*******************************************************/
#define IR_I2C_ADDR1    0x5A
#define IR_I2C_ADDR2    0x5B
#define IR_I2C_ADDR3    0x5C

/*******************************************************/
/****** Pini senzorilor de temperatura ambientala ******/
/*******************************************************/
#define AMB_PIN_1       14 // Cod GPIO 14 <-> Pin D5 pe NodeMCU
#define AMB_PIN_2       13 // Cod GPIO 13 <-> Pin D7 pe NodeMCU

/*******************************************************/
/****** Globale senzori de temperatura infra-rosu ******/
/*******************************************************/
IRTherm ir_temp1; 
IRTherm ir_temp2;
IRTherm ir_temp3;

/*******************************************************/
/****** Globale senzori de temperatura ambientala ******/
/*******************************************************/
DHT amb_temp1(AMB_PIN_1, DHT11);
DHT amb_temp2(AMB_PIN_2, DHT11);

/*******************************************************/
/*** Declarare functii senzori temperatura infra-rosu **/
/*******************************************************/
void setup_ir_temp();

/*******************************************************/
/*** Declarare functii senzori temperatura ambientala **/
/*******************************************************/
void setup_amb_temp();

/*******************************************************/
/******** Functie setup apelata inainte de loop ********/
/*******************************************************/
void setup() 
{
  Serial.begin(115200); // Initialize Serial to log output
  Wire.begin(); //Joing I2C bus
  Serial.println();

  setup_ir_temp();
  setup_amb_temp();
}

/*******************************************************/
/********* Functie principala, rulata in bucla *********/
/*******************************************************/
void loop() 
{
//  // Call therm.read() to read object and ambient temperatures from the sensor.
//  unsigned long before;
//  unsigned long after;
//  
//  before = millis(); 
//  if (ir_temp2.read()) // On success, read() will return 1, on fail 0.
//  {
//    // Use the object() and ambient() functions to grab the object and ambient
//	// temperatures.
//	// They'll be floats, calculated out to the unit you set with setUnit().
// 
//    float tempAmb = ir_temp1.ambient();
//    float tempObj = ir_temp1.object();
//
//    Serial.print("Object: " + String(tempObj, 2));
//    Serial.println("C");
//    Serial.print("Ambient: " + String(tempAmb, 2));
//    Serial.println("C");
//
//  }
//  after = millis();
//  Serial.println("Read took -> " + String(after - before) + "ms");
//  Serial.println();
//  Serial.println();
//  Serial.println();
//     
//  delay(1000);


   unsigned long before;
   unsigned long after;
   before = millis();
   int temp1 = amb_temp1.readTemperature();
   int temp2 = amb_temp2.readTemperature();
   after = millis();
   
   Serial.println("1: Temperature in C:"); 
   Serial.println(temp1);  
 
   Serial.println("2: Temperature in C:");  
   Serial.println(temp2);  

   Serial.println(after - before);
   Serial.println();
   Serial.println();
   Serial.println();
   delay(1000);  

}

/*******************************************************/
/**** Functie setup senzori temperatura infra-rosu *****/
/*******************************************************/
void setup_ir_temp()
{
  bool ret;
  ret = ir_temp1.begin(IR_I2C_ADDR1);
  if (ret == false) {
    Serial.println("Senzor IR1 eroare initializare");
  }
  if (!ret)
    ir_temp1.setUnit(TEMP_C); // Seteaza grade Celsius ca unitate de masura
    
  ret = ir_temp2.begin(IR_I2C_ADDR2);
  if (ret == false) {
    Serial.println("Senzor IR2 eroare initializare");
  }
  if (!ret)
    ir_temp2.setUnit(TEMP_C); // Seteaza grade Celsius ca unitate de masura

  ret = ir_temp3.begin(IR_I2C_ADDR3);
  if (ret == false) {
    Serial.println("Senzor IR3 eroare initializare");
  }
  if (!ret)
    ir_temp3.setUnit(TEMP_C); // Seteaza grade Celsius ca unitate de masura
}

/*******************************************************/
/**** Functie setup senzori temperatura ambientala *****/
/*******************************************************/
void setup_amb_temp()
{
  amb_temp1.begin();
  amb_temp2.begin();
}
