// Libraries
#include <Arduino.h>
#include <IRremote.h>

// 32ints
uint32_t Previous; 

// int collection
// Pins
int trekker = 12;
int trekker2 = 11;
int laser = 10; //de emitter zit nu op pin 3 - voor testen is de laser een led maar werkt het zelfde, dus dat is geen probleem
int ontvanger = 7; 
int safety = 6; // Can be changed idk just a random pin I chose
int firemode = 5; // If pressed then semi, else auto

// Vars
int full = 0;
int safe = 0;
int semi = 0;
int offtime = 5000;
int d = 0;
int shotDelay = 92; // Delay between shot with full-auto

// ulong = giant number to count 
unsigned long timee = 0;
unsigned long dupeTime = 0;
unsigned long e = 0;

// Bool(eans) for true false that help with on off and stuff
bool autoM = true; // If we add a switch later to choose between auto and semi
bool onof = false;
bool preventDupe = false;

// Object variables
IRrecv irrecv(ontvanger);
decode_results results;
IRsend irsend;

void setup() {
  Serial.begin(9600);
  irrecv.enableIRIn();
  pinMode(7, INPUT); //ontvanger
  pinMode(10, OUTPUT); //laser
  pinMode(11, OUTPUT); //trekker
  pinMode(12, INPUT); //trekker2
  pinMode(6, OUTPUT); //Safety button
  pinMode(5, OUTPUT); //Semi
}

void loop() {
  Serial.println(onof);
 if (irrecv.decode(&results)) { //if we have received an IR signal
  if (results.value==0x26BB33CE) { 
      results.value=Previous;
      }
  if (onof == false){
        Serial.println("ARGGG I GOT HIT FUCK");
        onof = true;
        timee = millis();
        }

  Serial.println (results.value, HEX); //display HEX results which is very accurate :]
  irrecv.resume(); //next value
 }
  Previous=results.value;
  full = digitalRead(trekker);
  safe = digitalRead(safety);
  semi = digitalRead(firemode);
  digitalWrite(laser, LOW);
  if (full == HIGH){ // If trigger pulled
    if (safe == LOW){
      if (onof == false){
        if (preventDupe == false){
          digitalWrite(laser, HIGH); // The laser visual indicator
          Serial.println("BoomStick");
          irsend.sendNEC(0xFFA25D, 24);
          irrecv.enableIRIn();
        }
    }
      } else {
        if (onof == true){
          Serial.println("your down");
        }
      }
  
  }

  if (autoM == false){
    if (full == HIGH){
      if (preventDupe == false){
        dupeTime = millis();
        preventDupe = true;
      }
    }
    if (preventDupe == true){
      e = millis() - dupeTime;
      if (e > shotDelay){
          preventDupe = false;
        }
      }
    } else {
        if (full == HIGH){
            preventDupe = true;
          } else {
            if (full == LOW){
              preventDupe = false;
            }
              
            }
      }
    
  if (onof == true){
    d = millis() - timee;
      if (d > offtime){
          onof = false;
        }
  }
}