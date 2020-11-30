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
#include <fstream>
#include <unistd.h>

using namespace std;

class Logger{
  public:
  string argv_0;
  //constructor
  Logger(string filename){
    argv_0 = filename;
  }
  // methodes
  void log(string msg) {
    // check if inizilized, if not do
    if (!initialized) initialize();
    // check if filesitze is bigger than xy bytes
    // todo
    // open or create file 
    ofstream logfile;
    logfile.open(file, ios::app);
    // write message with timestapm to file
    if (logfile.is_open()){
      logfile << timestamp() << " : " << msg << "\n";
    }
    else cout << "Unable to open file";
    // close file
    int size = logfile.tellp();
    logfile.close();
    cout << "log entry written, file size "<< size <<"Byte \n";
    if (size > 1000000) {
      exit(EXIT_FAILURE);
      initialized = false;
    }
  }
  private:
  string timestamp(){
    // get the current timestamp with timezone
    auto now = chrono::system_clock::now();
    string ts = date::format("%F %X %Z", now);
    return ts;
  }
  string today(){
    // get the date of today
    auto now = chrono::system_clock::now();
    string td = date::format("%F", now);
    return td;
  }
  string filename(string argv){
    // compose the filename from the executable file the current date and .log 
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
  bool initialized = false;
  string file;
  void initialize(){
    // inzilialize = create the filename 
    file = filename(argv_0);
    int i = 1;
    while (filesystem::exists(file)){ 
      string s = to_string(i);
      int minus = file.length() - file.find_last_of("-");
      if (i > 1) file = file.erase(file.size()-minus);
      else file = file.erase(file.size()-4);
      file = file + "-" + s + ".log";
      i++;
    }
    initialized = true;
  }
};

int main(int argc, char** argv){
  
  Logger log(argv[0]);
  for (int i =0; i < 5000; i++ ){
    log.log("This is a verry long log entry that probably will not happen in real life but for testin reasons we try how much disk space it will use up per log to calculate how many are needed to fill one Megabyte.");
    //usleep(5 * 1000 *1000);
  };
  return 0;
};