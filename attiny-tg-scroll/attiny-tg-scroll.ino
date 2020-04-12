// attiny85 digispark hid mouse scroll up/down
// 4eburashk https://csn.net4me.net
// Прикидываемся мышкой. Произвольно скроллим, останавливаемся на чтение.
// По клику кнопки в прерывании меняем направление скролла (up/down)
// можно и без кнопки - по умолчанию скроллит вверх.

#include <DigiMouse.h>
volatile bool FLAG = 0; // state up/down.
volatile uint32_t milsLast;
long rnd;     // random
int bigwait;  // read

void setup(){
  DigiMouse.begin();
  pinMode(2, INPUT_PULLUP); // P2 - button - gnd.
  randomSeed(analogRead(0));// shuffle перетряхиваем псевдо-рандом.
  attachInterrupt(0, btnInterr, FALLING); // прерывание P2 INT0
}

void loop(){
  rnd = random(7, 11);               // произвольный скролл 7-11
  bigwait = 5350 + rnd * 3;          // произвольная иммитация чтения 5371-5427мс
  for (int i = 0; i < rnd; ++i){
    DigiMouse.delay(75);             // скорость прокрутки
    DigiMouse.scroll((FLAG)?-1:1);   // процесс скролла.
  }
  DigiMouse.delay(bigwait);          // зависли подольше. Типа читаем.
}

void btnInterr() {
  if (millis() - milsLast > 300){
    milsLast = millis();             // засекли время - игнорим всё остальное.
    FLAG = !FLAG;
  } // Остальные обращения к прерыванию - считаем лишним дребезгом в течении 300мс.
}
// END
