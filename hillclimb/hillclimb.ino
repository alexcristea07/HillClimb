
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

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
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

  lcd.init();
  lcd.clear();         
  lcd.backlight();      //
  lcd.setCursor(2,0);   //Set cursor to character 2 on line 0
  lcd.print("Hello world!");
  
  lcd.setCursor(2,1);   //Move cursor to character 2 on line 1
  lcd.print("LCD Tutorial");
}

/*******************************************************/
/********* Functie principala, rulata in bucla *********/
/*******************************************************/
void loop() 
{
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
