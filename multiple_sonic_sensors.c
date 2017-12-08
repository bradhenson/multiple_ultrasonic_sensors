/********************************************************************************
Author: 
Date: 
Version: 0.0.0.1
License: USE AT YOUR OWN RISK

Description: 
------------
TODO

Hardware Connections:
-------------------------------------
TODO

Notes:
-------------------------------------
TODO

********************************************************************************/

#define F_CPU 16000000UL
#define SensorOneTrigger   PC5
#define SensorOneEcho      PC4
#define SensorTwoTrigger   PC3
#define SensorTwoEcho      PC2
#define SensorThreeTrigger PC1
#define SensorThreeEcho    PC0
#define SensorFourTrigger  PB5
#define SensorFourEcho	   PB4
#define SensorFiveTrigger  PB3
#define SensorFiveEcho     PB2
#define SensorSixTrigger   PB1
#define SensorSixEcho      PB0
#define SensorSevenTrigger PD7
#define SensorSevenEcho    PD6
#define SensorEightTrigger PD5
#define SensorEightEcho    PD4

#include <avr/io.h>
#include <util/delay.h>
#include <math.h>
#include <avr/interrupt.h>

//Prototypes of all functions
void initializeUART(int baud);
void transmitByte(unsigned char data);
void printString(const char myString[]);
uint16_t sensorOnePing(void);
uint16_t sensorTwoPing(void);
uint16_t sensorThreePing(void);
uint16_t sensorFourPing(void);
uint16_t sensorFivePing(void);
uint16_t sensorSixPing(void);
uint16_t sensorSevenPing(void);
uint16_t sensorEightPing(void);

//Global Variables
int numberCycles = 0;
int timeElasped = 0;
uint16_t distance = 0;
uint16_t distanceOne = 0;
uint16_t distanceTwo = 0;
uint16_t distanceThree = 0;
uint16_t distanceFour = 0;
uint16_t distanceFive = 0;
uint16_t distanceSix = 0;
uint16_t distanceSeven = 0;
uint16_t distanceEight = 0;
uint8_t pinChangeFlag = 0;
uint8_t timeOut = 0;
uint8_t distanceOneHigh = 0;
uint8_t distanceOneLow = 0;
uint8_t distanceTwoHigh = 0;
uint8_t distanceTwoLow = 0;
uint8_t distanceThreeHigh = 0;
uint8_t distanceThreeLow = 0;
uint8_t distanceFourHigh = 0;
uint8_t distanceFourLow = 0;
uint8_t distanceFiveHigh = 0;
uint8_t distanceFiveLow = 0;
uint8_t distanceSixHigh = 0;
uint8_t distanceSixLow = 0;
uint8_t distanceSevenHigh = 0;
uint8_t distanceSevenLow = 0;
uint8_t distanceEightHigh = 0;
uint8_t distanceEightLow = 0;

int main(void)
{
/********************************************************************************
                          
						     SETUP SECTION
							
********************************************************************************/

/******************************************************************
Sets the all of the trigger pins as outputs and all of the echo 
pin as inputs
******************************************************************/
DDRC |= (1 << SensorOneTrigger);
DDRC &= ~(1 << SensorOneEcho);
DDRC |= (1 << SensorTwoTrigger);
DDRC &= ~(1 << SensorTwoEcho);
DDRC |= (1 << SensorThreeTrigger); 
DDRC &= ~(1 << SensorThreeEcho);
DDRB |= (1 << SensorFourTrigger); 
DDRB &= ~(1 << SensorFourEcho);
DDRB |= (1 << SensorFiveTrigger); 
DDRB &= ~(1 << SensorFiveEcho);
DDRB |= (1 << SensorSixTrigger); 
DDRB &= ~(1 << SensorSixEcho);
DDRD |= (1 << SensorSevenTrigger); 
DDRD &= ~(1 << SensorSevenEcho);
DDRD |= (1 << SensorEightTrigger); 
DDRD &= ~(1 << SensorEightEcho);

/******************************************************************
Sets of the pins being used to a state of zero, it could be augured 
that this step is not needed, but it is better to initialize all 
the pins to a known state.			
******************************************************************/
PORTC &= ~(1 << SensorOneTrigger) | (1 << SensorOneEcho);
PORTC &= ~(1 << SensorTwoTrigger) | (1 << SensorTwoEcho);
PORTC &= ~(1 << SensorThreeTrigger) | (1 << SensorThreeEcho);
PORTB &= ~(1 << SensorFourTrigger)  | (1 << SensorFourEcho);
PORTB &= ~(1 << SensorFiveTrigger) | (1 << SensorFiveEcho);	
PORTB &= ~(1 << SensorSixTrigger) | (1 << SensorSixEcho);
PORTD &= ~(1 << SensorSevenTrigger) | (1 << SensorSevenEcho);
PORTD &= ~(1 << SensorEightTrigger) | (1 << SensorEightEcho);

initializeUART(9600);
sei();

/******************************************************************
Need to initialize the timer/counter here
TCCR1A bits 0 and 1 need to be set to zero for normal mode.
TCCR1B bits x and x need to be set to zero for normal mode.
Technically they should already be zero, but this ensures they are.			
******************************************************************/
TCCR1A &= ~(1 << WGM10) | (1 << WGM11);
TCCR1B &= ~(1 << WGM12) | (1 << WGM12);
TCCR1B &= ~(1 << CS10) | (1 << CS11) | (1 << CS12); //No Pre-Scaler for the clk

/******************************************************************
Need to enable Pin Change Interrupts on basically all the pins
PCICR is the Pin Change Interrupt Control Register, the following three bits
control which bank of pins is enabled for Pin change interrupts:
PCIE0 controls pins 0-7
PCIE1 controls pins 8-14
PCIE2 controls pins 16-23			
******************************************************************/
PCICR |= (1 << PCIE0) | (1 << PCIE1) | (1 << PCIE1);

/********************************************************************************
                          
						      LOOP SECTION
							
********************************************************************************/	
    while (1) 
    {
	   /******************************************************************
		First we capture the distance in a 16 bit integer by sending a ping
		So that we can transmit this data one byte at a time, we separate the
		16 bit integer into a high and low 8 bit integer.
		to reassemble this one the receive side use something like:
		distance = (distanceOneHigh << 8) | distanceOneLow				
	   ******************************************************************/		
		distanceOne = sensorOnePing();
		distanceOneLow = distanceOne & 0xFF;
		distanceOneHigh = (distanceOne >> 8) & 0xFF;
		
		distanceTwo = sensorTwoPing();
		distanceTwoLow = distanceTwo & 0xFF;
		distanceTwoHigh = (distanceTwo >> 8) & 0xFF;
		
		distanceTwo = sensorTwoPing();
		distanceTwoLow = distanceTwo & 0xFF;
		distanceTwoHigh = (distanceTwo >> 8) & 0xFF;
		
		distanceTwo = sensorTwoPing();
		distanceTwoLow = distanceTwo & 0xFF;
		distanceTwoHigh = (distanceTwo >> 8) & 0xFF;
		
		distanceTwo = sensorTwoPing();
		distanceTwoLow = distanceTwo & 0xFF;
		distanceTwoHigh = (distanceTwo >> 8) & 0xFF;
		
		distanceTwo = sensorTwoPing();
		distanceTwoLow = distanceTwo & 0xFF;
		distanceTwoHigh = (distanceTwo >> 8) & 0xFF;
		
		distanceTwo = sensorTwoPing();
		distanceTwoLow = distanceTwo & 0xFF;
		distanceTwoHigh = (distanceTwo >> 8) & 0xFF;
		
		distanceTwo = sensorTwoPing();
		distanceTwoLow = distanceTwo & 0xFF;
		distanceTwoHigh = (distanceTwo >> 8) & 0xFF;

	   /******************************************************************
		Next we send each byte of data to the UART interface			
	   ******************************************************************/	
		transmitByte(distanceOneLow);
		transmitByte(distanceOneHigh);
		transmitByte(distanceTwoLow);
		transmitByte(distanceTwoHigh);
		transmitByte(distanceThreeLow);
		transmitByte(distanceThreeHigh);
		transmitByte(distanceFourLow);
		transmitByte(distanceFourHigh);
		transmitByte(distanceFiveLow);
		transmitByte(distanceFiveHigh);
		transmitByte(distanceSixLow);
		transmitByte(distanceSixHigh);
		transmitByte(distanceSevenLow);
		transmitByte(distanceSevenHigh);
		transmitByte(distanceEightLow);
		transmitByte(distanceEightHigh);
    }
}
/********************************************************************************
                          
							   FUNCTIONS
							
********************************************************************************/

/********************************************************************************
Name: ISR(PCINT0_vect)

Description:

Returns: Nothing
********************************************************************************/
ISR (PCINT0_vect)
{
	pinChangeFlag = 1;
}

/********************************************************************************
Name: ISR(PCINT1_vect)

Description:

Returns: Nothing
********************************************************************************/
ISR (PCINT1_vect)
{
	pinChangeFlag = 1;
}

/********************************************************************************
Name: ISR(PCINT2_vect)

Description:

Returns: Nothing
********************************************************************************/
ISR (PCINT2_vect)
{
	pinChangeFlag = 1;
}

/********************************************************************************
Name: sensorOnePing()

Description:

Returns: Nothing
********************************************************************************/
uint16_t sensorOnePing(void)
{
	//the hc-sr04 requires a 10 microsecond pulse to initiate the trigger
	//we first set the pin low as a starting point and then high for 10 microseconds
	PORTC &= ~(1 << SensorOneTrigger);
	_delay_us(5);
	PORTC |= (1 << SensorOneTrigger);
	_delay_us(10);
	PORTC &= ~(1 << SensorOneTrigger);
	//Clear the timer register, starts the counter
	TCNT1 = 0x0000;
	//Enables Pin Change Interrupt for sensor one
	PCMSK1 |= (1 << PCINT12);
	//Start waiting in a loop for the counter to finish 
	while (pinChangeFlag == 0)
	{
		uint8_t i = 0;
		i++;
		//determine what the timeout should be, 1300000 is roughly 8 milliseconds
		//or that translates to about 4 ft at the speed of sound, the sensor should
		//receive a pulse by this point.
		if (i >= 130000) 
		{
			//In the event a pulse isn't received the timeOut variable is set
			timeOut = 1;
			break; 
		}
	}
	
   if (timeOut == 0)
   {
	   	numberCycles = TCNT1;
	   	pinChangeFlag = 0;
	   	
	   	//Need to write code to determine microsecond based on cycles
		//At 16Mhz we multiply the number of cycles by 1/16
		//Something to keep in mind is that timeElasped is a 16 bit integer and 
		//will round off to the nearest microsecond. This should be fine for this application
	   	timeElasped = (numberCycles/16);
	   	
	   	//Convert the timeElasped into millimeters and store in distanceOne variable
		//Something to keep in mind is that distanceOne is a 16 bit integer and 
		//will round off to the nearest millimeter. This should be fine for this application
	   	distance = ((timeElasped*0.334)/2);
	   	
	   	//Disable Pin Change Interrupt for sensor one
	   	PCMSK1 &= ~(1 << PCINT12);
   }
   else
   {
	   //No return pulse was recorded, so the distance value is set to zero
	   distance = 0;
   } 
	return distance;
}

/********************************************************************************
Name: sensorTwoPing()

Description:

Returns: Nothing
********************************************************************************/
uint16_t sensorTwoPing(void)
{
	//the hc-sr04 requires a 10 microsecond pulse to initiate the trigger
	//we first set the pin low as a starting point and then high for 10 microseconds
	PORTC &= ~(1 << SensorTwoTrigger);
	_delay_us(5);
	PORTC |= (1 << SensorTwoTrigger);
	_delay_us(10);
	PORTC &= ~(1 << SensorTwoTrigger);
	//Clear the timer register, starts the counter
	TCNT1 = 0x0000;
	//Enables Pin Change Interrupt for sensor one
	PCMSK1 |= (1 << PCINT10);
	//Start waiting in a loop for the counter to finish
	while (pinChangeFlag == 0)
	{
		uint8_t i = 0;
		i++;
		//determine what the timeout should be, 1300000 is roughly 8 milliseconds
		//or that translates to about 4 ft at the speed of sound, the sensor should
		//receive a pulse by this point.
		if (i >= 130000)
		{
			//In the event a pulse isn't received the timeOut variable is set
			timeOut = 1;
			break;
		}
	}
	
	if (timeOut == 0)
	{
		numberCycles = TCNT1;
		pinChangeFlag = 0;
		
		//Need to write code to determine microsecond based on cycles
		//At 16Mhz we multiply the number of cycles by 1/16
		//Something to keep in mind is that timeElasped is a 16 bit integer and
		//will round off to the nearest microsecond. This should be fine for this application
		timeElasped = (numberCycles/16);
		
		//Convert the timeElasped into millimeters and store in distanceOne variable
		//Something to keep in mind is that distanceOne is a 16 bit integer and
		//will round off to the nearest millimeter. This should be fine for this application
		distance = ((timeElasped*0.334)/2);
		
		//Disable Pin Change Interrupt for sensor one
		PCMSK1 &= ~(1 << PCINT10);
	}
	else
	{
		//No return pulse was recorded, so the distance value is set to zero
		distance = 0;
	}
	return distance;
}

/********************************************************************************
Name: sensorThreePing()

Description:

Returns: Nothing
********************************************************************************/
uint16_t sensorThreePing(void)
{
	//the hc-sr04 requires a 10 microsecond pulse to initiate the trigger
	//we first set the pin low as a starting point and then high for 10 microseconds
	PORTC &= ~(1 << SensorThreeTrigger);
	_delay_us(5);
	PORTC |= (1 << SensorThreeTrigger);
	_delay_us(10);
	PORTC &= ~(1 << SensorThreeTrigger);
	//Clear the timer register, starts the counter
	TCNT1 = 0x0000;
	//Enables Pin Change Interrupt for sensor one
	PCMSK1 |= (1 << PCINT8);
	//Start waiting in a loop for the counter to finish
	while (pinChangeFlag == 0)
	{
		uint8_t i = 0;
		i++;
		//determine what the timeout should be, 1300000 is roughly 8 milliseconds
		//or that translates to about 4 ft at the speed of sound, the sensor should
		//receive a pulse by this point.
		if (i >= 130000)
		{
			//In the event a pulse isn't received the timeOut variable is set
			timeOut = 1;
			break;
		}
	}
	
	if (timeOut == 0)
	{
		numberCycles = TCNT1;
		pinChangeFlag = 0;
		
		//Need to write code to determine microsecond based on cycles
		//At 16Mhz we multiply the number of cycles by 1/16
		//Something to keep in mind is that timeElasped is a 16 bit integer and
		//will round off to the nearest microsecond. This should be fine for this application
		timeElasped = (numberCycles/16);
		
		//Convert the timeElasped into millimeters and store in distanceOne variable
		//Something to keep in mind is that distanceOne is a 16 bit integer and
		//will round off to the nearest millimeter. This should be fine for this application
		distance = ((timeElasped*0.334)/2);
		
		//Disable Pin Change Interrupt for sensor one
		PCMSK1 &= ~(1 << PCINT8);
	}
	else
	{
		//No return pulse was recorded, so the distance value is set to zero
		distance = 0;
	}
	return distance;
}

/********************************************************************************
Name: sensorFourPing()

Description:

Returns: Nothing
********************************************************************************/
uint16_t sensorFourPing(void)
{
	//the hc-sr04 requires a 10 microsecond pulse to initiate the trigger
	//we first set the pin low as a starting point and then high for 10 microseconds
	PORTB &= ~(1 << SensorFourTrigger);
	_delay_us(5);
	PORTB |= (1 << SensorFourTrigger);
	_delay_us(10);
	PORTB &= ~(1 << SensorFourTrigger);
	//Clear the timer register, starts the counter
	TCNT1 = 0x0000;
	//Enables Pin Change Interrupt for sensor one
	PCMSK0 |= (1 << PCINT4);
	//Start waiting in a loop for the counter to finish
	while (pinChangeFlag == 0)
	{
		uint8_t i = 0;
		i++;
		//determine what the timeout should be, 1300000 is roughly 8 milliseconds
		//or that translates to about 4 ft at the speed of sound, the sensor should
		//receive a pulse by this point.
		if (i >= 130000)
		{
			//In the event a pulse isn't received the timeOut variable is set
			timeOut = 1;
			break;
		}
	}
	
	if (timeOut == 0)
	{
		numberCycles = TCNT1;
		pinChangeFlag = 0;
		
		//Need to write code to determine microsecond based on cycles
		//At 16Mhz we multiply the number of cycles by 1/16
		//Something to keep in mind is that timeElasped is a 16 bit integer and
		//will round off to the nearest microsecond. This should be fine for this application
		timeElasped = (numberCycles/16);
		
		//Convert the timeElasped into millimeters and store in distanceOne variable
		//Something to keep in mind is that distanceOne is a 16 bit integer and
		//will round off to the nearest millimeter. This should be fine for this application
		distance = ((timeElasped*0.334)/2);
		
		//Disable Pin Change Interrupt for sensor one
		PCMSK0 &= ~(1 << PCINT4);
	}
	else
	{
		//No return pulse was recorded, so the distance value is set to zero
		distance = 0;
	}
	return distance;
}
/********************************************************************************
Name: sensorFivePing()

Description:

Returns: Nothing
********************************************************************************/
uint16_t sensorFivePing(void)
{
	//the hc-sr04 requires a 10 microsecond pulse to initiate the trigger
	//we first set the pin low as a starting point and then high for 10 microseconds
	PORTB &= ~(1 << SensorFiveTrigger);
	_delay_us(5);
	PORTB |= (1 << SensorFiveTrigger);
	_delay_us(10);
	PORTB &= ~(1 << SensorFiveTrigger);
	//Clear the timer register, starts the counter
	TCNT1 = 0x0000;
	//Enables Pin Change Interrupt for sensor one
	PCMSK0 |= (1 << PCINT2);
	//Start waiting in a loop for the counter to finish
	while (pinChangeFlag == 0)
	{
		uint8_t i = 0;
		i++;
		//determine what the timeout should be, 1300000 is roughly 8 milliseconds
		//or that translates to about 4 ft at the speed of sound, the sensor should
		//receive a pulse by this point.
		if (i >= 130000)
		{
			//In the event a pulse isn't received the timeOut variable is set
			timeOut = 1;
			break;
		}
	}
	
	if (timeOut == 0)
	{
		numberCycles = TCNT1;
		pinChangeFlag = 0;
		
		//Need to write code to determine microsecond based on cycles
		//At 16Mhz we multiply the number of cycles by 1/16
		//Something to keep in mind is that timeElasped is a 16 bit integer and
		//will round off to the nearest microsecond. This should be fine for this application
		timeElasped = (numberCycles/16);
		
		//Convert the timeElasped into millimeters and store in distanceOne variable
		//Something to keep in mind is that distanceOne is a 16 bit integer and
		//will round off to the nearest millimeter. This should be fine for this application
		distance = ((timeElasped*0.334)/2);
		
		//Disable Pin Change Interrupt for sensor one
		PCMSK0 &= ~(1 << PCINT2);
	}
	else
	{
		//No return pulse was recorded, so the distance value is set to zero
		distance = 0;
	}
	return distance;
}

/********************************************************************************
Name: sensorSixPing()

Description:

Returns: Nothing
********************************************************************************/
uint16_t sensorSixPing(void)
{
	//the hc-sr04 requires a 10 microsecond pulse to initiate the trigger
	//we first set the pin low as a starting point and then high for 10 microseconds
	PORTB &= ~(1 << SensorSixTrigger);
	_delay_us(5);
	PORTB |= (1 << SensorSixTrigger);
	_delay_us(10);
	PORTB &= ~(1 << SensorSixTrigger);
	//Clear the timer register, starts the counter
	TCNT1 = 0x0000;
	//Enables Pin Change Interrupt for sensor one
	PCMSK0 |= (1 << PCINT0);
	//Start waiting in a loop for the counter to finish
	while (pinChangeFlag == 0)
	{
		uint8_t i = 0;
		i++;
		//determine what the timeout should be, 1300000 is roughly 8 milliseconds
		//or that translates to about 4 ft at the speed of sound, the sensor should
		//receive a pulse by this point.
		if (i >= 130000)
		{
			//In the event a pulse isn't received the timeOut variable is set
			timeOut = 1;
			break;
		}
	}
	
	if (timeOut == 0)
	{
		numberCycles = TCNT1;
		pinChangeFlag = 0;
		
		//Need to write code to determine microsecond based on cycles
		//At 16Mhz we multiply the number of cycles by 1/16
		//Something to keep in mind is that timeElasped is a 16 bit integer and
		//will round off to the nearest microsecond. This should be fine for this application
		timeElasped = (numberCycles/16);
		
		//Convert the timeElasped into millimeters and store in distanceOne variable
		//Something to keep in mind is that distanceOne is a 16 bit integer and
		//will round off to the nearest millimeter. This should be fine for this application
		distance = ((timeElasped*0.334)/2);
		
		//Disable Pin Change Interrupt for sensor one
		PCMSK0 &= ~(1 << PCINT0);
	}
	else
	{
		//No return pulse was recorded, so the distance value is set to zero
		distance = 0;
	}
	return distance;
}

/********************************************************************************
Name: sensorSevenPing()

Description:

Returns: Nothing
********************************************************************************/
uint16_t sensorSevenPing(void)
{
	//the hc-sr04 requires a 10 microsecond pulse to initiate the trigger
	//we first set the pin low as a starting point and then high for 10 microseconds
	PORTD &= ~(1 << SensorSevenTrigger);
	_delay_us(5);
	PORTD |= (1 << SensorSevenTrigger);
	_delay_us(10);
	PORTD &= ~(1 << SensorSevenTrigger);
	//Clear the timer register, starts the counter
	TCNT1 = 0x0000;
	//Enables Pin Change Interrupt for sensor one
	PCMSK2 |= (1 << PCINT22);
	//Start waiting in a loop for the counter to finish
	while (pinChangeFlag == 0)
	{
		uint8_t i = 0;
		i++;
		//determine what the timeout should be, 1300000 is roughly 8 milliseconds
		//or that translates to about 4 ft at the speed of sound, the sensor should
		//receive a pulse by this point.
		if (i >= 130000)
		{
			//In the event a pulse isn't received the timeOut variable is set
			timeOut = 1;
			break;
		}
	}
	
	if (timeOut == 0)
	{
		numberCycles = TCNT1;
		pinChangeFlag = 0;
		
		//Need to write code to determine microsecond based on cycles
		//At 16Mhz we multiply the number of cycles by 1/16
		//Something to keep in mind is that timeElasped is a 16 bit integer and
		//will round off to the nearest microsecond. This should be fine for this application
		timeElasped = (numberCycles/16);
		
		//Convert the timeElasped into millimeters and store in distanceOne variable
		//Something to keep in mind is that distanceOne is a 16 bit integer and
		//will round off to the nearest millimeter. This should be fine for this application
		distance = ((timeElasped*0.334)/2);
		
		//Disable Pin Change Interrupt for sensor one
		PCMSK2 &= ~(1 << PCINT22);
	}
	else
	{
		//No return pulse was recorded, so the distance value is set to zero
		distance = 0;
	}
	return distance;
}

/********************************************************************************
Name: sensorEightPing()

Description:

Returns: Nothing
********************************************************************************/
uint16_t sensorEightPing(void)
{
	//the hc-sr04 requires a 10 microsecond pulse to initiate the trigger
	//we first set the pin low as a starting point and then high for 10 microseconds
	PORTD &= ~(1 << SensorEightTrigger);
	_delay_us(5);
	PORTD |= (1 << SensorEightTrigger);
	_delay_us(10);
	PORTD &= ~(1 << SensorEightTrigger);
	//Clear the timer register, starts the counter
	TCNT1 = 0x0000;
	//Enables Pin Change Interrupt for sensor one
	PCMSK2 |= (1 << PCINT20);
	//Start waiting in a loop for the counter to finish
	while (pinChangeFlag == 0)
	{
		uint8_t i = 0;
		i++;
		//determine what the timeout should be, 1300000 is roughly 8 milliseconds
		//or that translates to about 4 ft at the speed of sound, the sensor should
		//receive a pulse by this point.
		if (i >= 130000)
		{
			//In the event a pulse isn't received the timeOut variable is set
			timeOut = 1;
			break;
		}
	}
	
	if (timeOut == 0)
	{
		numberCycles = TCNT1;
		pinChangeFlag = 0;
		
		//Need to write code to determine microsecond based on cycles
		//At 16Mhz we multiply the number of cycles by 1/16
		//Something to keep in mind is that timeElasped is a 16 bit integer and
		//will round off to the nearest microsecond. This should be fine for this application
		timeElasped = (numberCycles/16);
		
		//Convert the timeElasped into millimeters and store in distanceOne variable
		//Something to keep in mind is that distanceOne is a 16 bit integer and
		//will round off to the nearest millimeter. This should be fine for this application
		distance = ((timeElasped*0.334)/2);
		
		//Disable Pin Change Interrupt for sensor one
		PCMSK2 &= ~(1 << PCINT20);
	}
	else
	{
		//No return pulse was recorded, so the distance value is set to zero
		distance = 0;
	}
	return distance;
}
/********************************************************************************
Name: initializeUART()

Description: Allows user to set the UART baud rate and initializes the UART. 
The other settings are set as the typical settings:

Data Bit = 8
Parity = 0
Stop Bit = 1

Returns: Nothing
********************************************************************************/
void initializeUART(int baud)
{
	uint16_t UBBRValue = lrint(( (F_CPU) / (16L * baud) ) - 1);
	
	UBRR0H = (unsigned char) (UBBRValue >> 8);		  //set the upper part of the baud
	UBRR0L = (unsigned char) UBBRValue;				  //set the lower part of the baud
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);			  //enable the transmitter and receiver
	UCSR0B |= (1 << RXCIE0);					      //Enable interrupt
	UCSR0C |= (3 << UCSZ00);						  //8-bit data length
	UCSR0C |= (1 << UPM01);                           //Sets parity to NONE - 0

}

/********************************************************************************
Name: transmitByte()

Description: Allows the user to transmit a single byte out the UART interface

Returns: Nothing
********************************************************************************/

void transmitByte(unsigned char data)
{
	//Wait until the Transmitter is ready
	while (! (UCSR0A & (1 << UDRE0)) );
	UDR0 = data;
}
