#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char* WIFI_SSID = "IEEE";
const char* WIFI_PASSWORD = "1i2e3e4e@1333";
const IPAddress WIFI_LOCAL_IP(192, 168, 10, 100);
const IPAddress WIFI_GATEWAY(192, 168, 10, 1);
const IPAddress WIFI_SUBNET(255, 255, 255, 0);
const IPAddress WIFI_DNS_PRIMARY(192, 168, 10, 2);
const IPAddress WIFI_DNS_SECONDARY(192, 168, 10, 2);
ESP8266WebServer server(80);

bool bedRoomLightStatus = 0;
bool livingRoomLightStatus = 0;
bool wcLightStatus = 0;

void setup(void);
void loop(void);
void printBanner(void);
void setupSerial(void);
void setupWifi(void);
void setupWebserver(void);
void handleRoot(void);
void handleGetLightStatus(void);
void handleSetLightStatus();
void handleNotFound(void);

String _boolToString(bool value);

void setup(void) {
  setupSerial();
  printBanner();
  setupWifi();
  setupWebserver();
}

void loop(void) {
  server.handleClient();
  MDNS.update();
}

void printBanner(void) {
  Serial.println("-------------------------------------------------------");
  Serial.println("IoT Sample Project");
  Serial.println("Developed by Iordanis Kostelidis <kostelidis@ieee.org>");
  Serial.println("Developed for IEEE IHU Student Branch in Serres Campus");
  Serial.println("-------------------------------------------------------");
}

void setupSerial(void) {
  Serial.begin(115200);
}

void setupWifi(void) {
  if (!WiFi.config(WIFI_LOCAL_IP, WIFI_GATEWAY, WIFI_SUBNET, WIFI_DNS_PRIMARY, WIFI_DNS_SECONDARY)) {
    Serial.println("STA Failed to configure");
  }

  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void setupWebserver(void) {
  if (MDNS.begin("esp8266")) {
    Serial.println("mDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/api/rpc/light/status", handleGetLightStatus);
  server.on("/api/rpc/light/trigger", handleSetLightStatus);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void handleRoot(void) {
  String jsonString = "";
  jsonString += "{";
  jsonString += "\"status\" : \"online\"";
  jsonString += "}\r\n";
  server.send(200, "application/json", jsonString);
}

void handleGetLightStatus(void) {
  String jsonString = "";
  jsonString += "{";
  jsonString += "\"bedroomLight\" : " + _boolToString(bedRoomLightStatus) + ",";
  jsonString += "\"livingRoomLight\" : " + _boolToString(livingRoomLightStatus) + ",";
  jsonString += "\"wcLight\" : " + _boolToString(wcLightStatus) + "";
  jsonString += "}\r\n";
  server.send(200, "application/json", jsonString);
}

void handleSetLightStatus(void) {
  String inputMessage = "";

  if (server.hasArg("name")) {
    inputMessage = server.arg("name");
  }

  if (inputMessage == "bedroomLight") {
    bedRoomLightStatus = !bedRoomLightStatus;
  }

  if (inputMessage == "livingRoomLight") {
    livingRoomLightStatus = !livingRoomLightStatus;
  }

  if (inputMessage == "wcLight") {
    wcLightStatus = !wcLightStatus;
  }

  if (inputMessage == "") {
    bedRoomLightStatus = !bedRoomLightStatus;
    livingRoomLightStatus = !livingRoomLightStatus;
    wcLightStatus = !wcLightStatus;
  }

  

  handleGetLightStatus();
}

void handleNotFound(void) {
  server.send(404, "application/json", "");
}

String _boolToString(bool value) {
  if (value == 1) {
    return "1";
  }
  return "0";
}
