
#include <Wire.h> // I2C library
#include <SparkFunMLX90614.h> 
#include "DHT.h"  

#include <LiquidCrystal_I2C.h>

#include <Ticker.h>

//#define USE_SERIAL0_DEBUG

#ifdef USE_SERIAL0_DEBUG  // Nu ar trebui sa fie folosit, UART0 e folosit pt comunicatia cu modulul GSM
# define LOG Serial
#else
# define LOG Serial1      // Debug-ul (log consola) foloseste UART1, care are doar pin TX, insa e suficient
#endif                    // pentru ca NodeMCU sa trimita log catre consola seriala
                          // PIN TX UART1: D4
/*******************************************************/
/**** Structura ce stocheaza cele 5 valori numerice ****/
/*******************************************************/
struct Sensors {
  int ir0;
  int ir1;
  int ir2;
  int amb1;
  int amb2;
};

/*******************************************************/
/************ Macros vector stocare date ***************/
/*******************************************************/
#define MAX_RECORD_VALUES 30

/*******************************************************/
/*********** Globale vector stocare date  **************/
/*******************************************************/
unsigned int arrindex = 0;
unsigned int displayall = 0; // Dupa prima umplere cu date, aceasta variabila devine 1
                             // La request de afisare in web, daca valoarea ei e 0, atunci afiseaza
                             //   din vectorul de structuri pana la valoarea globalei 'arrindex', altfel
                             //   afiseaza toate valorile (cele vechi vor fi suprascrise)
Sensors values[MAX_RECORD_VALUES];

/*******************************************************/
/****************** Macros modul GSM *******************/
/*******************************************************/
#define GSM_NUM_SENSOR_VALUES 5 // Numar de valori primite prin fiecare mesaj text

/*******************************************************/
/****************** Globale modul GSM  *****************/
/*******************************************************/
unsigned int gsm_counter = 0;

/*******************************************************/
/************* Declarare functii modul GSM *************/
/*******************************************************/
void setup_gsm_uart();
void recv_sms_process();
int recv_sms_extract_values(char *, int *);

/*******************************************************/
/******** Functie setup apelata inainte de loop ********/
/*******************************************************/
void setup() 
{
  Serial.begin(115200);
#ifndef USE_SERIAL0_DEBUG // Daca folosim UART1 pentru debug (USE_SERIAL0_DEBUG -> nedefinit), atunci
  Serial1.begin(115200);  // e necesar sa initializam si pe UART1
#endif
  Wire.begin(); //Joing I2C bus
  Serial.println();

  setup_gsm_uart();

}

/*******************************************************/
/********* Functie principala, rulata in bucla *********/
/*******************************************************/
void loop() 
{
  recv_sms_process();
}

/*******************************************************/
/************** Functie setup modul GSM ****************/
/*******************************************************/
void setup_gsm_uart()
{
  delay(5000);  // Astept 5 secunde pana a trimite comenzi "AT" spre modul
  Serial.println("AT\r"); // Trimit multiple comenzi spre modul pentru ca acesta sa inteleaga ca lucram la baudrate 115200
  delay(300);
  Serial.println("AT\r");
  delay(300);
  Serial.println("AT\r");
  delay(300);
  Serial.println("AT+CMGF=1\r");    // Seteaza mod text pentru SMS-uri
  delay(300);
  Serial.println("AT+CMGDA=\"DEL ALL\"\r"); // Sterge mesajele anterioare de pe device
  delay(300);
  Serial.println("AT+CNMI=1,2,0,0,0\r"); // Setez functia de primit mesaje sa le afiseze direct in consola
  delay(300);
  while (Serial.available() > 0)  // Curata tot ce a venit pe seriala pana in acest moment, inclusiv raspunsurile "OK" care vin pt 
    Serial.read();                //   comenzile AT date anterior.

  Serial1.println("\nsetup_gsm_uart OK");
}


/*********************************************************/
/* Functie extragere valori intregi din sir de caractere */
/*********************************************************/
int recv_sms_extract_values(char *data, int *storage) {
  int count = 0;
  char *token;

  token = strtok(data, " ");
  while (token != NULL && count < GSM_NUM_SENSOR_VALUES)
  {
    // Culeg valoarea curenta din string si o transform in intreg, apoi o stochez in array-ul 'storage'
    storage[count] = atoi(token);
    count++;
    // Caut urmatoarea valoare din string
    token = strtok(NULL, " ");
  }
  return count;
}

/*******************************************************/
/*********** Functie tratare notificare SMS  ***********/
/*******************************************************/
void recv_sms_process()
{
  while (Serial.available())
  {
    String content;
    content = Serial.readString();

    // Daca mesajul primit de la modul GSM incepe cu '+CMT:', inseamna ca imi transmite un mesaj text primit
    // Altfel ignor ce imi transmite

    int poz;
    if ((poz = content.indexOf("+CMT:")) != -1)
    {
      int pozFirstNewline;
      int pozSecondNewline;\
      int storage[GSM_NUM_SENSOR_VALUES];
      char data[20];
      
      while (content[poz] != '\n')
      {
        poz++;
      }
      pozFirstNewline = poz;
      poz++;

      while (content[poz] != '\n' && poz < content.length())
      {
        poz++;
      }
      pozSecondNewline = poz;

      // Extrag datele utile intr-un alt sir de caractere, pentru a extrage intregii
      memcpy(data, &content[pozFirstNewline + 1], pozSecondNewline - pozFirstNewline - 1);
      data[pozSecondNewline - pozFirstNewline - 1] = '\0';

      LOG.print("data: ");
      LOG.println(data);

      if (recv_sms_extract_values(data, storage) == GSM_NUM_SENSOR_VALUES)
      {
        // Am gasit cele 5 valori, le pot afisa;
        for (int i = 0; i < GSM_NUM_SENSOR_VALUES; i++)
        {
          LOG.println("val[" + String(i) + "] -> " + String(storage[i]));
        }

        // Adaug in buffer circular
      }
      else
      {
        LOG.println("Format gresit al mesajului.");
      }
    }
    LOG.println("GSM recv:" + content);
  }
}
