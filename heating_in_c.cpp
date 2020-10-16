/*
MIT license 
ch010402 16.10.2020 15:00
*/

#include <wiringPi.h>
#include <iostream> // cout
#include <fstream> // file stream access
#include <string> // string class
#include <sstream> // string stream needed for file access to put data into string

using namespace std;

//----- initial variables and constantes
int wwallout[7] = {28, 25, 6, 5, 21, 4, 2}; // all warm water output needed
int em_catch = 10; // original 36 time to catch the elektro mischer (in s)
int em_fullturn = 24; // time to rotate the mischer fully (in s)
int em_pos = 9; // 9 unknown range 0-8
string testSensor1 = "28-3c01a81688f4"; // sensor on progpi 28-3c01a81688f4
string testSensor2 = "28-3c01a816d9c1"; // sensor on progpi 28-3c01a816d9c1
string ofenrueck = "28-0416a10e34ff"; // Ofenrücklauf Sensor
int wwvalve = 28;
int wwpump = 21;

//----- functions declaration
int setup(void);
int set_mischer(int new_pos);
double getTemp(string address, bool debug = false);
int testscipt(void);


//----- main
int main (void) {
  cout << "Starting..." << endl;
  setup();
  
  double orl, bu;


  cout << "Ofenrücklauf= " << orl << " Boiler unten= " << bu << endl;
  
  while (TRUE) {
    orl = getTemp(testSensor1);
    bu = getTemp(testSensor2);
    if (orl > bu) {
      digitalWrite (wwvalve, LOW);
      digitalWrite (wwpump, LOW);
      cout << "schalte ein" << endl;
      if (orl < bu) {
        digitalWrite (wwpump, HIGH);
        digitalWrite (wwvalve, HIGH);
        cout << "schalte aus" << endl;
      }
      else delay (3 * 1000); // 30 defaul
      delay (20 * 1000); // 300 defaul
    }
    else delay (10 * 1000);
  }
  
    
// test 
//  testscipt();
  cout << "Done." << endl;
  return 0;
}

//----- functions definition
int setup (void) {
  // setup wiringPi -> all needed GPIO pins
  cout << "Setup start" << endl;
  wiringPiSetup ();
  // set all gpio pins defined in wwallout as OUTPUT
  for (int i = 0; i < 7; i++){
    pinMode (wwallout[i], OUTPUT);
  }
  // set the OUTPUT to HIGH (switched off) -> NPN
  for (int i = 0; i < 7; i++){
    digitalWrite (wwallout[i], HIGH);
  }
  cout << "Setup done" << endl;
  
  // init mischer reset to 0
  cout << "Init Mischer start" << endl;
  digitalWrite (6, HIGH);
  digitalWrite (5, LOW);
  delay ((em_catch + em_fullturn)*1000);
  digitalWrite (5, HIGH);
  em_pos = 0;
  cout << "Init Mischer done" << endl;
  
  return 0;
}

int set_mischer (int new_pos) {
  if (em_pos == new_pos) {
    // do nothing
    cout << "Mischer at position " << em_pos << endl;
    return 0;
  }
  // calculate step size
  int step = em_fullturn / 8 ;
  // calculate step difference 
  int step_diff = new_pos - em_pos ;
  cout << "moving " << step_diff << " steps." << endl;
  // check if we need to open (+) or close (-) the mischer
  if (step_diff > 0) {
    // open mischer
    digitalWrite (5, HIGH);
    digitalWrite (6, LOW);
    delay (abs(step_diff) * step * 1000);
    digitalWrite (6, HIGH);
  }
  else {
    // close mischer
    digitalWrite (6, HIGH);
    digitalWrite (5, LOW);
    delay (abs(step_diff) * step * 1000);
    digitalWrite (5, HIGH);
  } 
  em_pos = new_pos;
  cout << "Mischer set to position " << em_pos << endl;
  return 0;
}

double getTemp(string deviceAddress, bool debug) {
  
  if (debug) cout << "start getting temperatur" << endl;
  double temp = 987.65;
  string baseDir = "/sys/bus/w1/devices/";
  string tempFile = "/w1_slave";
  string path = baseDir + deviceAddress + tempFile;
  stringstream buffer;
  string data;
  string strTemp;
  
  if (debug) cout << "try to read file " << path << endl;
  ifstream infile(path);
  if (infile) {
    if (debug) cout << "reading content" << endl;
    buffer << infile.rdbuf();
    data = buffer.str();
    infile.close();
  }
  else {
    infile.close();
    cout << "Error reading file at " << path << endl;
    return -1;
  }
  
  if (debug) cout << "the content is:" << endl;
  if (debug) cout << data;
  
  if (debug) cout << "checking CRC ";
  size_t crcCheck = data.find("YES");
  if (crcCheck != string::npos) {
    if (debug) cout << "ok" << endl;
  }
  else {
    cout << "fail" << endl;
    return -100;
  }
  if (debug) cout << "find temperatur position ";
  size_t TempPos = data.find("t=");
  if (TempPos != string::npos) {
    if (debug) cout << TempPos << endl;
  }
  else {
    cout << "failed to find vale -> abort!" << endl;
    return -101;
  }
  strTemp = data.substr(TempPos+2);
  if (debug) cout << "found: " << strTemp << endl;
  temp = stod(strTemp)/1000;
  if (debug) cout << "done found temperatur: " << temp <<endl; 
  return temp;
}

//----- test function
int testscipt(void) {
  cout << "Start testing..." << endl;
  double temperatur = getTemp(testSensor1, TRUE);
  cout << "es ist " << temperatur << "°C" << endl;
  set_mischer(3);
  set_mischer(1);
  set_mischer(1);
  set_mischer(0);
  set_mischer(8);
  temperatur = getTemp(testSensor1);
  cout << "es ist " << temperatur << "°C" << endl;
  cout << "Done testing..." << endl;
  return 0;
}
