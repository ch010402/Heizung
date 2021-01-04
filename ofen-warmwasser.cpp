/*
MIT license 
created: ch010402 16.10.2020
  learning: variable declaration is key; declaire variable as late and as narrow as possible and intialise even later
changed: ch010402 23.11.2020 mixup pump and valve IO corrected
*/

#include "wiringPi.h" // library to access the GPIO Pins on a raspberryPI !!depriciated!!
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
      if (newStatus == oldStatus) {
        cout << "Pumpe bereits eingeschaltet" << endl;
        return;
      }
      digitalWrite(pin, LOW);
      cout << "Pumpe eingeschaltet" << endl;
      oldStatus = newStatus;
    }
    void off() {
      bool newStatus = false;
      if (!initialized) initialize();
      if (newStatus == oldStatus) {
        cout << "Pumpe bereits ausgeschaltet" << endl;
        return;
      }
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
        digitalWrite(pin, HIGH);
        cout << "initialized pin: " << pin << endl;
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
      if (newStatus == oldStatus) {
        cout << "Ventil bereits geöffnet" << endl;
        return;
      }
      digitalWrite(pin, LOW);
      // Ventil öffnung 30s
      delay(15*1000);
      cout << "Ventil geöffnet" << endl;
      oldStatus = newStatus;
    }
    void close() {
      bool newStatus = false;
      if (!initialized) initialize();
      if (newStatus == oldStatus) {
        cout << "Ventil bereits geschlossen" << endl;
        return;
      }
      digitalWrite(pin, HIGH);
      delay(10*1000);
      cout << "Ventil geschlossen" << endl;
      oldStatus = newStatus;
    }
  private:
    bool initialized = false; // default false 
    bool oldStatus = false; // false = off true = on; default false as initialized
    void initialize() {
      wiringPiSetup();
      pinMode(pin,OUTPUT);
      digitalWrite(pin, HIGH);
      cout << "initialized pin: " << pin << endl;
      initialized = true;
    }
};

class temperaturSensor {
  private:
    string path;
  public:
    // constructor declaration
    temperaturSensor(string str);
    // methodes
    double temperatur() {
      string data;
      ifstream infile(path);
      if (infile) {
        stringstream buffer;
        buffer << infile.rdbuf();
        infile.close();
        data = buffer.str();
      }
      else {
        infile.close();
        cout << "Error reading file at " << path << endl;
        return -100;
      }
      size_t crcCheck = data.find("YES");
      if (crcCheck == string::npos) {
        cout << "CRC fail not reading temperatur" << endl;
        return -101;
      }
      size_t TempPos = data.find("t=");
      if (TempPos == string::npos) {
        cout << "failed to find value -> abort!" << endl;
        return -102;
      }
      string strTemp = data.substr(TempPos+2);
      double temp = stod(strTemp)/1000;
      return temp;
    }
};

// constructor
temperaturSensor::temperaturSensor(string str) {
  string device = str;
  const static string baseDir = "/sys/bus/w1/devices/";
  const static string tempFile = "/w1_slave";
  path = baseDir + device + tempFile;
}

int main(void) {
  // test setup
  pump boilerpumpe(28);
  valve boilervalve(21);
  temperaturSensor testSensor1("28-3c01a81688f4");
  temperaturSensor testSensor2("28-3c01a816d9c1");
  temperaturSensor ofenRuecklauf("28-0416a10e34ff");
  temperaturSensor boilerUnten("28-0416a1295fff");
  double orl;
  double bu;
  while (true)
  { 
    // set TRUE for productive system otherwise it will run on the test system
    if (true) {
      orl = ofenRuecklauf.temperatur();
      bu = boilerUnten.temperatur();
    }
    else {
      // test system
      orl = testSensor1.temperatur();
      bu = testSensor2.temperatur();
    }
    // wenn der Boiler unten unter 70°C hat prüfe weiter
    if ( bu < 70 ) {
      // wenn der Ofenrücklauf 5°C oder wärmer ist als der Boiler schalte ein
      if ( orl - 5 > bu) {
        cout <<"Ofen Rücklauf " << orl << "°C - Boiler unten " << bu << "°C" << endl;
        boilervalve.open();
        boilerpumpe.on();
      }
      // wenn der Ofenrücklauf 3° oder wäremer ist schalte nichts 
      else if (orl - 3 > bu) {
        cout <<"Ofen Rücklauf " << orl << "°C - Boiler unten " << bu << "°C" << endl;
        cout <<"schalte nichts" << endl;
      }
      // wenn der Ofenrücklauf 3°C wärmer oder weniger ist als der Boiler schalte aus
      else {
        cout <<"Ofen Rücklauf " << orl << "°C Boiler unten " << bu << "°C" << endl;
        boilerpumpe.off();
        boilervalve.close();
      }
    }
    // wenn der Boiler mehr als 70° hat schalte aus
    else {
      cout <<"Der Boiler hat " << bu << "°C" << endl;
      boilerpumpe.off();
      boilervalve.close();
    }
    delay(5*1000);
    
  }
  return 0;
}
