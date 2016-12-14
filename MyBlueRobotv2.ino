#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"
#include <Servo.h>
#include <NewPing.h>
#include <SoftwareSerial.h>
#include <GOFi2cOLED.h>

#define TRIGGER_PIN 1
#define ECHO_PIN 0
#define TRIGGER_PIN2 5
#define ECHO_PIN2 4
#define TRIGGER_PIN3 7
#define ECHO_PIN3 6
#define MAX_DISTANCE 200

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield();

Adafruit_DCMotor *motor1 = AFMS.getMotor(1);  //RIGHT MOTOR
Adafruit_DCMotor *motor2 = AFMS.getMotor(2);  //LEFT MOTOR
Adafruit_DCMotor *motor3 = AFMS.getMotor(3);  //Gripper Motor

Servo servo1;  //Tilt Servo

GOFi2cOLED GOFoled;   //OLED DISPLAY

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);  //CENTER PING SENSOR
NewPing sonar2(TRIGGER_PIN2, ECHO_PIN2, MAX_DISTANCE);  //RIGHT PING SENSOR
NewPing sonar3(TRIGGER_PIN3, ECHO_PIN3, MAX_DISTANCE);  //LEFT PING SENSOR



int distance;  
int distanceRight;
int distanceLeft;
int bluetoothTx = 9;
int bluetoothRx = 8;
const int ledPin = 12;
int ledState = LOW;

SoftwareSerial bluetooth(bluetoothTx, bluetoothRx);

void setup() {
  // put your setup code here, to run once:
  servo1.attach(10);
  pinMode(ledPin, OUTPUT);
  GOFoled.init(0x3C);
  GOFoled.display();
  delay(2000);
  GOFoled.clearDisplay();
  GOFoled.setTextSize(2);
  GOFoled.setTextColor(WHITE);
  GOFoled.setCursor(0,0);
  GOFoled.println("Hello!");
  GOFoled.setTextSize(1);
  GOFoled.println("Test to determine total number of characters to display!"); 
//  GOFoled.println("TEST LINE 3");  
//  GOFoled.setTextColor(BLACK, WHITE); // 'inverted' text
//  GOFoled.println("LINE 4");
//  GOFoled.setTextSize(2);
//  GOFoled.setTextColor(WHITE);
  //GOFoled.print("0x"); GOFoled.println(0xDEADBEEF, HEX);
//  GOFoled.println("Line 5");
  GOFoled.display();
  delay(2000);
  GOFoled.clearDisplay();
  GOFoled.setTextSize(2);
  GOFoled.setCursor(0,0);
  GOFoled.println("I LOVE YOU SARAH");
  GOFoled.display();
  
  Serial.begin(9600); // begin serial communitication
  Serial.println("Motor test!");
  bluetooth.begin(115200);
  bluetooth.print("$$$");
  delay(100);
  bluetooth.println("U,9600,N");
  bluetooth.begin(9600);
   AFMS.begin();  // create with the default frequency 1.6KHz
  //AFMS.begin(1000);  // OR with a different frequency, say 1KHz
  motor1->setSpeed(200); //set the speed of the motors, between 0-255
  motor2->setSpeed (162);
  motor3->setSpeed(200);

}


bool stoploop = false;
int i;
int x;
String content = "";
char character;

void loop() {
  // put your main code here, to run repeatedly:
  
  //NEW CODE BLOCK
  content = "";
  if(bluetooth.available()) {
   
    char toSend = (char)bluetooth.read();
    character = Serial.read();
    content.concat(toSend);
    Serial.print(toSend);
    digitalWrite(ledPin, ledState);
    //Serial.print("Bluetooth available!");
//    if (content != "") {
//      GOFoled.clearDisplay();
//      GOFoled.setCursor(0,0);
//      GOFoled.println(content);
//      GOFoled.display();
//      
//    }
    
    
    
    
    
    if(toSend == 'F') {
      motor1->run(FORWARD);
      motor2->run(FORWARD);
    }
   
    else if(toSend == 'S') {
      motor1->run(RELEASE);
      motor2->run(RELEASE);
    }
    else if(toSend == 'R') {
      motor1->run(BACKWARD);
      motor2->run(FORWARD);
    }
    else if(toSend == 'L') {
      motor1->run(FORWARD);
      motor2->run(BACKWARD);
    }
    else if(toSend == 'B') {
      motor1->run(BACKWARD);
      motor2->run(BACKWARD);
    }
    else if(toSend == 'U') {
      servo1.write(75);
    }
    else if(toSend == 'D') {
      servo1.write(90);
    }
    else if(toSend == 'G') {
      motor3->run(FORWARD);
      delay(1200);
      motor3->run(RELEASE);
    }
    else if(toSend == 'O') {
      motor3->run(BACKWARD);
      delay(1200);
      motor3->run(RELEASE);
    }
    else if(toSend == 'V') {
      motor3->run(RELEASE);
    }
    else if(toSend == 'H') {
      ledState = HIGH;
    }
    else if(toSend == 'J') {
      ledState = LOW;
    }
    else if(toSend == 'A') {
      stoploop = false;
      while (stoploop != true) { 
        char toSend2 = (char)bluetooth.read();
        Serial.print(toSend2);
        Serial.print("Blue working!");
        delay(100);
        distance = sonar.ping_cm();
        distanceRight = sonar2.ping_cm();
        distanceLeft = sonar3.ping_cm();
        if (((distance < 30) && (distance >= 1)) || ((distanceRight < 15) && (distance >= 1))) {
          motor1->run(FORWARD);  // Turn as long as there's an obstacle ahead.
          motor2->run (BACKWARD);
        }
        else if (((distance < 30) && (distance >=1)) || ((distanceLeft < 14) && (distanceLeft >= 1))) {
          motor1->run(BACKWARD);
          motor2->run(FORWARD); 
        }
        else if (toSend2 == 'C') {
          stoploop = true;
        }
        else {
          delay (15);
          motor1->run(FORWARD); //if there's no obstacle ahead, Go Forward!
          motor2->run(FORWARD);
        }
      }
    }
  }

  if(Serial.available()) {
    char toSend = (char)Serial.read();
    bluetooth.print(toSend);
    bluetooth.print("Bluetooth available! 2nd");
  }
}
