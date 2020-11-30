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
  string timestamp = date::format("%F %X %Z", now);
  string date = date::format("%F", now);
  
  
  string fullFileName = argv[0];
  size_t fileNamePos = fullFileName.find_last_of("/");
  if (fileNamePos == string::npos) {
    cout << "file name not found, abort" << endl;
    return -100;
  }
  string file = fullFileName.substr(fileNamePos+1);
  string fileName = file + "_" + date + ".log";
  
  cout << fileName << endl;
  cout << timestamp << endl;
  
  return 0;
}
