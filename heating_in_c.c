#include <wiringPi.h>

int main (void) {
  wiringPiSetup ();
  pinMode (29, OUTPUT);
  
  int wait = 500;
  for (;;) {
    digitalWrite (29, HIGH);
    delay (wait);
    digitalWrite (29, LOW);
    delay (wait);
    }
  return 0;
}