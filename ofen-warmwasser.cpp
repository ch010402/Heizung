/*
MIT license 
created: ch010402 16.10.2020
  learning: variable declaration is key; declaire variable as late and as narrow as possible and intialise even later
changed: 
  ch010402 23.11.2020 mixup pump and valve IO corrected
  ch010402 27.03.2021 ready for real run
  ch010402 27.04.2021 clean up and fix while summer full test 
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
  // Schlatet die Pumpe an (void) eg. pump.on()
  void on() {
    bool newStatus = true; // true = on
    if (!initialized) initialize();
    if (newStatus != oldStatus) {
      digitalWrite(pin, LOW);
      Log::Info("Pumpe eingeschaltet #" + std::to_string(pin));
      oldStatus = newStatus;
    }
    else Log::Debug("Pumpe bereits eingeschaltet");
  }
  // Schaltet die Pumpe aus (void) eg. pump.off()
  void off() {
    bool newStatus = false; // false = off
    if (!initialized) initialize();
    if (newStatus != oldStatus) {
      digitalWrite(pin, HIGH);
      Log::Info("Pumpe ausgeschaltet #" + std::to_string(pin));
      oldStatus = newStatus;
    }
    else Log::Debug("Pumpe bereits ausgeschaltet");
  }
private:
  bool initialized = false; // default false -> not initialized
  bool oldStatus; // false = off true = on; default false as initialized
  // Initialisiere Pumpen pin
  void initialize() {
    wiringPiSetup();
    pinMode(pin, OUTPUT);
    digitalWrite(pin, HIGH); // setze den Pin statndard auf HIGH = Aus
    Log::Warning("Initialisiere Pupmen pin: " + std::to_string(pin));
    oldStatus = false;
    initialized = true;
  }
};

class valve {
public:
  int pin;
  // constructor
  valve(int v) {
    pin = v;
  }
  // methodes
  // Oeffnet das Ventil (void) eg. valve.open()
  void open() {
    bool newStatus = true; // ture = open
    if (!initialized) initialize();
    if (newStatus != oldStatus) {
      digitalWrite(pin, LOW);
      // Ventil öffnung 30s
      std::this_thread::sleep_for(std::chrono::milliseconds(15 * 1000));
      Log::Info("Ventil geoeffnet #" + std::to_string(pin));
      oldStatus = newStatus;
    }
    else Log::Debug("Ventil bereits geoeffnet");
  }
  // Schliesst das Ventil (void) eg. valve.close()
  void close() {
    bool newStatus = false; // false = close
    if (!initialized) initialize();
    if (newStatus != oldStatus) {
      digitalWrite(pin, HIGH);
      std::this_thread::sleep_for(std::chrono::milliseconds(10 * 1000));
      Log::Info("Ventil geschlossen #" + std::to_string(pin));
      oldStatus = newStatus;
    }
    else Log::Debug("Ventil bereits geschlossen");
  }
private:
  bool initialized = false; // default false 
  bool oldStatus; // false = off true = on; default false as initialized
  void initialize() {
    wiringPiSetup();
    pinMode(pin, OUTPUT);
    digitalWrite(pin, HIGH);
    Log::Warning("Initialisire Ventil pin: " + std::to_string(pin));
    oldStatus = false;
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
  // Öffne den Mischer (void) eg. mixer.open()
  void open() {
    if (!initialized) initialize();
    if (currentStep <= steps - 1) {
      digitalWrite(closePin, HIGH);
      digitalWrite(openPin, LOW);
      std::this_thread::sleep_for(std::chrono::milliseconds(fullCycleSec * 1000 / steps));
      digitalWrite(openPin, HIGH);
      currentStep++;
      Log::Debug("oeffne Mischer auf current step : " + std::to_string(currentStep));
    }
    else Log::Debug("Mischer bereits voll geoeffnet");
  }
  // Schliesse den Mischer (void) eg. mixer.close()
  void close() {
    if (!initialized) initialize();
    if (currentStep >= 1) {
      digitalWrite(openPin, HIGH);
      digitalWrite(closePin, LOW);
      std::this_thread::sleep_for(std::chrono::milliseconds(fullCycleSec * 1000 / steps));
      digitalWrite(closePin, HIGH);
      currentStep--;
      Log::Debug("schliesse Mischer auf current step : " + std::to_string(currentStep));
    }
    else Log::Debug("Mischer bereits voll geschlossen");
  }
  // gibt den aktuellen step zurück eg. mixer.mixStep() --> 5
  int mixStep() {
    if (!initialized) initialize();
    Log::Debug("Mischer auf current step : " + std::to_string(currentStep));
    return currentStep;
  }
private:
  bool initialized = false; // default false
  int steps = 16;
  int currentStep;

  void initialize() {
    wiringPiSetup();
    pinMode(openPin, OUTPUT);
    digitalWrite(openPin, HIGH); // setze den Pin statndard auf HIGH = Aus
    Log::Warning("Initialisiere Open pin: " + std::to_string(openPin));
    pinMode(closePin, OUTPUT);
    digitalWrite(closePin, HIGH); // setze den Pin statndard auf HIGH = Aus
    Log::Warning("Initialisiere Close pin: " + std::to_string(closePin));
    Log::Info("Setze volle Zykluszeit auf: " + std::to_string(fullCycleSec) + "s");
    Log::Info("Schritt groesse in ms: " + std::to_string(fullCycleSec * 1000 / steps) + "ms");
    Log::Warning("Starte die Initialisierung.");
    // actually initialize
    digitalWrite(openPin, HIGH);
    digitalWrite(closePin, LOW);
    std::this_thread::sleep_for(std::chrono::milliseconds(fullCycleSec * 1000));
    digitalWrite(closePin, HIGH);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    digitalWrite(openPin, LOW);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    digitalWrite(openPin, HIGH);
    Log::Warning("Initialisierung beendet.");
    currentStep = 0;
    Log::Debug("setze currentStep auf: " + std::to_string(currentStep));
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
  // Schaltet einen I/O (void) eg. io.on()
  void on() {
    bool newStatus = true; // ture = open
    if (!initialized) initialize();
    if (newStatus != oldStatus) {
      digitalWrite(pin, LOW);
      Log::Info("I/O eingeschaltet #" + std::to_string(pin));
      oldStatus = newStatus;
    }
    else Log::Debug("I/O bereits eingeschaltet");
  }
  // Schaltet einen I/O (void) eg. io.off()
  void off() {
    bool newStatus = false; // false = close
    if (!initialized) initialize();
    if (newStatus != oldStatus) {
      digitalWrite(pin, HIGH);
      Log::Info("I/O eingeschaltet #" + std::to_string(pin));
      oldStatus = newStatus;
    }
    else Log::Debug("I/O bereits ausgeschaltet");
  }
private:
  bool initialized = false; // default false 
  bool oldStatus; // false = off true = on; default false as initialized
  void initialize() {
    wiringPiSetup();
    pinMode(pin, OUTPUT);
    digitalWrite(pin, HIGH);
    Log::Warning("Initialisiere I/O pin: " + std::to_string(pin));
    oldStatus = false;
    initialized = true;
  }
};


//----- constructor
temperaturSensor::temperaturSensor(string str) {
  string device = str;
  const static string baseDir = "/sys/bus/w1/devices/";
  const static string tempFile = "/w1_slave";
  path = baseDir + device + tempFile;
}

//----- methodes
bool checkNiederTarif() {
  int utc = 2; // UTC differeenz zu CET
  int morgen = 7 - utc; // ACHTUNG UTC in CET wäre es 7
  int abend = 20 - utc; // ACHTUNG UTC in CET wäre es 20
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
  if (hour < morgen || hour >= abend) {
    Log::Info("Es ist Niedertarif. Stunde: " + to_string(hour));
    return true;
  }
  else {
    Log::Info("Es ist Hochtarif. Stunde: " + to_string(hour));
    return false;
  }
  Log::Error("Fehler beim bestimmen der Zeit");
  return false;
}

// ********** main **********
int main(int argc, const char** argv) {

// SET LogLevel HERE LevelError, LevelWarning, LevelInfo, Debug
  Log::Setup(argv[0], Log::Level::LevelInfo);
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
    //build status 
    enum class elektroStatus {
      elektroAus = 0, elektroReady, elektroStart, elektroEin
    };
    elektroStatus status = elektroStatus::elektroAus;
    // start up defaults
    speichervalve.close();
    umlaufpumpe.off();
    // reset all i/o
    boilerpumpe.off();
    boilervalve.close();
    durchlauferhitzer.off();
    elektropumpe.off();

    // loop
    while (true) {
      // prüfe Tarif
      if (checkNiederTarif()) {
        Log::Info("Niedertarif aktiv");
        status = elektroStatus::elektroReady;
      }
      else {
        Log::Debug("Hochtarif warte");
        status = elektroStatus::elektroAus;
      }
      if (status >= elektroStatus::elektroReady && boilerUnten.temperatur() < 30.0) {
        Log::Info("Boiler unten Temperatur unter 30°");
        status = elektroStatus::elektroStart;
      }

      // Starte Durchlauferhitzer
      if (status == elektroStatus::elektroStart) {
        elektropumpe.on();
        Log::Info("Schalte Elektropumpe ein");
        durchlauferhitzer.on();
        Log::Info("Schalte Durchlauferhitzer ein");
        status = elektroStatus::elektroEin;
      }
      else {
        if (status < elektroStatus::elektroStart) {
          status = elektroStatus::elektroAus;
          durchlauferhitzer.off();
          Log::Info("Schalte Durchlauferhitzer aus");
          std::this_thread::sleep_for(std::chrono::milliseconds(60 * 1000));
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
        }
      }

      // Schalte Boiler Kreis ein
      if (status >= elektroStatus::elektroEin && elektroRuecklauf.temperatur() > 45.0) {
        boilervalve.open();
        Log::Info("Oeffne Boilerventil");
        boilerpumpe.on();
        Log::Info("Schalte Boilerpumpe ein");
      }
      // Schalte Boiler Kreis aus
      else {
        boilerpumpe.off();
        Log::Info("Schalte Boilerpumpe aus");
        boilervalve.close();
        Log::Info("Schliesse Boilerventil");
      }

      // Ueberwache Durchlauferhitzer
      if (elektroRuecklauf.temperatur() > 69.0) {
        durchlauferhitzer.off();
        Log::Warning("Schalte Durchlauferhitzer aus temperatur > 70° aktuell " + to_string(elektroRuecklauf.temperatur()));
      }
      else {
        if (status >= elektroStatus::elektroEin && elektroRuecklauf.temperatur() <= 65.0) {
          durchlauferhitzer.on();
          Log::Debug("Durchlauferhitzer ein");
        }
        else Log::Debug("Kuehle Durchlauferhitzer");
      }

      // Pruefe Boiler Temperatur
      if (status >= elektroStatus::elektroReady && boilerUnten.temperatur() > 60.0) {
        status = elektroStatus::elektroAus;
        durchlauferhitzer.off();
        Log::Info("Schalte Durchlauferhitzer aus");
        std::this_thread::sleep_for(std::chrono::milliseconds(60 * 1000));
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
      }
      
      // wenn der elektro Rücklauf > 60° öffne den Mixer um einen schritt
      if (status >= elektroStatus::elektroReady && elektroRuecklauf.temperatur() > 60.0) {
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