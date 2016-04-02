// Sourced from: https://www.hackster.io/gusgonnet/garage-commander-30383a

// IO mapping
// D0 : relay: 
// D1 : relay
// D2 : relay
// D3 : relay: garage_BUTTON - this output is connected to the garage wall button or the garage remote
// D4 : garage_CLOSE : high means contact open (garage door opened), low means contact closed (garage door closed)
// D5 : garage_OPEN : high means contact open (garage door opened), low means contact closed (garage door closed)
// D6 :
// D7 :

// A0 :
// A1 : 
// A2 : 
// A3 : 
// A4 :
// A5 :
// A6 :
// A7 :

#include "application.h"

String _version = "0.30";

#define GarageDoorStatus "GarageDoorStatus"

bool DEBUG = true;

#define GARAGE_READ_INTERVAL 1000
#define GARAGE_OPEN "open"
#define GARAGE_CLOSED "closed"
#define GARAGE_OPENING "opening"
#define GARAGE_CLOSING "closing"
 
int ANALOG_READ_THRESHOLD = 100;

unsigned long garage_interval = 0;
int garage_BUTTON = D3;
int garage_CLOSE = A5; // Normall Closed
//int garage_OPEN = A4; // Normally Open

String garage_status_string = "unknown";
bool garageDoorClosed_State = true;

void setup() {

 Particle.publish("device starting", "Firmware version: " + _version, 60, PRIVATE);

  pinMode(garage_BUTTON, OUTPUT);
//  pinMode(garage_OPEN, INPUT);
  pinMode(garage_CLOSE, INPUT_PULLUP);
  
  bool success = Particle.function("garage_open", garage_open);
  if (not success) {
      Particle.publish("ERROR", "Failed to register function garage_open", 60, PRIVATE);
  }
  
   success = Particle.function("garage_close", garage_close);
  if (not success) {
      Particle.publish("ERROR", "Failed to register function garage_close", 60, PRIVATE);
  }
  
  success = Particle.function("garage_stat", garage_stat);
  if (not success) {
      Particle.publish("ERROR", "Failed to register function garage_stat", 60, PRIVATE);
  }

}

void loop() {
 
  //read garage status every now and then
  if( millis() - garage_interval >= GARAGE_READ_INTERVAL ) {
    garage_read();
    garage_interval = millis();
  }

}

/*******************************************************************************
 * Function Name  : garage_open
 * Description    : garage_BUTTON goes up for one second
 * Return         : 0
 *******************************************************************************/
int garage_open(String args)
{
    // simulate 1 second button push
     if(digitalRead(garage_CLOSE)) {
        garage_button();
        return 0;
     }
    
    return 1;
}

int garage_close(String args) 
{
    if(!digitalRead(garage_CLOSE)) {
        garage_button();
        return 0;
    }
    
    return 1;
}

void garage_button()
{
    digitalWrite(garage_BUTTON, HIGH);
    delay(1000);
    digitalWrite(garage_BUTTON, LOW);
}

/*******************************************************************************
 * Function Name  : garage_read()
 * Description    : reads and publishes the status of the garage, intended for using it with a service like ifttt
 * Return         : 0
 *******************************************************************************/
int garage_read()
{
    int doorClosedReedSwitchValue = digitalRead(garage_CLOSE);
    bool previousDoorState = garageDoorClosed_State;
    
    if (doorClosedReedSwitchValue) {
        // this means the reed switch that is normally closed is open, meaning the door is closed.
        garage_status_string = GARAGE_CLOSED;
        garageDoorClosed_State = true;
    } else {
        // the reed switch that is normally closed is now open, meaning the door is open;
        garage_status_string = GARAGE_OPEN;
        garageDoorClosed_State = false;
    }
    
    // Garge door state changed.
    if(previousDoorState != garageDoorClosed_State) {
            Particle.publish(GarageDoorStatus, "Moving", 60, PRIVATE);
    }
    
    return 0;
}

/*******************************************************************************
 * Function Name  : garage_stat  // function name cannot be longer than 12 chars otherwise it wont be registered!
 * Description    : reads and publishes the status of the garage, intended for using it with a service like ifttt or pushbullet
 * Return         : 0
 *******************************************************************************/
int garage_stat(String args)
{
    publishGarageStatus();
    return 0;
    
}

void publishGarageStatus() {
    
     if(digitalRead(garage_CLOSE)) {
         Particle.publish(GarageDoorStatus, "Closed", 60, PRIVATE);
     } else {
         Particle.publish(GarageDoorStatus, "Open", 60, PRIVATE);
     }
     
}
