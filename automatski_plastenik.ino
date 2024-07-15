#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "DHT.h"
#define DHTTYPE DHT11
//192.168.4.1

//wifi konekcija
const char* ssid = "Blato";
const char* password = "123456789";

//unosni parametri
float parametarTmep=0;
float parametarHumi=0;
float parametarLight=0;
float parametarSoil=0;
float tolerancijaTmep=0;
float tolerancijaHumi=0;
float tolerancijaLight=0;
float tolerancijaSoil=0;


//vlaga zemlje
const int dry = 3500; 
const int wet = 1200;
const int soilPin = 33;
const int soilOut=18;

//fotorezistor
const int fotoPin=32;
int lightVal;
const int lightOut=16;

//temperatura i vlaznos vazduha
DHT dht(15,DHTTYPE);
const int tempOut=13;
const int tempOut2=21;
const int humiOut=19;
AsyncWebServer server(80);


//funkcije
String readTemp()
{
  return String(dht.readTemperature());
}
String readHumi()
{
  return String(dht.readHumidity());
}
String readSoil()
{
  return String(map(analogRead(soilPin), dry, wet, 0, 100));
}
String readLight()
{
  return String(map(analogRead(fotoPin), 0, 4095, 0, 100));
}


void temperatura(int pin1,int pin2, float tempVrijednost, float parametar)
{
  if(tempVrijednost>parametar+tolerancijaTmep)
  {
    digitalWrite(pin1,HIGH);
    digitalWrite(pin2,LOW);
  }
  else if(tempVrijednost<parametar-tolerancijaTmep)
  {
    digitalWrite(pin2,HIGH);
    digitalWrite(pin1,LOW);
  }
  
  else
  {
    digitalWrite(pin1,LOW);
    digitalWrite(pin2,LOW);
  } 
}

void svjetlost(int pin, float tempVrijednost,float parametar)
{
  if(tempVrijednost<parametar+tolerancijaLight && tempVrijednost>parametar-tolerancijaLight)
  {
    digitalWrite(pin,LOW);
  }
  else
  {
    digitalWrite(pin,HIGH);
  } 
}

void zemljaVlaznost(int pin, float tempVrijednost,float parametar)
{
  if(tempVrijednost<parametar-tolerancijaSoil)
  {
    digitalWrite(pin,HIGH);
  }
  else
  {
    digitalWrite(pin,LOW);
  } 
}

void vazduhVlaznost(int pin, float tempVrijednost,float parametar)
{
  if(tempVrijednost<parametar+tolerancijaHumi && tempVrijednost>parametar-tolerancijaHumi)
  {
    digitalWrite(pin,LOW);
  }
  else
  {
    digitalWrite(pin,HIGH);
  } 
}

void setup()
{
  Serial.begin(115200);
  dht.begin();
  pinMode(lightOut, OUTPUT);
  pinMode(tempOut, OUTPUT);
  pinMode(tempOut2, OUTPUT);
  pinMode(soilOut,OUTPUT);
  pinMode(humiOut,OUTPUT);
  Serial.println();
  Serial.print("Setting AP (Access Point)…");
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

server.on("/test", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    String x="yes";
    request->send_P(300, "text/plain",x.c_str());
  });
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    String x=readTemp();
    if(isnan(x.toFloat()))
    {
      request->send_P(300, "text/plain",x.c_str());
    }
    else
    {
      request->send_P(200, "text/plain",x.c_str());  
    }
    
  });

  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    String x=readHumi();
    if(isnan(x.toFloat()))
    {
      request->send_P(300, "text/plain",x.c_str());
    }
    else
    {
      request->send_P(200, "text/plain",x.c_str());  
    }
  });

  server.on("/soil", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    String x=readSoil();
    if(isnan(x.toFloat()))
    {
      request->send_P(300, "text/plain",x.c_str());
    }
    else
    {
      request->send_P(200, "text/plain",x.c_str());  
    }
  });

  server.on("/light", HTTP_GET, [](AsyncWebServerRequest *request)
  {
     String x=readLight();
    if(isnan(x.toFloat()))
    {
      request->send_P(300, "text/plain",x.c_str());
    }
    else
    {
      request->send_P(200, "text/plain",x.c_str());  
    }
  });

  server.on("/postTemp", HTTP_POST, [](AsyncWebServerRequest * request)
  {
    int params = request->params();
    for(int i=0;i<params;i++)
    {
      AsyncWebParameter* p = request->getParam(i);
      if(p->isPost())
      {
        Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
        String pomocni=p->value().c_str();
        parametarTmep=pomocni.toFloat();
        Serial.println(parametarTmep);
      }
    }
    request -> send(200);
  });

  server.on("/postHum", HTTP_POST, [](AsyncWebServerRequest * request)
  {
    int params = request->params();
    for(int i=0;i<params;i++)
    {
      AsyncWebParameter* p = request->getParam(i);
      if(p->isPost())
      {
        Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
        String pomocni=p->value().c_str();
        parametarHumi=pomocni.toFloat();
        Serial.println(parametarHumi);
      }
    } 
    request -> send(200);
  });

  server.on("/postSoil", HTTP_POST, [](AsyncWebServerRequest * request)
  {
    int params = request->params();
    for(int i=0;i<params;i++)
    {
      AsyncWebParameter* p = request->getParam(i);
      if(p->isPost())
      {
        Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
        String pomocni=p->value().c_str();
        parametarSoil=pomocni.toFloat();
        Serial.println(parametarSoil);
      }
    } 
    request -> send(200);
  });

  server.on("/postLight", HTTP_POST, [](AsyncWebServerRequest * request)
  {
    int params = request->params();
    for(int i=0;i<params;i++)
    {
      AsyncWebParameter* p = request->getParam(i);
      if(p->isPost())
      {
        Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
        String pomocni=p->value().c_str();
        parametarLight=pomocni.toFloat();
        Serial.println(parametarLight);
      }
    } 
    request -> send(200);
  });
  

// tolerancije
  server.on("/posttTemp", HTTP_POST, [](AsyncWebServerRequest * request)
  {
    int params = request->params();
    for(int i=0;i<params;i++)
    {
      AsyncWebParameter* p = request->getParam(i);
      if(p->isPost())
      {
        Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
        String pomocni=p->value().c_str();
        tolerancijaTmep=pomocni.toFloat();
        Serial.println(tolerancijaTmep);
      }
    } 
    request -> send(200);
  });
  


  server.on("/postTolerancijaHumi", HTTP_POST, [](AsyncWebServerRequest * request)
  {
    int params = request->params();
    for(int i=0;i<params;i++)
    {
      AsyncWebParameter* p = request->getParam(i);
      if(p->isPost())
      {
        Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
        String pomocni=p->value().c_str();
        tolerancijaHumi=pomocni.toFloat();
        Serial.println(tolerancijaHumi);
      }
    } 
    request -> send(200);
  });
  

  server.on("/postTolerancijaSoil", HTTP_POST, [](AsyncWebServerRequest * request)
  {
    int params = request->params();
    for(int i=0;i<params;i++)
    {
      AsyncWebParameter* p = request->getParam(i);
      if(p->isPost())
      {
        Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
        String pomocni=p->value().c_str();
        tolerancijaSoil=pomocni.toFloat();
        Serial.println(tolerancijaSoil);
      }
    } 
    request -> send(200);
  });
  

  server.on("/postTolerancijaLight", HTTP_POST, [](AsyncWebServerRequest * request)
  {
    int params = request->params();
    for(int i=0;i<params;i++)
    {
      AsyncWebParameter* p = request->getParam(i);
      if(p->isPost())
      {
        Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
        String pomocni=p->value().c_str();
        tolerancijaLight=pomocni.toFloat();
        Serial.println(tolerancijaLight);
      }
    } 
    request -> send(200);
  });
  server.begin();

}
void loop() 
{
 int soilmoisturepercent = map(analogRead(soilPin), dry, wet, 0, 100);
 int  lightVal= map(analogRead(fotoPin), 0, 4095, 0, 100);

 //poziv funkicaj za paljenje lampica
 temperatura(tempOut,tempOut2,dht.readTemperature(),parametarTmep);
 svjetlost(lightOut,lightVal,parametarLight);
 zemljaVlaznost(soilOut,soilmoisturepercent,parametarSoil);
 vazduhVlaznost(humiOut,dht.readHumidity(),parametarHumi);
 
 delay(2000);
}
