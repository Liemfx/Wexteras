#include <Servo.h>
#include <Firebase_ESP_Client.h>
#include <Adafruit_Sensor.h>
#include <ESP8266WiFi.h>
#include <DHT.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#include <TimeAlarms.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#define WIFI_SSID "ABB_Gym_IOT"
#define WIFI_PASSWORD "Welcome2abb"
#define API_KEY "AIzaSyCJO5qCb3U2pxThR1h0ZcNhw25za4rGNfY"
#define DATABASE_URL "https://wexteras-blv-default-rtdb.europe-west1.firebasedatabase.app/"
bool signupOK = false;

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

Servo servo;

#define motorPinRightDirA D0 // D2 for Motor A
#define motorPinRightSpeedA D1 // D1 for Motor A

#define motorPinRightDirB D2 // D3 for Motor B
#define motorPinRightSpeedB D3 // D4 for Motor B

#define DHTPIN D5
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
WiFiUDP udp;
int servoOpenAngle = 190;
int servoClosedAngle = 0;
bool luckaOpen = false;
bool flaktPa = false;
const int startHour = 12; // 12:00 PM
const int startMinute = 0;
const long utcOffsetInSeconds = 7200;
const char* ntpServerName = "pool.ntp.org";
const int ntpServerPort = 123;
NTPClient timeClient(udp, ntpServerName, utcOffsetInSeconds, ntpServerPort);
bool motorStarted = false;

void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  servo.attach(D6); // Attach servo to pin D6

  pinMode(motorPinRightDirA, OUTPUT);
  pinMode(motorPinRightSpeedA, OUTPUT);

  pinMode(motorPinRightDirB, OUTPUT);
  pinMode(motorPinRightSpeedB, OUTPUT);

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Firebase Sign Up successful.");
    signupOK = true;
  } else {
    Serial.printf("Firebase Sign Up Error: %s\n", config.signer.signupError.message.c_str());
  }

  dht.begin();

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  timeClient.begin();
  timeClient.setTimeOffset(7200);
  Serial.begin(115200);
}

void loop() {
timeClient.update();

  // Get the current time
  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();
  int temperature = dht.readTemperature();
  int humidity = dht.readHumidity();

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print("°C, Humidity: ");
  Serial.print(humidity);
  Serial.println("%");

  Firebase.RTDB.setInt(&fbdo, "dht/temperature", temperature);
  Firebase.RTDB.setInt(&fbdo, "dht/humidity", humidity);
  Firebase.RTDB.setBool(&fbdo, "dht/luckaopen", luckaOpen);
  Firebase.RTDB.setBool(&fbdo, "dht/flaktpa", flaktPa);

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
  }
   
// vattna 1 gång om dagen
  if (!motorStarted && currentHour == startHour && currentMinute == startMinute) {
  digitalWrite(motorPinRightDirA, 1); // Set motor A direction forward
   analogWrite(motorPinRightSpeedA, 100); // Set motor A speed
   delay(1500);
   digitalWrite(motorPinRightDirA, 0); // Set motor A direction forward
   analogWrite(motorPinRightSpeedA, 0);
   delay(1500);
   digitalWrite(motorPinRightDirA, 1); // Set motor A direction forward
   analogWrite(motorPinRightSpeedA, 100); // Set motor A speed
   delay(1500);

   digitalWrite(motorPinRightDirA, 0); // Set motor A direction forward
   analogWrite(motorPinRightSpeedA, 0);
   delay(300);
  }



  // Motor B control based on temperature and humidity
  if (temperature >= 26 && humidity > 80) {
    digitalWrite(motorPinRightDirB, 1); // Set motor B direction forward
    analogWrite(motorPinRightSpeedB, 1000); // Set motor B speed
    flaktPa = true;
  }
 else {
    digitalWrite(motorPinRightDirB, 0);
    analogWrite(motorPinRightSpeedB, 0);
    flaktPa = false;
  }


  //vattna om det blir för torrt inne
   if (humidity < 25) {
   digitalWrite(motorPinRightDirA, 1); // Set motor A direction forward
   analogWrite(motorPinRightSpeedA, 100); // Set motor A speed
   delay(1500);
   digitalWrite(motorPinRightDirA, 0); // Set motor A direction forward
   analogWrite(motorPinRightSpeedA, 0);
   delay(1500);
   digitalWrite(motorPinRightDirA, 1); // Set motor A direction forward
   analogWrite(motorPinRightSpeedA, 100); // Set motor A speed
   delay(1500);

   digitalWrite(motorPinRightDirA, 0); // Set motor A direction forward
   analogWrite(motorPinRightSpeedA, 0);
   delay(300);
  }   
  if (humidity > 80) {
    servo.write(servoOpenAngle);
    luckaOpen = true;
  } 
  else{
    servo.write(servoClosedAngle);
    luckaOpen = false;
  }

  
}