//sdm live page example by reaper7

#define READSDMEVERY  2000                                                      //read sdm every 2000ms
//#define USE_STATIC_IP

/*  WEMOS D1 Mini
                     ______________________________
                    |   L T L T L T L T L T L T    |
                    |                              |
                 RST|                             1|TX HSer
                  A0|                             3|RX HSer
                  D0|16                           5|D1
                  D5|14                           4|D2
                  D6|12                    10kPUP_0|D3
RX SSer/HSer swap D7|13                LED_10kPUP_2|D4
TX SSer/HSer swap D8|15                            |GND
                 3V3|__                            |5V
                       |                           |
                       |___________________________|

D1 mini -> MAX485
3 / RX -> RO
1 / TX -> DI
5 / D1 -> DE + RE
3V3 -> VCC (rot)
GND -> GND (schwarz)

D1 mini -> Netzteil (5V)
5V -> + (rot)
GND -> - (schwarz)

MAX485 -> Modbus
GND -> GND (schwarz)
A -> A (gelb)
B -> B (rot)

Get values via JSON
GET http://<IPADDR>/rest

*/

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>

#include <ESPAsyncTCP.h>                                                        //https://github.com/me-no-dev/ESPAsyncTCP
#include <ESPAsyncWebServer.h>                                                  //https://github.com/me-no-dev/ESPAsyncWebServer

#include <SDM.h>                                                                //https://github.com/datjan/SDM_Energy_Meter
#include <SoftwareSerial.h>

#include "index_page.h"


//------------------------------------------------------------------------------
AsyncWebServer server(80);

SoftwareSerial swSerSDM;

// SDM120 
// SDM sdm(swSerSDM, 2400, 5, SWSERIAL_8N1, 3, 1);    

// SDM72
SDM sdm(swSerSDM, 9600, 5, SWSERIAL_8N1, 3, 1);  

//------------------------------------------------------------------------------
String devicename = "PWRMETER";

#if defined ( USE_STATIC_IP )
IPAddress ip(192, 168, 0, 130);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);
#endif

const char* wifi_ssid = "xxx";
const char* wifi_password = "xxx";

String lastresetreason = "";

unsigned long readtime;
//------------------------------------------------------------------------------
typedef volatile struct {
  volatile float regvalarr;
  const uint16_t regarr;
  const char* regname;
  const char* regunit;
} sdm_struct;

//SDM72
volatile sdm_struct sdmarr[] = {  
  {0.00, SDM_AVERAGE_L_TO_N_VOLTS, "001_SDM_AVERAGE_L_TO_N_VOLTS", "V"},                               
  {0.00, SDM_SUM_LINE_CURRENT, "001_SDM_SUM_LINE_CURRENT", "A"},                                             
  {0.00, SDM_TOTAL_SYSTEM_POWER, "001_SDM_TOTAL_SYSTEM_POWER", "W"},                                            
  {0.00, SDM_TOTAL_SYSTEM_POWER_FACTOR, "001_SDM_TOTAL_SYSTEM_POWER_FACTOR", "PF"},                                       
  {0.00, SDM_FREQUENCY, "001_SDM_FREQUENCY", "Hz"},
  {0.00, SDM_TOTAL_ACTIVE_ENERGY, "001_SDM_TOTAL_ACTIVE_ENERGY", "kWh"},
  {0.00, SDM_IMPORT_ACTIVE_ENERGY, "001_SDM_IMPORT_ACTIVE_ENERGY", "kWh"},
  {0.00, SDM_EXPORT_ACTIVE_ENERGY, "001_SDM_EXPORT_ACTIVE_ENERGY", "kWh"},
  {0.00, SDM_PHASE_1_POWER, "001_SDM_PHASE_1_POWER", "W"},
  {0.00, SDM_PHASE_2_POWER, "001_SDM_PHASE_2_POWER", "W"},
  {0.00, SDM_PHASE_3_POWER, "001_SDM_PHASE_3_POWER", "W"}                                                  
};

//SDM120
//volatile sdm_struct sdmarr[] = {    
//  {0.00, SDM_PHASE_1_VOLTAGE, "001_SDM_PHASE_1_VOLTAGE", "V"},                               
//  {0.00, SDM_PHASE_1_CURRENT, "001_SDM_PHASE_1_CURRENT", "A"},                                             
//  {0.00, SDM_PHASE_1_POWER, "001_SDM_PHASE_1_POWER", "W"},                                            
//  {0.00, SDM_PHASE_1_POWER_FACTOR, "001_SDM_PHASE_1_POWER_FACTOR", "PF"},                                       
//  {0.00, SDM_FREQUENCY, "001_SDM_FREQUENCY", "Hz"},
//  {0.00, SDM_TOTAL_ACTIVE_ENERGY, "001_SDM_TOTAL_ACTIVE_ENERGY", "kWh"}                                                
//};
//------------------------------------------------------------------------------
void jsonrequest(AsyncWebServerRequest *request) {
  String JSON = F("[");
  for (int i = 0; i < (sizeof(sdmarr) / sizeof(sdmarr[0])); i++) {
    JSON += "{\"id\":\"" + String(sdmarr[i].regname) + "\", \"value\":\"" + String(sdmarr[i].regvalarr,2) + "\", \"unit\":\"" + sdmarr[i].regunit + "\"},";
  }
  JSON += "{\"id\":\"freeh\", \"value\":\"" + String(ESP.getFreeHeap()) + "\", \"unit\":\"bytes\"},";
  JSON += "{\"id\":\"rst\", \"value\":\"" + lastresetreason + "\", \"unit\":\"\"}";
  JSON += "]";
  request->send(200, "text/json", JSON);
}
//------------------------------------------------------------------------------
void indexrequest(AsyncWebServerRequest *request) {
  request->send_P(200, "text/html", index_page);
}
//------------------------------------------------------------------------------
void ledOn() {
  digitalWrite(LED_BUILTIN, LOW);
}
//------------------------------------------------------------------------------
void ledOff() {
  digitalWrite(LED_BUILTIN, HIGH);
}
//------------------------------------------------------------------------------
void ledSwap() {
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}
//------------------------------------------------------------------------------
void otaInit() {
  ArduinoOTA.setHostname(devicename.c_str());

  ArduinoOTA.onStart([]() {
    ledOn();
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    ledSwap();
  });
  ArduinoOTA.onEnd([]() {
    ledOff();
  });
  ArduinoOTA.onError([](ota_error_t error) {
    ledOff();
  });
  ArduinoOTA.begin();
}
//------------------------------------------------------------------------------
void serverInit() {
  server.on("/", HTTP_GET, indexrequest);
  server.on("/rest", HTTP_GET, jsonrequest);
  server.onNotFound([](AsyncWebServerRequest *request){
    if (request->method() == HTTP_OPTIONS){
      request->send(204);
    }
    else {
      request->send(404);
    }
  });
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "content-type");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "GET, HEAD, POST, PUT, DELETE, CONNECT, OPTIONS, TRACE, PATCH");
  server.begin();
}
//------------------------------------------------------------------------------
static void wifiInit() {
  WiFi.persistent(false);                                                       // Do not write new connections to FLASH
  WiFi.mode(WIFI_STA);
#if defined ( USE_STATIC_IP )
  WiFi.config(ip, gateway, subnet);                                             // Set fixed IP Address
#endif
  WiFi.begin(wifi_ssid, wifi_password);

  while( WiFi.status() != WL_CONNECTED ) {                                      //  Wait for WiFi connection
    ledSwap();
    delay(100);
  }
}
//------------------------------------------------------------------------------
void sdmRead() {
  float tmpval = NAN;

  for (uint8_t i = 0; i < (sizeof(sdmarr) / sizeof(sdmarr[0])); i++) {
    tmpval = sdm.readVal(sdmarr[i].regarr);

    if (isnan(tmpval))
      sdmarr[i].regvalarr = 0.00;
    else
      sdmarr[i].regvalarr = tmpval;

    yield();
  }
}
//------------------------------------------------------------------------------
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  ledOn();

  lastresetreason = ESP.getResetReason();

  wifiInit();
  otaInit();
  serverInit();
  sdm.begin();

  readtime = millis();

  ledOff();
}
//------------------------------------------------------------------------------
void loop() {
  ArduinoOTA.handle();

  if (millis() - readtime >= READSDMEVERY) {
    sdmRead();
    readtime = millis();
  }

  yield();
}
