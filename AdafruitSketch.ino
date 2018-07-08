#include <SPI.h>
#include <WiFi101.h>
#include <ArduinoJson.h>


const int airPump = 10;
const int hygrometer = A1;
const int waterlevel = A2;

const int has_Serial = 0;

char ssid[] = "Simons iPhone";      // your network SSID (name)
char pass[] = "flowerpot";     // your network password (use for WPA, or use as key for WEP)   // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;

// Initialize the WiFi client library
WiFiClient client;

// server address:
char server[] = "172.20.10.5";//"192.168.178.20";
//char server[] = "localhost"

unsigned long lastConnectionTime = 0;            // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 2L * 1000L; // delay between updates, in milliseconds

void setup() {
  //Initialize serial and wait for port to open:
  WiFi.setPins(8, 7, 4, 2);

  if(has_Serial == 1){
    Serial.begin(9600);
    while (!Serial) {
      ; // wait for serial port to connect. Needed for native USB port only
    }
  }

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    if(has_Serial == 1){
      Serial.println("WiFi shield not present");
    }
    // don't continue:
    while (true);
  }

  // attempt to connect to WiFi network:
  while ( status != WL_CONNECTED) {
    if(has_Serial == 1){
      Serial.print("Attempting to connect to SSID: ");
      Serial.println(ssid);
    }
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(6000);
  }
  // you're connected now, so print out the status:
  printWiFiStatus();

  pinMode(hygrometer, INPUT_PULLUP);
  pinMode(waterlevel, INPUT_PULLUP);

  pinMode(airPump,OUTPUT);
}





void loop() {
  
  // if there's incoming data from the net connection.
  // send it out the serial port.  This is for debugging
  // purposes only:
  char fullmessage [126];
  const char *openbracket = "[";
  const char *closebracket = "]";
  //char message;
  bool startcopy = false;
  int index = 0;
  while (client.available()) {
    char message = client.read();
    
    if (strchr(closebracket, message))
    {
      startcopy = false;
      fullmessage[index] = '\0'; 
    }
    if (startcopy == true) {
      if(index < 124) 
      {
        fullmessage[index] = message; // Store it
        index++; 
      }
    }
    if (strchr(openbracket, message))
    {
      startcopy = true;
    }
    
    //Serial.write(message);
  }
  
  

  if (millis() - lastConnectionTime > postingInterval) {
    int soilhumidity = analogRead(hygrometer);
    httpRequestSet(soilhumidity,"SoilHumidity");
    //Serial.println(fullmessage);
    int waterlevel_value = analogRead(waterlevel);
    httpRequestSet(waterlevel_value,"Waterlevel");
    if(has_Serial == 1){
      Serial.print("Soil Humidity: ");
      Serial.println(soilhumidity);
      Serial.print("WaterLevel: ");
      Serial.println(waterlevel_value);
    }

    StaticJsonBuffer<200> jsonBuffer;
    //const char* pin;
    //double epochtime;
    int value = 0;
    JsonObject& root = jsonBuffer.parseObject(fullmessage);

    value = root["value"];

    if (value == 0.0) {
      digitalWrite(airPump,LOW);
      if(soilhumidity<20){
        httpRequestSet(1,"WaterPump");
        digitalWrite(airPump,HIGH);
        delay(1500);
        digitalWrite(airPump, LOW);
        delay(400);
        httpRequestSet(0,"WaterPump");
      }
    }
    if (value == 1.0) {
      digitalWrite(airPump,HIGH);
      delay(1500);
      digitalWrite(airPump, LOW);
    }
    
    if(has_Serial == 1){
      Serial.print("WaterPump: ");
      Serial.println(value);
      Serial.println(FreeRam());
    }
    httpRequestGet();
    
  }
  
}

// this method makes a HTTP connection to the server:
void httpRequestGet() {
  // close any connection before send a new request.
  // This will free the socket on the WiFi shield
  client.stop();

  // if there's a successful connection:
  if (client.connect(server, 4000)) {
    if(has_Serial == 1){
      Serial.println("connecting...");
    }
    
    // send the HTTP PUT request:
    client.println("GET /WaterPump");
    client.println("Connection: close");
    client.println();

    // note the time that the connection was made:
    lastConnectionTime = millis();
  }
  else {
    // if you couldn't make a connection:
    if(has_Serial == 1){
      Serial.println("connection failed");
    }
  }
}

// this method makes a HTTP connection to the server:
void httpRequestSet(int value, String component) {
  // close any connection before send a new request.
  // This will free the socket on the WiFi shield
  client.stop();

  // if there's a successful connection:
  if (client.connect(server, 4000)) {
    if(has_Serial == 1){
      Serial.println("connecting...");
      // send the HTTP PUT request:
      Serial.println("GET /set/"+component+"?state="+String(value)); 
    }
    
    client.println("GET /set/"+component+"?state="+String(value));
    client.println("Connection: close");
    client.println();

    // note the time that the connection was made:
    lastConnectionTime = millis();
  }
  else {
    // if you couldn't make a connection:
    if(has_Serial == 1){
      Serial.println("connection failed");
    }
    
  }
}

extern "C" char *sbrk(int i);

int FreeRam () {
  char stack_dummy = 0;
  return &stack_dummy - sbrk(0);
}

void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  if(has_Serial == 1){
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());
  }

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  if(has_Serial == 1){
    Serial.print("IP Address: ");
    Serial.println(ip);
  }
  // print the received signal strength:
  if(has_Serial == 1){
  long rssi = WiFi.RSSI();
    Serial.print("signal strength (RSSI):");
    Serial.print(rssi);
    Serial.println(" dBm");
  }
}
