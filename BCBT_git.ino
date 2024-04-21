/* by bewayos
  beta 0.2.3
  The project is under active development, additional data will be available soon, at the moment this code is needed to test the basic functions
*/
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

// Initializing the LCD
LiquidCrystal_I2C lcd(0x27, 16, 4);

// Analog input for buttons
const int btnPin = A0;

// Pin for relays
const int relayPins[3] = {13, 5, 4}; // Relay for each group

// Variables for menu control
int currentGroup = 0;
int currentParam = 0;
bool inGroup = false;  // Are we within the group
bool needUpdate = true;  // Checkbox to refresh the display
const int numGroups = 3;
const int numParams = 3;
float sensorValues[numGroups][numParams]; // Data from sensors
int thresholdValues[numGroups][numParams] = {
  {30, 50, 700}, // Set values for Temp, Humidity, Illuminance
  {25, 45, 600},
  {28, 50, 800}
};

void setup() {
  lcd.begin();
  lcd.backlight();
  pinMode(btnPin, INPUT);
  for (int i = 0; i < 3; i++) {
    pinMode(relayPins[i], OUTPUT);
  }
  Serial.begin(9600);
}

void loop() {
  readSerialData(); // Function for reading data from the serial port
  int btnValue = analogRead(btnPin);
  static int lastButtonState = -1;  // Save the last state of the button to ensure debounce
  static unsigned long lastDebounceTime = 0; // Time of the last valid press
  unsigned long debounceDelay = 50;

  // Checking the values from the analog input to determine the pressed button
  if (btnValue > 1012 && btnValue < 1014) {
    handleButtonPress(1, &lastButtonState, &lastDebounceTime, debounceDelay);
  } else if (btnValue > 1002 && btnValue < 1004) {
    handleButtonPress(2, &lastButtonState, &lastDebounceTime, debounceDelay);
  } else if (btnValue > 976 && btnValue < 978) {
    handleButtonPress(3, &lastButtonState, &lastDebounceTime, debounceDelay);
  } else if (btnValue > 853 && btnValue < 855) {
    handleButtonPress(4, &lastButtonState, &lastDebounceTime, debounceDelay);
  } else {
    lastButtonState = -1;// If there are no active clicks, reset the last state
  }

  // Check if the display needs to be updated
  if (needUpdate) {
    displayMenu();
    needUpdate = false;
  }
  delay(100);
}

void handleButtonPress(int buttonNumber, int* lastButtonState, unsigned long* lastDebounceTime, unsigned long debounceDelay) {
  unsigned long currentTime = millis();
  if (*lastButtonState != buttonNumber && (currentTime - *lastDebounceTime) > debounceDelay) {
    *lastButtonState = buttonNumber;
    *lastDebounceTime = currentTime;
    needUpdate = true;  // Check the box to update the display
    switch (buttonNumber) {
      case 1: // Next / switches parameters in the edit mode
        if (inGroup) {
          currentParam = (currentParam + 1) % numParams;
        } else {
          currentGroup = (currentGroup + 1) % numGroups;
        }
        break;
      case 2: // OK / BACK
        inGroup = !inGroup;
        currentParam = 0; // Reset the parameter selection when entering the group
        break;
      case 3: // Increasing the value of the parameter
        if (inGroup) thresholdValues[currentGroup][currentParam]++;
        break;
      case 4: // Reducing the value of the parameter
        if (inGroup) thresholdValues[currentGroup][currentParam]--;
        break;
    }
  }
}

void readSerialData() {
  // Check if there is any data available on the serial port
  if (Serial.available()) {
    String dataString = Serial.readStringUntil('\n');
    // Pass the data string to the parseSerialData function for further processing
    parseSerialData(dataString);
  }
}

void parseSerialData(String data) {
  // Iterate over each group of sensor data included in the received string
  for (int group = 0; group < numGroups; group++) {
    sensorValues[group][0] = getValue(data, group * 3);      // Temp
    sensorValues[group][1] = getValue(data, group * 3 + 1);  // Humidity
    sensorValues[group][2] = getValue(data, group * 3 + 2);  // Illuminance
    // After updating the sensor values, check if any thresholds are crossed
    checkThresholds(group);
  }
}

void checkThresholds(int group) {
  // Check each parameter in the specified group
  for (int param = 0; param < numParams; param++) {
    if (sensorValues[group][param] < thresholdValues[group][param]) {
      digitalWrite(relayPins[group], HIGH);
    } else {
      digitalWrite(relayPins[group], LOW);
    }
  }
}

float getValue(String data, int index) {
  int firstComma = 0;
  int secondComma = -1;
  // Go through the data string to the desired value index
  for (int i = 0; i <= index; i++) {
    firstComma = secondComma + 1;
    secondComma = data.indexOf(',', firstComma);
  }
  // Return the found value as a floating point number
  return data.substring(firstComma, secondComma).toFloat();
}

void displayMenu() {
  lcd.clear(); // Clear the display before displaying new information
  if (!inGroup) {
    // Display general information about the terrarium and the parameters of the current group
    lcd.print("Terrarium ");
    lcd.print(currentGroup + 1);
    lcd.setCursor(0, 1);
    lcd.print("T: ");
    lcd.print(sensorValues[currentGroup][0]);
    lcd.print(" H: ");
    lcd.print(sensorValues[currentGroup][1]);
    lcd.setCursor(0, 2);
    lcd.print("I: ");
    lcd.print(sensorValues[currentGroup][2]);
  } else {
    // Displaying parameters for editing
    lcd.print(thresholdValues[currentParam][2]);
    lcd.setCursor(0, 1);
    lcd.print("Value: ");
    lcd.print(sensorValues[currentGroup][currentParam]);
  }
}
