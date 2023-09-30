#include <Servo.h>
#include <Firebase_ESP_Client.h>
#include <Adafruit_Sensor.h>
#include <ESP8266WiFi.h>
#include <DHT.h>
//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"
 
#define WIFI_SSID "ABB_Gym_IOT"
#define WIFI_PASSWORD "Welcome2abb"

#define API_KEY "AIzaSyCJO5qCb3U2pxThR1h0ZcNhw25za4rGNfY"

#define DATABASE_URL "https://wexteras-blv-default-rtdb.europe-west1.firebasedatabase.app/" 

bool signupOK = false;
//Define Firebase Data object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

Servo servo;

 

#define motorPinRightDir  D2 // D2

#define motorPinRightSpeed D1 // D1 

#define DHTPIN D5 // Pin where your DHT11 sensor is connected (D5 on ESP8266)

#define DHTTYPE DHT11 // Specify the DHT sensor type

 

DHT dht(DHTPIN, DHTTYPE);

 

int servoOpenAngle = 180;

int servoClosedAngle = 0;

bool luckaOpen = false;

bool flaktPa = false;
 

void setup() {


  Serial.begin(115200);
   WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  servo.attach(2); // Attach servo to pin 2 (D4)

  pinMode(motorPinRightDir, OUTPUT);

  pinMode(motorPinRightSpeed, OUTPUT);
 
  config.api_key = API_KEY;
  
  config.database_url = DATABASE_URL;

if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  dht.begin();



  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

}

 

void loop() {




  int temperature = dht.readTemperature();
  int humidity = dht.readHumidity();
  
  Serial.print(temperature);

  Serial.print("°C, Humidity: ");

  Serial.print(humidity);

  Serial.println("%");

 Firebase.RTDB.setInt(&fbdo, "dht/temperature", temperature); 
 Firebase.RTDB.setInt(&fbdo, "dht/humidity", humidity);   
 Firebase.RTDB.setBool(&fbdo, "dht/luckaopen", luckaOpen);   
 Firebase.RTDB.setBool(&fbdo, "dht/flaktpa", flaktPa); 
  Serial.print("Temperature: ");

  
 

  if (isnan(temperature) || isnan(humidity)) {

    Serial.println("Failed to read from DHT sensor!");

  }

 

  // Motor and servo control based on temperature and humidity

  if (temperature >= 26 && humidity > 60) {

    digitalWrite(motorPinRightDir, 1); // Set motor direction

    analogWrite(motorPinRightSpeed, 512); // Set motor 
    
    flaktPa = true;
  
  } else {

    digitalWrite(motorPinRightDir, 0);

    analogWrite(motorPinRightSpeed, 0);

    flaktPa = false;
  }

 

  if (humidity > 60) {

    servo.write(servoOpenAngle);
    luckaOpen = true;
  } 
  else {

    servo.write(servoClosedAngle);
    luckaOpen = false;
  }


}