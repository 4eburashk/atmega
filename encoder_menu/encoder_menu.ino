// Меню управление енкодером, со звуком и выводом на LCD 1602.
// author: 4eburashk csn.net4me.net
// за основу работы с энкодором взят класс гайвера (если это он писал).
// encoder (d7,d6,d5) + d4 as power + beeper (d3) + i2c led 1602 (a4,a5) 
//
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);
// пины энкодера:
#define SW 5  // на плате key
#define DT 6  // на плате s2
#define CLK 7 // на плате s1
#define PW5 4 // питалово энкодеру
// доп пины:
#define BP 3  // пищалка
int ArrMax = 7;      // максимальный элемент массива 0-7 (для переключения по меню)
int MenuState = 0;   // актуальный пункт меню.
int MSelected = 0;   // выбраный кликом пункт меню.          
char *menuStrs[] = {"0 Main", "1 One", "2 Two", "3 Three", "4 Four", "5 Five", "6 Six", "7 Seven"};

/*
  Отрсовка меню.
  В моём случае, отрисовываю с 7го символа первой (0) строки.
  В функцию отправляю состояние меню.
*/
void menuShow( int MS ){ // state
  lcd.setCursor(5, 0);
  lcd.print(F("           ")); // очистка места. 16-7=9 мы рисуем с 7 и значит есть 9 знаков на меню.
  lcd.setCursor(5, 0);
  lcd.print(MSelected);   // отрисуем что выбор из меню сделан. напишем номер элемента массива.
  lcd.setCursor(7, 0);
  lcd.print(menuStrs[MS]);
}

void events( int Choise ){
   switch( Choise ) {
    case 1:
      Serial.println("event 1");
      // внутрь обработки выборов
      // можно засунуть любые действия.
      break;
    case 2:
      Serial.println("event 2");
      // Для примера взят любимый led 13
      digitalWrite(13, HIGH);
      break;
    case 3:
      Serial.println("event 3");
      // или выключить его
      digitalWrite(13, LOW);
      break;
    case 4:
      Serial.println("event 4");
      break;
    case 5:
      Serial.println("event 5");
      break;
    case 6:
      Serial.println("event 6");
      break;
    case 7:
      Serial.println("event 7");
      break;
    default:
      Serial.println("event 0");
      break;
    }
}

class encMinim
{
  public:
    encMinim(uint8_t clk, uint8_t dt, uint8_t sw, boolean dir, boolean type);
    void tick();
    boolean isClick();
    boolean isTurn();
    boolean isRight();
    boolean isLeft();
    boolean isRightH();
    boolean isLeftH();

  private:
    byte _clk, _dt, _sw;
    boolean _type = false;
    boolean _state, _lastState, _turnFlag, _swState, _swFlag, _turnState;
    byte _encState;
    uint32_t _debTimer;
    // 0 - ничего, 1 - лево, 2 - право, 3 - правоНажат, 4 - левоНажат
};

encMinim::encMinim(uint8_t clk, uint8_t dt, uint8_t sw, boolean dir, boolean type) {
  if (dir) {
    _clk = clk;
    _dt = dt;
  } else {
    _clk = dt;
    _dt = clk;
  }
  _sw = sw;
  _type = type;
  pinMode (_clk, INPUT);
  pinMode (_dt, INPUT);
  pinMode (_sw, INPUT_PULLUP);
  _lastState = bitRead(PIND, _clk);
}

void encMinim::tick() {
  _encState = 0;
  _state = bitRead(PIND, _clk);
  _swState = bitRead(PIND, _sw);

  if (_state != _lastState) {
    _turnState = true;
    _turnFlag = !_turnFlag;
    if (_turnFlag || !_type) {
      if (bitRead(PIND, _dt) != _lastState) {
        if (_swState) _encState = 1;
        else _encState = 3;
      } else {
        if (_swState) _encState = 2;
        else _encState = 4;
      }
    }
    _lastState = _state;
  }

  if (!_swState && !_swFlag && millis() - _debTimer > 80) {
    _debTimer = millis();
    _swFlag = true;
    _turnState = false;
  }
  if (_swState && _swFlag && millis() - _debTimer > 80) {
    _debTimer = millis();
    _swFlag = false;
    if (!_turnState) _encState = 5;
  }
}
boolean encMinim::isTurn() {
  if (_encState > 0 && _encState < 5) {
    return true;
  } else return false;
}
boolean encMinim::isRight() {
  if (_encState == 1) {
    _encState = 0;
    tone(BP, 1500, 50);
    MenuState++;
    if(MenuState > ArrMax) { MenuState = 0;}
    menuShow(MenuState);
    return true;
  } else return false;
}
boolean encMinim::isLeft() {
  if (_encState == 2) {
    _encState = 0;
    tone(BP, 600, 50);
    MenuState--;
    if(MenuState < 0) { MenuState = ArrMax; }
    menuShow(MenuState);
    return true;
  } else return false;
}
boolean encMinim::isRightH() {
  if (_encState == 3) {
    _encState = 0;
    tone(BP, 1700, 50);
    return true;
  } else return false;
}
boolean encMinim::isLeftH() {
  if (_encState == 4) {
    _encState = 0;
    tone(BP, 800, 70);
    return true;
  } else return false;
}
boolean encMinim::isClick() {
  if (_encState == 5) {
    _encState = 0;
    tone(BP, 1000, 70);
    MSelected = MenuState; // запомнили выбор в перем. ??? разобраться.
    // тут можно вставить какой-нить выбор набора действий типа
    menuShow(MenuState);
    events(MenuState);
    return true;
  } else return false;
}

// пин clk, пин dt, пин sw, направление (0/1), тип (0/1)
encMinim enc(CLK, DT, SW, 1, 1);

void setup() {
  pinMode(PW5, OUTPUT);     // питалово
  digitalWrite(PW5, HIGH);  // просто было удобнее взять питалово на энкодер с пина.
  Serial.begin(9600);       // сериал. Пока. потом можно убрать терминал.
  lcd.init();               // initialize the lcd 
	lcd.init();
	// Print a message to the LCD.
	lcd.backlight();
	/* prints static text */
	lcd.setCursor(0, 0);
	lcd.print(F("Test"));
	lcd.setCursor(0, 1);
	lcd.print(F("Monitor"));
	pinMode(BP, OUTPUT );        // Настраиваем пищалку на выход
  menuShow(MenuState);         // Отрисоваать меню в первый раз.
  pinMode(13, OUTPUT);
}

void loop() {
  enc.tick();
  if (enc.isTurn()) {
    Serial.print("turn ");
  }
  if (enc.isLeft()) {
    Serial.print("left\n");
  }
  if (enc.isRight()) {
    Serial.print("right\n");
  }
  if (enc.isLeftH()) {
    Serial.print("leftH\n");
    lcd.setCursor(8, 1); // пока не придумал подо что использовать повороты с нажатием.
    lcd.print(F("leftH"));
  }
  if (enc.isRightH()) {
    Serial.print("rightH\n");
    lcd.setCursor(8, 1);
    lcd.print(F("rightH"));
  }
  if (enc.isClick()) {
    Serial.println("click");
    lcd.setCursor(8, 1);
    lcd.print(F("click "));
  }
}
