#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

using namespace std;

char device[16] = "28-3c01a81688f4";



double temp = -123.45;
char buffer_in[265];

double get_temp (const char* devAddr) {
  //create path
  char basedir[21] = "/sys/bus/w1/devices/";
  char subdir[10] = "/w1_slave";
  char path[47];
  //char* devAddr_ = strdup(devAddr);
  snprintf(path, 46, "%s%s%s", basedir, devAddr, subdir);
  //open file and check if openend
  FILE *devFile = fopen(path, "r");
  if (devFile == NULL) {
    fclose(devFile);
    cout << "Unable to open file" << path << endl;
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
  
  temp = get_temp(device);
  
  cout << "Temperatur: " << temp << endl;

  cout << "Done." << endl;
  return 0;
}
