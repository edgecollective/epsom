// Include the libraries we need
#include <OneWire.h>
#include <DallasTemperature.h>

#include <RHSoftwareSPI.h> // http://www.airspayce.com/mikem/arduino/RadioHead/RadioHead-1.113.zip
#include <RHRouter.h>
#include <RHMesh.h>
#include <RH_RF95.h>
#define RH_HAVE_SERIAL
#include <SPI.h>


// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 915.0
#define gatewayNode 1
#define this_node_id 2

#define LED 13


// Class to manage message delivery and receipt, using the driver declared above
RHMesh *manager;

typedef struct {
  float tempC;
  float battV;
} Payload;

Payload theData;

// Radio pins for feather M0
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3

#define LED 13

RH_RF95 rf95(RFM95_CS, RFM95_INT);


// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 12

#define VBATPIN A7


// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

double round2(double value) {
   return (int)(value * 100 + 0.5) / 100.0;
}

/*
 * The setup function. We only start the sensors here
 */
void setup(void)
{
  
  // start serial port
  Serial.begin(9600);
  Serial.println("Dallas Temperature IC Control Library Demo");
  
  // Start up the onewire library
  sensors.begin();

  //start the radio
  manager = new RHMesh(rf95, this_node_id);

  if (!manager->init()) {
    Serial.println(F("mesh init failed"));
    
  } else {
    delay(1000);
  }
  rf95.setTxPower(23, false);
  rf95.setFrequency(915.0);
  rf95.setCADTimeout(500);

  // long range configuration requires for on-air time
  boolean longRange = false;
  if (longRange) {
    RH_RF95::ModemConfig modem_config = {
      0x78, // Reg 0x1D: BW=125kHz, Coding=4/8, Header=explicit
      0xC4, // Reg 0x1E: Spread=4096chips/symbol, CRC=enable
      0x08  // Reg 0x26: LowDataRate=On, Agc=Off.  0x0C is LowDataRate=ON, ACG=ON
    };
    rf95.setModemRegisters(&modem_config);
    if (!rf95.setModemConfig(RH_RF95::Bw125Cr48Sf4096)) {
      Serial.println(F("set config failed"));
    }
  }
    Serial.println("RF95 ready");

}

/*
 * Main function, get and show the temperature
 */
void loop(void)
{ 

  // battery measurement
   float measuredvbat = analogRead(VBATPIN);
measuredvbat *= 2;    // we divided by 2, so multiply back
measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
measuredvbat /= 1024; // convert to voltage

  // call sensors.requestTemperatures() to issue a global temperature 
  // request to all devices on the bus
  //Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  //Serial.println("DONE");
  // After we got the temperatures, we can print them here.
  // We use the function ByIndex, and as an example get the temperature from the first sensor only.
  Serial.print("Temperature for the device 1 (index 0) is: ");
  Serial.println(sensors.getTempCByIndex(0));  

  // send over radio
  theData.tempC = round2(sensors.getTempCByIndex(0));
  theData.battV = measuredvbat;
  uint8_t error = manager->sendtoWait((uint8_t *)&theData, sizeof(theData), gatewayNode);
  Serial.print("radio sent");
  Serial.println(error);

digitalWrite(LED, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(100);                       // wait for a second
    digitalWrite(LED, LOW);    // turn the LED off by making the voltage LOW
    delay(100);       
  
}
