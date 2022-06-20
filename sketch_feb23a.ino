/* 
Arduino code for HAS-PAS system - TV "Mutter" according to landline status
Author: Elad Shabo

* Voltage mapping:
  ================

  +----------+---------+
  |  Status  | Voltage |
  +----------+---------+
  | on-hook  |  ~1.96V |
  | off-hook |  ~4.24V |
  +----------+---------+
  These voltages values depeneds on the distance from your home to the near telephone exchange of your telephon copmany.


* Pinout:
  =======
  +-----+----------------------------+
  | Pin |        Description         |
  +-----+----------------------------+
  |  A0 | Line in use circuit output |
  |  D3 |       IR emitter LED       |
  +-----+----------------------------+
  
*/

#include <IRremote.h>

const float THRESHOLD = 3.2;  // Voltage threshold for line voltage, each setup should use a different THD according to your telephone settings
const unsigned long IR_CODE = 0x20DF906F;   //LG MUTE code
const int IR_CODE_LENGTH = 32;
IRsend irsend;
float last_volt_read = 0; // IR emitter will be triggered only after two consecutive values above the THD
bool off_hook_detected = false;

void setup() {
  Serial.begin(9600);
}

void loop() {
  // Read the input on analog pin 0 (which goes from 0 - 1023) and convert it to a voltage (0 - 5V)
  float voltage = analogRead(A0) * (5.0 / 1023.0);

  Serial.print(voltage);

  // Phone goes off hook - mute TV
  if (!off_hook_detected  
      && (voltage > THRESHOLD) 
      && (last_volt_read > THRESHOLD)){
    off_hook_detected = true;
    Serial.print("**STATUS CHANGE** - Phone is now OFF-HOOK");
    irsend.sendLG(IR_CODE, IR_CODE_LENGTH);
  }
  
  // Phone goes on hook again - unmute TV
  else if (off_hook_detected 
           && (voltage < THRESHOLD) 
           && (last_volt_read < THRESHOLD)) {
    Serial.print("**STATUS CHANGE** - Phone is now ON-HOOK");
    off_hook_detected = false;
    irsend.sendLG(IR_CODE, IR_CODE_LENGTH);
  }  
  
  last_volt_read = voltage;
  // Sampleing every 100ms lowers the chance to sample noises on the line
  delay(100);
}
