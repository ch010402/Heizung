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
      cout << "Ventil geschlossen" << endl;
      oldStatus = newStatus;
    }
  private:
    bool initialized = false; // default false 
    bool oldStatus = false; // false = off true = on; default false as initialized
    void initialize() {
      wiringPiSetup();
      pinMode(pin,OUTPUT);
      cout << "initialized pin: " << pin << endl;
      initialized = true;
    }
};

class temperaturSensor {
  private:
    string device;
    string baseDir = "/sys/bus/w1/devices/";
    string tempFile = "/w1_slave";
    string path = baseDir + device + tempFile;
    stringstream buffer;
    string data;
    string strTemp;
    double temp = 987.6;
  public:
    // constructor declaration
    temperaturSensor(string str);
    // methodes
    double temperatur() {
      //cout << "----->>>> " << device << " ----->>>> " << path << endl;
      ifstream infile(path);
      if (infile) {
        buffer << infile.rdbuf();
        data = buffer.str();
        infile.close();
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
        cout << "failed to find vale -> abort!" << endl;
        return -102;
      }
      strTemp = data.substr(TempPos+2);
      temp = stod(strTemp)/1000;
      return temp;
    }
};

// constructor
    temperaturSensor::temperaturSensor(string str) {
       device = str;
    }

int main(void) {
  // test setup
  pump boilerpumpe(21);
  valve boilervalve(28);
  string tSensor1 = "28-3c01a81688f4";
  temperaturSensor testSensor1(tSensor1);
  temperaturSensor testSensor2("28-3c01a816d9c1");
  while (true)
  {
    boilerpumpe.on();
    boilervalve.close();
    cout << "TestSensor1= " << testSensor1.temperatur() << "°C \nTestSensor2= " << testSensor2.temperatur() << "°C" << endl;
    delay(5*1000);
    boilerpumpe.off();
    boilervalve.open();
    delay(5*1000);
  }
}
