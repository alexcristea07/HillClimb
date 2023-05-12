#include <Wire.h> // I2C library
#include <SparkFunMLX90614.h> 

#define IR_I2C_ADDR1    0x5A
#define IR_I2C_ADDR2    0x5B
#define IR_I2C_ADDR3    0x5C

IRTherm ir_temp1;
IRTherm ir_temp2;
IRTherm ir_temp3;


void setup() {
  Serial.begin(115200);
  Wire.begin();

  Serial.println();

  ir_temp1.begin(IR_I2C_ADDR1);
  ir_temp2.begin(IR_I2C_ADDR2);
  ir_temp3.begin(IR_I2C_ADDR3);

  ir_temp1.setUnit(TEMP_C);
  ir_temp2.setUnit(TEMP_C);
  ir_temp3.setUnit(TEMP_C);
}

void loop() {
  ir_temp1.read();
  ir_temp2.read();
  ir_temp3.read();

  Serial.println("IR0_temp: " + String(ir_temp1.object()));
  Serial.println("IR1_temp: " + String(ir_temp2.object()));
  Serial.println("IR2_temp: " + String(ir_temp3.object()));
  Serial.println();
  delay(1000);
}
