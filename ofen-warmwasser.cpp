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
      Log::Info("Pumpe eingeschaltet" + to_string(pin));
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
      Log::Info("Pumpe ausgeschaltet" + to_string(pin));
      oldStatus = newStatus;
    }
    private:
      bool initialized = false; // default false 
      bool oldStatus = false; // false = off true = on; default false as initialized
      void initialize() {
        wiringPiSetup();
        pinMode(pin,OUTPUT);
        digitalWrite(pin, HIGH);
        Log::Warning("Initialisiere Pupmen pin: " + to_string(pin));
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
        Log::Debug("Ventil bereits geoeffnet");
        return;
      }
      digitalWrite(pin, LOW);
      // Ventil öffnung 30s
      delay(15*1000);
      Log::Info("Ventil geoeffnet");
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
      Log::Warning("Initialisire Ventil pin: " + to_string(pin));
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
  int fullCycleSec;
  // constructor
  mixer(int open, int close, int sec) {
    openPin = open;
    closePin = close;
    fullCycleSec = sec;
  }
  // methodes
  void open() {
    if (!initialized) initialize();
    if (currentStep <= steps) {
      Log::Info("oeffne Mixer");
      digitalWrite(closePin, HIGH);
      digitalWrite(openPin, LOW);
      std::this_thread::sleep_for(std::chrono::milliseconds(fullCycleSec * 1000 / steps));
      digitalWrite(openPin, HIGH);
      currentStep++;
      Log::Debug("setze current step auf : "+ to_string(currentStep));
    }
    else Log::Debug("unable to open mixer");
  }
  void close() {
    // check if initialized
    if (!initialized) initialize();
    if (currentStep >= 0) {
      Log::Info("schliesse Mixer");
      digitalWrite(openPin, HIGH);
      digitalWrite(closePin, LOW);
      std::this_thread::sleep_for(std::chrono::milliseconds(fullCycleSec * 1000 / steps));
      digitalWrite(closePin, HIGH);
      currentStep--;
      Log::Debug("setze current step auf : " + to_string(currentStep));
    }
    else Log::Debug("unable to close mixer");
  }
  int mixStep() {
    if (!initialized) initialize();
    return currentStep;
  }
private:
  bool initialized = false; // default false
  bool oldStatus = false; // false = off true = on; default false as initialized
  int steps = 16;
  int currentStep; //

  void initialize() {
    wiringPiSetup();
    pinMode(openPin, OUTPUT);
    digitalWrite(openPin, HIGH);
    Log::Info("Initialisiere Open pin: " + to_string(openPin));
    pinMode(closePin, OUTPUT);
    digitalWrite(closePin, HIGH);
    Log::Info("Initialisiere Close pin: " + to_string(closePin));
    // actually initialize
    Log::Info("Setze volle Zykluszeit auf: " + to_string(fullCycleSec) + "s");
    Log::Info("Schritt groesse in ms: " + to_string(fullCycleSec * 1000 / steps) + "ms");
    Log::Warning("Starte die Initialisierung.");
    digitalWrite(openPin, HIGH);
    digitalWrite(closePin, LOW);
    std::this_thread::sleep_for(std::chrono::milliseconds(fullCycleSec*1000));
    digitalWrite(closePin, HIGH);
    Log::Warning("Initialisierung beendet.");
    currentStep = 0;
    Log::Debug("setze currentStep auf: " + to_string(currentStep));
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
      Log::Debug("I/O bereits eingeschaltet");
      return;
    }
    digitalWrite(pin, LOW);
    Log::Info("I/O eingeschaltet");
    oldStatus = newStatus;
  }
  void off() {
    bool newStatus = false;
    if (!initialized) initialize();
    if (newStatus == oldStatus) {
      Log::Debug("I/O bereits ausgeschaltet");
      return;
    }
    digitalWrite(pin, HIGH);
    Log::Info("I/O ausgeschaltet");
    oldStatus = newStatus;
  }
private:
  bool initialized = false; // default false 
  bool oldStatus = false; // false = off true = on; default false as initialized
  void initialize() {
    wiringPiSetup();
    pinMode(pin, OUTPUT);
    digitalWrite(pin, HIGH);
    Log::Warning("Initialisiere I/O pin: " + to_string(pin));
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
  int morgen = 6; // ACHTUNG UTC
  int abend = 19; // ACHTUNG UTC
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

// ********** main **********

int main(int argc, const char** argv) {

// SET LogLevel HERE LevelError, LevelWarning, LevelInfo, Debug
  Log::Setup(argv[0], Log::Level::Debug);
  Log::Warning("starting up ... ");
  
  bool winterbetrieb = true;
  std::string winter = argv[1];
  if (winter.compare("sommer") == 0) {
    winterbetrieb = false;
    Log::Warning("Sommerbetrieb gestartet");
  }
  else {
    Log::Warning("Winterbetrieb gestartet");
  }

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
        Log::Debug("Ofen Ruecklauf " + to_string(orl) + " °C - Boiler unten " + to_string(bu) + "°C");
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
    // reset
    boilerpumpe.off();
    boilervalve.close();
    durchlauferhitzer.off();
    elektropumpe.off();
    bool aufheizenEin = false;
    bool aufheizenAus = false;

    // loop
    while (true) {
      // wenn der Boiler unten unter 30° prüfe ob niedertarif 
      if (boilerUnten.temperatur() < 30) {
        Log::Debug("Boiler unten Temperatur unter 30°");
        if (checkNiederTarif()) {
          Log::Info("Niedertarif aktiv schalte ein");
          aufheizenEin = true;
        }
        else {
          Log::Debug("Hochtarif schalte aus");
          aufheizenEin = false;
        }
      }
      // starte Aufheizen
      if (aufheizenEin) {
        durchlauferhitzer.on();
        Log::Info("Schalte Durchlauferhitzer ein");
        elektropumpe.on();
        Log::Info("Schalte Elektropumpe ein");
        aufheizenEin = false;
        Log::Debug("Aufheizen EIN beendet");
      }
      // Schalte Boiler Kreis ein 
      if (elektroRuecklauf.temperatur() > 45) {
        boilervalve.open();
        Log::Info("Oeffne Boilerventil");
        boilerpumpe.on();
        Log::Info("Schalte Boilerpumpe ein");
      }
      // Ueberwache Durchlauferhitzer
      if (elektroRuecklauf.temperatur() > 70) {
        durchlauferhitzer.off();
        Log::Warning("Schalte Durchlauferhitzer aus temperatur > 70°");
      }
      else {
        durchlauferhitzer.on();
        Log::Debug("Durchlauferhitzer ein");
      }
      // wenn der Boiler unten unter 60° starte abschalten
      if (boilerUnten.temperatur() > 60) aufheizenAus = true;
      // beende Aufheizen
      if (aufheizenAus) {
        durchlauferhitzer.off();
        Log::Info("Schalte Durchlauferhitzer aus");
        std::this_thread::sleep_for(std::chrono::milliseconds(120 * 1000));
        elektropumpe.off();
        Log::Info("Schalte Elektropumpe aus");
        boilerpumpe.off();
        Log::Info("Schalte Boilerpumpe aus");
        boilervalve.close();
        Log::Info("Schliesse Boilerventil");
        // set mixer to 0
        Log::Info("Elektromischer auf Schritt. " + to_string(elektromixer.mixStep()));
        for (int i = 0; i < elektromixer.mixStep(); i++) {
          Log::Debug("Elektromischer auf Schritt. " + to_string(elektromixer.mixStep()));
          Log::Debug("Durchlauf: " + to_string(i));
          elektromixer.close();
        }
        aufheizenAus = false;
        Log::Debug("Aufheizen AUS beendet");
      }
      // wenn der elektro Rücklauf > 60° öffne den Mixer um einen schritt
      if (elektroRuecklauf.temperatur() > 60) {
        elektromixer.open();
        Log::Debug("oeffne Mischer");
      }
      // wenn der elektro Rücklauf < 60° schliesse den Mixer um einen schritt
      else {
        elektromixer.close();
        Log::Debug("schliesse Mischer");
      }
      
      // schlafe für 5 Sekunden
      std::this_thread::sleep_for(std::chrono::milliseconds(5 * 1000));
    }
  }
  return 0;
}
