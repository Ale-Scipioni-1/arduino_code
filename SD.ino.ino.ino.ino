#include <SPI.h>
#include <SD.h>
#define ANALOG_PIN A0

const int chipSelect = 6;
unsigned long currentMillis = 0;
unsigned long lastMillis = 0;

void setup() {
  pinMode(ANALOG_PIN, INPUT);
  analogReadResolution(14);
  
  Serial.begin(500000);
  while (!Serial);

  Serial.println("Initializing SD card:");
  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed.");
    Serial.println("Note: press reset button.");
    while (true);
  }
  Serial.println("initialization done.");
}

void analogReading(float* samples, const int sampleSize) {
  //analog_reading-conversion-filter
  for (int i = 0; i < sampleSize; i++) {
    samples[i] = analogRead(ANALOG_PIN) * (5.0 / 16383.0); 
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
}

void fileWriting(float* samples, const int sampleSize,const char* fileName) {
  //int_to_char_conversion
  String stringSamples = "";
  for (int i = 0; i < sampleSize; i++) {
    stringSamples += String(samples[i], 3);
    if (i < 49) {
      stringSamples += " V  ";
    }
  }
  //file_writing
  File dataFile = SD.open(fileName, FILE_WRITE);
  if (dataFile) {
    dataFile.println(stringSamples);
    dataFile.close();
  }
  else {
    Serial.println("Error writing file.");
  }
}

void fileReading(const char* fileName) {
  File dataFile = SD.open(fileName, FILE_READ);
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

void loop() {
  const int sampleSize = 50;
  float samples[sampleSize];
  analogReading(samples, sampleSize);

  const char* fileName = "DATALOG.txt";
  currentMillis = millis();
  if(!(currentMillis - lastMillis <= 500)){
    lastMillis = currentMillis; 
    fileWriting(samples, sampleSize, fileName);  
    Serial.print("writing done. ");
    fileReading(fileName);  
    SD.remove(fileName);
  }     
}
