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
bool autoM = false; // If we add a switch later to choose between auto and semi, If false then semi
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
  Serial.println(autoM);
}

void shoot(void){ // Call this when shoot
  digitalWrite(laser, HIGH); // The laser visual indicator
  irsend.sendNEC(0xFFA25D, 24);
  irrecv.enableIRIn(); 
}

void delayBetween(void){ // Ignore yellow line under delayBetween :]
  if (onof == true){
    d = millis() - timee;
      if (d > offtime){
          onof = false;
        }
  }
}

void fireSelector(void){
  full = digitalRead(trekker);
  if (autoM == true){ // If auto enabled do this
    if (full == HIGH){ // This part till
      if (preventDupe == false){
        dupeTime = millis();
        preventDupe = true;
      }}
      if (preventDupe == true){
        e = millis() - dupeTime; // Ignore yellow line
        if (e > shotDelay){
          preventDupe = false;
        }
      } // Here is for full auto
  } else { // If Semi is enabled do this
    if (autoM == false){
      if (preventDupe == true){
        if (full == LOW){ // Checks whether trigger is NOT pulled
          preventDupe = false; // If so then set preventDupe to false so it can shoot again
        }
      }
    }
  }
}

void loop() {
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
  if (full == HIGH) { // If trigger pulled
    if (safe == LOW){ // Checks if safety is off
      if (onof == false){ // Checks if weapons isn't down/hit
        if (preventDupe == false){ // Checks if the weapon is allowed to fire
          shoot(); // Then shoot
          if (autoM == false){ // If semi is enabled
              preventDupe = true; // Set to true
            }
        }
    }
      } else {
        if (onof == true){
          Serial.println("your down");
        }
      }
  }
  fireSelector(); // Goes to fireSelector function, does something when its semi or auto
  delayBetween(); // This is for the downtime if the weapon got hit, (Sets onof var)
}