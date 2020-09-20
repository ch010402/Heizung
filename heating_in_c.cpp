#include <wiringPi.h>
#include <iostream>

using namespace std;

// initial variables and constantes
int wwallout[7] = {28, 25, 6, 5, 21, 4, 2}; // all warm water output needed
int em_catch = 36; // time to catch the elektro mischer (in s)
int em_fullturn = 84; // time to rotate the mischer fully (in s)

int setup (void) {
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
  return 0;
}

int e-mischer (bool status) {
  digitalWrite (6, HIGH); // ema 
  digitalWrite (5, HIGH); // emz
  if (status == true) {
    digitalWrite (6, LOW);
  }
  else {
    digitalWrite (5, LOW);
  }
}

int init (void) {
  // enter function to switch mischer
  cout << "Setup done" << endl;
  return 0;
}

int main (void) {
  cout << "Start script" << endl;
  setup();
  init();
  
  int wait = 500;
  for (;;) {
    digitalWrite (2, HIGH);
    delay (wait);
    digitalWrite (2, LOW);
    delay (wait);
    }
  return 0;
}