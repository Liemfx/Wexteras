#include <Arduino.h>
#include <Servo.h>
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>

#define WIFI_SSID "Your_WiFi_SSID"
#define WIFI_PASSWORD "Your_WiFi_Password"
#define FIREBASE_HOST "your-firebase-project.firebaseio.com"
#define FIREBASE_AUTH "your-firebase-auth-token"

#define DHTPIN 2 // Pin where your DHT sensor is connected
#define DHTTYPE DHT11 // Change to DHT11 if you're using that sensor
#define FAN_PIN 5 // Pin for controlling the fan
#define SERVO_PIN 4 // Pin for controlling the servo

DHT dht(DHTPIN, DHTTYPE);
Servo servo;

const int TEMP_THRESHOLD = 25; // Adjust this temperature threshold as needed
const int HUMIDITY_THRESHOLD = 60; // Adjust this humidity threshold as needed

FirebaseData firebaseData;
bool fanStatus = false;

void setup()
{
  Serial.begin(115200);
  dht.begin();
  servo.attach(SERVO_PIN);
  pinMode(FAN_PIN, OUTPUT);
  servo.write(90); // Close the hatch initially

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
}

void loop()
{
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity))
  {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" °C, Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  // Check temperature and humidity thresholds
  if (temperature > TEMP_THRESHOLD)
  {
    // It's too hot, turn on the fan
    digitalWrite(FAN_PIN, HIGH);
    fanStatus = true;
  }
  else
  {
    // It's not too hot, turn off the fan
    digitalWrite(FAN_PIN, LOW);
    fanStatus = false;
  }

  if (humidity < HUMIDITY_THRESHOLD)
  {
    // Humidity is too low, activate the watering system (implement this part)
    // You may need a relay or transistor to control the water pump
    // Code for watering goes here
  }

  // Upload data to Firebase
  Firebase.setString(firebaseData, "/temperature", String(temperature));
  Firebase.setString(firebaseData, "/humidity", String(humidity));
  Firebase.setBool(firebaseData, "/fanStatus", fanStatus);

  if (Firebase.failed())
  {
    Serial.println("Firebase upload failed");
    return;
  }

  delay(5000); // Read and control every 5 seconds (adjust as needed)
}
