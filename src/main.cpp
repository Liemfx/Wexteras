#include <Servo.h>

#include <DHT.h>

 

Servo servo;

 

#define motorPinRightDir  0 // D2

#define motorPinRightSpeed 5 // D1

 

#define DHTPIN 14 // Pin where your DHT11 sensor is connected (D5 on ESP8266)

#define DHTTYPE DHT11 // Specify the DHT sensor type

 

DHT dht(DHTPIN, DHTTYPE);

 

int servoOpenAngle = 0;

int servoClosedAngle = 90;

 

void setup() {

  servo.attach(2); // Attach servo to pin 2 (D4)

  pinMode(motorPinRightDir, OUTPUT);

  pinMode(motorPinRightSpeed, OUTPUT);

  Serial.begin(115200);

  servo.write(90); // Close the hatch initially

  dht.begin();

}

 

void loop() {

  // Read temperature and humidity

  int temperature = dht.readTemperature();

  int humidity = dht.readHumidity();

 

  // Print temperature and humidity to the serial monitor

  Serial.print("Temperature: ");

  Serial.print(temperature);

  Serial.print("°C, Humidity: ");

  Serial.print(humidity);

  Serial.println("%");

 

  if (isnan(temperature) || isnan(humidity)) {

    Serial.println("Failed to read from DHT sensor!");

  }

 

  // Motor and servo control based on temperature and humidity

  if (temperature >= 26.00 && humidity > 60.00) {

    digitalWrite(motorPinRightDir, 0); // Set motor direction

    analogWrite(motorPinRightSpeed, 1024); // Set motor speed

    delay(2500); // Adjust the delay as needed

    digitalWrite(motorPinRightDir, 1); // Reverse motor direction

    analogWrite(motorPinRightSpeed, 1024); // Set motor speed

    delay(2500); // Adjust the delay as needed

    servo.write(0); // Open the hatch (adjust servo angle as needed)

    delay(2500); // Adjust the delay as needed

    servo.write(90); // Close the hatch (adjust servo angle as needed)

    delay(2500); // Adjust the delay as needed

  } else {

    digitalWrite(motorPinRightDir, 1);

    analogWrite(motorPinRightSpeed, 0);

  }

 

  if (humidity > 60.00) {

    servo.write(servoOpenAngle);

  } else {

    servo.write(servoClosedAngle);

  }

}