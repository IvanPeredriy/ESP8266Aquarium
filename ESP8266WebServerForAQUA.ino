#include <Arduino.h>
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <WiFiUdp.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <ESP8266FtpServer.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// линия данных подключена к цифровому выводу 2 ESP
#define ONE_WIRE_BUS 4

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

const char* ssid = "KAPTOLLIKA";
const char* password = "19216812Zz";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

File data;

float temp1, temp2;
uint32_t saveDataCounter = 0;
unsigned long tim0 = 0;
unsigned long watchDog = 0;
AsyncWebServer server(80);
FtpServer ftpSrv;

int red, green, blue;

void(* resetFunc) (void) = 0; // Функция перезагрузки. Присоединить на кнопку.
unsigned long getTime();
void updateTemp();
void updateColor();
  
String processor(const String& var) {
  //Serial.println(var);
  if (var == "TEMPERATURE1") {
    return String(temp1);
  }
  else if (var == "TEMPERATURE2") {
    return String(temp2);
  }
  return String();
}

void setup() {
  
  analogWriteRange(255);  // 256 - 1. Max is 1023.
  
  Serial.begin(115200);
  sensors.begin();
  
  WiFi.begin(ssid, password);

  sensors.requestTemperatures();
  temp1 = sensors.getTempCByIndex(0);
  temp2 = sensors.getTempCByIndex(1);

  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println(WiFi.localIP());

  SPIFFS.begin();
  data = SPIFFS.open("/data.txt", "a"); // If file didn't exist - create it.
  data.close(); // and close

    
  
  server.on("/", HTTP_ANY, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/style.css");
  });
  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/script.js");
  });

  server.on("/iro.js", HTTP_GET, [](AsyncWebServerRequest * request){
    request->send(SPIFFS, "/iro.js");
  });
  
  server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest * request){
    request->send(SPIFFS, "/favicon.ico");
  });
  server.on("/temperature1", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/plain", String(temp1).c_str()); 
    //updateTemp();
  });
  server.on("/temperature2", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/plain", String(temp2).c_str());
  });
 
  server.on("/rgbColor", HTTP_POST, [](AsyncWebServerRequest * request) {
    AsyncWebParameter* p = request->getParam(0);
    String parser = p->value().c_str();
    request->send(200);
    sscanf(parser.substring(1,3).c_str(),"%x", &red); 
    sscanf(parser.substring(3,5).c_str(),"%x", &green);
    sscanf(parser.substring(5,7).c_str(),"%x", &blue); 
    //Serial.printf("red: %d, green: %d, blue: %d \n", red, green, blue);
    updateColor();
  });
  server.on("/reset", HTTP_GET, [](AsyncWebServerRequest * request) {
    Serial.println("Resetting...");
    //resetFunc();
    ESP.restart();
  });

  server.begin();
  ftpSrv.begin("Aquarium", "19216812Zz");

  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT 0 = 0
  timeClient.setTimeOffset(0);
  
  tim0 = millis();
}

void loop() {
  ftpSrv.handleFTP();
  //Serial.println(getTime());
  if ((millis() - tim0) > 60000) {
    sensors.requestTemperatures();
    temp1 = sensors.getTempCByIndex(0);
    temp2 = sensors.getTempCByIndex(1);
    //updateTemp();
    tim0=millis();
  }
  //ftpSrv.handleFTP();
}

unsigned long getTime(){
  timeClient.update();
  return timeClient.getEpochTime();
}


void updateTemp(){
  String input = "";
  long ntp = getTime();

  // if NTP-connection not ready (npt incorrect) - no data will write.
  if(ntp>1000){
    float avgTemp = (temp1 + temp2) / 2;
    input += String(avgTemp);
    input += ",";
    input += String(ntp);
    input += ";";
  }
  
  if(input != ""){
    data = SPIFFS.open("/data.txt", "a+");
    data.println(input);
    data.close();
  }
}

void updateColor(){
  analogWrite(12, red); // 12 D6 red
  analogWrite(13, blue);   // 13 D7 blue
  analogWrite(14, green);  // 14 D5 green
}

/*
  bool handleFileRead(String path){                                       // Функция работы с файловой системой
  if(path.endsWith("/")) path += "index.html";                          // Если устройство вызывается по корневому адресу, то должен вызываться файл index.html (добавляем его в конец адреса)
  String contentType = getContentType(path);                            // С помощью функции getContentType (описана ниже) определяем по типу файла (в адресе обращения) какой заголовок необходимо возвращать по его вызову
  if(SPIFFS.exists(path)){                                              // Если в файловой системе существует файл по адресу обращения
    File file = SPIFFS.open(path, "r");                                 //  Открываем файл для чтения
    size_t sent = server.streamFile(file, contentType);                   //  Выводим содержимое файла по HTTP, указывая заголовок типа содержимого contentType
    file.close();                                                       //  Закрываем файл
    return true;                                                        //  Завершаем выполнение функции, возвращая результатом ее исполнения true (истина)
  }
  return false;                                                         // Завершаем выполнение функции, возвращая результатом ее исполнения false (если не обработалось предыдущее условие)
  }

  String getContentType(String filename){                                 // Функция, возвращающая необходимый заголовок типа содержимого в зависимости от расширения файла
  if (filename.endsWith(".html")) return "text/html";                   // Если файл заканчивается на ".html", то возвращаем заголовок "text/html" и завершаем выполнение функции
  else if (filename.endsWith(".css")) return "text/css";                // Если файл заканчивается на ".css", то возвращаем заголовок "text/css" и завершаем выполнение функции
  else if (filename.endsWith(".js")) return "application/javascript";   // Если файл заканчивается на ".js", то возвращаем заголовок "application/javascript" и завершаем выполнение функции
  else if (filename.endsWith(".png")) return "image/png";               // Если файл заканчивается на ".png", то возвращаем заголовок "image/png" и завершаем выполнение функции
  else if (filename.endsWith(".jpg")) return "image/jpeg";              // Если файл заканчивается на ".jpg", то возвращаем заголовок "image/jpg" и завершаем выполнение функции
  else if (filename.endsWith(".gif")) return "image/gif";               // Если файл заканчивается на ".gif", то возвращаем заголовок "image/gif" и завершаем выполнение функции
  else if (filename.endsWith(".ico")) return "image/x-icon";            // Если файл заканчивается на ".ico", то возвращаем заголовок "image/x-icon" и завершаем выполнение функции
  return "text/plain";                                                  // Если ни один из типов файла не совпал, то считаем что содержимое файла текстовое, отдаем соответствующий заголовок и завершаем выполнение функции
  }*/
