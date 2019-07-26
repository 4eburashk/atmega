// тестирование железа.
// держатель камеры. 2 сервы.

#include <Servo.h>

Servo SRVup;
Servo SRVdown;

void setup()
{
  SRVup.attach(6);		// 0-89
  SRVdown.attach(7);	// 0-90-180
  SRVup.write(0);		// to start 0
  SRVdown.write(91);	// to start 90
}

void upchange(uint8_t DT){
	for(uint8_t i = 0; i < 89; i++)
	{
		SRVup.write(i);
		delay(DT);
	}
	for(uint8_t i = 89; i > 0; i--)
	{
		SRVup.write(i);
		delay(DT);
	}
}

void downchange(uint8_t DT){
	for(uint8_t j = 91; j > 0; j--)
	{
		SRVdown.write(j);
		delay(DT);
	}
	for (uint8_t j = 0; j < 180; j++)
	{
		SRVdown.write(j);
		delay(DT);
	}
	for (uint8_t j = 180; j > 91; j--)
	{
		SRVdown.write(j);
		delay(DT);
	}
}

void loop() 
{ 
	upchange(20);
	delay(1000);
	downchange(20);
	delay(1000);

	downchange(15);
	upchange(15);
	delay(1000);

	downchange(10);
	delay(1000); 
	upchange(10);
	delay(100);
	upchange(7);

	// test position
	SRVup.write(89);
	delay(500);
	SRVdown.write(0);
	delay(500);
	SRVup.write(0);
	delay(500);
	SRVdown.write(180);
	delay(500);
	SRVdown.write(91);
	delay(500);
	SRVup.write(89);
	delay(500);
	SRVup.write(0);
	// fin
	delay(2000);
}

// END
