#include <wiringPi.h>
#include <iostream>
#include <string>

using namespace std;

//----- initial variables and constantes
int wwallout[7] = {28, 25, 6, 5, 21, 4, 2}; // all warm water output needed
int em_catch = 10; // original 36 time to catch the elektro mischer (in s)
int em_fullturn = 24; // time to rotate the mischer fully (in s)
int em_pos = 9; // 9 unknown range 0-8
// sensor on progpi 28-3c01a81688f4

//----- functions declaration
int setup(void);
int set_mischer(int new_pos);
float get_temp(string address);
int testscipt(void);


//----- main
int main (void) {
  cout << "Starting..." << endl;
  setup();
  
// test 
  testscipt();
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

float get_temp (string address) {
  string device_file = "/sys/bus/w1/devices/"+ address +"/w1_slave";
  cout << "Sensor file " << device_file << endl;
  // read sensorfile raw data 
  // exempel output 
  // a7 01 55 05 7f a5 a5 66 98 : crc=98 YES
  // a7 01 55 05 7f a5 a5 66 98 t=26437
  f = open(device_file, "r")
  //lines = f.readlines()
  f.close()
  
  float temp = 123.11;
  return temp;
}

//----- test function
int testscipt(void) {
  cout << "Start testing..." << endl;
  set_mischer(3);
  set_mischer(1);
  set_mischer(1);
  set_mischer(0);
  set_mischer(8);
  float temperatur = get_temp("28-3c01a81688f4");
  cout << "temp " << temperatur << endl;
  cout << "Done testing..." << endl;
  return 0;
}
