/*
 * GccApplication1.c
 *
 * Created: 10/20/2020 7:20:22 PM
 * Author : User
 */ 
#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <string.h>
#include <util/atomic.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <HX711.h>
#include <i2cmaster.h>
#include <pcf8574.h>
#include <lcdpcf8574.h>
#include <util/atomic.h>
#include <avr/eeprom.h>
#include <rtc.h>
#include <delay.h>
#include <timers.h>
#include <USART.h>
#include <avr/wdt.h>

#define Trigger_pin PA0 /*trigger pin for ultrasonic sensor*/
#define Echo_pin PA1 /*Echo pin for ultrasonic sensor*/
#define joystick_Y PA2// to take input for display y axis 
#define joystick_ok PA3// to take input for OK by using joystick
#define solonid_in PD4// water in
#define solonid_out PD7// water out
#define servo_pin PD2/*pin for servo motor*/
#define washing_motor PD3//DC gear motor
//#define PD_SCK_PIN  PD5 //for weight cell  **this is define in HX711.H***
//#define DOUT_PIN   PD6 //for weight cell   **this is define in HX711.H***
#define wash_lim_down PB0
#define wash_lim_up PB1
#define dc_motor1 PC3
#define dc_motor1_direc PC2
#define dc_motor2 PC5
#define dc_motor2_direc PC4
#define cooker_lim_down PB2
#define cooker_lim_up PB3
#define cooker_set_lim PB4
#define finished_pin PB5
#define ricecooker_on PC7
#define buzzer_pin  PC6
#define CTC_MATCH_OVERFLOW ((F_CPU / 1000) / 8) 
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)


float ultrasonic_distance(void);/*take the distance in cm*/
void take_rice_to_wash();/*take rice to washing unit*/
void wash_rice();// to wash rice;
void put_water1();
void put_water2();
void get_water2();
void get_water1();
void close_cooker();
void clean(void);// clean the washing unit
unsigned long micross ();
void ADC_Init();
int ADC_Read(char channel);
int cheak_input();
int manual_dis_input();
int ultra_ok();
void after_finished();
void open();
void set_time();
void On_cooker();
int isfinished();
 int cheak_sms();
 int get_gsm_input();
 void read_eeprom();



float distance;
int one_per_weight=170;//grams of rice for a one person            
int num_members=1;//number of person which the machine should cook           /////e2
int full_rice_weight=0;//one_per_weight*num_members                          ////e3
float def_weight=0;/* weight without rice*/
int clean_water=1500;//water for clean(ml)
int manual_mode;                                                                 ///e4
int quick_mode;/////////////////////                                          e5
int ADC_VAL;
volatile unsigned long timer1_micross;
int max_member=11;
int hours=0;                                                              //e6
int minits=0;                                                          //e7
char income_msg[200];
char phone_num[13];
int level=0;//////////ep1
//int last_time=0;
//int first_time=0;
rtc_t rtc;
int ok_state =0;
int arrow = 1;
int Dis_Refresh = 1;
long duration;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ISR (TIMER1_COMPA_vect)
{
	timer1_micross++;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(void)
{
      /* Replace with your application code */
      DDRA=0B00000001;
      DDRB=0B00000000;
      DDRC=0B11111111;
      DDRD=0B11111111;
     // UART_Init(9600);
     // UART_SetBaudRate(9600);
	
      
      // CTC mode, Clock/8
      TCCR1B |= (1 << WGM12) | (1 << CS11);
      
      // Load the high byte, then the low byte
      // into the output compare
      OCR1AH = (CTC_MATCH_OVERFLOW >> 8);
      OCR1AL = CTC_MATCH_OVERFLOW;
      
      // Enable the compare match interrupt
      TIMSK |= (1 << OCIE1A);
      
      //TIMER_Init(1,1);

      // Now enable global interrupts
      sei();
	  cli();
	  
      
      lcd_init(LCD_DISP_ON);
      lcd_home();
      lcd_led(1);
      lcd_gotoxy(0, 0);
      lcd_puts("****************");
      lcd_gotoxy(0, 1);
      lcd_puts("*   WELCOME!!  *");
      lcd_gotoxy(0, 2);
      lcd_puts("*   Waiting... *");
      lcd_gotoxy(0, 3);
      lcd_puts("****************");
	  
	   while (ok_state==0){
		   if((PINA &(1<<joystick_ok))==0){
			   ok_state=1;
			   _delay_ms(500);
			   }else{
			   ok_state=0;
		   }
	   }

      
      while (1)
      {
		  if((PINA &(1<<joystick_ok))==0){
			  ok_state=1;
			  _delay_ms(500);
		  }
		  else{
			  ok_state=0;
		  }
	     
	       if (Dis_Refresh==1)
  {
  lcd_gotoxy(0, 0);
  lcd_puts("****************");
  lcd_gotoxy(0, 1);
  lcd_puts("  Ultrasonic    ");
  lcd_gotoxy(-4, 2);
  lcd_puts("  send SMS      ");
  lcd_gotoxy(-4, 3);
  lcd_puts("  read SMS      ");

    
  }
  
  
  if (ADC_Read(2) > 900){
    arrow--;
    Dis_Refresh = 1;
  }
  if (ADC_Read(2) < 200)
  {
    arrow++;
    Dis_Refresh = 1;
  }
  
  if (arrow>3)
  {
     arrow=1;
  }
  if (arrow<1)
  {
     arrow=3;
  }

  //////////////////////////////// Move Arrow///////////////////////////
  if (Dis_Refresh==1){
  if (arrow == 1)
  {
    lcd_gotoxy(0, 1);
    lcd_puts(">");
    lcd_gotoxy(-4, 2);
    lcd_puts(" ");
    lcd_gotoxy(-4, 3);
    lcd_puts(" ");
    Dis_Refresh = 0;
    _delay_ms(500);
  }
  if (arrow == 2)
  {
    lcd_gotoxy(0, 1);
    lcd_puts(" ");
    lcd_gotoxy(-4, 2);
    lcd_puts(">");
    lcd_gotoxy(-4, 3);
    lcd_puts(" ");
    Dis_Refresh = 0;
    _delay_ms(500);
  }
  if (arrow == 3)
  {
    lcd_gotoxy(0, 1);
    lcd_puts(" ");
    lcd_gotoxy(-4, 2);
    lcd_puts(" ");
    lcd_gotoxy(-4, 3);
    lcd_puts(">");
    Dis_Refresh = 0;
    _delay_ms(500);
  }
  }
  /////////////////////////////////////////////////////////////
    
  if(ok_state==1)
  {
      if (arrow==1){
                   distance= ultrasonic_distance();
                   //distance=7;
      
                   
                   lcd_home();
                   lcd_gotoxy(0, 0);
                   lcd_puts("****************");
                   lcd_gotoxy(0, 1);
                   lcd_puts(" distance :   ");
                   lcd_gotoxy(-4, 2);
                   lcd_data(distance);
                   lcd_gotoxy(4, 2);
                   lcd_puts("cm");
                   lcd_gotoxy(-4, 3);
                   lcd_puts("****************");
                   _delay_ms(5000);
                   
                /*   lcd.setCursor(-4, 2);
                   lcd.print(19);
                   delay(1000);
                   lcd.setCursor(-4, 2);
                   lcd.print(51);
                   delay(1000);
                   lcd.setCursor(-4, 2);
                   lcd.print(5);
                   delay(5000);*/
      }
      if (arrow==2){
                   lcd_gotoxy(0, 0);
                   lcd_puts("****************");
                   lcd_gotoxy(0, 1);
                   lcd_puts("  sendding    ");
                   lcd_gotoxy(-4, 2);
                   lcd_puts("  ........     "); 
                   lcd_gotoxy(-4, 3);
                   lcd_puts("****************");
           
                   usart_string_transmit("AT+CMGF=1");
                   //usart_string_transmit("AT+CMGS=<number>13Conformed26");
                   usart_string_transmit("AT+CMGS=");
                   usart_string_transmit(phone_num);
                   usart_string_transmit("13Conformed26");
                    
                               
                   lcd_gotoxy(0, 1);
                   lcd_puts("  sent SMS     ");
                                
                   //send_SMS_ok=0;
                   _delay_ms(1000);
      }
      if (arrow==3){
                   lcd_gotoxy(0, 0);
                   lcd_puts("****************");
                   lcd_gotoxy(0, 1);
                   lcd_puts("  readding    ");
                   lcd_gotoxy(-4, 2);
                   lcd_puts("  ........     "); 
                   lcd_gotoxy(-4, 3);
                   lcd_puts("****************");
           
                   usart_string_transmit("AT+CMGF=1");
                   usart_string_transmit("AT+CMGL='REC UNREAD'");///////////////////////////////////////////////////////////
                   _delay_ms(5000);
      }
      
      ok_state=0;
      Dis_Refresh = 1;
    }
}
	   }



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


float ultrasonic_distance(void)
{
	lcd_gotoxy(0, 0);
	lcd_puts("****************");
	lcd_gotoxy(0, 1);
	lcd_puts("*<<<checking>>> *");
	lcd_gotoxy(0, 2);
	lcd_puts("*  rice stock  *");
	lcd_gotoxy(0, 3);
	lcd_puts("****************");
	unsigned long time=0;/*spend time for come back the wave(in ms)*/
	 unsigned long first_time=0;
	unsigned long last_time=0;
	PORTA|=(1<<Trigger_pin);/*to emit a wave from trigger pin */
	//first_time=TIMER_GetTime(1);
	//TIMER_Start(1);
	timer1_micross=0;
	_delay_us(10);
	PORTA &=(~(1<<Trigger_pin));/*to stop the emitting the wave from trigger pin */
	
	while(1)
	{
		if((PINA &(1<<Echo_pin))==1)
		{
         	time = micross();
			 //TIMER_Stop(1);
			 //last_time=TIMER_GetTime(1);
			break;
		}
		
	}
	
	/*if (last_time<first_time)
	{
		time=(65535-first_time)+last_time+1;
	}
	else
	{
		time=last_time-first_time;
	}*/
	
	float distance;
	//distance=(float)343*(10^-4)*time/2;
	distance=(float)343*(10^-7)*time/2;
	return distance;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


unsigned long micross ()
{
	unsigned long millis_return;

	// Ensure this cannot be disrupted
	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		millis_return = timer1_micross;
	}
	
	return millis_return;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////







/*void read_eeprom()
{
		
		level=eeprom_read_byte (0); 
		num_members=eeprom_read_byte (1); 
		manual_mode=eeprom_read_byte (2); 
		quick_mode=eeprom_read_byte (3); 
		hours=eeprom_read_byte (4); 
		minits=eeprom_read_byte (5); 
		full_rice_weight=eeprom_read_float (20);
		def_weight=eeprom_read_float (100);
		phone_num[13]=eeprom_read_dword(200);
		
}*/








int cheak_input()
{
	int have_sms=0;
	while(1)
	{
		if((PINA & (1<<joystick_ok))==1)
		{
			manual_mode=1;
			break;
		}
		
		have_sms = cheak_sms();////cheak whether there is msg
		if (have_sms==1)
		{
			break;
		}
		
	  
	}
	int  rice_enough=0;
	int ready=0;
	if(manual_mode==1)
	{
		ready=manual_dis_input();
		if (ready==1)
		{
			lcd_gotoxy(0, 0);
			lcd_puts("****************");
			lcd_gotoxy(0, 1);
			lcd_puts("     wait!!!    ");
			lcd_gotoxy(0, 2);
			lcd_puts("    .......     ");
			lcd_gotoxy(0, 3);
			lcd_puts("****************");
			rice_enough=ultra_ok();
			if (rice_enough==1)
			{
				return 1;
			}
			else if (rice_enough==0)
			{
				lcd_gotoxy(0, 0);
				lcd_puts("****************");
				lcd_gotoxy(0, 1);
				lcd_puts("     SORRY!!  ");
				lcd_gotoxy(0, 2);
				lcd_puts("not enough rice ");
				lcd_gotoxy(0, 3);
				lcd_puts("****************");
			
				_delay_ms(3000);
				return 0;
			}
			
		}
		else if(ready==0)
		{
			return 0;
		}
	}
	else if(manual_mode==0)
	{
		ready=get_gsm_input();
		if (ready==1)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	
}



 int cheak_sms()
 {
	 usart_string_transmit("AT+CMGF=1");
	usart_string_transmit("AT+CMGL='REC UNREAD'"); ///+CMGL: 1,"REC UNREAD","+85291234567",,"07/02/18,00:05:10+32"
	unsigned char c;
	int buf_size=0;
	while (c!='\0')
	{
		c=usart_data_receive();
		income_msg[buf_size]=c;
		buf_size++;
	}
	
	_delay_ms(2000);
	
	
	 if (income_msg[38]=='C'&& income_msg[40]=='M')
	 {
		 int i=0;
		 for (i=0;i<12;i++)
		 {
			 phone_num[i]=income_msg[23+i];
		 }
		 return 1;
	 }
	 else
	 {
		 usart_string_transmit("AT+CMGD=013");//to delete msg
		 return 0;
	 }
	 
 }


int get_gsm_input()
{
	if (income_msg[39]=='Q')
	{
		quick_mode=1;
	}
	if (income_msg[39]=='T')
	{
		quick_mode=0;
	}
	int ten_mark=0;
	int assval0=48;
	int i=0;
	for (i=0;i<10;i++)
	{
		if (assval0==income_msg[41])
		{
			ten_mark=i;
			assval0=48;
			break;
		}
		assval0++;
	}
	
	int one_mark=0;
	for (i=0;i<10;i++)
	{
		if (assval0==income_msg[42])
		{
			one_mark=i;
			assval0=48;
			break;
		}
		assval0++;
	}
	num_members=(ten_mark*10)+one_mark;
	full_rice_weight=one_per_weight* num_members;/*full weight of rice which machine should cook according to user input*/
	if (quick_mode==0)
	{
		for (i=0;i<10;i++)
		{
			if (assval0==income_msg[44])
			{
				ten_mark=i;
				assval0=48;
				break;
			}
			assval0++;
		}
		for (i=0;i<10;i++)
		{
			if (assval0==income_msg[45])
			{
				one_mark=i;
				assval0=48;
				break;
			}
			assval0++;
		}
		hours=(ten_mark*10)+one_mark;
		for (i=0;i<10;i++)
		{
			if (assval0==income_msg[47])
			{
				ten_mark=i;
				assval0=48;
				break;
			}
			assval0++;
		}
		for (i=0;i<10;i++)
		{
			if (assval0==income_msg[48])
			{
				one_mark=i;
				assval0=48;
				break;
			}
			assval0++;
		}
		minits=(ten_mark*10)+one_mark;
		
	}
	int enough_rice=0;
	if (quick_mode==1||quick_mode==0)
	{
		if (num_members>1 && num_members<max_member &&  hours>=0 && hours<=24 && minits>=0 && minits<=59)
		{
			enough_rice=ultra_ok();
			if (enough_rice==1)
			{
				usart_string_transmit("AT+CMGF=1");
				//usart_string_transmit("AT+CMGS=<number>13Conformed26");
				usart_string_transmit("AT+CMGS=");
				usart_string_transmit(phone_num);
				usart_string_transmit("13Conformed26");
				return 1;
			}
			if (enough_rice==0)
			{
				////send msg not enough rice
				usart_string_transmit("AT+CMGF=1");
				//UART_TxString("AT+CMGS=<number>13Not enough rice26");
				usart_string_transmit("AT+CMGS=");
				usart_string_transmit(phone_num);
				usart_string_transmit("13Not enough rice26");
				return 0;
			}
		}
	}
	else
	{
		usart_string_transmit("AT+CMGF=1");
		//usart_string_transmit("AT+CMGS=<number>13Error input26");
		usart_string_transmit("AT+CMGS=");
		usart_string_transmit(phone_num);
		usart_string_transmit("13Error input26");
		return 0;
	}
}



void put_water2()
{
	
	    PORTC &=(~(1<<dc_motor1_direc));
	
		PORTC|=(1<<dc_motor1);//motor on
		while(1)
		{
			if((PINB &(1<<wash_lim_up))==1)
			{
				PORTC &=(~(1<<dc_motor1));//motor off
				_delay_ms(20000);
				break;
			}
		}
		PORTC &=(~(1<<dc_motor1));//motor off
        _delay_ms(250);
		PORTC|=(1<<dc_motor1_direc);//change the direction of motor
		 _delay_ms(250);
		PORTC|=(1<<dc_motor1);
		  while(1)
		  {
			  if((PINB &(1<<wash_lim_down))==1)
			  {
				  PORTC &=(~(1<<dc_motor1));//motor off
				  PORTC &=(~(1<<dc_motor1_direc));//change the direction of motor
				  break;
			  }
		 }
	   
		
	}
	
	
	void get_water2()
	{
		PORTD|=(1<<solonid_out);//Open the water out
		_delay_ms(30000);
		PORTD &=(~(1<<solonid_out));//close the water out
	    //for empty washing unit	
	
		take_water2:
		if((PINB &(1<<wash_lim_down))==1)
		{
			float cheack2=0;//value of weight before put water
			float mark=0;//value of weight after put water
			float real_water=0;
			real_water=full_rice_weight/2;
			mark=HX711_get_scale();
			PORTD|=(1<<solonid_in);//open the water in
			while(1)
			{
				cheack2=HX711_get_scale();
				//read weight cell
				if(cheack2>=mark+real_water)
				{
					PORTD &=(~(1<<solonid_in));//close the water in
					break;
				}
				
			}
		}
		else
		{
				PORTC|=(1<<dc_motor1_direc);//change the direction of motor
				PORTC|=(1<<dc_motor1);
				while(1)
				{
					if((PINB &(1<<wash_lim_down))==1)
					{
						PORTC &=(~(1<<dc_motor1));//motor off
						PORTC &=(~(1<<dc_motor1_direc));//change the direction of motor
						break;
					}
				}
				goto take_water2;
		}
			
	}







void close_cooker()
{
	float ready=0;
	
	ready=HX711_get_scale();//read load cell to check finish the putting rice to the rice cooker
	if(ready<def_weight+20)
	{
		PORTC|=(1<<dc_motor2);
		while(1)
		{
			if((PINB &(1<<cooker_lim_down))==1)
			{
				PORTC &=(~(1<<dc_motor2));
				break;
			}
		}
	}
	
}




void On_cooker()
{
	PORTC|=(1<<ricecooker_on);
	lcd_gotoxy(0, 0);
	lcd_puts("****************");
	lcd_gotoxy(0, 1);
	lcd_puts("*<<<cooking>>> *");
	lcd_gotoxy(0, 2);
	lcd_puts("*   .........  *");
	lcd_gotoxy(0, 3);
	lcd_puts("****************");
}











int isfinished()
{
    On_cooker();
	while (1)
	{
		if((PINB & (1<<finished_pin))==1)
		{
			break;
		}
	}
	return 1;
}








int manual_dis_input()
{
	int quik_mark=1;
	while(1)
	{
		ADC_VAL=ADC_Read(2);
		if(ADC_VAL>=300)
		{
			quik_mark++;
		}
		else if(ADC_VAL<=700)
		{
			quik_mark--;
		}
		if(quik_mark<1)
		{
			quik_mark=1;
		}
		quick_mode=quik_mark%2;
		if(quick_mode==1)
		{
				lcd_gotoxy(0, 1);
				lcd_puts("* >Quick mode  *");
				lcd_gotoxy(0, 2);
				lcd_puts("*  Time mode   *");
		} 
		else if(quick_mode==0)
		{
			lcd_gotoxy(0, 1);
			lcd_puts("*  Quick mode  *");
			lcd_gotoxy(0, 2);
			lcd_puts("* >Time mode   *");
		}
		if((PINA &(1<<joystick_ok))==1)
		{
			break;
		}
		
	}
	////select the peoples**** num_members
	while(1)
	{
		ADC_VAL=ADC_Read(2);
			if(ADC_VAL>=300)
			{
				num_members++;
			}
			else if(ADC_VAL>=700)
			{
				num_members--;
			}
			if (num_members<1)
			{
				num_members=max_member;
			}
			if (num_members>max_member)
			{
				num_members=1;
			}
			switch(num_members)
			{
				case(1):
				{
					lcd_gotoxy(0, 0);
					lcd_puts("select members");
					lcd_gotoxy(0, 1);
					lcd_puts("  >1");
					lcd_gotoxy(0, 2);
					lcd_puts("   2");
					lcd_gotoxy(0, 3);
					lcd_puts("   3");
					break;
				}
				case(2):
				{
					lcd_gotoxy(0, 0);
					lcd_puts("   1");
					lcd_gotoxy(0, 1);
					lcd_puts("  >2");
					lcd_gotoxy(0, 2);
					lcd_puts("   3");
					lcd_gotoxy(0, 3);
					lcd_puts("   4");
					break;
				}
				case(3):
				{
					lcd_gotoxy(0, 0);
					lcd_puts("   1");
					lcd_gotoxy(0, 1);
					lcd_puts("   2");
					lcd_gotoxy(0, 2);
					lcd_puts("  >3");
					lcd_gotoxy(0, 3);
					lcd_puts("   4");
					break;
				}
				case(4):
				{
					lcd_gotoxy(0, 0);
					lcd_puts("   1");
					lcd_gotoxy(0, 1);
					lcd_puts("   2");
					lcd_gotoxy(0, 2);
					lcd_puts("   3");
					lcd_gotoxy(0, 3);
					lcd_puts("  >4");
					break;
				}
				case(5):
				{
					lcd_gotoxy(0, 0);
					lcd_puts("  >5");
					lcd_gotoxy(0, 1);
					lcd_puts("   6");
					lcd_gotoxy(0, 2);
					lcd_puts("   7");
					lcd_gotoxy(0, 3);
					lcd_puts("   8");
					break;
				}
				case(6):
				{
					lcd_gotoxy(0, 0);
					lcd_puts("   5");
					lcd_gotoxy(0, 1);
					lcd_puts("  >6");
					lcd_gotoxy(0, 2);
					lcd_puts("   7");
					lcd_gotoxy(0, 3);
					lcd_puts("   8");
					break;
				}
				case(7):
				{
					lcd_gotoxy(0, 0);
					lcd_puts("   5");
					lcd_gotoxy(0, 1);
					lcd_puts("   6");
					lcd_gotoxy(0, 2);
					lcd_puts("  >7");
					lcd_gotoxy(0, 3);
					lcd_puts("   8");
					break;
				}
				case(8):
				{
					lcd_gotoxy(0, 0);
					lcd_puts("   5");
					lcd_gotoxy(0, 1);
					lcd_puts("   6");
					lcd_gotoxy(0, 2);
					lcd_puts("   7");
					lcd_gotoxy(0, 3);
					lcd_puts("  >8");
					break;
				}
				case(9):
				{
					lcd_gotoxy(0, 0);
					lcd_puts("  >9");
					lcd_gotoxy(0, 1);
					lcd_puts("   10");
					lcd_gotoxy(0, 2);
					lcd_puts("   11");
					lcd_gotoxy(0, 3);
					lcd_puts("****************");
					break;
				}
				case(10):
				{
					lcd_gotoxy(0, 0);
					lcd_puts("   9");
					lcd_gotoxy(0, 1);
					lcd_puts("  >10");
					lcd_gotoxy(0, 2);
					lcd_puts("   11");
					lcd_gotoxy(0, 3);
					lcd_puts("****************");
					break;
				}
				case(11):
				{
					lcd_gotoxy(0, 0);
					lcd_puts("   9");
					lcd_gotoxy(0, 1);
					lcd_puts("   10");
					lcd_gotoxy(0, 2);
					lcd_puts("  >11");
					lcd_gotoxy(0, 3);
					lcd_puts("****************");
					break;
				}
			}
			if((PINA &(1<<joystick_ok))==1)
			{
				break;
			}
	}
	full_rice_weight=one_per_weight* num_members;/*full weight of rice which machine should cook according to user input
	//////if time mode take the time as a input*/
	if (quick_mode==0)
	{
	  set_time();
	}
	int ok=1;
	int ok_mark=1;
	   while(1)
	   {
		    ADC_VAL=ADC_Read(2);
		    if(ADC_VAL>=300)
		    {
			    ok_mark++;
		    }
		    else if(ADC_VAL<=700)
		    {
		     	ok_mark--;
		    }
		    if(ok_mark<1)
		    {
		    	ok_mark=1;
		    }
		    ok=ok_mark%2;
		    if(ok==1)
		    {
			    lcd_gotoxy(0, 0);
			    lcd_puts("****************");
			    lcd_gotoxy(0, 1);
			    lcd_puts(">conform details");
			    lcd_gotoxy(0, 2);
			    lcd_puts(" cancel");
			    lcd_gotoxy(0, 3);
			   lcd_puts("****************");
		    }
		    else if(ok==0)
		    {
			    lcd_gotoxy(0, 0);
			    lcd_puts("****************");
			    lcd_gotoxy(0, 1);
			    lcd_puts(" conform details");
			    lcd_gotoxy(0, 2);
			    lcd_puts(">cancel");
			    lcd_gotoxy(0, 3);
			    lcd_puts("****************");
		    }
		    if((PINA &(1<<joystick_ok))==1)
		    {
			     break;
		    }
		
	    }
	if(ok==1)
	{
		
		lcd_gotoxy(0, 0);
		lcd_puts("****************");
		lcd_gotoxy(0, 1);
		lcd_puts("    waiting     ");
		lcd_gotoxy(0, 2);
		lcd_puts("    .......     ");
		lcd_gotoxy(0, 3);
		lcd_puts("****************");
		return 1;
	}
	else if(ok==0)
	{
		lcd_gotoxy(0, 0);
		lcd_puts("****************");
		lcd_gotoxy(0, 1);
		lcd_puts(" please reenter ");
		lcd_gotoxy(0, 2);
		lcd_puts("    .......     ");
		lcd_gotoxy(0, 3);
		lcd_puts("****************");
		return 0;
	}
}









void take_rice_to_wash()
{
	lcd_gotoxy(0, 0);
	lcd_puts("****************");
	lcd_gotoxy(0, 1);
	lcd_puts("*  Take rice   *");
	lcd_gotoxy(0, 2);
	lcd_puts("*   .........  *");
	lcd_gotoxy(0, 3);
	lcd_puts("****************");
	full_rice_weight=one_per_weight* num_members;/*full weight of rice which machine should cook according to user input*/
	float all_weight=0;/* weight with rice*/
	def_weight=HX711_get_scale();
	/*rotate motor to 180 degree*/
	PORTC|=(1<<servo_pin);
	_delay_us(2000);
	PORTC &=(~(1<<servo_pin));
	_delay_ms(2000);
	while(1)
	{
		all_weight=HX711_get_scale();
		if(full_rice_weight<=all_weight-def_weight)
		{
			/*rotate the motor to 0 degree*/
			PORTC|=(1<<servo_pin);
	        _delay_us(1000);
	        PORTC &=(~(1<<servo_pin));
	        _delay_ms(2000);
			break;
		}
	}
	
	
} 

void wash_rice()
{
	lcd_gotoxy(0, 0);
	lcd_puts("****************");
	lcd_gotoxy(0, 1);
	lcd_puts("*<<<Washing>>> *");
	lcd_gotoxy(0, 2);
	lcd_puts("*   .........  *");
	lcd_gotoxy(0, 3);
	lcd_puts("****************");
	
	PORTD|=(1<<solonid_out);//Open the water out
	_delay_ms(30000);
	PORTD &=(~(1<<solonid_out));//close the water out
	//for empty washing unit
	
	float mark=0;
	mark=HX711_get_scale();//read weight cell and take this as a default value
	float full =0;
	float water=0;
	int i;
	for(i=0;i<3;i++)
	{
		PORTD|=(1<<solonid_in);
		while(1)
		{
			full=HX711_get_scale();//read weight cell
			water=full-mark;
			if(water>=full_rice_weight) // take enough  water to wash
			{
				PORTD &=(~(1<<solonid_in));//close the water in
				break;
			}
			
		}
		PORTC|=(1<<washing_motor);
		_delay_ms(30000);
		PORTC &=(~(1<<washing_motor));
		PORTD|=(1<<solonid_out);//Open the water out
		_delay_ms(30000);
		PORTD &=(~(1<<solonid_out));//close the water out
	}
	
	
}




void clean(void)
{
	lcd_gotoxy(0, 0);
	lcd_puts("****************");
	lcd_gotoxy(0, 1);
	lcd_puts("*<<<cooking>>> *");
	lcd_gotoxy(0, 2);
	lcd_puts("*cleaning..... *");
	lcd_gotoxy(0, 3);
	lcd_puts("****************");
	PORTD|=(1<<solonid_out);//Open the water out
	_delay_ms(30000);
	PORTD &=(~(1<<solonid_out));//close the water out
	int i;
	for(i=0;i<4;i++)
	{
		float cheak=0;
		cheak=HX711_get_scale();
		float test=clean_water+cheak;
		PORTD|=(1<<solonid_in);//open the water in
		float take_water =0;
		while(1)
		{
			take_water=HX711_get_scale();
			//read weight cell
			if(take_water>=test)
			{
				PORTD &=(~(1<<solonid_in));//close the water in
				break;
			}
			
		}
		PORTC|=(1<<washing_motor);//on the motor of washing unit
		_delay_ms(30000);
		PORTC &=(~(1<<washing_motor));//of the motor of washing unit
		PORTD|=(1<<solonid_out);//Open the water out
		_delay_ms(30000);
		PORTD &=(~(1<<solonid_out));//close the water out
		
	}
	
}



void put_water1()
{
	
		PORTC|=(1<<dc_motor1);//motor on
		while(1)
		{
			if((PINB &(1<<wash_lim_up))==1)
			{
				PORTC &=(~(1<<dc_motor1));
				_delay_ms(20000);
				break;
			}
		}
		PORTC &=(~(1<<dc_motor1));//motor off
		 _delay_ms(250);
		PORTC|=(1<<dc_motor1_direc);//change the direction of motor
		 _delay_ms(250);
		PORTC|=(1<<dc_motor1);//motor on
		while(1)
		{
			if((PINB &(1<<wash_lim_down))==1)
			{
				PORTC &=(~(1<<dc_motor1));//motor off
				PORTC &=(~(1<<dc_motor1_direc));//change the direction of motor
				break;
			}
		}
		_delay_ms(1000);
	
	
}




void get_water1()
{
	PORTD|=(1<<solonid_out);//Open the water out
	_delay_ms(30000);
	PORTD &=(~(1<<solonid_out));//close the water out
	//for empty washing unit
	take_water1:
	if((PINB &(1<<wash_lim_down))==1)
	{
		PORTD|=(1<<solonid_in);
		float cheak1=0;
		float real_water=0;
		real_water=full_rice_weight/2;
		while(1)
		{
			cheak1=HX711_get_scale();
			//read weight cell
			if(cheak1>=full_rice_weight+real_water)
			{
				PORTD &=(~(1<<solonid_in));
				break;
			}
			
		}
      }
      else
      {
	      PORTC|=(1<<dc_motor1_direc);//change the direction of motor
	      PORTC|=(1<<dc_motor1);
	      while(1)
	      {
		      if((PINB &(1<<wash_lim_down))==1)
		      {
			      PORTC &=(~(1<<dc_motor1));//motor off
			      PORTC &=(~(1<<dc_motor1_direc));//change the direction of motor
			      break;
		      }
	      }
	      goto take_water1;
      }
	
}










void ADC_Init()
{
	//DDRA=0x0;			// Make ADC port as input 
	ADCSRA = 0x87;			// Enable ADC, fr/128  
	ADMUX = 0x40;			//Vref: Avcc, ADC channel: 0 
	
}



int ADC_Read(char channel)
{
	int Ain,AinLow;
	
	ADMUX=ADMUX|(channel & 0x0f);	// Set input channel to read 

	ADCSRA |= (1<<ADSC);		// Start conversion 
	while((ADCSRA&(1<<ADIF))==0);	// Monitor end of conversion interrupt 
	
	_delay_us(10);
	AinLow = (int)ADCL;		//Read lower byte
	Ain = (int)ADCH*256;		/// Read higher 2 bits and 
					//Multiply with weight 
	Ain = Ain + AinLow;				
	return(Ain);			/// Return digital value
}  

int ultra_ok()
{
	float current_dis=0;
	current_dis=ultrasonic_distance();
	float full_dis=30;//distance of rice tank
	float ones_dis=7;//rice per one person
	float max_dis=0;
	max_dis=full_dis-(num_members*ones_dis);
	if (current_dis<=max_dis)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


void after_finished()
{
	lcd_gotoxy(0, 0);
	lcd_puts("****************");
	lcd_gotoxy(0, 1);
	lcd_puts("*<<<finished>>>*");
	lcd_gotoxy(0, 2);
	lcd_puts("* you can eat  *");
	lcd_gotoxy(0, 3);
	lcd_puts("****************");
	if(manual_mode==1)
	{
		PORTC|=(1<<buzzer_pin);//switch on buzzer
		_delay_ms(10000);
	    PORTC &=(~(1<<buzzer_pin));	//switch off buzzer
	}
	else if (manual_mode==0)
	{
		UART_TxString("AT+CMGF=1");
		//UART_TxString("AT+CMGS=<number>13Finished cook26");
		UART_TxString("AT+CMGS=");
		UART_TxString(phone_num);
		UART_TxString("13Finished cook26");
	}
}


void open()
{
	PORTC|=(1<<dc_motor2_direc);//change direction of motor
	PORTC|=(1<<dc_motor2);//switch on the motor
	while(1)
	{
		if(1<<cooker_lim_up)
		{
			PORTC &=(~(1<<dc_motor2));//switch off the motor
			break;
		}
		PORTC &=(~(1<<dc_motor2_direc));//change direction of motor
	}
	
}


void set_time()
{
	while(1)
	{
		ADC_VAL=ADC_Read(2);
		if(ADC_VAL>=300)
		{
			hours++;
		}
		else if(ADC_VAL>=700)
		{
			hours--;
		}
		if (hours<1)
		{
			hours=12;
		}
		if (hours>12)
		{
			hours=1;
		}
		switch(hours)
		{
			case 1:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(" set hours :");
				lcd_gotoxy(0, 1);
				lcd_puts(">01         ");
				lcd_gotoxy(0, 2);
				lcd_puts(" 02         ");
				lcd_gotoxy(0, 3);
				lcd_puts(" 03         ");
				break;
			}
			case 2:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(" set hours :");
				lcd_gotoxy(0, 1);
				lcd_puts(" 01         ");
				lcd_gotoxy(0, 2);
				lcd_puts(">02         ");
				lcd_gotoxy(0, 3);
				lcd_puts(" 03         ");
				break;
			}
			case 3:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(" set hours :");
				lcd_gotoxy(0, 1);
				lcd_puts(" 01         ");
				lcd_gotoxy(0, 2);
				lcd_puts(" 02         ");
				lcd_gotoxy(0, 3);
				lcd_puts(">03         ");
				break;
			}
			case 4:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(">04         ");
				lcd_gotoxy(0, 1);
				lcd_puts(" 05         ");
				lcd_gotoxy(0, 2);
				lcd_puts(" 06         ");
				lcd_gotoxy(0, 3);
				lcd_puts(" 07         ");
				break;
			}
			case 5:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(" 04         ");
				lcd_gotoxy(0, 1);
				lcd_puts(">05         ");
				lcd_gotoxy(0, 2);
				lcd_puts(" 06         ");
				lcd_gotoxy(0, 3);
				lcd_puts(" 07         ");
				break;
			}
			case 6:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(" 04         ");
				lcd_gotoxy(0, 1);
				lcd_puts(" 05         ");
				lcd_gotoxy(0, 2);
				lcd_puts(">06         ");
				lcd_gotoxy(0, 3);
				lcd_puts(" 07         ");
				break;
			}
			case 7:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(" 04         ");
				lcd_gotoxy(0, 1);
				lcd_puts(" 05         ");
				lcd_gotoxy(0, 2);
				lcd_puts(" 06         ");
				lcd_gotoxy(0, 3);
				lcd_puts(">07         ");
				break;
			}
			case 8:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(">08         ");
				lcd_gotoxy(0, 1);
				lcd_puts(" 09         ");
				lcd_gotoxy(0, 2);
				lcd_puts(" 10         ");
				lcd_gotoxy(0, 3);
				lcd_puts(" 11         ");
				break;
			}
			case 9:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(" 08         ");
				lcd_gotoxy(0, 1);
				lcd_puts(">09         ");
				lcd_gotoxy(0, 2);
				lcd_puts(" 10         ");
				lcd_gotoxy(0, 3);
				lcd_puts(" 11         ");
				break;
			}
			case 10:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(" 08         ");
				lcd_gotoxy(0, 1);
				lcd_puts(">09         ");
				lcd_gotoxy(0, 2);
				lcd_puts(" 10         ");
				lcd_gotoxy(0, 3);
				lcd_puts(" 11         ");
				break;
			}
			case 11:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(" 08         ");
				lcd_gotoxy(0, 1);
				lcd_puts(" 09         ");
				lcd_gotoxy(0, 2);
				lcd_puts(" 10         ");
				lcd_gotoxy(0, 3);
				lcd_puts(">11         ");
				break;
			}
			case 12:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(">12         ");
				lcd_gotoxy(0, 1);
				lcd_puts(" 01         ");
				lcd_gotoxy(0, 2);
				lcd_puts(" 02         ");
				lcd_gotoxy(0, 3);
				lcd_puts(" 03         ");
				break;
			}
		}
		if((PINA &(1<<joystick_ok))==1)
		{
			break;
		}
	}
	
	while(1)
	{
		ADC_VAL=ADC_Read(2);
		if(ADC_VAL>=300)
		{
			minits++;
		}
		else if(ADC_VAL>=700)
		{
			minits--;
		}
		if (minits<0)
		{
			minits=59;
		}
		if (minits>59)
		{
			minits=0;
		}
		switch(minits)
		{
			case 0:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(" set minits:");
				lcd_gotoxy(0, 1);
				lcd_puts(">00         ");
				lcd_gotoxy(0, 2);
				lcd_puts(" 01         ");
				lcd_gotoxy(0, 3);
				lcd_puts(" 02         ");
				break;
			}
			case 1:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(" set minits:");
				lcd_gotoxy(0, 1);
				lcd_puts(" 00         ");
				lcd_gotoxy(0, 2);
				lcd_puts(">01         ");
				lcd_gotoxy(0, 3);
				lcd_puts(" 02         ");
				break;
			}
			case 2:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(" 00");
				lcd_gotoxy(0, 1);
				lcd_puts(" 01         ");
				lcd_gotoxy(0, 2);
				lcd_puts(">02         ");
				lcd_gotoxy(0, 3);
				lcd_puts(" 03         ");
				break;
			}
			case 3:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(" 00:");
				lcd_gotoxy(0, 1);
				lcd_puts(" 01         ");
				lcd_gotoxy(0, 2);
				lcd_puts(" 02         ");
				lcd_gotoxy(0, 3);
				lcd_puts(">03         ");
				break;
			}
			case 4:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(">04         ");
				lcd_gotoxy(0, 1);
				lcd_puts(" 05         ");
				lcd_gotoxy(0, 2);
				lcd_puts(" 06         ");
				lcd_gotoxy(0, 3);
				lcd_puts(" 07         ");
				break;
			}
			case 5:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(" 04         ");
				lcd_gotoxy(0, 1);
				lcd_puts(">05         ");
				lcd_gotoxy(0, 2);
				lcd_puts(" 06         ");
				lcd_gotoxy(0, 3);
				lcd_puts(" 07         ");
				break;
			}
			case 6:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(" 04         ");
				lcd_gotoxy(0, 1);
				lcd_puts(" 05         ");
				lcd_gotoxy(0, 2);
				lcd_puts(">06         ");
				lcd_gotoxy(0, 3);
				lcd_puts(" 07         ");
				break;
			}
			case 7:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(" 04         ");
				lcd_gotoxy(0, 1);
				lcd_puts(" 05         ");
				lcd_gotoxy(0, 2);
				lcd_puts(" 06         ");
				lcd_gotoxy(0, 3);
				lcd_puts(">07         ");
				break;
			}
			case 8:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(">08         ");
				lcd_gotoxy(0, 1);
				lcd_puts(" 09         ");
				lcd_gotoxy(0, 2);
				lcd_puts(" 10         ");
				lcd_gotoxy(0, 3);
				lcd_puts(" 11         ");
				break;
			}
			case 9:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(" 08         ");
				lcd_gotoxy(0, 1);
				lcd_puts(">09         ");
				lcd_gotoxy(0, 2);
				lcd_puts(" 10         ");
				lcd_gotoxy(0, 3);
				lcd_puts(" 11         ");
				break;
			}
			case 10:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(" 08         ");
				lcd_gotoxy(0, 1);
				lcd_puts(">09         ");
				lcd_gotoxy(0, 2);
				lcd_puts(" 10         ");
				lcd_gotoxy(0, 3);
				lcd_puts(" 11         ");
				break;
			}
			case 11:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(" 08         ");
				lcd_gotoxy(0, 1);
				lcd_puts(" 09         ");
				lcd_gotoxy(0, 2);
				lcd_puts(" 10         ");
				lcd_gotoxy(0, 3);
				lcd_puts(">11         ");
				break;
			}
			case 12:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(">12         ");
				lcd_gotoxy(0, 1);
				lcd_puts(" 13         ");
				lcd_gotoxy(0, 2);
				lcd_puts(" 14         ");
				lcd_gotoxy(0, 3);
				lcd_puts(" 15         ");
				break;
			}
			case 13:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(" 12         ");
				lcd_gotoxy(0, 1);
				lcd_puts(">13         ");
				lcd_gotoxy(0, 2);
				lcd_puts(" 14         ");
				lcd_gotoxy(0, 3);
				lcd_puts(" 15         ");
				break;
			}
			case 14:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(" 12         ");
				lcd_gotoxy(0, 1);
				lcd_puts(" 13         ");
				lcd_gotoxy(0, 2);
				lcd_puts(">14         ");
				lcd_gotoxy(0, 3);
				lcd_puts(" 15         ");
				break;
			}
			case 15:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(" 12         ");
				lcd_gotoxy(0, 1);
				lcd_puts(" 13         ");
				lcd_gotoxy(0, 2);
				lcd_puts(" 14         ");
				lcd_gotoxy(0, 3);
				lcd_puts(">15         ");
				break;
			}
			case 16:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(">16         ");
				lcd_gotoxy(0, 1);
				lcd_puts(" 17         ");
				lcd_gotoxy(0, 2);
				lcd_puts(" 18         ");
				lcd_gotoxy(0, 3);
				lcd_puts(" 19         ");
				break;
			}
			case 17:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(" 16         ");
				lcd_gotoxy(0, 1);
				lcd_puts(">17         ");
				lcd_gotoxy(0, 2);
				lcd_puts(" 18         ");
				lcd_gotoxy(0, 3);
				lcd_puts(" 19         ");
				break;
			}
			case 18:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(" 16         ");
				lcd_gotoxy(0, 1);
				lcd_puts(" 17         ");
				lcd_gotoxy(0, 2);
				lcd_puts(">18         ");
				lcd_gotoxy(0, 3);
				lcd_puts(" 19         ");
				break;
			}
			case 19:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(" 16         ");
				lcd_gotoxy(0, 1);
				lcd_puts(" 17         ");
				lcd_gotoxy(0, 2);
				lcd_puts(" 18         ");
				lcd_gotoxy(0, 3);
				lcd_puts(">19         ");
				break;
			}
			case 20:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(">20         ");
				lcd_gotoxy(0, 1);
				lcd_puts(" 21         ");
				lcd_gotoxy(0, 2);
				lcd_puts(" 22         ");
				lcd_gotoxy(0, 3);
				lcd_puts(" 23         ");
				break;
			}
			case 21:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(" 20         ");
				lcd_gotoxy(0, 1);
				lcd_puts(">21         ");
				lcd_gotoxy(0, 2);
				lcd_puts(" 22         ");
				lcd_gotoxy(0, 3);
				lcd_puts(" 23         ");
				break;
			}
			case 22:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(" 20         ");
				lcd_gotoxy(0, 1);
				lcd_puts(" 21         ");
				lcd_gotoxy(0, 2);
				lcd_puts(">22         ");
				lcd_gotoxy(0, 3);
				lcd_puts(" 23         ");
				break;
			}
			case 23:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(" 20         ");
				lcd_gotoxy(0, 1);
				lcd_puts(" 21         ");
				lcd_gotoxy(0, 2);
				lcd_puts(" 22         ");
				lcd_gotoxy(0, 3);
				lcd_puts(">23         ");
				break;
			}
			case 24:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(">24         ");
				lcd_gotoxy(0, 1);
				lcd_puts(" 25         ");
				lcd_gotoxy(0, 2);
				lcd_puts(" 26         ");
				lcd_gotoxy(0, 3);
				lcd_puts(" 27         ");
				break;
			}
			case 25:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(" 24         ");
				lcd_gotoxy(0, 1);
				lcd_puts(">25         ");
				lcd_gotoxy(0, 2);
				lcd_puts(" 26         ");
				lcd_gotoxy(0, 3);
				lcd_puts(" 27         ");
				break;
			}
			case 26:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(" 24         ");
				lcd_gotoxy(0, 1);
				lcd_puts(" 25         ");
				lcd_gotoxy(0, 2);
				lcd_puts(">26         ");
				lcd_gotoxy(0, 3);
				lcd_puts(" 27         ");
				break;
			}
			case 27:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(" 24         ");
				lcd_gotoxy(0, 1);
				lcd_puts(" 25         ");
				lcd_gotoxy(0, 2);
				lcd_puts(" 26         ");
				lcd_gotoxy(0, 3);
				lcd_puts(">27         ");
				break;
			}
			case 28:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(">28         ");
				lcd_gotoxy(0, 1);
				lcd_puts(" 29         ");
				lcd_gotoxy(0, 2);
				lcd_puts(" 30         ");
				lcd_gotoxy(0, 3);
				lcd_puts(" 31         ");
				break;
			}
			case 29:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(" 28         ");
				lcd_gotoxy(0, 1);
				lcd_puts(">29         ");
				lcd_gotoxy(0, 2);
				lcd_puts(" 30         ");
				lcd_gotoxy(0, 3);
				lcd_puts(" 31         ");
				break;
			}
			case 30:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(" 28         ");
				lcd_gotoxy(0, 1);
				lcd_puts(" 29         ");
				lcd_gotoxy(0, 2);
				lcd_puts(">30         ");
				lcd_gotoxy(0, 3);
				lcd_puts(" 31         ");
				break;
			}
			case 31:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(" 28         ");
				lcd_gotoxy(0, 1);
				lcd_puts(" 29         ");
				lcd_gotoxy(0, 2);
				lcd_puts(" 30         ");
				lcd_gotoxy(0, 3);
				lcd_puts(">31         ");
				break;
			}
			case 32:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(">32         ");
				lcd_gotoxy(0, 1);
				lcd_puts(" 33         ");
				lcd_gotoxy(0, 2);
				lcd_puts(" 34         ");
				lcd_gotoxy(0, 3);
				lcd_puts(" 35         ");
				break;
			}
			case 33:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(" 32         ");
				lcd_gotoxy(0, 1);
				lcd_puts(">33         ");
				lcd_gotoxy(0, 2);
				lcd_puts(" 34         ");
				lcd_gotoxy(0, 3);
				lcd_puts(" 35         ");
				break;
			}
			case 34:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(" 32         ");
				lcd_gotoxy(0, 1);
				lcd_puts(" 33         ");
				lcd_gotoxy(0, 2);
				lcd_puts(" 34         ");
				lcd_gotoxy(0, 3);
				lcd_puts(" 35         ");
				break;
			}
			case 35:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(" 32         ");
				lcd_gotoxy(0, 1);
				lcd_puts(" 33         ");
				lcd_gotoxy(0, 2);
				lcd_puts(" 34         ");
				lcd_gotoxy(0, 3);
				lcd_puts(">35         ");
				break;
			}
			case 36:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(">36         ");
				lcd_gotoxy(0, 1);
				lcd_puts(" 37         ");
				lcd_gotoxy(0, 2);
				lcd_puts(" 38         ");
				lcd_gotoxy(0, 3);
				lcd_puts(" 39         ");
				break;
			}
			case 37:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(" 36         ");
				lcd_gotoxy(0, 1);
				lcd_puts(">37         ");
				lcd_gotoxy(0, 2);
				lcd_puts(" 38         ");
				lcd_gotoxy(0, 3);
				lcd_puts(" 39         ");
				break;
			}
			case 38:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(" 36         ");
				lcd_gotoxy(0, 1);
				lcd_puts(" 37         ");
				lcd_gotoxy(0, 2);
				lcd_puts(">38         ");
				lcd_gotoxy(0, 3);
				lcd_puts(" 39         ");
				break;
			}
			case 39:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(" 36         ");
				lcd_gotoxy(0, 1);
				lcd_puts(" 37         ");
				lcd_gotoxy(0, 2);
				lcd_puts(" 38         ");
				lcd_gotoxy(0, 3);
				lcd_puts(">39         ");
				break;
			}
			case 40:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(">40         ");
				lcd_gotoxy(0, 1);
				lcd_puts(" 41         ");
				lcd_gotoxy(0, 2);
				lcd_puts(" 42         ");
				lcd_gotoxy(0, 3);
				lcd_puts(" 43         ");
				break;
			}
			case 41:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(" 40         ");
				lcd_gotoxy(0, 1);
				lcd_puts(">41         ");
				lcd_gotoxy(0, 2);
				lcd_puts(" 42         ");
				lcd_gotoxy(0, 3);
				lcd_puts(" 43         ");
				break;
			}
			case 42:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(" 40         ");
				lcd_gotoxy(0, 1);
				lcd_puts(" 41         ");
				lcd_gotoxy(0, 2);
				lcd_puts(">42         ");
				lcd_gotoxy(0, 3);
				lcd_puts(" 43         ");
				break;
			}
			case 43:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(" 40         ");
				lcd_gotoxy(0, 1);
				lcd_puts(" 41         ");
				lcd_gotoxy(0, 2);
				lcd_puts(" 42         ");
				lcd_gotoxy(0, 3);
				lcd_puts(">43         ");
				break;
			}
			case 44:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(">44         ");
				lcd_gotoxy(0, 1);
				lcd_puts(" 45         ");
				lcd_gotoxy(0, 2);
				lcd_puts(" 46         ");
				lcd_gotoxy(0, 3);
				lcd_puts(" 47         ");
				break;
			}
			case 45:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(" 44         ");
				lcd_gotoxy(0, 1);
				lcd_puts(">45         ");
				lcd_gotoxy(0, 2);
				lcd_puts(" 46         ");
				lcd_gotoxy(0, 3);
				lcd_puts(" 47         ");
				break;
			}
			case 46:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(" 44         ");
				lcd_gotoxy(0, 1);
				lcd_puts(" 45         ");
				lcd_gotoxy(0, 2);
				lcd_puts(">46         ");
				lcd_gotoxy(0, 3);
				lcd_puts(" 47         ");
				break;
			}
			case 47:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(" 44         ");
				lcd_gotoxy(0, 1);
				lcd_puts(" 45         ");
				lcd_gotoxy(0, 2);
				lcd_puts(" 46         ");
				lcd_gotoxy(0, 3);
				lcd_puts(">47         ");
				break;
			}
			case 48:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(">48         ");
				lcd_gotoxy(0, 1);
				lcd_puts(" 49         ");
				lcd_gotoxy(0, 2);
				lcd_puts(" 50         ");
				lcd_gotoxy(0, 3);
				lcd_puts(" 51         ");
				break;
			}
			case 49:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(" 48         ");
				lcd_gotoxy(0, 1);
				lcd_puts(">49         ");
				lcd_gotoxy(0, 2);
				lcd_puts(" 50         ");
				lcd_gotoxy(0, 3);
				lcd_puts(" 51         ");
				break;
			}
			case 50:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(" 48         ");
				lcd_gotoxy(0, 1);
				lcd_puts(" 49         ");
				lcd_gotoxy(0, 2);
				lcd_puts(">50         ");
				lcd_gotoxy(0, 3);
				lcd_puts(" 51         ");
				break;
			}
			case 51:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(" 48         ");
				lcd_gotoxy(0, 1);
				lcd_puts(" 49         ");
				lcd_gotoxy(0, 2);
				lcd_puts(" 50         ");
				lcd_gotoxy(0, 3);
				lcd_puts(">51         ");
				break;
			}
			case 52:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(">52         ");
				lcd_gotoxy(0, 1);
				lcd_puts(" 53         ");
				lcd_gotoxy(0, 2);
				lcd_puts(" 54         ");
				lcd_gotoxy(0, 3);
				lcd_puts(" 55         ");
				break;
			}
			case 53:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(" 52         ");
				lcd_gotoxy(0, 1);
				lcd_puts(">53         ");
				lcd_gotoxy(0, 2);
				lcd_puts(" 54         ");
				lcd_gotoxy(0, 3);
				lcd_puts(" 55         ");
				break;
			}
			case 54:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(" 52         ");
				lcd_gotoxy(0, 1);
				lcd_puts(" 53         ");
				lcd_gotoxy(0, 2);
				lcd_puts(">54         ");
				lcd_gotoxy(0, 3);
				lcd_puts(" 55         ");
				break;
			}
			case 55:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(" 52         ");
				lcd_gotoxy(0, 1);
				lcd_puts(" 53         ");
				lcd_gotoxy(0, 2);
				lcd_puts(" 54         ");
				lcd_gotoxy(0, 3);
				lcd_puts(">55         ");
				break;
			}
			case 56:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(">56         ");
				lcd_gotoxy(0, 1);
				lcd_puts(" 57         ");
				lcd_gotoxy(0, 2);
				lcd_puts(" 58         ");
				lcd_gotoxy(0, 3);
				lcd_puts(" 59         ");
				break;
			}
			case 57:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(" 56         ");
				lcd_gotoxy(0, 1);
				lcd_puts(">57         ");
				lcd_gotoxy(0, 2);
				lcd_puts(" 58         ");
				lcd_gotoxy(0, 3);
				lcd_puts(" 59         ");
				break;
			}
			case 58:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(" 56         ");
				lcd_gotoxy(0, 1);
				lcd_puts(" 57         ");
				lcd_gotoxy(0, 2);
				lcd_puts(">58         ");
				lcd_gotoxy(0, 3);
				lcd_puts(" 59         ");
				break;
			}
			case 59:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(" 56         ");
				lcd_gotoxy(0, 1);
				lcd_puts(" 57         ");
				lcd_gotoxy(0, 2);
				lcd_puts(" 58         ");
				lcd_gotoxy(0, 3);
				lcd_puts(">59         ");
				break;
			}
		}
		if((PINA &(1<<joystick_ok))==1)
		{
			break;
		}
		
	}
	
	int time_mark=0;
	while(1)
	{
		ADC_VAL=ADC_Read(2);
		if(ADC_VAL>=300)
		{
			time_mark++;
		}
		else if(ADC_VAL<=700)
		{
			time_mark--;
		}
		if(time_mark>1)
		{
			time_mark=0;
		}
		if(time_mark<0)
		{
			time_mark=1;
		}
		
		if(time_mark==0)
		{
			lcd_gotoxy(0, 1);
			lcd_puts("  >am           ");
			lcd_gotoxy(0, 2);
			lcd_puts("   pm           ");
		}
		else if(quick_mode==1)
		{
			lcd_gotoxy(0, 1);
			lcd_puts("   am           ");
			lcd_gotoxy(0, 2);
			lcd_puts("  >pm           ");
		}
		if((PINA &(1<<joystick_ok))==1)
		{
			break;
		}
		
	}
	if (time_mark==0)
	{
		if (hours==12)
		{
			hours=00;
		}
	}
	if (time_mark==0)
	{
		if (hours==12)
		{
			hours=12;
		}
		else
		{
			hours=hours+12;
		}
	}

    
}