/*
MIT license 
ch010402 16.10.2020
*/

#include <wiringPi.h> // library to access the GPIO Pins on a raspberryPI !!depriciated!!
#include <iostream>   // cout
#include <fstream>    // file stream access
#include <string>     // string class
#include <sstream>    // string stream needed for file access to put data into string

using namespace std;

//----- classes
class pump {
  public:
    int pin;
    // constructor
    pump(int p) {
      pin = p;
    }
    // methodes
    void on() {
      bool newStatus = true;
      if (!initialized) initialize();
      if (newStatus == oldStatus) return;
      digitalWrite(pin, LOW);
      cout << this <<" Pumpe eingeschaltet" << endl;
      oldStatus = newStatus;
    }
    void off() {
      bool newStatus = false;
      if (!initialized) initialize();
      if (newStatus == oldStatus) return;
      digitalWrite(pin, HIGH);
      cout << "Pumpe ausgeschaltet" << endl;
      oldStatus = newStatus;
    }
    private:
      bool initialized = false; // default false 
      bool oldStatus = false; // false = off true = on; default false as initialized
      void initialize() {
        wiringPiSetup();
        pinMode(pin,OUTPUT);
        initialized = true;
      }
};

class valve {
  public:
    int pin;
    // constructor
    valve(int v){
      pin = v;
    }
    // methodes
    void open() {
      bool newStatus = true;
      if (!initialized) initialize();
      if (newStatus == oldStatus) return;
      digitalWrite(pin, LOW);
      cout << "Ventil geöffnet" << endl;
      oldStatus = newStatus;
    }
    void close() {
      bool newStatus = false;
      if (!initialized) initialize();
      if (newStatus == oldStatus) return;
      digitalWrite(pin, HIGH);
      cout << "Ventil geschlossen" << endl;
      oldStatus = newStatus;
    }
    private:
      bool initialized = false; // default false 
      bool oldStatus = false; // false = off true = on; default false as initialized
      void initialize() {
        wiringPiSetup();
        pinMode(pin,OUTPUT);
        initialized = true;
      }
};

int main(void) {
  // test setup
  pump boilerpumpe(21);
  valve boilervalve(28);
  while (true)
  {
    boilerpumpe.on();
    boilervalve.close();
    delay(5*1000);
    boilerpumpe.off();
    boilervalve.open();
    delay(5*1000);
  }
}