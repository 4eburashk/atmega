// Приветствие входящего поднятием флага (таблички с надписью или еще чем).
// Используются: Серва с флагом + ультразвуковой датчик расстояния HC-SR04 (до 4х метров).
// author: 4eburashk csn.net4me.net

#include "Servo.h"
#define ECHO_PIN 4  // испускаем ультразвук. d4
#define TRIG_PIN 3  // принимаем ультразвук. d3
#define PWR_PIN  2  // питание датчика.      d2
#define SERVO_PIN 5 // управление сервоприводом на PWM pin d5
uint32_t SCST = 0;       // первоночальное состояние скана (от балды).
boolean LSTATE = false;   // состояние флага 0 - флаг внизу. Пока лишнее.
Servo servo;

uint32_t scan(){
    // задача - вернуть расстояние, чтобы сверить с предыдущим значением.
    //Serial.println("* run scan");
    uint32_t duration = 0;
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2); // время чтобы точно выключится.
    // ping
    digitalWrite(TRIG_PIN, HIGH);   // импульс на trig
    delayMicroseconds(10);          // длительностью 10 микросекунд. (преобразование сигнала в 8 импульсов с частотой 40 кГц)
    digitalWrite(TRIG_PIN, LOW);    // закончили импульс.
    duration = pulseIn(ECHO_PIN, HIGH); // включили приёмник.
    return duration / 580; // дециметры.
}

boolean updown(){
    //Serial.println("* in updown ====== ");
    uint8_t SV = 0; // угол подъема. изнач. 0
    servo.write(0); // убеждаемся, что перед подъёмом серва у нас внизу.
    delay(1500); // дадим серве время опуститься, нам некуда спешить.
    // Подъём флага:
    for (uint8_t i = 0; i < 4; i++){
        SV += 45; // должны поднять за 4 рывка. 0-45, 45-90, 90-135, 135-180
        servo.write(SV);
        delay(500); // полсекунды на рывок.
    }
    delay(10000);   // Зависли. чтение надписи флага.
    servo.write(0); // разом опустили. Заодно и посмотрим как лучше двигать.
    delay(1500);    // дадим серве время опуститься, если что. 
    //Serial.println("* out updown ===== ");
    return false;
}

void setup(){
    //Serial.begin (9600);      // раскоментировать для вывода в терминал.
    pinMode(PWR_PIN, OUTPUT);   // питалово датчику.
    digitalWrite(PWR_PIN, HIGH);// сразу и включим питание датчика.
    pinMode(TRIG_PIN, OUTPUT);  // trig
    pinMode(ECHO_PIN, INPUT);   // echo
    servo.attach(SERVO_PIN);
}

void loop(){
    uint32_t SCNOW = scan(); // нынешний скан.
    //Serial.print("Old SCST = ");
    //Serial.print(SCST);
    //Serial.print("\n");
    if( SCST != SCNOW ){
        //Serial.println("Old != Now rescan");
        SCST = SCNOW;     // запомнили последнее состояние в SCST
        //Serial.print("write Now SCST = ");
        //Serial.print(SCST);
        //Serial.print("\n");
        LSTATE = updown(); // операции с флагом.
    }
    delay(15000); // 15 sec между сканами расстояния.
}

// END
