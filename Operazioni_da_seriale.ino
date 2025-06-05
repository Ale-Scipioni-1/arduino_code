//Arduino 
//Lettura di comandi da seriale tramite menu.
void setup() {
  Serial.begin(500000);
  while (!Serial);  
}

void readFile(const char* fileName) {
  Serial.println("Error opening file.");
}

void readCommand(String &command) {
  if (Serial.available() > 0) {
    command = Serial.readStringUntil('\n');
    command.trim();  // remove trailing newline and spaces
  }
}

void showMenu(String &command, const char* filename) {
  Serial.println("Select mode:");
  Serial.println(" A Read-filename");

  while (command == "") {
    readCommand(command);
  }

  if (command == "A") {
    readFile(filename);
  } else {
    Serial.println("Invalid command.");
  }
}

void loop() {
  const char* filename = "DATALOG.txt";
  String command = "";

  showMenu(command, filename);
}
