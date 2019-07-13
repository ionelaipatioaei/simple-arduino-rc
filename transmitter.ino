#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// nRF24L01 (CE, CSN)
RF24 radio(7, 8);

// The address needs to match the receiver address
const byte address[6] = "00001";

// Create a struct which will store the values which will be sent
struct Data_Package {
  byte j1PotX;
  byte j1PotY;
  byte j2PotX;
  byte j2PotY;
  byte pot1;
};
Data_Package data;

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.openWritingPipe(address);
  radio.setAutoAck(false);
  // Lower data rate gives more range
  // RF24_250KBPS for 250kbs, RF24_1MBPS for 1Mbps, or RF24_2MBPS for 2Mbps
  radio.setDataRate(RF24_250KBPS);
  // Set Power Amplifier (PA) level to one of four levels: RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH and RF24_PA_MAX
  // Use RF24_PA_MAX for the max range
  radio.setPALevel(RF24_PA_MAX);
  
  // Set initial default values
  // Values from 0 to 255. When Joystick is in resting position, the value is in the middle, or 127
  data.j1PotX = 127; 
  data.j1PotY = 127;
  data.j2PotX = 127;
  data.j2PotY = 127;
  data.pot1 = 1;
}

void loop() {
  // Read all analog inputs and map them to one Byte value
  data.j1PotX = map(analogRead(1), 0, 1023, 0, 255);
  data.j1PotY = map(analogRead(2), 0, 1023, 0, 255);
  data.j2PotX = map(analogRead(3), 0, 1023, 0, 255);
  data.j2PotY = map(analogRead(4), 0, 1023, 0, 255);
  data.pot1 = map(analogRead(0), 0, 1023, 0, 255);
  // Send the whole data from the structure to the receiver
  radio.write(&data, sizeof(Data_Package));
}
