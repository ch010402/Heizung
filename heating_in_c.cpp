#include <wiringPi.h>
#include <iostream>

using namespace std;

//----- initial variables and constantes
int wwallout[7] = {28, 25, 6, 5, 21, 4, 2}; // all warm water output needed
int em_catch = 36; // time to catch the elektro mischer (in s)
int em_fullturn = 80; // time to rotate the mischer fully (in s)
int em_pos = 9; // 9 unknown range 0-8

//----- functions 
int setup (void) {
  // setup wiringPi -> all needed GPIO pins
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
  digitalWrite (6, HIGH);
  digitalWrite (5, LOW);
  delay ((em_catch + em_fullturn)*1000);
  digitalWrite (5, HIGH);
  em_pos = 0;
  cout << "Init Mischer Done" << endl;
  
  return 0;
}

void set_mischer (int new_pos) {
  // calculate step size
  int step = em_fullturn / 8 ;
  // check if we need to open (+) or close (-) the mischer
  if (new_pos > em_pos) {
    // open mischer
    int set_pos = new_pos - em_pos;
    digitalWrite (5, HIGH);
    digitalWrite (6, LOW);
    delay (set_pos * step * 1000);
    digitalWrite (6, HIGH);
    em_pos = new_pos;
    cout << "Set Mischer to position" << em_pos << endl;
  }
  else if (new_pos < em_pos) {
    // close mischer
    int set_pos = em_pos - new_pos;
    digitalWrite (6, HIGH);
    digitalWrite (5, LOW);
    delay (set_pos * step * 1000);
    digitalWrite (5, HIGH);
    em_pos = new_pos;
    cout << "Set Mischer to position" << em_pos << endl;
  }
  else {
    // do nothing
    cout << "Mischer at position" << em_pos << endl;
    return 0; } 
}

//----- main
int main (void) {
  cout << "Starting..." << endl;
  setup();
  
// test 
  int wait = 500;
  for (;;) {
    digitalWrite (2, HIGH);
    delay (wait);
    digitalWrite (2, LOW);
    delay (wait);
    }
  return 0;
}