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
  cout << argv[0] << endl;

  return 0;
}
