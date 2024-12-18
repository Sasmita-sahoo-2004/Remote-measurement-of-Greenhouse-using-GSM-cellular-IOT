#include <SoftwareSerial.h>
#include <DHT.h>

SoftwareSerial sim(9, 10);
DHT dht(7, DHT22); // DHT22 connected to pin 7

const int soilMoisturePin = A0; // Analog pin for soil moisture sensor
const int gasSensorPin = A2;    // Analog pin for gas sensor
const int ldrPin = A1;          // Analog pin for LDR
const int fanControlPin = 12;   // Digital pin to control the DC fan
const int motorControlPin = 11; // Digital pin to control the DC motor
const int lightControlPin = 13; // Digital pin to control the glowing light

void setup() {
  pinMode(fanControlPin, OUTPUT); // Set the fan control pin as an output
  pinMode(motorControlPin, OUTPUT); // Set the motor control pin as an output
  pinMode(lightControlPin, OUTPUT); // Set the light control pin as an output
  
  Serial.begin(9600);
  sim.begin(9600);
  dht.begin();

  Serial.println("Initializing...");
  delay(1000);
  
  sim.println("AT");
  updateSerial();
  
  sim.println("AT+CSQ");
  updateSerial();
  
  sim.println("AT+CCID");
  updateSerial();
  
  sim.println("AT+CREG?");
  updateSerial();
  
  sim.println("AT+CMGF=1");
  updateSerial();
}

void loop() {
  updateSerial();
  
  // Read sensor values and control fan, motor, and light here
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  int soilMoistureRaw = analogRead(soilMoisturePin);
  int gasSensorRaw = analogRead(gasSensorPin);
  int ldrValue = analogRead(ldrPin);

  // Convert soil moisture to percentage (0-100%)
  float soilMoisture = map(soilMoistureRaw, 0, 1023, 0, 100);

  // Convert gas sensor value to ppm (adjust the formula based on your sensor's characteristics)
  float gasValuePPM = map(gasSensorRaw, 0, 1023, 0, 1000); // Adjust the range based on your sensor
  
  // Control the fan based on temperature and gas sensor values
  if (temperature > 22.0 && gasValuePPM > 200) {
    digitalWrite(fanControlPin, HIGH); // Turn on the fan
  } else {
    digitalWrite(fanControlPin, LOW);  // Turn off the fan
  }
  
  // Control the motor based on humidity value
  if (humidity < 70.0) {
    digitalWrite(motorControlPin, HIGH); // Turn on the motor
  } else {
    digitalWrite(motorControlPin, LOW);  // Turn off the motor
  }
  
  // Control the light based on LDR value
  if (ldrValue < 200) {
    digitalWrite(lightControlPin, HIGH); // Turn on the light
  } else {
    digitalWrite(lightControlPin, LOW);  // Turn off the light
  }
  
  // Display sensor values
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" °C, Humidity: ");
  Serial.print(humidity);
  Serial.print(" %, Soil Moisture: ");
  Serial.print(soilMoisture);
  Serial.print(" %, Gas Value: ");
  Serial.print(gasValuePPM);
  Serial.print(" ppm, LDR Value: ");
  Serial.println(ldrValue);
  
  // Sending SMS with sensor values
  sim.println("AT+CMGS=\"+917606878264\"");
  updateSerial();
  
  String message = "Temperature: " + String(temperature, 2) + "C, Humidity: " + String(humidity, 2) + "%, Soil Moisture: " + String(soilMoisture, 2) + "%, Gas Value: " + String(gasValuePPM, 2) + " ppm, LDR Value: " + String(ldrValue);
  
  sim.print(message);
  updateSerial();
  sim.write(26);
  updateSerial();
  
  Serial.println();
  Serial.println("Message Sent");
}

void updateSerial() {
  delay(500);
  while (Serial.available()) {
    sim.write(Serial.read());
  }
  while (sim.available()) {
    Serial.write(sim.read());
  }
}
