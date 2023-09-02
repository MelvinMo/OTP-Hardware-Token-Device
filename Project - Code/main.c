/*
 * Project-Code.c
 *
 * Created: 1/25/2022 7:14:01 PM
 * Author : Melvin Mokhtari
 */ 

#define F_CPU 1000000
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define LCD_DPRT PORTD
#define LCD_DDDR DDRD
#define LCD_DPIN PIND
#define LCD_CPRT PORTC
#define LCD_CDDR DDRC
#define LCD_CPIN PINC
#define LCD_RS 0
#define LCD_RW 1
#define LCD_EN 2

#define KEY_PRT 	PORTA
#define KEY_DDR		DDRA
#define KEY_PIN		PINA

unsigned char keypad[4][4] = {
	{'7','8','9','/'},
	{'4','5','6','*'},
	{'1','2','3','-'},
	{' ','0','=','+'}
};
unsigned char colloc, rowloc;
unsigned char currentPassword[10]="1234";
unsigned char enterdPassword[10];
char enterdPasswordIndex=0;
unsigned char enterdNewPassword[10];
char  enterdNewPasswordIndex=0;


unsigned char * menus[4]={"GENERATE PASS: 1","CHANGE PASS: 2","DISABLE CARD: 3","EXIT: 4"};

char menuIndex=0;
char state=1;

// 1 => show menu
// 2 => show generate menu
// 3 => password enterd correctly
// 4 => show change password menu
// 5 => password enterd correctly
// 6 => disable card
// 7 => display off

// LCD
void lcdCommand(unsigned char);
void lcdData(unsigned char);
void lcdInit();
void lcdGoToxy(unsigned char, unsigned char);
void lcdPrint(unsigned char *);

// key find
char keyfind();
void showIntro();
void showLoading();
// main menu
void showMenu();
// generate password
void showGenerateMenu();
void handleSubmitGenerateMenu();
// change password
void showChangePasswordMenu();
void handleSubmitChangePasswordMenu();
void showNewChangePasswordMenu();
void handleSubmitNewChangePasswordMenu();
// disable card
void showDisableCardMenu();
// turn display on/off
void displayOn();
void displayOff();
// initial timer
void initialTimer();

int main(void)
{
	lcdInit();
	showIntro();
	_delay_ms(2000);
	showLoading();
	showMenu();
	srand(time(NULL));
	while(1) {
		char key = keyfind();
		if(state == 1){
			if(key == '1')
			showGenerateMenu();
			else if(key == '2')
			showChangePasswordMenu();
			else if(key == '3')
			showDisableCardMenu();
			else if(key == '4')
			displayOff();
			else if(key == '+')
			{
				menuIndex=(menuIndex+1)%4;
				showMenu();
			}

		}
		else if(state == 2){
			if(key == '*')
			{
				showLoading();
				handleSubmitGenerateMenu();
			}
			else
			{
				initialTimer();
				lcdData(key);
				enterdPassword[enterdPasswordIndex]=key;
				enterdPasswordIndex++;
			}
		}
		else if(state == 4){
			if(key == '*')
			{
				showLoading();
				handleSubmitChangePasswordMenu();
			}
			else
			{
				initialTimer();
				lcdData(key);
				enterdPassword[enterdPasswordIndex]=key;
				enterdPasswordIndex++;
			}
		}
		else if(state == 5){
			if(key == '*')
			{
				showLoading();
				handleSubmitNewChangePasswordMenu();
			}
			else
			{
				initialTimer();
				lcdData(key);
				enterdNewPassword[enterdNewPasswordIndex]=key;
				enterdNewPasswordIndex++;
			}
		}
		else if(state == 6){
			if(key == '1')
			{
				initialTimer();
				lcdPrint("PROCCESS DONE!");
				_delay_ms(1000);
				showMenu();
			}
			else if(key == '2')
			{
				initialTimer();
				lcdPrint("PROCESS FAILED!");
				_delay_ms(1000);
				showMenu();
			}
		}
		else if(state == 7){
			if(key == '*')
			{
				displayOn();
			}
		}
	}
}

void lcdCommand(unsigned char command)
{
	LCD_DPRT=command;
	LCD_CPRT &= ~(1<<LCD_RS);
	LCD_CPRT &= ~(1<<LCD_RW);
	LCD_CPRT |= (1<<LCD_EN);
	_delay_us(1);
	LCD_CPRT &= ~ (1<<LCD_EN);
	_delay_us(100);
}

void lcdData(unsigned char data)
{
	LCD_DPRT=data;
	LCD_CPRT |= (1<<LCD_RS);
	LCD_CPRT &= ~(1<<LCD_RW);
	LCD_CPRT |= (1<<LCD_EN);
	_delay_us(1);
	LCD_CPRT &= ~ (1<<LCD_EN);
	_delay_us(100);
}

void lcdInit()
{
	LCD_DDDR = 0xFF;
	LCD_CDDR = 0xFF;
	LCD_CPRT &= ~(1 << LCD_EN);

	_delay_ms(2);
	lcdCommand(0x38); // Initializing to 2 lines & 5x7 font
	lcdCommand(0x06); // After displaying a character on the LCD, shift cursor to right
	// lcdCommand(0x0E); // Display on, cursor on
	lcdCommand(0x0C); // Display on, cursor off
	lcdCommand(0x01); // Clear display screen
	_delay_ms(2);
}

void lcdGoToxy(unsigned char x, unsigned char y)
{
	unsigned char firstCharAdr[]={0x80,0xC0,0x94,0xD4};
	lcdCommand(firstCharAdr[y-1] + x - 1);
	_delay_us(100);
}

void lcdPrint(unsigned char *string)
{
	unsigned char i = 0;
	while (string[i]!=0)
	{
		lcdData(string[i]);
		_delay_ms(25);
		i++;
	}
}

char keyfind()
{
	while(1)
	{
		KEY_DDR = 0xF0;           /* set port direction as input-output */
		KEY_PRT = 0xFF;

		do {
			KEY_PRT &= 0x0F;      /* mask PORT for column read only */
			asm("NOP");
			colloc = (KEY_PIN & 0x0F); /* read status of column */
		} while(colloc != 0x0F);
		
		do
		{
			do
			{
				_delay_ms(20);             /* 20ms key debounce time */
				colloc = (KEY_PIN & 0x0F); /* read status of column */
				}while(colloc == 0x0F);        /* check for any key press */
				
				_delay_ms (40);	            /* 20 ms key debounce time */
				colloc = (KEY_PIN & 0x0F);
			}while(colloc == 0x0F);

			/* now check for rows */
			KEY_PRT = 0xEF;            /* check for pressed key in 1st row */
			asm("NOP");
			colloc = (KEY_PIN & 0x0F);
			if(colloc != 0x0F)
			{
				rowloc = 0;
				break;
			}

			KEY_PRT = 0xDF;		/* check for pressed key in 2nd row */
			asm("NOP");
			colloc = (KEY_PIN & 0x0F);
			if(colloc != 0x0F)
			{
				rowloc = 1;
				break;
			}
			
			KEY_PRT = 0xBF;		/* check for pressed key in 3rd row */
			asm("NOP");
			colloc = (KEY_PIN & 0x0F);
			if(colloc != 0x0F)
			{
				rowloc = 2;
				break;
			}

			KEY_PRT = 0x7F;		/* check for pressed key in 4th row */
			asm("NOP");
			colloc = (KEY_PIN & 0x0F);
			if(colloc != 0x0F)
			{
				rowloc = 3;
				break;
			}
		}

		if(colloc == 0x0E)
		return(keypad[rowloc][0]);
		else if(colloc == 0x0D)
		return(keypad[rowloc][1]);
		else if(colloc == 0x0B)
		return(keypad[rowloc][2]);
		else
		return(keypad[rowloc][3]);
	}
	
	void showIntro() {
		lcdGoToxy(1,1);
		lcdPrint(">> MMMokhtari <<");
		lcdGoToxy(1,2);
		lcdPrint(">>My OTP Token<<");
	}

	void showLoading() {
		char i=0,j=0;
		lcdCommand(0x01);
		_delay_ms(2);
		lcdGoToxy(17,1);
		lcdPrint("<<<<<");
		lcdGoToxy(17,2);
		lcdPrint("<<<<<");
		for(i=0 ; i<=2 ; i++)
		{
			for(j=0 ; j<=20 ; j++)
			{
				lcdCommand(0x18);
				_delay_ms(25);
			}
			lcdCommand(0x02);
			_delay_ms(25);
		}
	}

	void showMenu() {
		state=1;
		initialTimer();
		lcdCommand(0x01);
		_delay_ms(2);
		lcdGoToxy(1,1);
		lcdPrint(menus[menuIndex]);
		lcdGoToxy(1,2);
		lcdPrint("NEXT: +");
	}

	void showGenerateMenu() {
		state=2;
		initialTimer();
		lcdCommand(0x01);
		_delay_ms(2);
		lcdGoToxy(1,1);
		lcdPrint("PASSWORD:(*)");
		lcdGoToxy(1,2);
	}

	void handleSubmitGenerateMenu(){
		initialTimer();
		lcdCommand(0x01);
		_delay_ms(2);
		lcdGoToxy(1,1);
		
		if(strcmp(currentPassword, enterdPassword) == 0)
		{
			state=3;
			lcdPrint("YOUR CODE IS:");
			lcdGoToxy(1,2);
			//Generate Temp Pass
			int size =9;
			char str[size];
			const char charset[] = "0123456789";
			if (size) {
				--size;
				for (int n = 0; n < size; n++) {
					int key = rand() % (int) (sizeof charset - 1);
					str[n] = charset[key];
				}
				str[size] = '\0';
			}
			lcdPrint(str);
			memset(enterdPassword, 0, sizeof(enterdPassword));
			enterdPasswordIndex=0;
			_delay_ms(4000);
			showMenu();
		}
		else
		{
			lcdPrint("WRONG PASSWORD!");
			memset(enterdPassword, 0, sizeof(enterdPassword));
			enterdPasswordIndex=0;
			_delay_ms(1000);
			showMenu();
		}
	}

	void showChangePasswordMenu(){
		initialTimer();
		state=4;
		lcdCommand(0x01);
		_delay_ms(2);
		lcdGoToxy(1,1);
		lcdPrint("PASSWORD:(*)");
		lcdGoToxy(1,2);
	}

	void handleSubmitChangePasswordMenu(){
		initialTimer();
		lcdCommand(0x01);
		_delay_ms(2);
		lcdGoToxy(1,1);
		
		if(strcmp(currentPassword, enterdPassword) == 0)
		{
			showNewChangePasswordMenu();
		}
		else
		{
			lcdPrint("WRONG PASSWORD!");
			memset(enterdPassword, 0, sizeof(enterdPassword));
			enterdPasswordIndex=0;
			_delay_ms(1000);
			showMenu();
		}
	}

	void showNewChangePasswordMenu(){
		initialTimer();
		state=5;
		lcdCommand(0x01);
		_delay_ms(2);
		lcdGoToxy(1,1);
		lcdPrint("NEW PASSWORD:(*)");
		lcdGoToxy(1,2);
	}

	void handleSubmitNewChangePasswordMenu(){
		initialTimer();
		char i=0;
		lcdCommand(0x01);
		_delay_ms(2);
		lcdGoToxy(1,1);
		lcdPrint("PASSWORD CHANGED!");
		memset(currentPassword, 0, sizeof(currentPassword));
		for (i =0 ;i<=9;i++)
		{
			currentPassword[i]=enterdNewPassword[i];
		}
		memset(enterdPassword, 0, sizeof(enterdPassword));
		enterdPasswordIndex=0;
		memset(enterdNewPassword, 0, sizeof(enterdNewPassword));
		enterdNewPasswordIndex=0;
		_delay_ms(1000);
		showMenu();
	}

	void showDisableCardMenu(){
		initialTimer();
		state=6;
		lcdCommand(0x01);
		_delay_ms(2);
		lcdGoToxy(1,1);
		lcdPrint("SURE?: [Y:1/N:2]");
		lcdGoToxy(1,2);
	}

	void displayOn() {
		state=1;
		lcdCommand(0x0C);
		initialTimer();
	}

	void displayOff() {
		state=7;
		lcdCommand(0x08);
	}

	void initialTimer(){
		TCNT1=0;
		OCR1A=10000;
		TCCR1A=0x00;
		TCCR1B=0x0D;
		TIMSK=(1<<OCIE1A);
		sei();
	}

	ISR (TIMER1_COMPA_vect) {
		displayOff();
	}
