/*
MIT license 
created: ch010402 23.11.2020
  learning:
changed: 
*/


#include <iostream>   // cout
#include <chrono>
#include "include/date.h"
#include <string>

using namespace std;


int main(int argc, char** argv){

  auto now = chrono::system_clock::now(); 
  string formated_now = date::format("%F %X %Z", now);
  cout << formated_now << endl;
  
  site_t fileNamePos = argv[0].find("/");
  if (fileNamePos == string::npos) {
    cout << "file name not found, abort" << endl;
    return -100;
  }
  string fileName = argv[0].substr(fileNamePos+1);
  
  cout << filename << endl;

  /*
  size_t TempPos = data.find("t=");
      if (TempPos == string::npos) {
        cout << "failed to find value -> abort!" << endl;
        return -102;
      }
      string strTemp = data.substr(TempPos+2);
      double temp = stod(strTemp)/1000;
      return temp;
    }
  */
  
  return 0;
}
