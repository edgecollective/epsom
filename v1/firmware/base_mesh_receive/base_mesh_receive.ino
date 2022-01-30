
#include <RHSoftwareSPI.h>  // http://www.airspayce.com/mikem/arduino/RadioHead/RadioHead-1.113.zip
#include <RHRouter.h>
#include <RHMesh.h>
#include <RH_RF95.h>
#include <SPI.h>

#define RF95_FREQ 915.0
#define gatewayNode 1

RHMesh *manager;

static const uint32_t GPSBaud = 9600;
typedef struct {
  float tempC;
  float battV;
  int ID;
} Payload;

Payload theData;

// Radio pins for feather M0
#define RFM95_CS 11
#define RFM95_RST 6
#define RFM95_INT 12

// Radio pins for feather M0
//#define RFM95_CS 8
//#define RFM95_RST 4
//#define RFM95_INT 3

#define LED 13

RH_RF95 rf95(RFM95_CS, RFM95_INT);

#define waitTime 1000

void setup()
{
  
while(!Serial) {
  ;
}
 manager = new RHMesh(rf95, gatewayNode);

   if (!manager->init()) {
    Serial.println(F("mesh init failed"));
    
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

  //Serial.println("RF95 ready");


  delay(1000);
  
}

void loop()
{

  uint8_t buf[sizeof(Payload)];
  uint8_t len = sizeof(buf);
  uint8_t from;
  
   if (manager->recvfromAckTimeout((uint8_t *)buf, &len, waitTime, &from)) {  // this runs until we receive some message
      // entering this block means the message is for us

    digitalWrite(LED, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(10);                       // wait for a second
    digitalWrite(LED, LOW);    // turn the LED off by making the voltage LOW
    delay(10);                       // wait for a second
    
     // the rest of this code only runs if we were the intended recipient; which means we're the gateway
      theData = *(Payload*)buf;

      float tempC = theData.tempC;
      float battV = theData.battV;
      int ID = theData.ID;
      Serial.print(millis());
      Serial.print(",");
      Serial.print(ID);
      Serial.print(",");
      Serial.print(tempC);
      Serial.print(",");
      Serial.println(battV);
      
   }
}
