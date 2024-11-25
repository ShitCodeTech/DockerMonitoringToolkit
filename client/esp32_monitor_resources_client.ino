#include <WebServer.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include "WiFiSetup.h"
#include "DisplaySetup.h"

// Initialize parameters
String cpu ="", mem="", uptime ="", ip="", name="", id="",allmem ="", usedmem ="";

// Declare websocket client class variable
WebSocketsClient webSocket;

// Allocate the JSON document
StaticJsonDocument<512> doc;

void setup() {
  
  //init setup of display and graphics
  display_setup();
  initPlot(); 

  // Connect to WiFi
  WiFiSetup();
  Serial.begin(115200);

  // server address, port, and URL path
  webSocket.begin(server_adress, 8080, "/ws");

  // event handler
  webSocket.onEvent(webSocketEvent);

  // try ever 5sec again if connection has failed
  webSocket.setReconnectInterval(5000);

}

void loop() {
  WiFiLoop();
  webSocket.loop();
    
  static uint32_t tmr;
  if (millis() - tmr >= 30) {
    btns_tick();
    tmr = millis();
  }
  switch(mode){
    case 0: display1(cpu, mem, uptime, name);
      break;
    case 1: display2(cpu);
      break;
    case 2: display3(mem,allmem,usedmem);
      break;
    case 3:display4(server_adress,wifi_ssid,ip);
      break;
  }
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  if(type == WStype_TEXT)
  {
    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, payload);
  
    // Test if parsing succeeds.
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.c_str());
      return;
    }
    const String cpu_str = doc["cpu"];
    const String allmem_str = doc["allmem"];
    const String usedmem_str = doc["usedmem"];
    const String uptime_str = doc["uptime"];
    const String mem_str = doc["mem"];
    const String ip_str=doc["ip"];
    const String name_str=doc["name"];
    const String id_str=doc["id"];

    
    allmem = allmem_str;
    usedmem = usedmem_str;
    cpu = cpu_str;
    mem = mem_str;
    uptime = uptime_str;
    ip=ip_str;
    name=name_str;
    id=id_str;
  }
}
