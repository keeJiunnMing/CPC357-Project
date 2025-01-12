#include "DHT.h"
#include "VOneMqttClient.h"
#define DHTTYPE DHT11 // DHT11
 
int MinMoistureValue = 4095;
int MaxMoistureValue = 1500;
int MinMoisture = 0;
int MaxMoisture = 100;
int Moisture = 0;
const int SOIL_DRY_THRESHOLD = 30;

int MinDepthValue = 4095;
int MaxDepthValue = 2170;
int MinDepth = 0;
int MaxDepth = 100;
int depth = 0;

//define device id
const char* MoistureSensor = "53df0fe8-248e-4513-b7b1-33c92fbb4af5";
const char* WaterLevelSensor = "24d870d8-d420-4314-98a0-dbc6735ee24d";
const char* DHTSensor = "e0318a00-6908-479d-8f01-8db90e1cd84f";
const char* LedG = "716d7c80-b50a-48f3-9f3b-f31f74d4a60f";
const char* LedR = "94e9619a-cadb-44fa-b895-df2e0db3adb2";
const char* LedY = "a6926442-600c-4d3c-83fa-123088d50e83";
const char* Relay = "aae94ae8-a7f0-42c2-9803-ff519b7bbfdf";

// Used Pins
const int relayPin = 39; // Relay
const int soilPin = A4;  // Soil Sensor
const int depthPin = 5;   // depth Sensor

const int ledPinR = 9;   // Red LED
const int ledPinY = 6;   // Yellow LED
const int ledPinG = 7;   // Green LED

const int dht11Pin = 42; // Digital pin connected to the DHT11 sensor

DHT dht(dht11Pin, DHTTYPE);

//Create an instance of VOneMqttClient
VOneMqttClient voneClient;

//last message time
unsigned long lastMsgTime = 0;
 
void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void triggerActuator_callback(const char* actuatorDeviceId, const char* actuatorCommand) {
  //actuatorCommand format
  Serial.print("Main received callback : ");
  Serial.print(actuatorDeviceId);
  Serial.print(" : ");
  Serial.println(actuatorCommand);

  String errorMsg = "";

  JSONVar commandObjct = JSON.parse(actuatorCommand);
  JSONVar keys = commandObjct.keys();

  if (String(actuatorDeviceId) == Relay) {
    //{"LEDLight":false}
    String key = "";
    bool commandValue = "";
    for (int i = 0; i < keys.length(); i++) {
      key = (const char*)keys[i];
      commandValue = (bool)commandObjct[keys[i]];
      Serial.print("Key : ");
      Serial.println(key.c_str());
      Serial.print("value : ");
      Serial.println(commandValue);
    }

    if (commandValue == true) {
      Serial.println("Relay ON");
      digitalWrite(relayPin, true);
    } else {
      Serial.println("Relay OFF");
      digitalWrite(relayPin, false);
    }

    voneClient.publishActuatorStatusEvent(actuatorDeviceId, actuatorCommand, true);
  }

  if (String(actuatorDeviceId) == LedG) {
    //{"LEDLight":false}
    String key = "";
    bool commandValue = "";
    for (int i = 0; i < keys.length(); i++) {
      key = (const char*)keys[i];
      commandValue = (bool)commandObjct[keys[i]];
      Serial.print("Key : ");
      Serial.println(key.c_str());
      Serial.print("value : ");
      Serial.println(commandValue);
    }

    if (commandValue == true) {
      Serial.println("Green LED ON, Red LED OFF");
      digitalWrite(ledPinG, true);
      digitalWrite(ledPinR, false);  // Turn off Red LED when Green is turned on
      
      // Publish status update for Red LED
      String redOffCommand = "{\"LEDLight\":false}";
      voneClient.publishActuatorStatusEvent(LedR, redOffCommand.c_str(), true);
    } else {
      Serial.println("Green LED OFF");
      digitalWrite(ledPinG, false);
    }
    voneClient.publishActuatorStatusEvent(actuatorDeviceId, actuatorCommand, true);
  }

  if (String(actuatorDeviceId) == LedY) {
    //{"LEDLight":false}
    String key = "";
    bool commandValue = "";
    for (int i = 0; i < keys.length(); i++) {
      key = (const char*)keys[i];
      commandValue = (bool)commandObjct[keys[i]];
      Serial.print("Key : ");
      Serial.println(key.c_str());
      Serial.print("value : ");
      Serial.println(commandValue);
    }

    if (commandValue == true) {
      Serial.println("LED ON");
      digitalWrite(ledPinY, true);
    } else {
      Serial.println("LED OFF");
      digitalWrite(ledPinY, false);
    }
    voneClient.publishActuatorStatusEvent(actuatorDeviceId, actuatorCommand, true);
  }

  if (String(actuatorDeviceId) == LedR) {
    //{"LEDLight":false}
    String key = "";
    bool commandValue = "";
    for (int i = 0; i < keys.length(); i++) {
      key = (const char*)keys[i];
      commandValue = (bool)commandObjct[keys[i]];
      Serial.print("Key : ");
      Serial.println(key.c_str());
      Serial.print("value : ");
      Serial.println(commandValue);
    }

    if (commandValue == true) {
      Serial.println("Red LED ON, Green LED OFF");
      digitalWrite(ledPinR, true);
      digitalWrite(ledPinG, false);  // Turn off Green LED when Red is turned on
      
      // Publish status update for Green LED
      String greenOffCommand = "{\"LEDLight\":false}";
      voneClient.publishActuatorStatusEvent(LedG, greenOffCommand.c_str(), true);
    } else {
      Serial.println("Red LED OFF");
      digitalWrite(ledPinR, false);
    }
    voneClient.publishActuatorStatusEvent(actuatorDeviceId, actuatorCommand, true);
  }
}

void setup() {
  Serial.begin(9600);
  dht.begin();
  
  pinMode(ledPinR, OUTPUT);
  pinMode(ledPinG, OUTPUT);
  pinMode(ledPinY, OUTPUT);
  pinMode(soilPin, INPUT);
  pinMode(depthPin, INPUT);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);
  setup_wifi();
  voneClient.setup();
  voneClient.registerActuatorCallback(triggerActuator_callback);
}

int readSoilSensor() {
  int soilVal = analogRead(soilPin);
  Moisture = map(soilVal, MinMoistureValue, MaxMoistureValue, MinMoisture, MaxMoisture);
  return Moisture;
}

int readWaterLevel() {
  int depthVal = analogRead(depthPin);
  depth = map(depthVal, MinDepthValue, MaxDepthValue, MinDepth, MaxDepth);
  return depth;
}

void readDHTSensor(float &humidity, float &temperature) {
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from the DHT sensor");
    humidity = -1;
    temperature = -1;
  }
}

void loop() {
  if (!voneClient.connected()) {
    voneClient.reconnect();
    String errorMsg = "Sensor Fail";
    voneClient.publishDeviceStatusEvent(MoistureSensor, true);
    voneClient.publishDeviceStatusEvent(WaterLevelSensor, true);
  }
  voneClient.loop();

  unsigned long cur = millis();
  if (cur - lastMsgTime > INTERVAL) {
    lastMsgTime = cur;

    //Publish telemtry Soil moiture
    Moisture = readSoilSensor();
    voneClient.publishTelemetryData(MoistureSensor, "Soil moisture", Moisture);

    //Publish telemtry depth value
    depth = readWaterLevel();
    voneClient.publishTelemetryData(WaterLevelSensor, "Depth", depth);

    //Publish telemtry Humidity, Temperature
    float h = dht.readHumidity();
    int t = dht.readTemperature();

    JSONVar payloadObject;     
    payloadObject["Humidity"] = h;
    payloadObject["Temperature"] = t;
    voneClient.publishTelemetryData(DHTSensor, payloadObject);
  }
}
