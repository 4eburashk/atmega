/*  
    Моя попытка сделать миниатюрную метеостанцию с дисплеем.
    Но в диджиспарк по памяти не влезло. Пришлось на нано сделать...
    А раз на нано, то почему бы и не размахнуться.
    Author: 4tburashk http://csn.net4me.net

    Распиновка:
    Arduino nano(atmega328P) + bme280 + oled 128x32
    На нано 2 пина i2c (A4,A5) к ним подключены датчик bme280 и дисплей.
    Всё запитано от 3.3v наны, земля общая GND наны.
    Усё. В проекте 4 провода только.

    путаюсь в пинах I2C напоминалка:
    SDA A4 18
    SCL A5 19
*/
#include <Wire.h>
#include <Adafruit_Sensor.h> // ХЗ. Adafruit пишут что надо, в связке с Adafruit_BME280
#include <Adafruit_BME280.h>
// OLED
#include <Arduino.h>
#include <U8x8lib.h>
//
Adafruit_BME280 bme; // I2C
// OLED
U8X8_SSD1306_128X32_UNIVISION_HW_I2C u8x8(SCL, SDA, U8X8_PIN_NONE);

uint8_t c, r, e, led;

void setup(){
    u8x8.begin();
    pinMode(led, OUTPUT); // используется при алерте влажности.
    digitalWrite(led, 1); // off в этой нано оказалось наизнанку.
    unsigned status;
    status = bme.begin();  
    if (!status) {
        u8x8.print(".");
        while (1);
    }
    myClear(1);
}

void loop(){
    printValues(15000); // показать основные данные.
    printPrognoz(7000); // барометр с "прогнозом".
    if( bme.readHumidity() < 34 ) printHumAlert(bme.readHumidity()); // алерт низкой влажности.
}

void myClear(uint8_t x){
  // красивости. Очистка экрана. Наз уж памяти хватает.
  // 0 - only clean
  // 1 - full show
  // 2 - minimal
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  if(x != 0){
  for( c = 1; c < u8x8.getCols(); c++ )
    {
      for( r = 0; r < u8x8.getRows(); r++ )
      {
        if(x == 1) {
        u8x8.inverse();
        u8x8.setCursor(c, r);
        u8x8.print(" ");
        u8x8.noInverse();
        }
        u8x8.setCursor(c-1, r);
        u8x8.print(" ");
      }
      delay(50);
    }
  }
  u8x8.clear();
}

void printHumAlert(float vl){
  // функция алерта при влажности ниже 34%
  u8x8.clear();
  u8x8.setFont(u8x8_font_open_iconic_thing_4x4);
  u8x8.drawGlyph(0, 0, '@'+8);
  u8x8.setFont(u8x8_font_inb21_2x4_n);
  u8x8.setCursor(4, 0);
  u8x8.print(vl);
  u8x8.setFont(u8x8_font_open_iconic_thing_2x2);
  u8x8.drawGlyph(14, 0, '@'+8);
  u8x8.setFont(u8x8_font_open_iconic_thing_1x1);
  u8x8.drawGlyph(15, 3, '@'+8);
  digitalWrite(led, 0); // on
  delay(5000);
  digitalWrite(led, 1); // off
  myClear(1);
}

void printPrognoz(uint16_t pt){
  // телепаем по давлению погоду. Данные взяты со старого "аналогового" барометра.
  // насколько это будет совпадать с реальностью - не имею понятия. Надо смотреть.
  uint16_t a = (int)(bme.readPressure()/133.3);
  //u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.setFont(u8x8_font_amstrad_cpc_extended_f);
  switch(a){
    case 690 ... 705:
      // буря
    printOform(a);
    u8x8.drawString(5, 3, "Burya");
      break;
    case 706 ... 715:
      // ветеръ+дождь
    printOform(a);
    u8x8.drawString(5, 3, "Vet-Dozhd");
    u8x8.setFont(u8x8_font_open_iconic_weather_4x4);
    u8x8.drawGlyph(0, 0, '@');
      break;
    case 716 ... 725:
      // ветеръ
    printOform(a);
    u8x8.drawString(5, 3, "Veter");
    u8x8.setFont(u8x8_font_open_iconic_weather_4x4);
    u8x8.drawGlyph(0, 0, '@');
      break;
    case 726 ... 735:
      // дождь
    printOform(a);
    u8x8.drawString(5, 3, "Dozhd");
    u8x8.setFont(u8x8_font_open_iconic_weather_4x4);
    u8x8.drawGlyph(0, 0, '@'+3);
      break;
    case 736 ... 765:
      // переменно
    printOform(a);
    u8x8.drawString(5, 3, "Peremenno");
    u8x8.setFont(u8x8_font_open_iconic_weather_4x4);
    u8x8.drawGlyph(0, 0, '@'+1);
      break;
    case 766 ... 775:
      // ясно
    printOform(a);
    u8x8.drawString(5, 3, "Yasno");
    u8x8.setFont(u8x8_font_open_iconic_weather_4x4);
    u8x8.drawGlyph(0, 0, '@'+5);
      break;
    case 776 ... 785:
      // х погода
    printOform(a);
    u8x8.drawString(5, 3, "H-Pogoda");
    u8x8.setFont(u8x8_font_open_iconic_weather_4x4);
    u8x8.drawGlyph(0, 0, '@'+5);
      break;
    case 786 ... 800:
      // в сушь
    printOform(a);
    u8x8.drawString(5, 3, "Sush");
      break;
    default:
    // Значения уже вне диапазона.
    u8x8.drawString(5, 3, "---------");  
      break;
  }
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  delay(pt);
  myClear(2);
}

void printOform(uint16_t b){
  // повторяющийся кусок оформления вынес в функцию, чтоб не грамоздить.
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.drawString(5, 0, "za bortom:");
  u8x8.setFont(u8x8_font_amstrad_cpc_extended_f);
  u8x8.drawString(6, 2, u8x8_u16toa(b, 3));
  u8x8.drawString(10, 2, "mmH");
}

void printValues(uint16_t pt){
    //u8x8.clear();
    e = (int)u8x8.getCols();
    u8x8.setFont(u8x8_font_chroma48medium8_r);
    // Заголовок
    u8x8.setCursor(2, 0);
    u8x8.inverse();
    u8x8.print(" Home Meteo ");
    u8x8.noInverse();
    // Температура
    u8x8.setCursor(0, 1);
    u8x8.print("Temp:");
    u8x8.setCursor(e-9, 1);
    u8x8.print(bme.readTemperature());
    u8x8.drawString(e-3, 1, "*C");
    // Давление
    u8x8.setCursor(0,2);
    u8x8.print("Davl:");
    u8x8.setCursor(e-10, 2);
    u8x8.print(bme.readPressure()/133.3); // 133.3
    u8x8.drawString(e-3, 2,"mmH");
    // Влажность
    u8x8.setCursor(0,3);
    u8x8.print("Vlazn:");
    u8x8.setCursor(e-9, 3);
    u8x8.print(bme.readHumidity());
    u8x8.drawString(e-3, 3,"%");
    delay(pt);
    myClear(2);
}

// END
