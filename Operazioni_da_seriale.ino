//Arduino 
//Lettura di comandi da seriale tramite menu.
void setup() {
  Serial.begin(500000);
  while (!Serial);  
}

void readFile(String &filename) {
  Serial.println("Error opening file " + String(filename));
}

void readCommand(String &command) {
  if (Serial.available() > 0) {
    command = Serial.readStringUntil('\n');
    command.trim(); 
  }
}

void showMenu(String &command, String &filename) {
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
  String filename = "DATALOG.txt";
  String command = "";

  showMenu(command, filename);
}


