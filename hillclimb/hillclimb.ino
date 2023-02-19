
#include <Wire.h> // I2C library
#include <SparkFunMLX90614.h> 

#define IR_I2C_ADDR1    0x5A
#define IR_I2C_ADDR2    0x5B
#define IR_I2C_ADDR3    0x5C

IRTherm ir_temp1; 
IRTherm ir_temp2;
IRTherm ir_temp3;

void setup() 
{
  bool ret;
  Serial.begin(115200); // Initialize Serial to log output
  Wire.begin(); //Joing I2C bus
  Serial.println();

/* Setup senzori IR */
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
/* Sfarsit setup senzori IR */
}

void loop() 
{
//  digitalWrite(LED_BUILTIN, HIGH);
    
  // Call therm.read() to read object and ambient temperatures from the sensor.
  unsigned long before;
  unsigned long after;
  
  before = millis(); 
  if (ir_temp2.read()) // On success, read() will return 1, on fail 0.
  {
    // Use the object() and ambient() functions to grab the object and ambient
	// temperatures.
	// They'll be floats, calculated out to the unit you set with setUnit().
 
    float tempAmb = ir_temp1.ambient();
    float tempObj = ir_temp1.object();

    Serial.print("Object: " + String(tempObj, 2));
    Serial.println("C");
    Serial.print("Ambient: " + String(tempAmb, 2));
    Serial.println("C");

  }
  after = millis();
  Serial.println("Read took -> " + String(after - before) + "ms");
  Serial.println();
  Serial.println();
  Serial.println();
     
  delay(1000);
}
