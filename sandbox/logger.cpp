/*
MIT license 
created: ch010402 23.11.2020
  learning: argc (count of argumnets given with the program start) argv (arguments array)
changed: ch010402 30.11.2020
*/


#include <iostream>   // cout
#include <chrono>
#include "include/date.h"
#include <string>

using namespace std;

class logger{
  public:
  string argv_0;
  //constructor
  logger(string filename){
    argv_0 = filename;
  }
  void log(string msg) {
    // example text
    cout << filename(argv_0) << "\n";
    cout << timestamp() << " : " << msg << "\n";
  }
  private:
  string timestamp(){
    auto now = chrono::system_clock::now();
    string ts = date::format("%F %X %Z", now);
    return ts;
  }
  string today(){
    auto now = chrono::system_clock::now();
    string td = date::format("%F", now);
    return td;
  }
  string filename(string argv){
    string fullFileName = argv;
    size_t fileNamePos = fullFileName.find_last_of("/");
    if (fileNamePos == string::npos) {
      cout << "file name not found, abort" << endl;
      return "filename";
    }
    string file = fullFileName.substr(fileNamePos+1);
    string fn = file + "_" + today() + ".log";
    return fn;
  }
};

int main(int argc, char** argv){
  
  logger log(argv[0]);
  log.log("Hallo Test");

  return 0;
}

/*
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

  for (int i = 0; i < argc; i++){
    cout << argv[i] << endl;
  }
  
  return 0;
}
*/