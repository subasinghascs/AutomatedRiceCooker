int set_time()
{
	while(1)
	{
		ADC_VAL=ADC_Read(2);
		if(ADC_VAL>=700)
		{
			hours++;
		}
		else if(ADC_VAL>=300)
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
		if(ADC_VAL>=700)
		{
			minits++;
		}
		else if(ADC_VAL>=300)
		{
			minits--;
		}
		if (minits<0)
		{
			minits=12;
		}
		if (minits>12)
		{
			minits=1;
		}
		switch(minits)
		{
			case 1:
			{
				lcd_gotoxy(0, 0);
				lcd_puts(" set minits:");
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
				lcd_puts(" set minits:");
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
				lcd_puts(" set minits:");
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
				lcd_puts(" 26         ");
				lcd_gotoxy(0, 3);
				lcd_puts(" 27         ");
				break;
			}
		}
		if((PINA &(1<<joystick_ok))==1)
		{
			break;
		}
		
	}
	

}