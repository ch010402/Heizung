/*
MIT license 
ch010402
*/

#include <iostream> // cout
#include <fstream> //file stream
#include <string>
#include <sstream> 

using namespace std;

double getTemp(string deviceAddress, bool debug = false) {
  
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

int main(void){

  string testSensor = "28-3c01a81688f4";
  
  double temperatur = getTemp(testSensor);
  cout << "es ist " << temperatur << "°C" << endl;
  
  return 0; 
} 