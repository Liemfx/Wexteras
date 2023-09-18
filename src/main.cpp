#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Servo.h>

// Pin assignments
#define DHTPIN 2           // Pin for DHT22 sensor
#define SERVO_PIN 3        // Pin for servo motor control
#define FAN_PIN 4          // Pin for fan motor control
#define WATER_PUMP_PIN 5   // Pin for water pump control

// Constants
#define DHTTYPE DHT11      // DHT sensor type
#define TEMPERATURE_THRESHOLD 25.0  // Adjust as needed
#define HUMIDITY_THRESHOLD 60.0     // Adjust as needed
#define FAN_THRESHOLD 28.0          // Temperature threshold to turn on the fan
#define WATERING_INTERVAL 60000     // Watering interval in milliseconds (e.g., 1 minute)

DHT_Unified dht(DHTPIN, DHTTYPE);
Servo servo;
bool isFanOn = false;
unsigned long lastWateringTime = 0;

void setup() {
  Serial.begin(9600);

  // Initialize sensors
  dht.begin();

  // Attach servo
  servo.attach(SERVO_PIN);

  // Set pin modes
  pinMode(FAN_PIN, OUTPUT);
  pinMode(WATER_PUMP_PIN, OUTPUT);

  // Connect to Wi-Fi and Firebase here (not included in this example)
}

void loop() {
  // Read temperature and humidity
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  float temperature = event.temperature;
  dht.humidity().getEvent(&event);
  float humidity = event.relative_humidity;

  // Check temperature and humidity thresholds
  if (temperature > TEMPERATURE_THRESHOLD) {
    // If temperature is too high, open the roof hatch (adjust angles accordingly)
    servo.write(90);
  } else {
    // Close the roof hatch if the temperature is within the threshold
    servo.write(0);
  }

  // Check humidity and activate the watering system if needed
  if (humidity < HUMIDITY_THRESHOLD && millis() - lastWateringTime > WATERING_INTERVAL) {
    // Turn on the water pump for a brief moment
    digitalWrite(WATER_PUMP_PIN, HIGH);
    delay(1000);  // Run the pump for 1 second (adjust as needed)
    digitalWrite(WATER_PUMP_PIN, LOW);
    lastWateringTime = millis();
  }

  // Check temperature to control the fan
  if (temperature > FAN_THRESHOLD) {
    if (!isFanOn) {
      // Turn on the fan if the temperature is above the threshold
      digitalWrite(FAN_PIN, HIGH);
      isFanOn = true;
    }
  } else {
    // Turn off the fan if the temperature is within the threshold
    digitalWrite(FAN_PIN, LOW);
    isFanOn = false;
  }

  // Send data to Firebase here (not included in this example)

  delay(10000);  // Delay for 10 seconds before the next reading
}
