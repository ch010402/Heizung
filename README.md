# Heizung
 Heizungssteuerung umgeschrieben auf wiringPi </br>
 **ACHTUNG wiringPi ist deprecated http://wiringpi.com/news/** </br>
- [ ] -> http://abyz.me.uk/rpi/pigpio/index.html als Alternative anschauen

## Outputs
	
nr. | shortname | BCM | wPI | O | Bezeichnung | Kommentar
---|---|---|---|---|---|---
 1 | up  | 21 | **29** | 1.4 | Umlaufpumpe |
 2 | bp  | 20 | **28** | 1.3 | Boilerpumpe |
 3 | ep  | 26 | **25** | 1.2 | Elektropumpe |
 4 | op1 | 19 | **24** | 1.1 | Ofenpumpe Stufe 1 |
 5 | op2 | 16 | **27** | 3.4 | Ofenpumpe Stufe 2 |
 6 | op3 | 12 | **26** | 3.3 | Ofenpumpe Stufe 3 |
 7 | hma | 13 | **23** | 3.2 | Heizmischer auf |
 8 | hmz |  6 | **22** | 3.1 | Heizmischer zu |
 9 | ema | 25 |  **6** | 2.4 | Elektromischer auf |
10 | emz | 24 |  **5** | 2.3 | Elektromischer zu |
11 | bv  |  5 | **21** | 2.2 | Boilerventil auf/zu |
12 | sv  | 22 |  **3** | 2.1 | Speicherventil auf/zu | 
13 | er  | 23 |  **4** | 1.4 | Elektrorelais | 
14 | sp1 | 18 |  **1** | 1.3 | Reserve | 
15 | sp2 | 27 |  **2** | 1.2 | Reserve |
16 | sp3 | ~~17~~ |  **0** | 1.1 | Reserve | defekt

## Inputs
device address | description
---|---
28-0416a10c28ff | Aussen Temperatur
28-0516a12e05ff | Heizung Vorlauf
28-0316a162a9ff | Heizung Rücklauf
28-0516a1529bff | Boiler Vorlauf
28-0416a10f65ff | Boiler Rücklauf
28-0416a13049ff | Boiler Temperatur Oben
28-0416a1295fff | Boiler Temperatur Unten
28-0516a15419ff | Elektro Vorlauf
28-0316a15a36ff | Elektro Rücklauf
28-0416a12058ff | Ofen Vorlauf
28-0416a10e34ff | Ofen Rücklauf
28-0316a15f04ff | Speicher Oben
28-0416a10cc1ff | Speicher Mitte
28-0416a10de1ff | Speicher Unten
28-3c01a81688f4 | TestSensor @ progpi

## Tasks

- [x] Setup output
- [ ] ~~Initialisierung~~ -> implemented in classes 
- [x] Read DS18B20 Sensor
- [x] Read multiple Sensors and react to them
- [X] Long test run script -> successful 
- [x] Build mix valve class -> mixer
- [x] Long test run script -> >600d
- [ ] ~~Change cabling~~
- [ ] ~~Extend script to oven pump~~
- [ ] ~~Long test run script~~
- [ ] ~~Change cabling~~
- [ ] Calculate heating function
- [ ] Build heating class function ???
- [ ] Change cabling (control heating mixer)
- [ ] Long test run script
