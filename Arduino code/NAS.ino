#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


//BEGIN OLED CONFIG
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
//END OLED CONFIG

//BEGIN TIMER CONFIG
long Day = 0;
int Hour = 0;
int Minute = 0;
int Second = 0;
int HighMillis = 0;
int Rollover = 0;
//END TIMER CONFIG

//BEGIN SENSOR CONFIG
int LED = 5;             //LED output pin
int FAN = 9;             //FAN output pin
int flagFAN = 0;
int flagLED = 0;
double Temp = 0;
int t = 0;
//END SENSOR CONFIG

void setup() {
  // put your setup code here, to run once:
  digitalWrite(FAN, LOW);

  Serial.begin(9600);
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
  // Clear the buffer
  display.clearDisplay();
  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.display();
}

void loop() {

  int y = analogRead(A3);    //read light sensor
  //Serial.println(y);

  if (y > 700 && flagLED == 0)
  {
    analogWrite(LED, 100);
    flagLED = 1;
  }
  if (y < 500 && flagLED == 1)
  {
    flagLED = 0;
    digitalWrite(LED, LOW);
  }
  
  temp();      //read temp and convert to Celsius

  // display temp
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(40  , 3);
  display.print(t);
  display.drawCircle(70, 6, 3, WHITE);
  display.print(" C");

  // start FAN
  if (t >= 35 && flagFAN == 0)     //threshold temperature to start the fans at ~70%
  {
    analogWrite(FAN, 200);
    flagFAN = 1;
  }

  if (t >= 50 && flagFAN == 1)     //threshold temperature to start the fans at max speed
  {
    digitalWrite(FAN, HIGH);
  }

  if (t <= 30 && flagFAN == 1)     //stop the fans when temperature is 5 degrees below threshold
  {
    flagFAN = 0;
    digitalWrite(FAN, LOW);
  }


  // display timer
  uptime(); //Runs the uptime script located below the main loop and reenters the main loop
  print_Uptime();


  // update display with all of the above graphics
  display.display();
  display.clearDisplay();

  delay(100);  //update screen every 100ms
}


void temp()
{
  int Vo = analogRead(A2);
  Temp = log(((10240000 / Vo) - 10000));
  Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp )) * Temp );
  Temp = Temp - 273.15;
  t = Temp;
}

void uptime() {
  //** Making Note of an expected rollover *****//
  if (millis() >= 3000000000) {
    HighMillis = 1;

  }
  //** Making note of actual rollover **//
  if (millis() <= 100000 && HighMillis == 1) {
    Rollover++;
    HighMillis = 0;
  }

  long secsUp = millis() / 1000;

  Second = secsUp % 60;

  Minute = (secsUp / 60) % 60;

  Hour = (secsUp / (60 * 60)) % 24;

  Day = (Rollover * 50) + (secsUp / (60 * 60.0 * 24)); //First portion takes care of a rollover [around 50 days]
}

void print_Uptime()
{
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(20, 23);
  if (Day < 10)
    display.print("0");
  display.print(Day);
  display.print("d ");
  if (Hour < 10)
    display.print("0");
  display.print(Hour);
  display.print("h ");
  if (Minute < 10)
    display.print("0");
  display.print(Minute);
  display.print("m ");
  if (Second < 10)
    display.print("0");
  display.print(Second);
  display.print("s");

}
