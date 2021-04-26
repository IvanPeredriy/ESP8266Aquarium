{
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
