  /*****************
  Tally light ESP32 for Blackmagic ATEM switcher

  Version 2

  A wireless (WiFi) tally light for Blackmagic Design
  ATEM video switchers, based on the M5StickC ESP32 development
  board and the Arduino IDE.


  Based on the work of Kasper Skårhøj:
  https://github.com/kasperskaarhoj/SKAARHOJ-Open-Engineering


  sysLINK88
******************/

#include <M5StickC.h>
#include <Adafruit_NeoPixel.h>
#include <WiFi.h>
#include <SkaarhojPgmspace.h>
#include <ATEMbase.h>
#include <ATEMstd.h>
#include <EEPROM.h>
/*****************
Nur in diesem Bereich Variabel anpassen.
******************/
IPAddress clientIp(192, 168, 100, 204);          // IP address of the ESP32
IPAddress switcherIp(192, 168, 100, 205);       // IP address of the ATEM switcher
const char* ssid = "WIFI-NAME"; // WIFI Name
const char* password =  "12345678901234567890"; // WIFI Password
/*****************
Nur bis diesem Bereich Variabel anpassen.
******************/


// http://www.barth-dev.de/online/rgb565-color-picker/
#define GRAY  0x0020 //   8  8  8
#define GREEN 0x0200 //   0 64  0
#define RED   0xF800 // 255  0  0
#define YELLOW   0xFFE0


#define PIN 32

#define NUMPIXELS 52
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

ATEMstd AtemSwitcher;
int cameraNumber;
int ledbrightness;
int ledpixel;
int batt;
int ledPin = 10;
int EinstellungenStatus = 0;
int EinstellungenMenue = 0;
int EinstellungenMenuePunkt = 1;
int PreviewTallyPrevious = 1;
int ProgramTallyPrevious = 1;
int prevcolor;
int tallycolor;
bool Einstellungen = false;
bool Batterie_display = true;


double vbat = 0.0;



void setup() {
 

  Serial.begin(9600);


  
 // initialize the M5StickC object
  M5.begin();
    M5.update();

  
    M5.Lcd.setRotation(3);

  if (!EEPROM.begin(2048)) // size in Byte
  {
    M5.Lcd.println("failed to initialise EEPROM"); delay(1000000);
  }
EEPROM.get(0, cameraNumber);
if(cameraNumber == -1)
{
    cameraNumber = 1;  
    EEPROM.put(0,cameraNumber);
    EEPROM.commit();
}
EEPROM.get(5, ledbrightness);
if(ledbrightness == -1)
{
    ledbrightness = 1;  
    EEPROM.put(5,ledbrightness);
    EEPROM.commit();
}
EEPROM.get(10, ledpixel);
if(ledpixel == -1)
{
    ledpixel = 1;  
    EEPROM.put(10,ledpixel);
    EEPROM.commit();
}
EEPROM.get(15, batt);
if(batt == -1)
{
    batt = 0;  
    EEPROM.put(15,batt);
    EEPROM.commit();
}
else if(batt == 0)
{
  Batterie_display = false;
}
else
{
  Batterie_display = true;
}
EEPROM.get(20, prevcolor);
if(prevcolor == -1)
{
    prevcolor = 1;  
    EEPROM.put(10,prevcolor);
    EEPROM.commit();
}
EEPROM.get(25, tallycolor);
if(tallycolor == -1)
{
    tallycolor = 1;  
    EEPROM.put(10,tallycolor);
    EEPROM.commit();
}  

    
    
    
    if (M5.BtnB.isPressed())
    {
        Einstellungen = true;
        EinstellungenStatus = 1;
        M5.Lcd.println("Einstellungen");
        delay(500);
        goto End;
    }
  
  
  // Start the Ethernet, Serial (debugging) and UDP:
  M5.Lcd.println("Connecting to WiFi..");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
M5.Lcd.println("Connected to the WiFi network");
 delay(500);

  pinMode(ledPin, OUTPUT);  // LED: 1 is on Program (Tally)
  digitalWrite(ledPin, HIGH); // off

  // Initialize a connection to the switcher:
  M5.Lcd.println("Connecting to ATEM Mischer...");
  delay(500);
  AtemSwitcher.begin(switcherIp);
  AtemSwitcher.serialOutput(0x80);
  AtemSwitcher.connect();
  M5.Lcd.println("Connected to the ATEM Mischer");
  delay(500);
End:
  M5.Lcd.setRotation(0);
  pixels.begin(); // This initializes the NeoPixel library.
  pixels.show(); // All pixels off
  pixels.setBrightness(ledbrightness);
}

void loop() {
M5.update();
if (Einstellungen)
{

if (EinstellungenMenue == 0)
{
if (EinstellungenStatus == 1)
{
M5.Lcd.setCursor(1,1);
  M5.Lcd.fillScreen(0x0000);


  M5.Lcd.setTextColor(WHITE, 0x0000);
  M5.Lcd.println("Einstellungen");  
  M5.Lcd.setCursor(3,20);
    if (EinstellungenMenuePunkt == 1)
  {
    M5.Lcd.setTextColor(0xF800, 0x0200);
  }
  else
  {
    M5.Lcd.setTextColor(WHITE, 0x0000);
  }
  M5.Lcd.println("1 Camera    ");
  M5.Lcd.setCursor(3,30);
    if (EinstellungenMenuePunkt == 2)
  {
    M5.Lcd.setTextColor(0xF800, 0x0200);
  }
  else
  {
    M5.Lcd.setTextColor(WHITE, 0x0000);
  }
  M5.Lcd.println("2 LED Hellig");
  M5.Lcd.setCursor(3,40);
    if (EinstellungenMenuePunkt == 3)
  {
    M5.Lcd.setTextColor(0xF800, 0x0200);
  }
  else
  {
    M5.Lcd.setTextColor(WHITE, 0x0000);
  }
  M5.Lcd.println("3 LED       ");
M5.Lcd.setCursor(3,50);
    if (EinstellungenMenuePunkt == 4)
  {
    M5.Lcd.setTextColor(0xF800, 0x0200);
  }
  else
  {
    M5.Lcd.setTextColor(WHITE, 0x0000);
  }
M5.Lcd.println("4 Batterie");
M5.Lcd.setCursor(3,60);
    if (EinstellungenMenuePunkt == 5)
  {
    M5.Lcd.setTextColor(0xF800, 0x0200);
  }
  else
  {
    M5.Lcd.setTextColor(WHITE, 0x0000);
  }
M5.Lcd.println("5 Prev Col");
M5.Lcd.setCursor(3,70);
    if (EinstellungenMenuePunkt == 6)
  {
    M5.Lcd.setTextColor(0xF800, 0x0200);
  }
  else
  {
    M5.Lcd.setTextColor(WHITE, 0x0000);
  }
M5.Lcd.println("6 Tally Col");



  
  M5.Lcd.setCursor(3,80);
    if (EinstellungenMenuePunkt == 7)
  {
    M5.Lcd.setTextColor(0xF800, 0x0200);
  }
  else
  {
    M5.Lcd.setTextColor(WHITE, 0x0000);
  }
  M5.Lcd.println("7 Neustarten");

  
    M5.Lcd.drawLine(3,135,77,135,WHITE);
  M5.Lcd.drawLine(3,135,3,160,WHITE);
  M5.Lcd.drawLine(77,135,77,160,WHITE);
  M5.Lcd.setCursor(20,145);
  M5.Lcd.setTextColor(WHITE, 0x0000);
  M5.Lcd.println("Auswahl");

    EinstellungenStatus = 0;  
}
else
{
  if (M5.BtnB.isPressed())
  {
    

    EinstellungenMenuePunkt++;
    if (EinstellungenMenuePunkt >=8)
    {
      EinstellungenMenuePunkt =1;
    }

    EinstellungenStatus = 1;
    delay(500);
    
  }
  if (M5.BtnA.isPressed())
  {
    EinstellungenMenue = EinstellungenMenuePunkt;

    EinstellungenStatus = 1;
    delay(500);
  }

  
}

  
}//Ende Menue 0
if (EinstellungenMenue == 1)
{
  if (EinstellungenStatus == 1)
  {
      M5.Lcd.setCursor(1,1);
  //digitalWrite(ledPin, ledValue);
  M5.Lcd.fillScreen(0x0000);
  M5.Lcd.println("Einstellungen");
  M5.Lcd.drawLine(0,10,80,10,WHITE);
  M5.Lcd.setCursor(10,15);
  M5.Lcd.println("Kamera");
  M5.Lcd.setCursor(10,25);
  M5.Lcd.println("auswahl:");
  M5.Lcd.drawString(String(cameraNumber), 3, 40, 8);
  M5.Lcd.drawLine(60,67,80,67,WHITE);
  M5.Lcd.drawLine(60,97,80,97,WHITE);
  M5.Lcd.drawLine(60,67,60,97,WHITE);  
  M5.Lcd.setCursor(70,79);
  M5.Lcd.println("+");
  M5.Lcd.drawLine(3,135,77,135,WHITE);
  M5.Lcd.drawLine(3,135,3,160,WHITE);
  M5.Lcd.drawLine(77,135,77,160,WHITE);
  M5.Lcd.setCursor(10,140);
  M5.Lcd.println("Speichern &");
  M5.Lcd.setCursor(10,150);
  M5.Lcd.println("Menue");
  
  EinstellungenStatus = 0;
  }
  else
  {
      if (M5.BtnB.isPressed())
  {
    

    cameraNumber++;
    if (cameraNumber >=10)
    {
      cameraNumber =1;
    }

    EinstellungenStatus = 1;
    delay(500);
    
  }

if (M5.BtnA.isPressed())
{
    EEPROM.put(0,cameraNumber);
    EEPROM.commit();
    EinstellungenMenue = 0;
    EinstellungenStatus = 1;
    delay(500);
}
  }
}//Ende Menue 1
if (EinstellungenMenue == 2) // LED Hellig
{
  if (EinstellungenStatus == 1)
  {

  ledPower(128,0,0);

    
  M5.Lcd.setCursor(1,1);
  M5.Lcd.fillScreen(0x0000);
  M5.Lcd.println("Einstellungen");
  
  M5.Lcd.drawLine(0,10,80,10,WHITE);
  M5.Lcd.setCursor(10,15);
  M5.Lcd.println("LED");
  M5.Lcd.setCursor(10,25);
  M5.Lcd.println("Helligkeit:");
  M5.Lcd.drawString(String(ledbrightness), 3, 40, 6);
  M5.Lcd.drawLine(60,67,80,67,WHITE);
  M5.Lcd.drawLine(60,97,80,97,WHITE);
  M5.Lcd.drawLine(60,67,60,97,WHITE);  
  M5.Lcd.setCursor(70,79);
  M5.Lcd.println("+");
  M5.Lcd.drawLine(3,135,77,135,WHITE);
  M5.Lcd.drawLine(3,135,3,160,WHITE);
  M5.Lcd.drawLine(77,135,77,160,WHITE);
  M5.Lcd.setCursor(10,140);
  M5.Lcd.println("Speichern &");
  M5.Lcd.setCursor(10,150);
  M5.Lcd.println("Menue");




  
  EinstellungenStatus = 0;
  }
  else
  {
      if (M5.BtnB.isPressed())
  {
    

    ledbrightness = ledbrightness+10;
    if (ledbrightness >=250)
    {
      ledbrightness =1;
    }
    EinstellungenStatus = 1;
delay(500);
  
ledPower(128,0,0);

    

    
  }
    
if (M5.BtnA.isPressed())
{
  
ledPower(0,0,0);

    EEPROM.put(5,ledbrightness);
    EEPROM.commit();
    EinstellungenMenue = 0;
    EinstellungenStatus = 1;
    delay(500);
}    
  }
}//Ende Menue 2
if (EinstellungenMenue == 3)
{
  if (EinstellungenStatus == 1)
  {
  pixels.show(); // All pixels off
  ledPower(128,0,0);
    
  M5.Lcd.setCursor(1,1);
  M5.Lcd.fillScreen(0x0000);
  M5.Lcd.println("Einstellungen");
  
  M5.Lcd.drawLine(0,10,80,10,WHITE);
  M5.Lcd.setCursor(10,15);
  M5.Lcd.println("LED");
  M5.Lcd.setCursor(10,25);
  M5.Lcd.println("Status:");
  M5.Lcd.drawString(String(ledpixel), 3, 40, 6);
  M5.Lcd.drawLine(60,67,80,67,WHITE);
  M5.Lcd.drawLine(60,97,80,97,WHITE);
  M5.Lcd.drawLine(60,67,60,97,WHITE);  
  M5.Lcd.setCursor(70,79);
  M5.Lcd.println("+");
  M5.Lcd.drawLine(3,135,77,135,WHITE);
  M5.Lcd.drawLine(3,135,3,160,WHITE);
  M5.Lcd.drawLine(77,135,77,160,WHITE);
  M5.Lcd.setCursor(10,140);
  M5.Lcd.println("Speichern &");
  M5.Lcd.setCursor(10,150);
  M5.Lcd.println("Menue");




  
  EinstellungenStatus = 0;
  }
  else
  {

      if (M5.BtnB.isPressed())
  {
    

    ledpixel++;
    if (ledpixel >=5)
    {
      ledpixel=1;
    }
    EinstellungenStatus = 1;
    ledPower(0,0,0);
    ledPower(128,0,0);
delay(500);


  
    

    
  }
    
if (M5.BtnA.isPressed())
{
    EEPROM.put(10,ledpixel);
    EEPROM.commit();
    EinstellungenMenue = 0;
    EinstellungenStatus = 1;
    delay(500);
    ledPower(0,0,0);
}    
  }
}//Ende Menue 3


if (EinstellungenMenue == 4)//Batterie Anzeige
{
  if (EinstellungenStatus == 1)
  {
  M5.Lcd.setCursor(1,1);
  M5.Lcd.fillScreen(0x0000);
  M5.Lcd.println("Einstellungen");
  M5.Lcd.drawLine(0,10,80,10,WHITE);
  M5.Lcd.setCursor(10,15);
  M5.Lcd.println("Batterie");
  M5.Lcd.setCursor(10,25);
  M5.Lcd.println("anzeige ?");
  M5.Lcd.setCursor(10,80);
if (Batterie_display)
{
  M5.Lcd.println("JA");
}
else
{
  M5.Lcd.println("NEIN");
}
  
  M5.Lcd.drawLine(60,67,80,67,WHITE);
  M5.Lcd.drawLine(60,97,80,97,WHITE);
  M5.Lcd.drawLine(60,67,60,97,WHITE);  
  M5.Lcd.setCursor(70,79);
  M5.Lcd.println("+");
  M5.Lcd.drawLine(3,135,77,135,WHITE);
  M5.Lcd.drawLine(3,135,3,160,WHITE);
  M5.Lcd.drawLine(77,135,77,160,WHITE);
  M5.Lcd.setCursor(10,140);
  M5.Lcd.println("Speichern &");
  M5.Lcd.setCursor(10,150);
  M5.Lcd.println("Menue");


  
  EinstellungenStatus = 0;
  }
  else
  {
if (M5.BtnB.isPressed())
  {
    if (Batterie_display)
    {
      Batterie_display = false;
    }
else
{
  Batterie_display = true;
}
   


    EinstellungenStatus = 1;
    delay(500);
    
  }

if (M5.BtnA.isPressed())
{
if (Batterie_display)
{
  batt = 1;
}
  else
  {
    batt = 0;
  }
    EEPROM.put(15,batt);
    EEPROM.commit();
    EinstellungenMenue = 0;
    EinstellungenStatus = 1;
    delay(500);
}
  }
}//Ende Menue 4

if (EinstellungenMenue == 5)//Prev Anzeige
{
  if (EinstellungenStatus == 1)
  {
  M5.Lcd.setCursor(1,1);
  M5.Lcd.fillScreen(0x0000);
  M5.Lcd.println("Einstellungen");
  M5.Lcd.drawLine(0,10,80,10,WHITE);
  M5.Lcd.setCursor(10,15);
  M5.Lcd.println("Prev Color");
  
if(prevcolor == 1)
{
  M5.Lcd.setTextColor(WHITE, 0x0000);
  M5.Lcd.setCursor(13,40);
  M5.Lcd.println("AUS");
  M5.Lcd.setTextColor(WHITE, 0x0000);
}
M5.Lcd.fillRect(10, 55, 40, 20, YELLOW);  
if(prevcolor == 2)
{
  M5.Lcd.setCursor(13,60);
  M5.Lcd.setTextColor(BLACK, YELLOW);
  M5.Lcd.println("GELB");
  M5.Lcd.setTextColor(WHITE, 0x0000);
}  

M5.Lcd.fillRect(10, 75, 40, 20, RED);
if(prevcolor == 3)
{
  M5.Lcd.setCursor(13,80);
  M5.Lcd.setTextColor(BLACK, RED);
  M5.Lcd.println("ROT");
  M5.Lcd.setTextColor(WHITE, 0x0000);
}  

M5.Lcd.fillRect(10, 95, 40, 20, BLUE);
if(prevcolor == 4)
{
  M5.Lcd.setCursor(13,100);
  M5.Lcd.setTextColor(BLACK, BLUE);
  M5.Lcd.println("BLAU");
  M5.Lcd.setTextColor(WHITE, 0x0000);
}  

M5.Lcd.fillRect(10, 115, 40, 20, WHITE);
if(prevcolor == 5)
{
  M5.Lcd.setCursor(13,120);
  M5.Lcd.setTextColor(BLACK, WHITE);
  M5.Lcd.println("WEISS");
  M5.Lcd.setTextColor(WHITE, 0x0000);
}

  
  M5.Lcd.drawLine(60,67,80,67,WHITE);
  M5.Lcd.drawLine(60,97,80,97,WHITE);
  M5.Lcd.drawLine(60,67,60,97,WHITE);  
  M5.Lcd.setCursor(70,79);
  M5.Lcd.println("+");
  M5.Lcd.drawLine(3,135,77,135,WHITE);
  M5.Lcd.drawLine(3,135,3,160,WHITE);
  M5.Lcd.drawLine(77,135,77,160,WHITE);
  M5.Lcd.setCursor(10,140);
  M5.Lcd.println("Speichern &");
  M5.Lcd.setCursor(10,150);
  M5.Lcd.println("Menue");


  
  EinstellungenStatus = 0;
  }
  else
  {
    
if (M5.BtnB.isPressed())
  {
prevcolor ++;
if(prevcolor == 6)
{
  prevcolor = 1;
}
  
      EinstellungenStatus = 1;
    delay(500);
  
  }


   


    //EinstellungenStatus = 1;
    delay(500);
    
  

if (M5.BtnA.isPressed())
{

    EEPROM.put(20,prevcolor);
    EEPROM.commit();
    EinstellungenMenue = 0;
    EinstellungenStatus = 1;
    delay(500);
}
  }
}//Ende Menue 6

if (EinstellungenMenue == 6)//Telly Anzeige
{
  if (EinstellungenStatus == 1)
  {
  M5.Lcd.setCursor(1,1);
  M5.Lcd.fillScreen(0x0000);
  M5.Lcd.println("Einstellungen");
  M5.Lcd.drawLine(0,10,80,10,WHITE);
  M5.Lcd.setCursor(10,15);
  M5.Lcd.println("Telly Color");
  
if(tallycolor == 1)
{
  M5.Lcd.setTextColor(WHITE, 0x0000);
  M5.Lcd.setCursor(13,40);
  M5.Lcd.println("AUS");
  M5.Lcd.setTextColor(WHITE, 0x0000);
}
M5.Lcd.fillRect(10, 55, 40, 20, YELLOW);  
if(tallycolor == 2)
{
  M5.Lcd.setCursor(13,60);
  M5.Lcd.setTextColor(BLACK, YELLOW);
  M5.Lcd.println("GELB");
  M5.Lcd.setTextColor(WHITE, 0x0000);
}  

M5.Lcd.fillRect(10, 75, 40, 20, RED);
if(tallycolor == 3)
{
  M5.Lcd.setCursor(13,80);
  M5.Lcd.setTextColor(BLACK, RED);
  M5.Lcd.println("ROT");
  M5.Lcd.setTextColor(WHITE, 0x0000);
}  

M5.Lcd.fillRect(10, 95, 40, 20, BLUE);
if(tallycolor == 4)
{
  M5.Lcd.setCursor(13,100);
  M5.Lcd.setTextColor(BLACK, BLUE);
  M5.Lcd.println("BLAU");
  M5.Lcd.setTextColor(WHITE, 0x0000);
}  

M5.Lcd.fillRect(10, 115, 40, 20, WHITE);
if(tallycolor == 5)
{
  M5.Lcd.setCursor(13,120);
  M5.Lcd.setTextColor(BLACK, WHITE);
  M5.Lcd.println("WEISS");
  M5.Lcd.setTextColor(WHITE, 0x0000);
}

  
  M5.Lcd.drawLine(60,67,80,67,WHITE);
  M5.Lcd.drawLine(60,97,80,97,WHITE);
  M5.Lcd.drawLine(60,67,60,97,WHITE);  
  M5.Lcd.setCursor(70,79);
  M5.Lcd.println("+");
  M5.Lcd.drawLine(3,135,77,135,WHITE);
  M5.Lcd.drawLine(3,135,3,160,WHITE);
  M5.Lcd.drawLine(77,135,77,160,WHITE);
  M5.Lcd.setCursor(10,140);
  M5.Lcd.println("Speichern &");
  M5.Lcd.setCursor(10,150);
  M5.Lcd.println("Menue");


  
  EinstellungenStatus = 0;
  }
  else
  {
if (M5.BtnB.isPressed())
  {
tallycolor ++;
if(tallycolor == 6)
{
  tallycolor = 1;
}
  
      EinstellungenStatus = 1;
    delay(500);
  
  }


   


  //  EinstellungenStatus = 1;
    delay(500);
    
  

if (M5.BtnA.isPressed())
{

    EEPROM.put(25,tallycolor);
    EEPROM.commit();
    EinstellungenMenue = 0;
    EinstellungenStatus = 1;
    delay(500);
}
  }
}//Ende Menue 6

if (EinstellungenMenue == 7) //Neustart
{
  if (EinstellungenStatus == 1)
  {
  M5.Lcd.setCursor(1,1);
  M5.Lcd.fillScreen(0x0000);
  M5.Lcd.println("Neustart");
  EinstellungenStatus = 0;
      delay(500);
    
  esp_restart();
  }
}//Ende Menue 7

}
else
{

  // Check for packets, respond to them etc. Keeping the connection alive!
  AtemSwitcher.runLoop();

  int ProgramTally = AtemSwitcher.getProgramTally(cameraNumber);
  int PreviewTally = AtemSwitcher.getPreviewTally(cameraNumber);

  if ((ProgramTallyPrevious != ProgramTally) || (PreviewTallyPrevious != PreviewTally)) { // changed?

    if ((ProgramTally && !PreviewTally) || (ProgramTally && PreviewTally) ) { // only program, or program AND preview
      drawLabel(RED, GREEN, LOW);

if(tallycolor == 1)
{
  ledPower(0,0,0);
}
 else if(tallycolor == 2)
{
  ledPower(255,255,0);
}
 else if(tallycolor == 3)
{
  ledPower(255,0,0);    
}
 else if(tallycolor == 4)
{
  ledPower(0,0,255);
}
 else if(tallycolor == 5)
{
  ledPower(255,255,255);
}

    } 
    else if (PreviewTally && !ProgramTally) { // only preview
    drawLabel(GREEN, RED, HIGH);
    
if(prevcolor == 1)
{
  ledPower(0,0,0);
}
 else if(prevcolor == 2)
{
  ledPower(255,255,0);
}
 else if(prevcolor == 3)
{
  ledPower(255,0,0);    
}
 else if(prevcolor == 4)
{
  ledPower(0,0,255);
}
 else if(prevcolor == 5)
{
  ledPower(255,255,255);
}
    }
    else if (!PreviewTally || !ProgramTally) { // neither
    drawLabel(BLACK, GREEN, HIGH);
    ledPower(0,0,0);
    }

  }

  ProgramTallyPrevious = ProgramTally;
  PreviewTallyPrevious = PreviewTally;
if (Batterie_display)
{
    M5.Lcd.setCursor(5,150);

vbat      = M5.Axp.GetVbatData() * 1.1 / 1000;
  
M5.Lcd.printf("vbat:%.3fV\r\n",vbat);
}
}
}

void drawLabel(unsigned long int screenColor, unsigned long int labelColor, bool ledValue) {
  digitalWrite(ledPin, ledValue);
  M5.Lcd.fillScreen(screenColor);
  M5.Lcd.setTextColor(labelColor, screenColor);
  M5.Lcd.drawString(String(cameraNumber), 15, 40, 8);

}



void ledPower(int col1, int col2, int col3)
{
pixels.setBrightness(ledbrightness);
if(col1 == 0& col2 == 0 & col3==0)
{
for(int i=0; i <=NUMPIXELS;i++)
{
pixels.setPixelColor(i, pixels.Color(col1, col2, col3));
}  
}
else
{
for(int i=0; i <=NUMPIXELS;i=i+ledpixel)
{
pixels.setPixelColor(i, pixels.Color(col1, col2, col3));
}  
}



pixels.show();
}
