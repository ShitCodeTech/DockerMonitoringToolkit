#ifndef WIFISETUP_H
#define WIFISETUP_H

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <EEPROM.h>

AsyncWebServer server(80);

#define P_RESET 12                 

int Reset = 1;

// EEPROM byte used  (EEPROM range byte 0-511)
int ssidAddress = 0;
int passAddress = 100;
int serverAdress = 250;
int Rst = 400;

const char* ssid = "ESP_CONFIG";     // Edit here AP name
const char* password = "12345678";   //Edit here AP password
IPAddress softAPIP(192, 168, 4, 1);  //Edit here AP website IP

String wifi_ssid = "";
String wifi_pass = "";
String server_adress ="";

const char* html = R"(
<!DOCTYPE HTML>
<html>
<head>
<meta charset="UTF-8">
</head>
  <body>
    <h2>ESP32 Configuration Wi-Fi</h2>
    <form id="myForm">
      SSID WiFi: <input type="text" id="input1" name="SSID WiFi:"><br><br>
      Password: <input type="text" id="input2" name="Password:"><br><br>
      Server Adress: <input type="text" id="input3" name="ServerAdress:"><br><br>
      <input type="submit" value="Send">
    </form>
  </body>
<script>
document.getElementById("myForm").onsubmit = function(e) {
  e.preventDefault();
  var input1Value = encodeURIComponent(document.getElementById("input1").value);
  var input2Value = encodeURIComponent(document.getElementById("input2").value);
  var input3Value = encodeURIComponent(document.getElementById("input3").value);

  fetch("/", {
    method: "POST",
    headers: {
      "Content-Type": "application/x-www-form-urlencoded",
    },
    body: "input1=" + input1Value + "&input2=" + input2Value+"&input3=" + input3Value,
  })
      .then((response) => {
        console.log("Data sent successfully.");
      })
      .catch((error) => {
        console.error("Error sending data.");
      });
  };
</script>
</html>
)";

//writing and reading  data to EEPROM
void writeStringToEEPROM(int address, const String &data) {
  for (int i = 0; i < data.length(); i++) {
    EEPROM.write(address + i, data[i]);
  }
  EEPROM.write(address + data.length(), '\0');
  EEPROM.commit();
}

String readStringFromEEPROM(int address) {
  String result = "";
  char c;
  for (int i = 0; (c = EEPROM.read(address + i)) != '\0'; i++) {
    result += c;
  }
  return result;
}

void saveCredentialsToEEPROM(const String &ssid, const String &pass,const String &serveradress) {
  writeStringToEEPROM(ssidAddress, ssid);
  writeStringToEEPROM(passAddress, pass);
  writeStringToEEPROM(serverAdress, serveradress);
}

//check btn Reset
void CheckReset(){
    if(digitalRead(P_RESET) == LOW){
    riavvio1:
    if(digitalRead(P_RESET) == HIGH){
    EEPROM.write(Rst, 1);
    EEPROM.commit();
    Serial.println("Reset");
    } else {goto riavvio1;}
  }
}

//init wifi connect
void WiFiSetup(){
  pinMode(P_RESET, INPUT_PULLUP);
  EEPROM.begin(511);
  Serial.println("Starting..");
  Reset = EEPROM.read(Rst);
  String savedSsid = readStringFromEEPROM(ssidAddress);
  String savedPass = readStringFromEEPROM(passAddress);
  String savedAdress = readStringFromEEPROM(serverAdress);
  wifi_ssid = savedSsid;
  wifi_pass = savedPass;
  server_adress = savedAdress;

 if(Reset == 1){
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(softAPIP, IPAddress(0, 0, 0, 0), IPAddress(255, 255, 255, 0));
  WiFi.softAP(ssid, password);
  Serial.println("> Configuration mode <");
  Serial.println("Access Point Started!");
  Serial.print("AP SSID: ");
  Serial.print(ssid);
  Serial.print(" Password: ");
  Serial.println(password);
  Serial.println("Configuration page: http://192.168.4.1");


  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", html);
  });
  server.on("/", HTTP_POST, [](AsyncWebServerRequest *request){
    String input1 = request->arg("input1");
    String input2 = request->arg("input2");
    String input3 = request->arg("input3");
    wifi_ssid = input1;
    wifi_pass = input2;
    server_adress = input3;
  Serial.print("SSID: ");
  Serial.println(input1);
  Serial.print("Password: ");
  Serial.println(input2);
  Serial.println("Restarting..");
    saveCredentialsToEEPROM(wifi_ssid, wifi_pass,server_adress);
    request->send(200, "text/plain", "Data received successfully! Restarting...");
    EEPROM.write(Rst, 0);
    EEPROM.commit();
    delay(2000);
    ESP.restart();
  });
  server.begin();

 } else {
  
  WiFi.begin(wifi_ssid.c_str(), wifi_pass.c_str());
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connection in progress...");
    CheckReset();
  }
  Serial.println("Connected to the Wi-Fi network.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  }
}


void WiFiLoop(){
  CheckReset();
}

#endif
