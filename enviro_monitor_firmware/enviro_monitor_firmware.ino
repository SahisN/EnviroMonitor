#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DHT.h"

// oled screen screen dimensions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

// dht sensor pin, type and object
#define DHTPIN 2
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// light dependent resistor pin
const int LDR_pin = A0;

// Air quality sensor pin
const int mq_125_pin = A2;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


void splash_screen() {
  display.clearDisplay();
  delay(500);
  display.setCursor(0, 25);
  display.setTextSize(2);
  display.println("Booting...");
  display.display();
  delay(3000);
  display.clearDisplay();
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0X3C)) {
    Serial.println("SSD1306 allocation failed");
    for(;;);
  }

  display.clearDisplay();
  display.setTextColor(WHITE);
  splash_screen();
  display.setTextSize(1);

  dht.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(2000);

  const float temp_in_celsius = dht.readTemperature();
  const float temp_in_fahrenheit = dht.readTemperature(true);
  const float humidity = dht.readHumidity();
  const int light_level = analogRead(LDR_pin);
  const int air_quality_level = analogRead(mq_125_pin);

  // categorizing light level based on resistance value from LDR for a more intutive light level reading
  char light_level_message[7];

  if(light_level >= 800) {
    strcpy(light_level_message, "dark");
  }

  else if(light_level >= 400) {
    strcpy(light_level_message, "dim");
  }

  else {
    strcpy(light_level_message, "bright");
  }

  // Categorizing air quality levels based on the sensor’s resistance (RS) measured by the MQ-125, in order to provide a more intuitive representation of air quality.
  String air_quality_message;

  if(air_quality_level <= 300) {
    air_quality_message = "Good";
  }

  else if(air_quality_level <= 500) {
    air_quality_message = "Moderate";
  }

  else if(air_quality_level <= 700) {
    air_quality_message = "Poor";
  }

  else if(air_quality_level <= 900) {
    air_quality_message = "Very poor";
  }

  else {
    air_quality_message = "Hazardous";
  }

  display.clearDisplay();

  delay(200);

  // displays temperature data in celsius
  display.setCursor(0, 0);
  display.print("Temp:");
  display.print(temp_in_celsius);
  display.print("C | ");
  display.print(temp_in_fahrenheit);
  display.println("F");
  display.display();

  // displays humidity data
  display.setCursor(0, 15);
  display.print("Humidity: ");
  display.print(humidity);
  display.println("%");

  // display light level data
  display.setCursor(0, 30);
  display.print("Light level: ");
  display.println(light_level_message);

  // display air quality level data
  display.setCursor(0, 45);
  display.print("Air Quality:");
  display.println(air_quality_message);

  display.display();
  
}
