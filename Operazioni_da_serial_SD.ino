#include <SPI.h>
#include <SD.h>
#define ANALOG_PIN_0 A0
#define ANALOG_PIN_1 A1

const int chipSelect = 6;

void setup() {
  Serial.begin(500000);
  while (!Serial);  

  if (!SD.begin(chipSelect)) {
    Serial.println(">>> SD initialization error. Press reset button.");
    while (true);
  }
  Serial.println(">>> SD initialization successful.");
}

void readFile(String &filename) {
  File dataFile = SD.open(filename, FILE_READ);
  if (dataFile) {
    Serial.println("Reading file:");
    String stringSamples = "";
    while (dataFile.available()) {
      stringSamples = dataFile.readStringUntil('\n');
      Serial.println(stringSamples);
    }
    dataFile.close();
  } else {
    Serial.println("Error opening file.");
  }
}

void writeFile(String &fileName) {
  //analog_reading
  const int sampleSize = 50;
  float samples[sampleSize];
  analogReading(samples, sampleSize);

  //file_writing
  File dataFile = SD.open(fileName, FILE_WRITE);
  if (dataFile) {
    dataFile.print(millis());
    dataFile.print(",");
    dataFile.print(samples[0]);
    dataFile.print(",");
    dataFile.println(samples[1]);
    dataFile.close();
  }
  else {
    Serial.println("Error writing file.");
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
  if (Serial.available() > 0) {
    command = Serial.readStringUntil('\n');
    command.trim(); 
  }
}

void showMenu(String &command, String &filename) {
  Serial.println("");
  Serial.println(">>> Select mode:");
  Serial.println("  A Read " + filename);
  Serial.println("  B Log A0 A1");
  Serial.println("  C Remove " + filename);
  while (command == "") {
    readCommand(command);
  }

  if (command == "A") {
    readFile(filename);
  }

  else if (command == "B") {
    Serial.println(">>> Writing file, insert STOP for stop.");
    while(command != "STOP"){
      readCommand(command);
      writeFile(filename);
    }
    Serial.println(">>> Writing successful.");
  }

  else if (command == "C") {  
    if (SD.remove(filename)) {
      Serial.println(">>> Removal successful.");
    } else {
      Serial.println(">>> Removal error.");
    }
  }

  else {
    Serial.println(">>> Invalid command.");
  }
}

void loop() {
  String filename = "DATALOG.csv";
  String command = "";
  showMenu(command, filename);
}

