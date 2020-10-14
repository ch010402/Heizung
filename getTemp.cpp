/*
MIT license 
ch010402
*/

#include <iostream> // cout
#include <fstream> //file stream
#include <string>
#include <sstream> 

using namespace std;

double getTemp(string deviceAddress) {
  
  cout << "start getting temperatur" << endl;
  double temp = 987.65;
  string baseDir = "/sys/bus/w1/devices/";
  string tempFile = "/w1_slave";
  string path = baseDir + deviceAddress + tempFile;
  stringstream buffer;
  string data;
  
  cout << "try to read file " << path << endl;
  ifstream infile(path);
  if (infile) {
    cout << "reading content" << endl;
    buffer << infile.rdbuf();
    data = buffer.str();
    infile.close();
  }
  else {
    infile.close();
    cout << "Error reading file at " << path << endl;
    return -1;
  }
  
  size_t found = str.find("YES");
  if (found != string::npos) cout << "ok" << endl;
  
  cout << data;
  return temp;
}

int main(void){
  
  double temperatur = getTemp("28-3c01a81688f4");
  cout << "es ist " << temperatur << "°C" << endl;
  
  return 0; 
} 