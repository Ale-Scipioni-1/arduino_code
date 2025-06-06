//Done by AleScipioni with ARDUINO UNO R4//
#include <SPI.h>
#include <SD.h>
#define ANALOG_PIN_0 A0
#define ANALOG_PIN_1 A1

String filename = "";
const int chipSelect = 6;

void setup() {
  Serial.begin(1000000);
  while (!Serial);  

  Serial.println(F(">>> -----SETUP----- >"));
  if (!SD.begin(chipSelect)) {
    Serial.println(F(">>> SD initialization error. Press reset button."));
    while (true);
  }
  Serial.println(F(">>> SD initialization successful."));
  Serial.println(F(">>> Insert the working file: "));
}

void readFile(String &filename) {
  File dataFile = SD.open(filename, FILE_READ);
  if (dataFile) {
    Serial.println(F("Reading file:"));
    String stringSamples = "";
    while (dataFile.available()) {
      stringSamples = dataFile.readStringUntil('\n');
      Serial.println(stringSamples);
    }
    dataFile.close();
  } else {
    Serial.println(F("Error opening file."));
  }
}

void writeFile(String &filename) {
  //analog_reading
  const int sampleSize = 50;
  float samples[sampleSize];
  analogReading(samples, sampleSize);

  //file_writing
  File dataFile = SD.open(filename, FILE_WRITE);
  if (dataFile) {
    dataFile.print(millis());
    dataFile.print(",");
    dataFile.print(samples[0]);
    dataFile.print(",");
    dataFile.println(samples[1]);
    dataFile.close();
  }
  else {
    Serial.println(F(">>> Error writing file."));
  }
}

void analogReading(float* samples, const int sampleSize) {
  //analog_reading-conversion-filter
  float outSamples[2];

  //ANALOG_PIN_0
  for (int i = 0; i < sampleSize; i++) {
    samples[i] = analogRead(ANALOG_PIN_0) * (5.0 / 16383.0); 
    delay(1); 
  }

  for (int i = 0; i < sampleSize - 1; i++) {
    for (int j = 0; j < sampleSize - i - 1; j++) {
      if (samples[j] > samples[j + 1]) {
        float temp = samples[j];
        samples[j] = samples[j + 1];
        samples[j + 1] = temp;
      }
    }
  }
  outSamples[0] = samples[sampleSize/2];

  //ANALOG_PIN_1
  for (int i = 0; i < sampleSize; i++) {
    samples[i] = analogRead(ANALOG_PIN_1) * (5.0 / 16383.0); 
    delay(1); 
  }

  for (int i = 0; i < sampleSize - 1; i++) {
    for (int j = 0; j < sampleSize - i - 1; j++) {
      if (samples[j] > samples[j + 1]) {
        float temp = samples[j];
        samples[j] = samples[j + 1];
        samples[j + 1] = temp;
      }
    }
  }
  outSamples[1] = samples[sampleSize/2];

  samples[0] = outSamples[0];
  samples[1] = outSamples[1];
}

void readCommand(String &command) {
  command = "";
  if (Serial.available() > 0) {
    command = Serial.readStringUntil('\n');
    command.trim(); 
  }
}

void menu(String &command, String &filename){ 
  showMenu(filename);
  while (command == "") {
    readCommand(command);
  }
  Serial.println(String(F("user: ")) + command);
  if (command == "A") {
    Serial.println(F(">>> Writing file, insert STOP to end."));
    while(command != "STOP"){
      readCommand(command);
      writeFile(filename);
    }
    Serial.println(String(F("user: ")) + command);
    Serial.println(F(">>> Writing successful."));
  }
  else if (command == "B") {
    readFile(filename);
  }
  else if (command == "C") {  
    if (SD.remove(filename)) {
      Serial.println(F(">>> Removal successful."));
    } else {
      Serial.println(F(">>> Removal error."));
    }
  }
  else if (command == "D") {  
    showInfo();
  }
  else {
    Serial.println(F(">>> Invalid command."));
  }
}

void loop() {
  while(filename == ""){
    readCommand(filename);
  }
  String command = "";
  menu(command, filename);
}

void showMenu(String &filename) {
  Serial.println(F(""));
  Serial.println(F(">>> -----COMMANDS----- >"));
  Serial.println(F("    [A] Log A0 A1"));
  Serial.println(String(F("    [B] Read ")) + filename);
  Serial.println(String(F("    [C] Remove ")) + filename);
  Serial.println(F("    [D] info"));
}

void showInfo(){
  Serial.println(F(">>>-----------> Programmable storage system >>>-----------> PSS"));
  Serial.println(F("SETUP -> Insert working file, automatic reading of pins A0 A1"));
  Serial.println(F("COMMANDS -> A start writing to the working file from pins A0 A1"));
  Serial.println(F("            B read the working file"));
  Serial.println(F("            C remove the working file"));
  Serial.println(F("            D help"));
}
