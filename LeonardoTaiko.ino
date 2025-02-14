#include <Keyboard.h>

//#define DEBUG

const float min_threshold = 25;  // The minimum rate on triggering a input
const int cd_length = 20; //Buffer loop times.
const float k_decay = 0.99; //decay speed on the dynamite threshold.
const float k_increase = 0.8;  //Dynamite threshold range.

const int outputDuration_pc = 30; // For PC. How long a key should be pressed when triggering a input.
const int outputDuration_sim = 8; // For simulator output duration

const int keymapping[4] = {'f','d','j','k'};

//
int mode; //0 for PC, 2 for simulator
int key;
const int buffer_size = cd_length*4;
int buffer[buffer_size];
int threshold = min_threshold;

bool switchMode = 0;

int buttonPressed = -1;

int buttonStatusLK = -1;
int buttonStatusLD = -1;
int buttonStatusRD = -1;
int buttonStatusRK = -1;

unsigned long currentMillis = 0;

unsigned long previousMillisLK_1 = 0;
unsigned long previousMillisLD_1 = 0;
unsigned long previousMillisRD_1 = 0;
unsigned long previousMillisRK_1 = 0;

void setup() {
  // 
  mode = 0;
  #ifdef DEBUG
  Serial.println("start with PC mode");
  #endif
  Keyboard.begin();              
}

void loop() {
  #ifdef DEBUG
  analogMonitor();
  #endif 

  #ifndef DEBUG
  unsigned long currentMillis = millis();
  
  // 处理PC模式按键释放
  if(buttonStatusLK != -1 && currentMillis - previousMillisLK_1 >= outputDuration_pc){
    Keyboard.release(keymapping[1]);
    buttonStatusLK = -1;
  }
  if(buttonStatusLD != -1 && currentMillis - previousMillisLD_1 >= outputDuration_pc){
    Keyboard.release(keymapping[0]);
    buttonStatusLD = -1;
  }
  if(buttonStatusRD != -1 && currentMillis - previousMillisRD_1 >= outputDuration_pc){
    Keyboard.release(keymapping[2]);
    buttonStatusRD = -1;
  }
  if(buttonStatusRK != -1 && currentMillis - previousMillisRK_1 >= outputDuration_pc){
    Keyboard.release(keymapping[3]);
    buttonStatusRK = -1;
  }

  bool output = false;
  int sensorValue[] = {analogRead(A0),analogRead(A3),analogRead(A1),analogRead(A2)};
  for (int i = 0; i <= 3; i++) {
    if (sensorValue[i] > threshold) {
      output = true;
    }
  }
  
  if (output){
    // Storage pin value into buffer
    int j = 0;
    while (j < buffer_size) {
      for (int pin = A0; pin < A4; pin++) {
        buffer[j] = analogRead(pin);
        j++;
      }
    }

    // Finding the largest value
    int temp = buffer[0];
    int count = 1;
    for (int i = 0; i < buffer_size; i++) {
      if (temp < buffer[i]) {
        temp = buffer[i];
        count = i+1;
      }
    }
    threshold = temp*k_increase;
    key = count%4;

    if(temp >= min_threshold*0.6){
      switch(key){
        case 1:
          buttonStatusLK = 1;
          Keyboard.press(keymapping[key]);
          previousMillisLK_1 = currentMillis;
          break;
        case 0:
          buttonStatusLD = 1;
          Keyboard.press(keymapping[key]);
          previousMillisLD_1 = currentMillis;
          break;
        case 2:
          buttonStatusRD = 1;
          Keyboard.press(keymapping[key]);
          previousMillisRD_1 = currentMillis;
          break;
        case 3:
          buttonStatusRK = 1;
          Keyboard.press(keymapping[key]);
          previousMillisRK_1 = currentMillis;
          break;
      }
      delay(10);
    }
  }
  
  if(threshold < min_threshold){
    threshold = min_threshold;
  }
  else if(threshold > min_threshold) {
    threshold = threshold*k_decay;
  }
  #endif
}

#ifdef DEBUG
void analogMonitor(){
  if(analogRead(A1)>20||analogRead(A2)>20||analogRead(A3)>20||analogRead(A0)>20){
    Serial.print("||");
    Serial.print(analogRead(A0));
    Serial.print("||");
    Serial.print(analogRead(A1));
    Serial.print("||");
    Serial.print(analogRead(A2));
    Serial.print("||");
    Serial.print(analogRead(A3));
    Serial.println("||");
  }
}
#endif
