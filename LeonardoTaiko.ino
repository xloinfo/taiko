#include <Keyboard.h>
#include <EEPROM.h>

//Analog Monitor feature
//#define DEBUG


const float min_threshold = 25;  // The minimum rate on triggering an input
const int cd_length = 10; //Buffer loop times
const float k_decay = 0.99                                                                  ; //Decay speed on the dynamite threshold
const float k_increase = 0.8;  //Dynamite threshold range
const int outputDuration_pc = 30; // For PC. How long a key should be pressed when triggering an input

const int keymapping[4] = {'f','d','j','k'};


// Mode and computation key and buffer
int key;
const int buffer_size = cd_length * 4;
int buffer[buffer_size];
int threshold = min_threshold;


void setup() {
  pinMode(0, INPUT_PULLUP);
  Keyboard.begin();  // Initialize in PC mode
}


void loop() {
  
  #ifdef DEBUG
  analogMonitor();
  #endif
  
  #ifndef DEBUG
  unsigned long begin = millis();
  extendKey();
  bool output = false;
  int sensorValue[] = {analogRead(A0), analogRead(A3), analogRead(A1), analogRead(A2)};
  for (int i = 0; i <= 3; i++) {
    if (sensorValue[i] > threshold) {
      output = true;
    }
  }
  if (output){
    //Store pin value into buffer
    int j = 0;
    while (j < buffer_size) {
      for (int pin = A0; pin < A4; pin++) {
        buffer[j] = analogRead(pin);
        j++;
      }
    }
    //Find the largest value
    int temp = buffer[0];
    int count = 1;
    for (int i = 0; i < buffer_size; i++) {
      if (temp < buffer[i]) {
        temp = buffer[i];
        count = i + 1;
      }
    }
    threshold = temp * k_increase;
    key = count % 4;

    if (temp >= min_threshold) {
      Keyboard.press(keymapping[key]);
      delay(outputDuration_pc);
      Keyboard.releaseAll();
    }
  }
  
  if (threshold < min_threshold) {
    threshold = min_threshold;
  } else if (threshold > min_threshold) {
    threshold = threshold * k_decay;
  }
  #endif
}


void extendKey() {
  // Additional key extension logic if needed
}


void analogMonitor() {
  Serial.print("||");
  Serial.print(analogRead(A1));
  Serial.print("||");
  Serial.print(analogRead(A0));
  Serial.print("||");
  Serial.print(analogRead(A2));
  Serial.print("||");
  Serial.print(analogRead(A3));
  Serial.println("||");
}
