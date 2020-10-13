#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

using namespace std;

string device = "28-3c01a81688f4";
string basedir = "/sys/bus/w1/devices/";
string subdir = "/w1_slave";
string text;
double temp = -123.45;
char buffer_in[265];

double get_temp (const char* dev_address) {
  //open file and check if openend
  FILE *devFile = fopen(dev_address, "r");
  if (devFile == NULL) {
    fclose(devFile);
    cout << "Unable to open file" << dev_address << endl;
    return 1;
  }
  //define variable
  double temperatur = 987.65;
  int tempInt;
  char crcConf[5];
  //read file
  fscanf(devFile, "%*x %*x %*x %*x %*x %*x %*x %*x %*x : crc=%*x %s", crcConf);
  if (strncmp(crcConf, "YES", 3) == 0) {
    fscanf(devFile, "%*x %*x %*x %*x %*x %*x %*x %*x %*x t=%5d", &tempInt);
    temperatur = tempInt / 1000.0;
  }
  fclose(devFile);
  
  return temperatur;
}


int main (void) {
  cout << "Start readning Temp-Sensor ..." << endl;
  string address = basedir + device + subdir;
  
  temp = get_temp(address);
  
  cout << "Temperatur: " << temp << endl;

  cout << "Done." << endl;
  return 0;
}
