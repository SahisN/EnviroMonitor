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

// digital pin for button
const int buttonPin = 3;
int buttonState = 0;

// sensor update frequency
unsigned long last_sensor_update = 0;
const unsigned long sensor_interval = 3000; // 2 seconds in millisecond

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

  dht.begin();
  pinMode(buttonPin, INPUT_PULLUP);

  display.clearDisplay();
  display.setTextColor(WHITE);
  splash_screen();
  display.setTextSize(1);
}

void loop() {
  buttonState = digitalRead(buttonPin);

  // button interrupt
  if(buttonState == HIGH) {
    update_sensor_data();
    last_sensor_update = 0;
    return;
  }

  if(millis() - last_sensor_update >= sensor_interval) {
    last_sensor_update = millis();
    Serial.begin("display");
    update_sensor_data();
  }
  
}

/**
  Only displays temperature and humidity data for a focused view
*/
void display_temp_humidity(float temperature) {
  display.clearDisplay();

  // Draw icon (thermometer) - simple rectangle + circle as placeholder
  display.drawRect(10, 10, 16, 40, SSD1306_WHITE); // thermometer body
  display.fillCircle(18, 55, 5, SSD1306_WHITE);   // thermometer bulb

  // Big Temperature Value
  display.setTextSize(3);        // Large font
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(40, 15);
  display.print(temperature, 1); // 1 decimal
  display.print("C");

  // Mini bar visualization (temperature progress)
  int barWidth = map(temperature, 0, 50, 0, 80); // map temp to bar width
  display.fillRect(40, 55, barWidth, 6, SSD1306_WHITE);
  display.drawRect(40, 55, 80, 6, SSD1306_WHITE); // bar outline

  // Optional: status label
  display.setTextSize(1);
  display.setCursor(40, 50);
  if (temperature < 20) display.print("Cold");
  else if (temperature < 30) display.print("Comfortable");
  else display.print("Hot");

  display.display();
}

/**
  Only displays light level data for a focused view
*/

void display_light_level() {

}

/**
  Only displays air quality data for a focused view
*/
void display_air_quality() {

}

/**
  Reads sensor data & updates the screen with new data
  Using dht22, LDR and MQ-126 sensor for enviroment read
  Displays to oled display for easy visualization
*/

static const unsigned char PROGMEM image_download_1_bits[] = {0x04,0x00,0x04,0x00,0x0c,0x00,0x0e,0x00,0x1e,0x00,0x1f,0x00,0x3f,0x80,0x3f,0x80,0x7e,0xc0,0x7f,0x40,0xff,0x60,0xff,0xe0,0x7f,0xc0,0x7f,0xc0,0x3f,0x80,0x0f,0x00};

static const unsigned char PROGMEM image_download_2_bits[] = {0x02,0x00,0x07,0x00,0x05,0x00,0x08,0x80,0x08,0x80,0x08,0x80,0x30,0x60,0x40,0x10,0x80,0x08,0x7f,0xf0,0x08,0x80,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

static const unsigned char PROGMEM image_download_3_bits[] = {0x00,0x00,0x00,0x00,0x00,0x30,0x03,0x88,0x04,0x44,0x04,0x44,0x00,0x44,0x00,0x88,0xff,0x32,0x00,0x00,0xad,0x82,0x00,0x60,0x00,0x10,0x00,0x10,0x01,0x20,0x00,0xc0};

static const unsigned char PROGMEM image_download_bits[] = {0x1c,0x00,0x22,0x02,0x2b,0x05,0x2a,0x02,0x2b,0x38,0x2a,0x60,0x2b,0x40,0x2a,0x40,0x2a,0x60,0x49,0x38,0x9c,0x80,0xae,0x80,0xbe,0x80,0x9c,0x80,0x41,0x00,0x3e,0x00};


void update_sensor_data() {
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
    air_quality_message = "normal";
  }

  else if(air_quality_level <= 700) {
    air_quality_message = "Poor";
  }

  else if(air_quality_level <= 900) {
    air_quality_message = "Bad";
  }

  else {
    air_quality_message = "toxic";
  }

  display.clearDisplay();

    // Layer 2
    display.drawRect(0, 1, 125, 63, 1);

    // Layer 3
    display.drawLine(63, 1, 63, 64, 1);

    // Layer 3
    display.drawLine(1, 32, 124, 32, 1);

    // download
    display.drawBitmap(5, 9, image_download_bits, 16, 16, 1);

    // Layer 5
    display.setTextColor(1);
    display.setTextWrap(false);
    display.setCursor(23, 8);
    display.print(temp_in_celsius);
    display.println(F("C"));

    // Layer 6
    display.setCursor(24, 19);
    display.print(temp_in_fahrenheit);
    display.println(F("F"));

    // download
    display.drawBitmap(70, 9, image_download_1_bits, 11, 16, 1);

    // Layer 8
    display.setCursor(84, 14);
    display.print(humidity);
    display.println("%");

    // download
    display.drawBitmap(5, 41, image_download_2_bits, 13, 16, 1);

    // Layer 10
    display.setCursor(21, 44);
    display.print(light_level_message);

    // download
    display.drawBitmap(68, 40, image_download_3_bits, 15, 16, 1);

    // Layer 12
    display.setCursor(86, 44);
    display.print(air_quality_message);

    display.display();
}





