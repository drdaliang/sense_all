#include <PID_v1.h>
#include <Time.h>
#include <DHT.h>
#include <DS1307RTC.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include "max6675.h"

//initialize sensors with i2c or spi or wire protocol
#define DHTPIN 4     // DHT 22 PIN
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE);
Adafruit_BMP085 bmp;
int thermoDO = 12;
int thermoCS = 5;
int thermoCLK = 13;
MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);

//initialize IRQ for measuring motor speed
volatile int IRQcount;
int pin_irq0 = 0; //IRQ that matches to pin 2
int pin_irq1 = 1; //IRQ that matches to pin 3

//Define PID 
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor!");
    while (1) {}
  }
  dht.begin();
  attachInterrupt(pin_irq0, IRQcounter, RISING);
  attachInterrupt(pin_irq1, IRQcounter, RISING);
}

void IRQcounter() {
  IRQcount++;
}

void loop() {
  // put your main code here, to run repeatedly:
  tmElements_t tm;

  if (RTC.read(tm)) {
    print2digits(tm.Hour);
    Serial.write(':');
    print2digits(tm.Minute);
    Serial.write(':');
    print2digits(tm.Second);
    Serial.print(" ");
    Serial.print(tm.Day);
    Serial.write('/');
    Serial.print(tm.Month);
    Serial.write('/');
    Serial.print(tmYearToCalendar(tm.Year));
    Serial.print("\t");
  }

  Serial.print(bmp.readTemperature());
  Serial.print("\t");

  Serial.print(bmp.readPressure());
  Serial.print("\t");

  float h = dht.readHumidity();
  // Read temperature as Celsius
  float t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    //    Serial.println("Failed to read from DHT sensor!");
    return;
  }


  Serial.print(h);
  Serial.print("\t");
  Serial.print(t);
  Serial.print("\t");
  Serial.print(thermocouple.readCelsius());
  cli();//disable interrupts
  IRQcount = 0;
  sei();//enable interrupts
  delay(500);
  cli();//disable interrupts
  int result = IRQcount*15;
  sei();//enable interrupts
  Serial.print("\t");
  Serial.println(result);
  delay(300);
}

void print2digits(int number) {
  if (number >= 0 && number < 10) {
    Serial.write('0');
  }
  Serial.print(number);
}
