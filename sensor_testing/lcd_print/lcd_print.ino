#include <Wire.h> // I2C library
#include <LiquidCrystal_I2C.h>

#define LCD_I2C_ADDR    0x27
#define LCD_NUM_COLS    16
#define LCD_NUM_ROWS    2

LiquidCrystal_I2C lcd(LCD_I2C_ADDR, LCD_NUM_COLS, LCD_NUM_ROWS);

void setup() {
  lcd.init();
  lcd.clear();         
  lcd.backlight();
}

void loop() {
  lcd.clear();
  
  lcd.setCursor(5, 0); // Cursor caracterul 0, linia 0
  lcd.print(String("Testare"));

  lcd.setCursor(4, 1); // Cursor caracterul 0, linia 1
  lcd.print(String("Ecran LCD"));

  delay(3000);
}
