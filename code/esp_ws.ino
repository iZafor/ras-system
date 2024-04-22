#include <ESP8266WiFi.h>
#include <ArduinoWebsockets.h>
// #include <PubSubClient.h>

// Wifi Credentilas
const char* ssid = "";
const char* password = "";

// Websocket Configs
const char* ws_server_host = "";
const uint16_t ws_server_port = 1880;
const char* ws_path = "/ws/ras";

using namespace websockets;

WebsocketsClient client;

char response[200];
int count = 0;
bool firstTwo = true;

void setupWifi();
void setupWS();
void messageCallback(WebsocketsMessage message);
void eventCallback(WebsocketsEvent event, String data);
void parseSensorData();
void sendPh();
void sendNTU();
void sendTemp();

enum ACTION {
  SEND_PH,
  SEND_NTU,
  SEND_TEMP
};

ACTION ac = SEND_PH;

void setup() {
  Serial.begin(115200);
  setupWifi();
  setupWS();
}

void loop() {
  if(client.available()) {
    client.poll();
    if(Serial.available()) {
      switch (ac) {
        case SEND_PH: {
          sendPh();
          ac = SEND_NTU;
          break;
        }case SEND_NTU: {
          sendNTU();
          ac = SEND_TEMP;
          break;
        }case SEND_TEMP: {
          sendTemp();
          ac = SEND_PH;
          break;
        }
      }
    }
  }
  delay(1000);
}

void setupWifi() {
  delay(100);
  Serial.println("\nConnecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);

  while(WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print("-");
  }
  Serial.print("\nConnected to ");
  Serial.println(ssid);
}

void setupWS() {
  bool connected = false;
  while(!(connected = client.connect(ws_server_host, ws_server_port, ws_path))) {
    delay(100);
  }
  Serial.println("WS connected");
  client.onMessage(messageCallback);
  client.onEvent(eventCallback);
  client.ping();
  sprintf(response, "{\"message\": \"connection successfull\"}");
  client.send(response);
}

void messageCallback(WebsocketsMessage message) {
  Serial.println(message.data());
}

void eventCallback(WebsocketsEvent event, String data) {
  if(event == WebsocketsEvent::ConnectionOpened) {
    Serial.println("Connection opened");
  }else if(event == WebsocketsEvent::ConnectionClosed) {
    Serial.println("Connection closed");
  }else if(event == WebsocketsEvent::GotPing) {
    Serial.println("Pinged, responding with PONG!");
    client.pong();
  }else if(event == WebsocketsEvent::GotPong) {
    Serial.println("Got a PONG!");
  }
}

void sendPh() {
  String phData = "";
  char data = ' ';
  while(true) {
    data = Serial.read();
    if (data == ',') {
      break;
    }
    phData += data;
    delay(100);
  }

  sprintf(response, "{\"ph\": %s}", phData);
  client.send(response);
}

void sendNTU() {
  String ntuData = "";
  char data = ' ';
  while(true) {
    data = Serial.read();
    if (data == ',') {
      break;
    }
    ntuData += data;
    delay(100);
  }

  sprintf(response, "{\"ntu\": %s}", ntuData);
  client.send(response); 
}

void sendTemp() {
  String tempData = "";
  char data = ' ';
  while(true) {
    data = Serial.read();
    if (data == '\n') {
      break;
    }
    tempData += data;
    delay(100);
  }

  sprintf(response, "{\"temp\": %s}", tempData);
  client.send(response);
}