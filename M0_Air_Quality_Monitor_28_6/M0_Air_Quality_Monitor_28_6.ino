// Adafruit M0 with PMS7003, GPS, TFT and SD
// ACBatchelor 05 to 06/2019
//ongoing updates to 20/6/19
//SD card safe 28/6/2019

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>
#include <SD.h>             //for SD card
#include <Adafruit_GPS.h>
#include <Arduino.h>        // required before wiring_private.h
#include <wiring_private.h> // pinPeripheral() function for sercom
#include <PMS.h>           //Partical Sensor PMS7003


// Name of the hardware serial port for GPS
#define GPSSerial Serial1

// Using sercom to set up another serial2 on the M0 for PMS sensor
Uart Serial2 (&sercom1, 11, 10, SERCOM_RX_PAD_0, UART_TX_PAD_2);
void SERCOM1_Handler() {
  Serial2.IrqHandler();
}
PMS pms(Serial2);
PMS::DATA data;

// For the TFT breakout board, you can use any 2 or 3 pins.
// These pins will also work for the 1.8" TFT shield.

#define TFT_DC        A1
#define TFT_RST       A2 // Or set to -1 and connect to Arduino RESET pin
#define TFT_CS        A3
#define chipSelect    A4

#define BACKCOLOR 0x0000
#define SCALECOLOR 0xFFFF
#define BLACK           0x0000
#define BLUE            0x001F
#define RED             0xF800
#define GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0
#define WHITE           0xFFFF
#define LIGHT_GREY      0xBDF7
#define DARK_GREY       0x7BEF
#define ORANGE          0xFBE0
#define BROWN           0x79E0
#define PINK            0xF81F

uint32_t pmsInterval = 10000; //How often to take a reading.
uint32_t timer = millis();

int count = 0;
int PM1;
int PM2_5;
int PM10;
String dataString;
String header = "PM 1.0 ;PM 2.5 ;PM 10.0 ;Time ;Date ;Loc N ;Loc W ;";

int health2_5 = 10; //Annual Average Limit above which has an effect on health (WHO guidelines)
int health10 = 20;  //Annual Average Limit above which has an effect on health (WHO guidelines)
int mort2_5 = 35;  //Annual Average Limit above which has 15% increase in mortality (WHO guidelines)
int mort10 = 70;  //Annual Average Limit above which has 15% increase in mortality  (WHO guidelines)

//TFT setup
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// Connect to the GPS on the hardware port
Adafruit_GPS GPS(&GPSSerial);

// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences
#define GPSECHO false

//Bars setup
int TopC = 50;
int BarW = 6;
int gap = 8;
const int Bars = 8;
int barIndex = 0;
int RawValue = 0;
int histPM10[Bars];
int histPM2_5[Bars];
int BARCOLOR = CYAN;

int Gfix = 0;
int Gqual = 0;
int gsat = 0;
bool SDcard = true;

void setup()
{
  //while (!Serial);  // uncomment to have the sketch wait until Serial is ready

  // connect at 115200 so we can read the GPS fast enough and echo without dropping chars also spit it out
  Serial.begin(9600);  // To computer for the USB Debugging
  Serial2.begin(9600);  // Sercom for the PMS on 10 and 11
  // Assign pins 10 & 11 SERCOM functionality
  pinPeripheral(10, PIO_SERCOM);
  pinPeripheral(11, PIO_SERCOM);

  tft.initR(INITR_BLACKTAB);      // Init ST7735S chip, black tab
  tft.setRotation(2);
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextWrap(true);
  tft.setTextSize(1);
  tft.setCursor(0, 0);
  tft.println("Screen is working.");


  // Serial.print("Initializing SD card...");
  tft.println("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    tft.println("Card failed, no data logging.");
    SDcard = false;
    delay(3000);
  }
if (SDcard) {
  //Serial.println("card initialized.");
  tft.println("card initialized.");
  writeToCard(header);
}

  // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
  GPS.begin(9600);
  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // uncomment this line to turn on only the "minimum recommended" data
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
  // the parser doesn't care about other sentences at this time
  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // 1 Hz update rate
  //GPS.sendCommand(PMTK_SET_NMEA_UPDATE_5HZ); // 1 Hz update rate

  // For the parsing code to work nicely and have time to sort thru the data, and
  // print it out we don't suggest using anything higher than 1 Hz

  // Request updates on antenna status, comment out to keep quiet
  GPS.sendCommand(PGCMD_ANTENNA);

  delay(1000); //can I make this shorter?

  // Ask for firmware version
  GPSSerial.println(PMTK_Q_RELEASE);

  tft.fillScreen(ST77XX_BLACK);
  //For the bargraph
  drawScale();
  for (int i = 0; i < Bars; i++) {
    histPM2_5[i] = 1;
    histPM10[i] = 1;

  }
}

void loop()
{
  // read data from the GPS in the 'main loop'
  char c = GPS.read();
  // if you want to debug, this is a good time to do it!
  if (GPSECHO)
    // if (c) tft.print(c);
    if (c) Serial.print(c);
  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences!
    // so be very wary if using OUTPUT_ALLDATA and trying to print out data
    //  tft.println(GPS.lastNMEA()); // this also sets the newNMEAreceived() flag to false
    //  Serial.println(GPS.lastNMEA()); // this also sets the newNMEAreceived() flag to false
    if (!GPS.parse(GPS.lastNMEA())) // this also sets the newNMEAreceived() flag to false
      return; // we can fail to parse a sentence in which case we should just wait for another
  }

  // if millis() or timer wraps around, we'll just reset it
  if (timer > millis()) timer = millis();

  // approximately every 2 seconds or so, print out the current stats
  if (millis() - timer > pmsInterval) {
    timer = millis(); // reset the timer

    //  tft.fillScreen(ST77XX_BLACK);

    if (pms.readUntil(data, 1000))
    {

      PM1 = data.PM_AE_UG_1_0;
      PM2_5 = data.PM_AE_UG_2_5;
      PM10 = data.PM_AE_UG_10_0;
      Serial.println("Got Data:)");

        if (SDcard) {
 //   tftTextDisplay();
      BuildString();
      writeToCard(dataString);
        }
      buildHist();
      PMSMiniTextDisplay();
      GPSMiniTextDisplay();
      drawBar();
    }
 //   GPSTextDisplay();
  }

}
