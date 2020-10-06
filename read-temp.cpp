#include <iostream>
#include <fstream>
#include <string>

using namespace std;

string device = "28-3c01a81688f4";
string basedir = "/sys/bus/w1/devices/";
string subdir = "/w1_slave";
string text;
float temp = 123.45;

int main (void) {
  cout << "Starting..." << endl;
  string address = basedir + device + subdir;

  ifstream file( address );
  while (getline (file, text)) {
    cout << text << endl;
    if ( text.find("YES") == string::npos) {
      break;
    }
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
