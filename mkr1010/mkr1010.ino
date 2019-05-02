#include <WiFiNINA.h>
#include <ArduinoHttpClient.h>
#include <ArduinoJson.h>

//const char ssid[] = "ATTBsaeSVS";
const char ssid[] = "Bebop2-044561";// Network SSID (name)
//const char pass[] = "5wsth5eabem+";    // Network password (use for WPA, or use as key for WEP)
const char pass[] = ""; 

//const char server[] = "192.168.1.135";
const char server[] = "192.168.42.5";
const int port = 5000;
//const String discord_webhook = SECRET_WEBHOOK;
//const String discord_tts = SECRET_TTS;

int status = WL_IDLE_STATUS;
//WiFiSSLClient client;
WiFiClient wifi;
HttpClient http_client = HttpClient(wifi, server, port);

void connect_to_wifi() {
  String fv = WiFi.firmwareVersion();
  if (fv < "1.0.0") {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("[WiFi] Connecting to: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(100);
  }

  // you're connected now, so print out the data:
  Serial.println("[WiFi] Connected");
}


void json_send(String content) {
  Serial.println("[HTTP] Connecting to Server...");
  Serial.println("[HTTP] Message: " + content);
  //String data =  "{\"min_ahead\":\"" + content + "\"}";
  http_client.post("/nav", "application/json",content );
  // read the status code and body of the response
  int statusCode = http_client.responseStatusCode();
  String response = http_client.responseBody();

  Serial.print("[HTTP] Status code: ");
  Serial.println(statusCode);
  Serial.print("[HTTP] Response: ");
  Serial.println(response);
  content = "";
}

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  connect_to_wifi();
  //json_send("302");
}


StaticJsonDocument<400> sample;
float min_ahead = 0.0;
float min_left = 0.0 ;
float min_right = 0.0; 
float max_left = 0.0;
float max_right = 0.0;
String s = "";
String data = "";
String response = "";
int statusCode = 0;
String cmd ="";

void loop() {
  if(Serial1.available())
  {
    DeserializationError err = deserializeJson(sample, Serial1); //Deserialize incoming data into Json object, check for err
    if(err)
    {
      return;
    }
    serializeJson(sample,cmd); //write the Json object into a string
    //Serial1.flush();
    json_send(cmd); // send the string over wifi to the server
    Serial.println(cmd);
    cmd = "";
  }
  
  //http_client.post("/nav", "application/json",cmd );
  //statusCode = http_client.responseStatusCode();
  //Serial.print("[HTTP] Status code: ");
  //Serial.println(statusCode);
  
  //delay(2000);
  //min_ahead = sample["min_ahead"];
  //s = String(min_ahead);
  //Serial.println("[HTTP] Connecting to Server...");
  //Serial.println("[HTTP] Message: " + s);
  //data =  "{\"min_ahead\":\"" + s + "\"}";
  //Serial.println(data);
  //http_client.post("/nav", "application/json",data );
  //statusCode = http_client.responseStatusCode();
  //Serial.print("[HTTP] Status code: ");
  //Serial.println(statusCode);
  //String response = http_client.responseBody();
  //Serial.println(s);
  //json_send(s);
  /*if (Serial1.available()){
  DeserializationError err = deserializeJson(sample, Serial1);
  if (err) {
    }
    else
    {
      //min_ahead = sample["min_ahead"];
      //cmd = "{\"min_ahead\":\"" + String(min_ahead) + "\"}";
      json_send("302");
      //http_client.post("/nav", "application/json", cmd );
      Serial.println(cmd);
    }
  }*/
}
