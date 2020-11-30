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
  
  string fullFileName = argv[0];
  size_t fileNamePos = fullFileName.find("/");
  if (fileNamePos == string::npos) {
    cout << "file name not found, abort" << endl;
    return -100;
  }
  string fileName = fullFileName.substr(fileNamePos+1);
  
  cout << fileName << endl;
  
  return 0;
}
