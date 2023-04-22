

#include <Wire.h> // I2C library
#include <SparkFunMLX90614.h> 
#include "DHT.h"  

#include <LiquidCrystal_I2C.h>

#include <Ticker.h>

//#define SERIAL_DEBUG

/*******************************************************/
/************ Macros senzori temperatura IR ************/
/*******************************************************/
#define IR_TEMP_NUM     3
#define IR_I2C_ADDR1    0x5A
#define IR_I2C_ADDR2    0x5B
#define IR_I2C_ADDR3    0x5C
#define IR_ERR_THRESH   25

/*******************************************************/
/****** Macros senzori de temperatura ambientala *******/
/*******************************************************/
#define AMB_TEMP_NUM    2
#define AMB_PIN_1       14 // Cod GPIO 14 <-> Pin D5 pe NodeMCU
#define AMB_PIN_2       12 // Cod GPIO 12 <-> Pin D6 pe NodeMCU
#define AMB_ERR_THRESH  25

/*******************************************************/
/**************** Macros ecranului LCD *****************/
/*******************************************************/
#define LCD_LOOPS       2
#define LCD_I2C_ADDR    0x27
#define LCD_NUM_COLS    16
#define LCD_NUM_ROWS    2

/*******************************************************/
/********** Interval rulare bucla procesare ************/
/*******************************************************/
#define ACTION_TIMER    0.5 // Interval (in secunde) folosit pentru executia callback-ului
                            // pe timer hardware

/*******************************************************/
/****** Globale senzori de temperatura infra-rosu ******/
/*******************************************************/
IRTherm ir_temp[IR_TEMP_NUM]; 
int ir_temp_val[IR_TEMP_NUM];

/*******************************************************/
/****** Globale senzori de temperatura ambientala ******/
/*******************************************************/
DHT amb_temp1(AMB_PIN_1, DHT11);
DHT amb_temp2(AMB_PIN_2, DHT11);
int amb_temp_val[AMB_TEMP_NUM];

/*******************************************************/
/****************** Globale afisaj LCD *****************/
/*******************************************************/
LiquidCrystal_I2C lcd(LCD_I2C_ADDR, LCD_NUM_COLS, LCD_NUM_ROWS);
int lcd_loop_cnt = 0;

/*******************************************************/
/*************** Globale timer hardware ****************/
/*******************************************************/
Ticker action_ticker;
bool action_flag = false;

/*******************************************************/
/****** Declarare functie callback timer hardware ******/
/*******************************************************/
void action_set()
{
  action_flag = true;
}

/*******************************************************/
/*** Declarare functii senzori temperatura infra-rosu **/
/*******************************************************/
void setup_ir_temp();
void read_ir_temp();

/*******************************************************/
/*** Declarare functii senzori temperatura ambientala **/
/*******************************************************/
void setup_amb_temp();
void read_amb_temp();

/*******************************************************/
/************* Declarare functii afisaj LCD ************/
/*******************************************************/
void setup_lcd_display();
void update_lcd_display();

/*******************************************************/
/************* Declarare functii modul GSM *************/
/*******************************************************/
void setup_gsm_uart();

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
  setup_lcd_display();
  setup_gsm_uart();

  action_ticker.attach(ACTION_TIMER, action_set);
}

/*******************************************************/
/********* Functie principala, rulata in bucla *********/
/*******************************************************/
void loop() 
{

  if (!action_flag) // Daca timer-ul hardware nu a fost apelat, atunci valoarea este 'false'
    return;

  action_flag = false;

  read_amb_temp();
  read_ir_temp();
  update_lcd_display();

#ifdef SERIAL_DEBUG
  Serial.println();
#endif
}

/*******************************************************/
/**** Functie setup senzori temperatura infra-rosu *****/
/*******************************************************/
void setup_ir_temp()
{
  bool ret;
  int i;
  ret = ir_temp[0].begin(IR_I2C_ADDR1);
  if (ret == false) {
    Serial.println("Senzor IR1 eroare initializare");
  }
  if (!ret)
    ir_temp[0].setUnit(TEMP_C); // Seteaza grade Celsius ca unitate de masura
    
  ret = ir_temp[1].begin(IR_I2C_ADDR2);
  if (ret == false) {
    Serial.println("Senzor IR2 eroare initializare");
  }
  if (!ret)
    ir_temp[1].setUnit(TEMP_C); // Seteaza grade Celsius ca unitate de masura

  ret = ir_temp[2].begin(IR_I2C_ADDR3);
  if (ret == false) {
    Serial.println("Senzor IR3 eroare initializare");
  }
  if (!ret)
    ir_temp[2].setUnit(TEMP_C); // Seteaza grade Celsius ca unitate de masura
}

/*******************************************************/
/**** Functie setup senzori temperatura ambientala *****/
/*******************************************************/
void setup_amb_temp()
{
  amb_temp1.begin();
  amb_temp2.begin();
}


/*******************************************************/
/************** Functie setup afisaj LCD ***************/
/*******************************************************/
void setup_lcd_display()
{
  lcd.init();
  lcd.clear();         
  lcd.backlight();
}

/*******************************************************/
/************** Functie setup modul GSM ****************/
/*******************************************************/
void setup_gsm_uart()
{
  delay(3000);  // Astept 3secunde pana a trimite comenzi "AT" spre modul
  Serial.println("AT\n"); // Trimit multiple comenzi spre modul pentru ca acesta sa inteleaga ca lucram la baudrate 115200
  delay(300);
  Serial.println("AT\n");
  delay(300);
  Serial.println("AT\n");
  delay(300);
  while (Serial.available() > 0)  // Curata tot ce a venit pe seriala pana in acest moment, inclusiv raspunsurile "OK" care vin pt 
    Serial.read();                //   comenzile AT date anterior.
}

/*******************************************************/
/******** Functie citire temperaturi infrarosu *********/
/*******************************************************/
void read_ir_temp()
{
  int i;
  for (i = 0; i < IR_TEMP_NUM; i++)
  {
    if (!ir_temp[i].read())
    {
#ifdef SERIAL_DEBUG
      Serial.print("IR Senzor");
      Serial.print(i);
      Serial.println(" eroare citire");
#endif
      ir_temp_val[i] = 0;
    }
    else
    {
      ir_temp_val[i] = ir_temp[i].object();
#ifdef SERIAL_DEBUG
      Serial.print("IR");
      Serial.print(i);
      Serial.print("_temp: ");
      Serial.println(ir_temp_val[i]);
#endif
    }
  }
}

/*******************************************************/
/******* Functie citire temperaturi ambientale *********/
/*******************************************************/
void read_amb_temp()
{
  amb_temp_val[0] = amb_temp1.readTemperature();
  amb_temp_val[1] = amb_temp2.readTemperature();
  
#ifdef SERIAL_DEBUG
  Serial.print("AMB0_temp: ");
  Serial.println(amb_temp_val[0]);
  Serial.print("AMB1_temp: ");
  Serial.println(amb_temp_val[1]);
#endif
}

/*******************************************************/
/*********** Functie actualizare display LCD ***********/
/*******************************************************/
void update_lcd_display()
{
  if ((lcd_loop_cnt % LCD_LOOPS) == 0)
  {
    int i;
    String line;

    lcd.clear();

    lcd.setCursor(0, 0); // Cursor caracterul 0, linia 0
    for (i = 0; i < IR_TEMP_NUM; i++)
    {
      if (ir_temp_val[i] > IR_ERR_THRESH)
        line += "E:"; // Adaug semnalizatorul de eroare
      else
        line += "  "; // Adaug 2 caractere spatiu
      line += String(ir_temp_val[i]);
      line += " ";    // Adaug spatiul de la final
    }
    lcd.print(line);

    line = "";
    lcd.setCursor(0, 1); // Cursor caracterul 0, linia 1
    for  (i = 0; i < AMB_TEMP_NUM; i++)
    {
      if (amb_temp_val[i] > IR_ERR_THRESH)
        line += "E:"; // Adaug semnalizatorul de eroare
      else
        line += "  "; // Adaug 2 caractere spatiu
      line += String(amb_temp_val[i]);
      line += " ";    // Adaug spatiul de la final     
    }
    lcd.print(line);
  }
  lcd_loop_cnt = (lcd_loop_cnt + 1) % LCD_LOOPS;
}
