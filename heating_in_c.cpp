#include <wiringPi.h>

int wwallout[7] = {28, 25, 6, 5, 21, 4, 2};

int setup (void) {
  wiringPiSetup ();
  for (int i = 0; i < 7; i++){
    pinMode (wwallout[i], OUTPUT);
  }
}

int init (void) {
  for (int i = 0; i < 7; i++){
    digitalWrite (wwallout[i], HIGH);
  }
}

int main (void) {
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