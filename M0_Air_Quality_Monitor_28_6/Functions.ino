void BuildString() {
  dataString = String(PM1) + ";" + String(PM2_5) + ";" + String(PM10) + ";";
  dataString += String(GPS.hour, DEC) + ":" + String(GPS.minute, DEC) + ":" + String(GPS.seconds, DEC) + ";";
  dataString += String(GPS.day, DEC) + "/" + String(GPS.month, DEC) + "/20" + String(GPS.year, DEC) + ";";
 
  if (GPS.fix) {
     dataString += String(GPS.latitude, 4) + String(GPS.lat) + ", " + String(GPS.longitude, 4) + String(GPS.lon) + ";";
  } else {
    dataString += "No GPS;";
  }
}

void writeToCard(String output) {
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("datalog.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(output);
    dataFile.close();
   // Serial.println("Data written to SD card.");
    // print to the serial port too:
      Serial.println(dataString);
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }
}
void PMSMiniTextDisplay() {
    tft.fillRect(0, 0, 90, 30,  BACKCOLOR);
  tft.setTextWrap(false);
  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(0, 0);
  tft.setTextSize(1);
  tft.print("PM 1.0: ");
    tft.setTextColor(ST77XX_GREEN);
  tft.println(PM1);
 // tft.setCursor(0, 10);
   tft.setTextColor(ST77XX_WHITE);
 tft.print("PM 2.5: ");
     tft.setTextColor(ST77XX_GREEN);
  tft.println(PM2_5);
 // tft.setCursor(0, 20);
   tft.setTextColor(ST77XX_WHITE);
  tft.print("PM10.0: ");
      tft.setTextColor(ST77XX_GREEN);
  tft.println(PM10);
}

//Larger text output of PMS data
/*
void tftTextDisplay() {
  tft.setTextWrap(false);
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 10);
  tft.setTextColor(ST77XX_RED);
  tft.setTextSize(1);
  tft.println("PM 1.0 (ug/m3): ");
  tft.setTextSize(2);
  tft.println(PM1);

  tft.setCursor(0, 40);
  tft.setTextColor(ST77XX_BLUE);
  tft.setTextSize(1);
  tft.println("PM 2.5 (ug/m3): ");
  tft.setTextSize(2);
  tft.println(PM2_5);

  tft.setCursor(0, 70);
  tft.setTextColor(ST77XX_GREEN);
  tft.setTextSize(1);
  tft.println("PM 10.0 (ug/m3): ");
  tft.setTextSize(2);
  tft.println(PM10);
  tft.println();
}
*/

void GPSRead() {
  // read data from the GPS in the 'main loop'
  char c = GPS.read();
  // if you want to debug, this is a good time to do it!
  if (GPSECHO)
    //  if (c) tft.print(c);
    if (c) Serial.print(c);
  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences!
    // so be very wary if using OUTPUT_ALLDATA and trying to print out data
    //  tft.println(GPS.lastNMEA()); // this also sets the newNMEAreceived() flag to false
    Serial.println(GPS.lastNMEA()); // this also sets the newNMEAreceived() flag to false
    if (!GPS.parse(GPS.lastNMEA())) // this also sets the newNMEAreceived() flag to false
      return; // we can fail to parse a sentence in which case we should just wait for another
  }
}

/*void GPSRead() {

  // if you want to debug, this is a good time to do it!
  if (GPSECHO)
    //  if (c) tft.print(c);
 //   if (c) Serial.print(c);
  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences!
    // so be very wary if using OUTPUT_ALLDATA and trying to print out data
    //  tft.println(GPS.lastNMEA()); // this also sets the newNMEAreceived() flag to false
    //Serial.println(GPS.lastNMEA()); // this also sets the newNMEAreceived() flag to false
    if (!GPS.parse(GPS.lastNMEA())) // this also sets the newNMEAreceived() flag to false
      return; // we can fail to parse a sentence in which case we should just wait for another
  }
}*/

void GPSMiniTextDisplay() {
  tft.fillRect(90, 0, 50, 30,  BACKCOLOR);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);
  tft.setCursor(90, 0);
  tft.print("GPS ");
    tft.setCursor(90, 10);
  tft.print("Fix: "); tft.println((int)GPS.fix);
    tft.setCursor(90, 20);
    tft.print("Sat: ");
 // if (GPS.fix) {
   tft.println((int)GPS.satellites);
//  }
}

// Full output of GPS text
/*
void GPSTextDisplay() {

  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);
  tft.setCursor(0, 90);
  tft.print("\nTime: ");
  tft.print(GPS.hour, DEC); tft.print(':');
  tft.print(GPS.minute, DEC); tft.print(':');
  tft.print(GPS.seconds, DEC); tft.print('.');
  tft.println(GPS.milliseconds);

  tft.print("Date: ");
  tft.print(GPS.day, DEC); tft.print('/');
  tft.print(GPS.month, DEC); tft.print("/20");
  tft.println(GPS.year, DEC);

  tft.print("Fix: "); tft.print((int)GPS.fix);
  tft.print(" quality: "); tft.println((int)GPS.fixquality);
  if (GPS.fix) {
    tft.print("Location: ");
    tft.print(GPS.latitude, 4); tft.print(GPS.lat);
    tft.print(", ");
    tft.print(GPS.longitude, 4); tft.println(GPS.lon);
    //    tft.print("Speed (knots): "); tft.println(GPS.speed);
    //   tft.print("Angle: "); tft.println(GPS.angle);
    //   tft.print("Altitude: "); tft.println(GPS.altitude);
    tft.print("Satellites: "); tft.println((int)GPS.satellites);
  }
}
*/

void drawScale() {
  tft.drawFastVLine(6, TopC, 100, SCALECOLOR ); // Vertical Scale Line
  tft.drawFastHLine(0, TopC, 6, SCALECOLOR); // Major Division
  tft.drawFastHLine(3, TopC + 24, 3, SCALECOLOR); // Minor Division
  tft.drawFastHLine(0, TopC + 49, 6, SCALECOLOR); // Major Division
  tft.drawFastHLine(3, TopC + 74, 3, SCALECOLOR); // Minor Division
  tft.drawFastHLine(0, TopC + 99, 6, SCALECOLOR); // Major Division
  tft.drawFastHLine(0, TopC + 100 - health10, 128, RED); // PM2_5 limit
  tft.drawFastHLine(0, TopC + 100 - mort10, 128, PINK); // PM2_5 limit
  
}

 void buildHist(){
    for (int i = 0; i < Bars-1; i++)
    { histPM2_5[i] = histPM2_5[i + 1]; 
      histPM10[i] = histPM10[i + 1];      
    } 
   // RawValue = PM10;
    histPM2_5[Bars-1] =  constrain (int(PM2_5), 1, 100); 
    histPM10[Bars-1] =  constrain (int(PM10), 1, 100);

/*debug
          Serial.print("  PM2_5: ");
    for (int i = 0; i < Bars; i++)
    {    Serial.print(histPM2_5[i]); Serial.print(" ");                  
    }  
          Serial.print("  PM10:  ");
    for (int i = 0; i < Bars; i++)
    {
          Serial.print(histPM10[i]); Serial.print(" ");                   
    }   
          Serial.println();*/
    }

void drawBar () {
//Clumsy! for PM2_5's
  for (int i = 0; i < Bars; i++)
  {
    int nPer = histPM2_5[i];
         if (nPer > mort2_5)
            { BARCOLOR = ORANGE; } 
         else if (nPer < health2_5)
            { BARCOLOR = CYAN; }
         else
            { BARCOLOR = YELLOW; }
    int BarTL = i * (BarW + gap) + 12;
    
        tft.fillRect(BarTL, TopC, BarW, 100 - nPer,  BACKCOLOR);
        tft.fillRect(BarTL, TopC + 100 - nPer, BarW, nPer,  BARCOLOR);
    }

//Clumsy! for PM10's
  for (int i = 0; i < Bars; i++)
  {
    int nPer = histPM10[i];
         if (nPer > mort10)
            { BARCOLOR = ORANGE; } 
         else if (nPer < health10)
            { BARCOLOR = CYAN; }
         else
            { BARCOLOR = YELLOW; }
    int BarTL = i * (BarW + gap) + 12 + BarW;
    
        tft.fillRect(BarTL, TopC, BarW, 100 - nPer,  BACKCOLOR);
        tft.fillRect(BarTL, TopC + 100 - nPer, BarW, nPer,  BARCOLOR);
  }
}
