// Download and include the RF24 library: https://github.com/nRF24/RF24
// Documenation here: http://tmrh20.github.io/RF24/classRF24.html 
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

// Control the brushless motor using an ESC
Servo esc;

// Servos which you can use for the rudder, elevator and ailerons
Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;

// nRF24L01 (CE, CSN)
RF24 radio(7, 8);

// The address needs to match the transmitter address
const byte address[6] = "00001";

// These variables are used to calculate how long it's been since we last received data
unsigned long lastReceiveTime = 0;
unsigned long currentTime = 0;

// Create a struct which will store the received values
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
  radio.openReadingPipe(0, address);
  radio.setAutoAck(false);
  // Lower data rate gives more range
  // RF24_250KBPS for 250kbs, RF24_1MBPS for 1Mbps, or RF24_2MBPS for 2Mbps
  radio.setDataRate(RF24_250KBPS);
  // Set Power Amplifier (PA) level to one of four levels: RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH and RF24_PA_MAX
  // Use RF24_PA_MAX for the max range
  radio.setPALevel(RF24_PA_MAX);
  //  Set the module as receiver
  radio.startListening();
  resetData();


  // Attach the esc and servos to digital pins
  esc.attach(6, 1000, 2000);
  
  servo1.attach(5);
  servo2.attach(4);
  servo3.attach(3);
  servo4.attach(2);
}

// This functions is used to reset the arduino if the connection is lost,
// you might not need it if you have a good connection
void(* resetFunc) (void) = 0;

void loop() {
  // Check whether there is data to be received
  if (radio.available()) {
    // Read the whole data and store it into the 'data' structure
    radio.read(&data, sizeof(Data_Package));
    // At this moment we have received the data
    lastReceiveTime = millis(); 
  }

  /* OPTIONAL, YOU CAN DELETE THIS IF YOU WANT */
  /* You can also changes the values (1000 and 1500) which are in ms and see what works better for you. */
  // Check whether we keep receving data, or we have a connection between the two modules
  currentTime = millis();
  // First if we last received some data 1 second ago we'll try to reset the arduino in order to establish a new connection
  if (currentTime - lastReceiveTime > 1000) {
    resetFunc();
  }

  // 500 ms later if a new connection wasn't established we rest the data
  if (currentTime - lastReceiveTime > 1500) {
    resetData(); // If connection is lost, reset the data.
  }
  /* END OPTIONAL */

  // Map the data in order to control the esc and servos
  esc.write(map(data.pot1, 0, 255, 0, 180));

  servo1.write(map(data.j1PotX, 0, 255, 0, 180));
  servo2.write(map(data.j1PotY, 0, 255, 0, 180));
  servo3.write(map(data.j2PotX, 0, 255, 0, 180));
  servo4.write(map(data.j2PotY, 0, 255, 0, 180));
}

void resetData() {
  // Reset the values when there is no radio connection
  data.j1PotX = 127;
  data.j1PotY = 127;
  data.j2PotX = 127;
  data.j2PotY = 127;
  data.pot1 = 1;
}
