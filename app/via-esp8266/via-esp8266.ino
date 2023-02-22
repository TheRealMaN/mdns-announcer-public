//
// ESP8266 mDNS Announcer for Homebridge API Server
//
// For a debug you can turn on the settings in 'Tools->Debug level' & 'Tools->Debug port' menu of Arduino IDE
//

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#ifndef STASSID
#define STASSID "<YOUR-WIFI-NET-NAME>"
#define STAPSK  "<YOURPASSWORD>"
#endif

#define BRIDGE_NAME "Homebridge A2BA 3214"
#define BRIDGE_ID   "0E:6F:63:56:A3:BA" // homebridge ID
#define BRIDGE_SH   "6G1uIR=="          // setup hash
#define BRIDGE_C    "4"                 // config version
#define BRIDGE_SF   "0"                 // status flag

#define BRIDGE_HOST "myvps"             //.ddns.net"
#define BRIDGE_PORT 51226
#define BRIDGE_IP   "67.200.100.1"     // You can use it in MDNS.begin() function, but should work without

const char* ssid        = STASSID;
const char* password    = STAPSK;
byte triesToConnect     = 10;

MDNSResponder::hMDNSService bridgeService = 0;  // The handle of the Homebridge service in the MDNS responder
ESP8266WebServer server(BRIDGE_PORT);


void setup() {
  Serial.begin(115200);

  setupWIFIConnection();
  setupWebServer();
  setupMDNS();
}

void loop() {
  MDNS.update();
  server.handleClient();         
}


// Connect to Wi-Fi Network
void setupWIFIConnection() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.printf("Connecting to WiFi '%s'", ssid);

  while (--triesToConnect && WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.printf("Couldn't connect to '%s' network", ssid);
  }
  else {
    // Successful connection to WiFi
    Serial.println("\nConnected");
    Serial.println("IP address: " + WiFi.localIP().toString());
  }
}


// Web-server setup
void setupWebServer() {
  server.on("/", handleRootPath);
  server.begin();
  Serial.printf("Server is listening on port %s ...\n", String(BRIDGE_PORT));
}

// Route handler for "/"
void handleRootPath() {
  server.send(200, "text/plain", "Greetings from esp mDNS Announcer");
}


// mDNS Announcer setup
void setupMDNS() {
  if (!MDNS.begin(BRIDGE_HOST)) {
    Serial.println("Error while setting up an mDNS announcer!");
    while (1) {
      delay(1000);
    }
  }
  bridgeService = MDNS.addService(BRIDGE_NAME, "hap", "tcp", BRIDGE_PORT);

  MDNS.addServiceTxt(bridgeService, "id", BRIDGE_ID);
  MDNS.addServiceTxt(bridgeService, "sh", BRIDGE_SH);
  MDNS.addServiceTxt(bridgeService, "c#", BRIDGE_C);
  MDNS.addServiceTxt(bridgeService, "sf", BRIDGE_SF);
  MDNS.addServiceTxt(bridgeService, "ci", "2");
  MDNS.addServiceTxt(bridgeService, "pv", "1.1");
  MDNS.addServiceTxt(bridgeService, "ff", "0");
  MDNS.addServiceTxt(bridgeService, "s#", "1");

  MDNS.addServiceTxt(bridgeService, "md", "homebridge");

  Serial.println("mDNS announcer is started");
  Serial.printf("Bridge service '%s' was announced for %s:%s", BRIDGE_NAME, BRIDGE_HOST, String(BRIDGE_PORT));
}
