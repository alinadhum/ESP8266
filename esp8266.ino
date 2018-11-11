/*
   ESP8266 SPIFFS HTML Web Page with JPEG, PNG Image

*/
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>   //Include File System Headers
#include <max6675.h>
#include <ESP8266HTTPClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

ESP8266WebServer server(80);

//ESP AP Mode configuration
const char *ssid = "iPhone (Juha)";
const char *password = "87654321";

const int thermoDO = 12;
const int thermoCS = 13;
const int thermoCLK = 14;

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 2

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature DallasSensors(&oneWire);

MAX6675 thermocouple_1(thermoCLK, thermoCS, thermoDO);

float meter_temp_1;
float meter_temp_2;

void handleRoot() {
  server.sendHeader("Location", "/index.html", true);
  server.send(302, "text/plain", " ");
}

void handleWebRequests() {
  if (loadFromSpiffs(server.uri())) return;
  String message = "File Not Detected\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " NAME:" + server.argName(i) + "\n VALUE:" + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  Serial.println(message);
}

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println('\n');

  //Initialize File System
  SPIFFS.begin();
  Serial.println("File System Initialized");

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.println("Trying to connect to Wifi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
#ifndef ThermoOnly
  Serial.println("Dallas Temperature IC Control Library Demo");
  Serial.println("...and MAX6675 Thermocouple.");

  // Start up the library
  DallasSensors.begin();
#endif
  Serial.println("\nWiFi connected..");
  Serial.print("Use this URL to connect: http://");// Print the IP address
  Serial.println(WiFi.localIP());
  Serial.println("\r\n");
  server.onNotFound(handleWebRequests); //Set setver all paths are not found so we can handle as per URI
  server.on("/", handleRoot);
  server.on("/temp", HTTP_GET, []() {
    String json = "{";
    json += "\"meter_temp_1\":" + String(meter_temp_1) + ",";
    json += "\"meter_temp_2\":" + String(meter_temp_2);
    json += "}";
    server.send(200, "text/json", json);
    json = String();
  });
  server.begin();
  Serial.println("Webserver started..."); // Start the webserver
  Serial.print("\n");
}

void loop() {
  server.handleClient();
  if (WiFi.status() == WL_CONNECTED) {
    meter_temp_1 = thermocouple_1.readCelsius();
    meter_temp_2 = readOneWire();
    if (isnan(meter_temp_1) && isnan(meter_temp_2)) {
      Serial.println("Failed to read temprature from esp-sensor");
      return;
    }
    Serial.print("meter1 = ");
    Serial.print(meter_temp_1);
    Serial.print("\n");
    Serial.print("meter2 = ");
    Serial.print(meter_temp_2);
    Serial.print("\n");
    delay(500);
  }
}

bool loadFromSpiffs(String path) {
  String dataType = "text/plain";
  if (path.endsWith("/")) path += "index.htm";

  if (path.endsWith(".src")) path = path.substring(0, path.lastIndexOf("."));
  else if (path.endsWith(".html")) dataType = "text/html";
  else if (path.endsWith(".htm")) dataType = "text/html";
  else if (path.endsWith(".css")) dataType = "text/css";
  else if (path.endsWith(".js")) dataType = "application/javascript";
  else if (path.endsWith(".png")) dataType = "image/png";
  else if (path.endsWith(".gif")) dataType = "image/gif";
  else if (path.endsWith(".jpg")) dataType = "image/jpeg";
  else if (path.endsWith(".ico")) dataType = "image/x-icon";
  else if (path.endsWith(".xml")) dataType = "text/xml";
  else if (path.endsWith(".pdf")) dataType = "application/pdf";
  else if (path.endsWith(".zip")) dataType = "application/zip";
  File dataFile = SPIFFS.open(path.c_str(), "r");
  if (server.hasArg("download")) dataType = "application/octet-stream";
  if (server.streamFile(dataFile, dataType) != dataFile.size()) {
  }
  dataFile.close();
  return true;
}

float readOneWire() {
  DallasSensors.requestTemperatures(); // Send the command to get temperatures
  return DallasSensors.getTempCByIndex(0);
}
