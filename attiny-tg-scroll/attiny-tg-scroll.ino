// attiny85 digispark mouse scroll
// 4eburashk https://csn.net4me.net
// Прикиываемся мышкой. Произвольно кроллим, останавливаемся на чтение.
// По клику кнопки в прерывании меняем направление скролла (up/down)

#include <DigiMouse.h>
volatile bool FLAG = 0; // state up/down.
volatile uint32_t milsLast;
long rnd;     // random
int bigwait;  // wait read

void setup() {
  DigiMouse.begin();
  pinMode(2, INPUT_PULLUP); // P2 - button - gnd.
  randomSeed(analogRead(0));// shuffle перетряхиваем псевдо-рандом.
  attachInterrupt(0, btnInterr, FALLING); // прерывание P2 INT0
}

void loop() {
  // Иммитация пальца для смартфона.
  rnd = random(7, 11);      // произвольный скролл 7-11
  bigwait = 5350 + rnd * 3; // произвольная иммитация чтения 5371-5427
  for (int i = 0; i < rnd; ++i) {
    DigiMouse.delay(75);
    DigiMouse.scroll((FLAG)?-1:1);
  }
  // зависли подольше. Типа читаем.
  DigiMouse.delay(bigwait);
}

void btnInterr() {
  if (millis() - milsLast > 300){
    // первый раз тут. поэтому:
    milsLast = millis(); // засекли и пока не время - игнорим всё остальное.
    FLAG = !FLAG;        // передёрнули флаг.
  } // Остальные обращения к прерыванию, считаем лишним дребезгом в течении 300мс.
}
// END
