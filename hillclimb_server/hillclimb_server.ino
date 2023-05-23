#include <Ticker.h>

#include <ESP8266WiFi.h>

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
struct SensorsData {
  int ir0;
  int ir1;
  int ir2;
  int amb0;
  int amb1;
};

/*******************************************************/
/************ Macros vector stocare date ***************/
/*******************************************************/
#define MAX_RECORD_VALUES 30
#define AMB_ERR_THRESH  25
#define IR_ERR_THRESH   25

/*******************************************************/
/******************** Macros WiFi **********************/
/*******************************************************/
#define MAX_WIFI_RETRIES 30

/*******************************************************/
/*********** Globale vector stocare date  **************/
/*******************************************************/
unsigned int arrindex = 0;
unsigned int displayall = 0;
SensorsData values[MAX_RECORD_VALUES];

/*******************************************************/
/***************** Functii afisare web *****************/
/*******************************************************/
void wifi_setup(void);
void wifi_process(void);

/*******************************************************/
/*************** Globale afisare web  ******************/
/*******************************************************/
WiFiServer server(80);
String header;
char *ssid = "AccessPoint";
char *pass = "12345678";
int wifi_counter = 0;


/*******************************************************/
/****************** Macros modul GSM *******************/
/*******************************************************/
#define GSM_NUM_SENSOR_VALUES 5 // Numar de valori primite prin fiecare mesaj text

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
  Serial.println();

  LOG.println();

  wifi_setup();

  setup_gsm_uart();
}

/*******************************************************/
/********* Functie principala, rulata in bucla *********/
/*******************************************************/
void loop() 
{
  wifi_process();

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

  int i;
  for (i = 0; i < MAX_RECORD_VALUES; i++)
  {
    values[i].ir0 = 0;
    values[i].ir1 = 0;
    values[i].ir2 = 0;
    values[i].amb0 = 0;
    values[i].amb1 = 0;
  }
  LOG.println("setup: " + String(__func__)  + " OK");
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

      /*
      LOG.print("data: ");
      LOG.println(data);
      */

      if (recv_sms_extract_values(data, storage) == GSM_NUM_SENSOR_VALUES)
      {
        // Am gasit cele 5 valori, le pot afisa;
        /*
        for (int i = 0; i < GSM_NUM_SENSOR_VALUES; i++)
        {
          LOG.println("val[" + String(i) + "] -> " + String(storage[i]));
        }
        */

        values[arrindex].ir0 = storage[0];
        values[arrindex].ir1 = storage[1];
        values[arrindex].ir2 = storage[2];
        values[arrindex].amb0 = storage[3];
        values[arrindex].amb1 = storage[4];
        arrindex = (arrindex + 1) % MAX_RECORD_VALUES;
        if (arrindex == 0)
          displayall = 1;
      }
      else
      {
        LOG.println("Format gresit al mesajului.");
      }
    }
    LOG.println("GSM recv:" + content);
  }
}

/*******************************************************/
/**************** Functie setup WiFi *******************/
/*******************************************************/
void wifi_setup(void) {
  //Conectarea la Wi-FI si verificarea si afisarea Ip-ului in monitorul serial.
  LOG.print("Se conecteaza la retea ");
//  Serial.println(ssid);
  LOG.print(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED && wifi_counter < MAX_WIFI_RETRIES) {
    delay(500);  // 500ms
    LOG.print(".");
    wifi_counter++;
  }

  if (wifi_counter == MAX_WIFI_RETRIES) {
    LOG.println("Conectarea la retea a esuat");
    LOG.println("setup: " + String(__func__)  + " OK");
    return;
  }

  server.begin();  // pornire server web
  LOG.println();
  LOG.println("setup: " + String(__func__)  + " OK");
  LOG.println(WiFi.localIP()); // afiseaza in consola debug IP-ul obtinut
}

/*******************************************************/
/************** Functie procesare WiFi *****************/
/*******************************************************/
void wifi_process(void) {
  int i;
  unsigned int last;
  WiFiClient client = server.available();

  if (!client)
    return;

  if (displayall == 0) {
    last = arrindex;
  }
  else {
    last = MAX_RECORD_VALUES;
  }

  String request = client.readStringUntil('\r');
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println("Connection: close");
  client.println();
  client.println("<!DOCTYPE html><html>");
  client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
  client.println("<link rel=\"icon\" href=\"data:,\">");
  client.print("<meta http-equiv=\"refresh\" content=\"3\">"); // Refresh la fiecare 3 secunde
  client.println("</style></head><body><h1>Monitorizare</h1>");

  client.println("<table><tr><th>Index</th><th>Senzor IR1</th><th>Senzor IR2</th><th>Senzor IR3</th><th>Senzor AMB1</th> <th>Senzor AMB2</th></tr>");

  for (i = 0; i < last; i++)
  {
    client.println("<tr>");

    /* Afisare index tabel */
    client.println("<td><span class=\"sensor\">");
    /* Index curent e afisat cu verde */
    if ((i + 1) == arrindex)
      client.println("<font color=\"#0f0\">");
    client.println(i + 1);
    if ((i + 1) == arrindex)
      client.println("</font>");    
    client.println("</span></td>");

    /* Afisare valoarea 1 senzori IR */
    client.println("<td><span class=\"sensor\">");
    if (values[i].ir0 > IR_ERR_THRESH)
      client.println("<font color=\"#f00\">");
    client.println(values[i].ir0);
    if (values[i].ir0 > IR_ERR_THRESH)
      client.println("</font>");    
    client.println("</span></td>");

    /* Afisare valoarea 2 senzori IR */
    client.println("<td><span class=\"sensor\">");
    if (values[i].ir1 > IR_ERR_THRESH)
      client.println("<font color=\"#f00\">");
    client.println(values[i].ir1);
    if (values[i].ir1 > IR_ERR_THRESH)
      client.println("</font>");    
    client.println("</span></td>");

    /* Afisare valoarea 3 senzori IR */
    client.println("<td><span class=\"sensor\">");
    if (values[i].ir2 > IR_ERR_THRESH)
      client.println("<font color=\"#f00\">");
    client.println(values[i].ir2);
    if (values[i].ir2 > IR_ERR_THRESH)
      client.println("</font>");    
    client.println("</span></td>");

    /* Afisare valoarea 1 senzori AMB */
    client.println("<td><span class=\"sensor\">");
    if (values[i].amb0 > AMB_ERR_THRESH)
      client.println("<font color=\"#f00\">");
    client.println(values[i].amb0);
    if (values[i].amb0 > IR_ERR_THRESH)
      client.println("</font>");    
    client.println("</span></td>");

    /* Afisare valoarea 2 senzori AMB */
    client.println("<td><span class=\"sensor\">");
    if (values[i].amb1 > AMB_ERR_THRESH)
      client.println("<font color=\"#f00\">");
    client.println(values[i].amb1);
    if (values[i].amb1 > IR_ERR_THRESH)
      client.println("</font>");    
    client.println("</span></td>");
    
    client.println("</tr>");
  }

  client.println("</body>");
}
