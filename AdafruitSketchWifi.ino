#include <SPI.h>
#include <WiFi101.h>
#include <ArduinoJson.h>


const int airPump = 10;     //pin of the water pump
const int hygrometer = A1;  //pin of the hygrometer measuring soil humidity
const int waterlevel = A2;  //pin of water sensor measuring water level

const int wateringInterval = 1500   //watering interval in miliseconds
const int lowSoilHumdity = 20       //below that point of soil humdity, the pot automatically waters itself

const int has_Serial = 0;   //0 = Serial connection disabled, 1 = Serial connection enabled

char ssid[] = "your-ssid";      // your network SSID (name)
char pass[] = "your-password";  // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;               // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;

// Initialize the WiFi client library
WiFiClient client;

// server address:
char server[] = "localhost";

unsigned long lastConnectionTime = 0;            // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 2L * 1000L; // delay between updates, in milliseconds

void setup() {
  //Initialize serial and wait for port to open:
  WiFi.setPins(8, 7, 4, 2); //setting wifi pins, necessary for Adafruit

  //start serial connection if enabled
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

    // wait 6 seconds for connection:
    delay(6000);
  }
  printWiFiStatus();

  //setting pin modes
  pinMode(hygrometer, INPUT_PULLUP);
  pinMode(waterlevel, INPUT_PULLUP);
  pinMode(airPump,OUTPUT);
}


void loop() {
  //receive full message
  char fullmessage [126];
  const char *openbracket = "[";
  const char *closebracket = "]";
  //char message;
  bool startcopy = false;
  int index = 0;

  //read message (this can be done better surely!)
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
        fullmessage[index] = message; // Store char
        index++;
      }
    }
    if (strchr(openbracket, message))
    {
      startcopy = true;
    }
  }


  if (millis() - lastConnectionTime > postingInterval) {
    //get hygrometer value
    int soilhumidity = analogRead(hygrometer);
    httpRequestSet(soilhumidity,"SoilHumidity");
    //get waterlevel value
    int waterlevel_value = analogRead(waterlevel);
    httpRequestSet(waterlevel_value,"Waterlevel");

    //print out values if serial is enabled
    if(has_Serial == 1){
      Serial.print("Soil Humidity: ");
      Serial.println(soilhumidity);
      Serial.print("WaterLevel: ");
      Serial.println(waterlevel_value);
    }

    //decode string to json
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(fullmessage);

    //get waterpump value: 0 = waterpump off, 1 = waterpump on
    int isWaterPumpOn = root["value"];

    // if water pump should be turned off
    if (isWaterPumpOn == 0.0) {
      //turn water pump off
      digitalWrite(airPump,LOW);
      //if soil humidity is low enough, the system automatically turns on pump
      if(soilhumidity<lowSoilHumdity){
        httpRequestSet(1,"WaterPump");
        digitalWrite(airPump,HIGH);
        //let pump run for 1.5 sec
        delay(wateringInterval);
        digitalWrite(airPump, LOW);
        delay(400);
        httpRequestSet(0,"WaterPump");
      }
    }
    // if water pump should be turned on
    if (isWaterPumpOn == 1.0) {
      //let pump run for 1.5 sec
      digitalWrite(airPump,HIGH);
      delay(wateringInterval);
      digitalWrite(airPump, LOW);
    }

    if(has_Serial == 1){
      Serial.print("WaterPump: ");
      Serial.println(value);
    }
    //requests for new water pump value
    httpRequestGet("WaterPump");

  }

}

// HTTP connection to the server:
void httpRequestGet(String component) {
  // close any connection before send a new request.
  // This will free the socket on the WiFi shield
  client.stop();

  // if there's a successful connection:
  if (client.connect(server, 4000)) {
    if(has_Serial == 1){
      Serial.println("connecting...");
    }

    // send the HTTP PUT request for component
    client.println("GET /"+component);
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

//HTTP connection to the server:
void httpRequestSet(int value, String component) {
  // close any connection before send a new request.
  // This will free the socket on the WiFi shield
  client.stop();

  // if there's a successful connection:
  if (client.connect(server, 4000)) {
    //print on serial if enabled
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


// print the SSID of the network you're attached to:
void printWiFiStatus() {
  //print if serial is enabled
  if(has_Serial == 1){
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());

    // print your WiFi shield's IP address:
    IPAddress ip = WiFi.localIP();
    Serial.print("IP Address: ");
    Serial.println(ip);

    // print the received signal strength:
    long rssi = WiFi.RSSI();
    Serial.print("signal strength (RSSI):");
    Serial.print(rssi);
    Serial.println(" dBm");
  }
}
