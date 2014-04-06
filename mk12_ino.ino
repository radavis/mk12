// mk12.ino - Arduino Mega 2650 + 12 potentiometers + MIDI port

const int KNOBS = 12;    // analog inputs on A0-A11
const int READS = 1;     // number of reads to average
const float ALPHA = 0.1; // alpha value to determine when knob_data[i] changes

float knob_data[KNOBS];   // running average of knob positions (0 to 1023)
float knob_write[KNOBS];  // last value written to serial port

byte status_byte = 0xB0;  // MIDI channel 1
byte cc_start = 0x46;     // Control Change start value
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
  for (int i = 0; i < READS; i++) {
    for (int j = 0; j < KNOBS; j++) {
      knob_data[j] = ALPHA * analogRead(j) + (1-ALPHA) * knob_data[j];
    }
  }
}

void sendData() {
  for (int i = 0; i < KNOBS; i++) {
    if (ceil(knob_data[i]/8) != ceil(knob_write[i]/8)) {  // translate 10-bit to 8-bit
      sendMidi(status_byte, cc_start + i, (int)(knob_data[i]/8));
      knob_write[i] = knob_data[i];
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
