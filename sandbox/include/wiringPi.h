#pragma once

/// <summary>
/// FAKE just to make intellisense happy 
/// </summary>

enum state 
{
	LOW = 0, HIGH
};

enum kind
{
	OUTPUT = 0, INPUT 
};

void digitalWrite(int pin, state) {};
void wiringPiSetup() {};
void pinMode(int pin, kind) {};
void delay(int milliseconds) {};
