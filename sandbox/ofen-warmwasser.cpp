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
#include "logger/Log.h" // Logger class
#include <chrono>
#include <thread>

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
        Log::Info("Pumpe bereits eingeschaltet");
        return;
      }
      digitalWrite(pin, LOW);
      Log::Info("Pumpe eingeschaltet");
      oldStatus = newStatus;
    }
    void off() {
      bool newStatus = false;
      if (!initialized) initialize();
      if (newStatus == oldStatus) {
        Log::Info("Pumpe bereits ausgeschaltet");
        return;
      }
      digitalWrite(pin, HIGH);
      Log::Info("Pumpe ausgeschaltet");
      oldStatus = newStatus;
    }
    private:
      bool initialized = false; // default false 
      bool oldStatus = false; // false = off true = on; default false as initialized
      void initialize() {
        wiringPiSetup();
        pinMode(pin,OUTPUT);
        digitalWrite(pin, HIGH);
        std::string msg = "initialized pin: " + to_string(pin);
        Log::Warning(msg.c_str());
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
        Log::Info("Ventil bereits geöffnet");
        return;
      }
      digitalWrite(pin, LOW);
      // Ventil öffnung 30s
      delay(15*1000);
      Log::Info("Ventil geöffnet");
      oldStatus = newStatus;
    }
    void close() {
      bool newStatus = false;
      if (!initialized) initialize();
      if (newStatus == oldStatus) {
        Log::Info("Ventil bereits geschlossen");
        return;
      }
      digitalWrite(pin, HIGH);
      delay(10*1000);
      Log::Info("Ventil geschlossen");
      oldStatus = newStatus;
    }
  private:
    bool initialized = false; // default false 
    bool oldStatus = false; // false = off true = on; default false as initialized
    void initialize() {
      wiringPiSetup();
      pinMode(pin,OUTPUT);
      digitalWrite(pin, HIGH);
      std::string msg = "initialized pin: " + to_string(pin);
      Log::Warning(msg.c_str());
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
        std::string msg = "Error reading file at " + path;
        Log::Error(msg.c_str());
        return -100;
      }
      size_t crcCheck = data.find("YES");
      if (crcCheck == string::npos) {
        Log::Error("CRC fail not reading temperatur");
        return -101;
      }
      size_t TempPos = data.find("t=");
      if (TempPos == string::npos) {
        Log::Error("failed to find value -> abort!");
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

int main(int argc, const char** argv) {
  Log::Setup(argv[0], Log::Level::LevelInfo);
  Log::Warning("starting up ... ");
  // test setup
  pump boilerpumpe(28);
  valve boilervalve(21);
  temperaturSensor testSensor1("28-3c01a81688f4");
  temperaturSensor testSensor2("28-3c01a816d9c1");
  temperaturSensor ofenRuecklauf("28-0416a10e34ff");
  temperaturSensor boilerUnten("28-0416a1295fff");
  float orl;
  float bu;
  while (true)
  { 
    // set TRUE for productive system otherwise it will run on the test system
    if (true) {
      Log::Info("running on productive system");
      orl = ofenRuecklauf.temperatur();
      bu = boilerUnten.temperatur();
    }
    else {
      Log::Info("running on test system");
      // test system
      orl = testSensor1.temperatur();
      bu = testSensor2.temperatur();
    }
    // wenn der Boiler unten unter 70°C hat prüfe weiter
    if ( bu < 70 ) {
      // wenn der Ofenrücklauf 5°C oder wärmer ist als der Boiler schalte ein
      std::string msg = "Ofen R\x81\cklauf " + to_string(orl) + " \370\C - Boiler unten " + to_string(bu) + "\370\C";
      Log::Info(msg.c_str());
      if ( orl - 5 > bu) {      
        boilervalve.open();
        boilerpumpe.on();
      }
      // wenn der Ofenrücklauf 3° oder wäremer ist schalte nichts 
      else if (orl - 3 > bu) {
        Log::Info("schalte nichts");
      }
      // wenn der Ofenrücklauf 3°C wärmer oder weniger ist als der Boiler schalte aus
      else {
        boilerpumpe.off();
        boilervalve.close();
      }
    }
    // wenn der Boiler mehr als 70° hat schalte aus
    else {
      std::string msg = "Der Boiler hat " + to_string(bu) + "\370\C";
      Log::Info(msg.c_str());
      boilerpumpe.off();
      boilervalve.close();
    }
    
    std::this_thread::sleep_for(std::chrono::milliseconds(5 * 1000));
    
  }
  return 0;
}
