/*
MIT license 
created: ch010402 23.11.2020
  learning: argc (count of argumnets given with the program start) argv (arguments array)
changed: ch010402 30.11.2020
description:
  Logger class with main methode log(string msg) takes a string and appends it to a logfile that has the same name as the running script with the current date. If the logfile is bigge than 1 MB (hard coded) a new logfile will be created either with the current date or if the same date with an itterator eg: -1..n.
*/


#include <iostream>       // cout
#include <chrono>         // get the syste
#include "include/date.h" // format the time https://howardhinnant.github.io/date/date.html#to_stream_formatting
#include <string>         // handle strings
#include <fstream>        // handle files 
#include <unistd.h>       // used for usleep

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
    // open or create file 
    ofstream logfile;
    logfile.open(file, ios::app);
    // write message with timestapm to file
    if (logfile.is_open()){
      logfile << timestamp() << " : " << msg << "\n";
    }
    else cout << "Unable to open file";
    int size = logfile.tellp();
    // close file
    logfile.close();
    cout << "log entry written, file size "<< size <<" Byte \n";
    // check if filesitze is bigger than 1M bytes
    if (size > 1000000) {
      // exit(EXIT_FAILURE);
      initialized = false;
    }
  }
  private:
  string timestamp(){
    // get the current timestamp with timezone
    auto now = chrono::system_clock::now();
    string ts = date::format("%F %X %Z", now);
    // the timezone is UTC TODO: change to locale tz
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
      return "nofilename";
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
    // if the filename already exists create a new one with -itterator 
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
  
  bool test = true;
  if (test) {
    Logger log(argv[0]);
    for (int i =0; i < 5000; i++ ){
      log.log("This is log entry number " + to_string(i));
      usleep(20 * 1000 *1000);
    };
  }
  return 0;
};