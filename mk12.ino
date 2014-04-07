// mk12.ino - Arduino Mega 2650 + 12 potentiometers + MIDI port

const int KNOBS = 12;    // analog inputs on A0-A11
const float ALPHA = 0.1; // alpha value to determine when knob_data[i] changes

float knobData[KNOBS];   // running average of knob positions (0 to 1023)
float knobWrite[KNOBS];  // last value written to serial port

byte statusByte = 0xB0;  // MIDI channel 1
byte ccStart = 0x46;     // Control Change start value
                         // 0x46 to 0x4F -> Sound Controller 1-10
                         // 0x50 to 0x53 -> General Purpose 1-4

void setup() {
  Serial.begin(115200);  // for Serial USB->MIDI bridge
  Serial1.begin(31250);  // for MIDI port
}

void loop() {
  readKnobs();
  sendData();
}

void readKnobs() {
  for (int j = 0; j < KNOBS; j++) {
    knobData[j] = ALPHA * analogRead(j) + (1-ALPHA) * knobData[j];
  }
}

void sendData() {
  for (int i = 0; i < KNOBS; i++) {
    if (ceil(knobData[i]/8) != ceil(knobWrite[i]/8)) {  // translate 10-bit to 8-bit
      sendMidi(statusByte, cc_start + i, (int)(knobData[i]/8));
      knobWrite[i] = knobData[i];
    } 
  }
}

void sendMidi(byte stat, byte ctrl, byte value) {
  // Write to USB port
  Serial.write(stat);
  Serial.write(ctrl);
  Serial.write(value);
  
  // Write to MIDI port
  Serial1.write(stat);
  Serial1.write(ctrl);
  Serial1.write(value);
  
  // Debugging
  //Serial.print(stat);
  //Serial.print(',');
  //Serial.print(ctrl);
  //Serial.print(',');
  //Serial.print(value);
  //Serial.print('\n');
}
