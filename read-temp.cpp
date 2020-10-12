#include <iostream>
#include <fstream>
#include <string>

using namespace std;

string device = "28-3c01a81688f4";
string basedir = "/sys/bus/w1/devices/";
string subdir = "/w1_slave";
string text;
double temp = 123.45;

int main (void) {
  cout << "Starting..." << endl;
  string address = basedir + device + subdir;

  ifstream file( address );
  while (getline (file, text)) {
    cout << text << endl;
/*  // ACHTUNG bricht bei der 2. Linie ab, fixen
    if ( text.find("YES") == string::npos) {
      cout << "checksum error, temperatur not read" << endl;
      break; 
    }
*/
/*    if ( text.find("t=") != string::npos) {
      int pos = text.find("t=");
      cout << "pos: " << pos << endl;
      // int temp_raw = 
    } */
    cout << text.find("t=") << endl;
  }
  file.close();

 // cout << text << endl;
  cout << "Done." << endl;
  return 0;
}
