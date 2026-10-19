#define BLYNK_TEMPLATE_ID "TMPL2oNxElFgv"
#define BLYNK_TEMPLATE_NAME "Smart Irrigation"
#define BLYNK_AUTH_TOKEN "aVkZqkf1sYOKzmlANr7ZzTkp0PWOruj3"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Servo.h>

// Wi-Fi
char ssid[] = "Redmi Pad 2 4G";
char pass[] = "7017790779";

// Pins
#define SOIL_PIN A0
#define RELAY_PIN D1
#define SERVO_PIN D2

// Servo
Servo sprinklerServo;

// Blynk timer
BlynkTimer timer;

// Moisture thresholds
int DRY_THRESHOLD = 500;
int WET_THRESHOLD = 600;

// Current system state
bool watering = false;

// Servo position
int servoAngle = 30;
int servoDirection = 1;

int moisture = analogRead(SOIL_PIN);

// --------------------------------------------------
// Read soil moisture and control irrigation
// --------------------------------------------------

void checkSoil() {

  int moisture = analogRead(SOIL_PIN);

  Serial.print("Soil Moisture: ");
  Serial.println(moisture);

  // Send moisture value to Blynk
  Blynk.virtualWrite(V0, moisture);

//
//  // -----------------------------------------------
//  // SOIL IS DRY
//  // -----------------------------------------------
//
//

   
  if (moisture < DRY_THRESHOLD && !watering) {

    watering = true;
    
//    // Turn pump ON
    digitalWrite(RELAY_PIN, HIGH);
//    delay(1000);
//    digitalWrite(RELAY_PIN, LOW);
//    delay(1000); 
//    digitalWrite(RELAY_PIN, HIGH);
//    delay(1000);
//    digitalWrite(RELAY_PIN, LOW);
//    delay(1000);
 
    

   

    // Blynk status
    Blynk.virtualWrite(V1, "🔴 SOIL DRY");

    Blynk.virtualWrite(V2, 255);   // Red indicator

    // Send notification
    Blynk.logEvent(
      "soil_dry",
      "Soil moisture is low. Irrigation has started."
    );

    Serial.println("SOIL DRY - PUMP ON");
  }


  // -----------------------------------------------
  // SOIL IS WET
  // -----------------------------------------------

  if (moisture > WET_THRESHOLD && watering) {

    watering = false;

    // Turn pump OFF
    digitalWrite(RELAY_PIN, LOW);

    // Stop servo at center
    sprinklerServo.write(90);

    // Blynk status
    Blynk.virtualWrite(V1, "🟢 SOIL WET");

    Blynk.virtualWrite(V2, 0);

    // Send notification
    Blynk.logEvent(
      "soil_wet",
      "Soil moisture is sufficient. Irrigation has stopped."
    );

    Serial.println("SOIL WET - PUMP OFF");
  }
}


// --------------------------------------------------
// Move sprinkler servo
// --------------------------------------------------

void moveSprinkler() {

  if (watering) {

    sprinklerServo.write(servoAngle);

    servoAngle += servoDirection;
    
    if (servoAngle >= 150) {
      servoDirection = -1;
    }

    if (servoAngle <= 85) {
      servoDirection = 1;
    }
  }
}


// --------------------------------------------------
// Setup
// --------------------------------------------------

void setup() {

  Serial.begin(115200);

  // Relay
  pinMode(RELAY_PIN, OUTPUT);

  // Pump OFF initially
  digitalWrite(RELAY_PIN, LOW);

  // Servo
  sprinklerServo.attach(SERVO_PIN);
  sprinklerServo.write(85);

  // Connect to Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Check soil every 2 seconds
  timer.setInterval(2000L, checkSoil);

  // Move servo every 30 milliseconds
  timer.setInterval(30L, moveSprinkler);

  Serial.println("Irrigation System Started");
}


// --------------------------------------------------
// Main Loop
// --------------------------------------------------

void loop() {

  Blynk.run();

  timer.run();
  

}
