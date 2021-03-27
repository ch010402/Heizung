/*
MIT license 
created: ch010402 16.10.2020
  learning: variable declaration is key; declaire variable as late and as narrow as possible and intialise even later
changed: 
  ch010402 23.11.2020 mixup pump and valve IO corrected
  ch010402 27.03.2021 ready for real run
*/

#include "wiringPi.h" // library to access the GPIO Pins on a raspberryPI !!depriciated!!
#include <iostream>   // cout
#include <fstream>    // file stream access
#include <string>     // string class
#include <sstream>    // string stream needed for file access to put data into string
#include "include/logger/Log.h" // Logger class
#include <chrono>
#include <thread>
#include <iomanip>


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
        Log::Debug("Pumpe bereits eingeschaltet");
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
        Log::Debug("Pumpe bereits ausgeschaltet");
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
        Log::Warning("initialized pin: " + to_string(pin));
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
        Log::Debug("Ventil bereits geöffnet");
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
        Log::Debug("Ventil bereits geschlossen");
        return;
      }
      digitalWrite(pin, HIGH);
      std::this_thread::sleep_for(std::chrono::milliseconds(10 * 1000));
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
      Log::Warning("initialized pin: " + to_string(pin));
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
        Log::Error("Error reading file at " + path);
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

class mixer {
public:
  int openPin;
  int closePin;
  int fullCycleSec = 10;
  // constructor
  mixer(int open, int close, int sec) {
    openPin = open;
    closePin = close;
    fullCycleSec = sec;
  }
  // methodes
  void open() {
    if (!initialized) initialize();
    if (currentStep < 8) {
      digitalWrite(closePin, HIGH);
      digitalWrite(openPin, LOW);
      std::this_thread::sleep_for(std::chrono::milliseconds(stepTime));
      digitalWrite(openPin, HIGH);
      currentStep++;
      Log::Debug("current step: "+ currentStep);
    }
    else Log::Warning("unable to open mixer");
  }
  void close() {
    // check if initialized
    if (!initialized) initialize();
    if (currentStep > 0) {
      digitalWrite(openPin, HIGH);
      digitalWrite(closePin, LOW);
      std::this_thread::sleep_for(std::chrono::milliseconds(stepTime));
      digitalWrite(closePin, HIGH);
      currentStep++;
      Log::Debug("current step: " + currentStep);
    }
    else Log::Warning("unable to close mixer");
  }
  int mixStep() {
    if (!initialized) initialize();
    return currentStep;
  }
private:
  bool initialized = false; // default false
  bool oldStatus = false; // false = off true = on; default false as initialized
  int steps = 8;
  int stepTime = fullCycleSec * 1000 / steps;
  int currentStep; //
  
  void initialize() {
    wiringPiSetup();
    pinMode(openPin, OUTPUT);
    digitalWrite(openPin, HIGH);
    Log::Warning("initialized pin: " + to_string(openPin));
    pinMode(closePin, OUTPUT);
    digitalWrite(closePin, HIGH);
    Log::Warning("initialized pin: " + to_string(closePin));
    
    // actually initialize
    Log::Warning("initialize mixer: " + to_string(fullCycleSec) + "s");
    digitalWrite(openPin, HIGH);
    digitalWrite(closePin, LOW);
    for (int i = 0; i < steps; i++) {
      std::this_thread::sleep_for(std::chrono::milliseconds(stepTime));
      std::cout << (100 / steps * i + "% ");
    }
    std::cout << endl;
    digitalWrite(closePin, HIGH);
    Log::Warning("initialize mixer: done");
    currentStep = 0;
    // done
    initialized = true;
  }
};

class onoff {
public:
  int pin;
  // constructor
  onoff(int p) {
    pin = p;
  }
  // methodes
  void on() {
    bool newStatus = true;
    if (!initialized) initialize();
    if (newStatus == oldStatus) {
      Log::Debug("Elektro bereits eingeschaltet");
      return;
    }
    digitalWrite(pin, LOW);
    Log::Info("Elektro eingeschaltet");
    oldStatus = newStatus;
  }
  void off() {
    bool newStatus = false;
    if (!initialized) initialize();
    if (newStatus == oldStatus) {
      Log::Debug("Elektro bereits ausgeschaltet");
      return;
    }
    digitalWrite(pin, HIGH);
    Log::Info("Elektro ausgeschaltet");
    oldStatus = newStatus;
  }
private:
  bool initialized = false; // default false 
  bool oldStatus = false; // false = off true = on; default false as initialized
  void initialize() {
    wiringPiSetup();
    pinMode(pin, OUTPUT);
    digitalWrite(pin, HIGH);
    Log::Warning("initialized pin: " + to_string(pin));
    initialized = true;
  }
};


// constructor
temperaturSensor::temperaturSensor(string str) {
  string device = str;
  const static string baseDir = "/sys/bus/w1/devices/";
  const static string tempFile = "/w1_slave";
  path = baseDir + device + tempFile;
}

//----- methodes
bool checkNiederTarif() {
  int morgen = 7;
  int abend = 20;
  int hour;
  auto now = std::chrono::system_clock::now();
  std::time_t in_time_t = std::chrono::system_clock::to_time_t(now);
  std::tm buffer;
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
  //define something for Windows (32-bit and 64-bit, this part is common)
  localtime_s(&buffer, &in_time_t);
#elif defined __linux__
  localtime_r(&in_time_t, &buffer);
#else
#   error "Unknown compiler"
#endif
  std::stringstream ss;
  ss << std::put_time(&buffer, "%H");
  ss >> hour;
  if (hour < morgen || hour > abend) {
    Log::Debug("Niedertarif " + hour);
    return true;
  }
  else {
    Log::Debug("Hochtarif " + hour);
    return false;
  }
  Log::Error("Fehler beim bestimmen der Zeit");
  return false;
}

int main(int argc, const char** argv) {

  Log::Setup(argv[0], Log::Level::LevelInfo);
  Log::Warning("starting up ... ");
  
  std::cout << "argv-1 " << argv[1] << std::endl;
  if (argv[1] == "sommer") std::cout << "ja sommer baby" << std::endl;
  else std::cout << "brr winter";

  // setup
  pump boilerpumpe(28);
  pump umlaufpumpe(29);
  pump elektropumpe(25);
  valve boilervalve(21);
  valve speichervalve(3);
  mixer elektromixer(6, 5, 82);
  onoff durchlauferhitzer(1);

  temperaturSensor testSensor1("28-3c01a81688f4");
  temperaturSensor testSensor2("28-3c01a816d9c1");
  temperaturSensor ofenRuecklauf("28-0416a10e34ff");
  temperaturSensor boilerUnten("28-0416a1295fff");
  temperaturSensor elektroRuecklauf("28-0316a15a36ff");
  temperaturSensor elektroVorlauf("28-0516a15419ff");

  float orl;
  float bu;
  
  bool winterbetrieb = true;
  
  if (winterbetrieb) {
    speichervalve.open();
    umlaufpumpe.on();

    // loop
    while (true)
    {
      // set TRUE for productive system otherwise it will run on the test system
      if (true) {
        Log::Debug("running on productive system");
        orl = ofenRuecklauf.temperatur();
        bu = boilerUnten.temperatur();
      }
      else {
        Log::Debug("running on test system");
        // test system
        orl = testSensor1.temperatur();
        bu = testSensor2.temperatur();
      }
      // wenn der Boiler unten unter 70°C hat prüfe weiter
      if (bu < 70) {
        // wenn der Ofenrücklauf 5°C oder wärmer ist als der Boiler schalte ein
        Log::Debug("Ofen Rücklauf " + to_string(orl) + " °C - Boiler unten " + to_string(bu) + "°C");
        if (orl - 5 > bu) {
          boilervalve.open();
          boilerpumpe.on();
        }
        // wenn der Ofenrücklauf 3° oder wäremer ist schalte nichts 
        else if (orl - 3 > bu) {
          Log::Debug("schalte nichts");
        }
        // wenn der Ofenrücklauf 3°C wärmer oder weniger ist als der Boiler schalte aus
        else {
          boilerpumpe.off();
          boilervalve.close();
        }
      }
      // wenn der Boiler mehr als 70° hat schalte aus
      else {
        Log::Debug("Der Boiler hat " + to_string(bu) + "°C");
        boilerpumpe.off();
        boilervalve.close();
      }

      std::this_thread::sleep_for(std::chrono::milliseconds(5 * 1000));

    }
  }
  // sommerbetrieb
  else { 
    speichervalve.close();
    umlaufpumpe.off();
    bool aufheizen = false;

    // loop
    while (true) {
      // wenn der Boiler unten unter 30° prüfe ob niedertarif
      if (boilerUnten.temperatur() < 30) {
        if (checkNiederTarif()) {
          Log::Warning("schalte Aufheizen ein");
          aufheizen = true;
        }
        else {
          Log::Warning("schalte Aufheizen aus");
          aufheizen = false;
        }
      }
      // starte aufheizen
      if (aufheizen) {
        durchlauferhitzer.on();
        elektropumpe.on();
        boilervalve.open();
      }
      // beende aufheizen
      else {
        durchlauferhitzer.off();
        elektropumpe.off();
        boilerpumpe.off();
        boilervalve.close();
        // set mixer to 0
        for (int i = 0; i < elektromixer.mixStep(); i++) {
          elektromixer.close();
        }
      }
      // wenn elektro Rücklauf > 45° schalte Boiler Pumpe ein
      if (elektroRuecklauf.temperatur() > 45) boilerpumpe.on();
      // wenn der elektro Rücklauf < 60° schliesse den Mixer um einen schritt
      if (elektroRuecklauf.temperatur() < 60) {
        elektromixer.close();
        Log::Debug("schliesse Mischer");
      }
      // wenn der elektro Rücklauf > 60° öffne den Mixer um einen schritt
      else {
        elektromixer.open();
        Log::Debug("öffne Mischer");
      }
      if (boilerUnten.temperatur() > 60) {
        Log::Warning("schalte Aufheizen aus");
        aufheizen = false;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(5 * 1000));
    }
  }
  return 0;
}
