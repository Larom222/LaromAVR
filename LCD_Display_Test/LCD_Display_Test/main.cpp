/*
 * LCD_Display_Test.cpp
 *
 * Created: 2020-01-19 14:31:23
 * Author : Gabriel
 */ 

#include <avr/io.h>
#include "../../_Libraries/LCD_Display/LCD_Display.h"
#include "../../_Libraries/DebugTools/DebugTools.h"

char menu1[9] = "MENU  1 ";
char menu2[9] = "POZ DWA ";
char menu3[9] = "LISTTRZY";
char menu4[9] = "DATACZAS";
char menu5[9] = "TRYBAUTO";

char *actualMenu1 = menu1;
char *actualMenu2 = menu2;
char *actualMenu3 = menu3;
char *actualMenu4 = menu4;

bool line = true;
bool menuDir = true;

char firstLineBuf[16];
char secondLineBuf[16];

void clearBuffs()
{
	for(int i = 0; i < 16; i++)
	{
		firstLineBuf[i] = ' ';
		secondLineBuf[i] = ' ';
	}
}

void WriteBuf(char *source, char *dest, int size)
{
	for(int i = 0; i < size; i++)
	{
		dest[i] = source[i];
	}	
}

void WriteBreces()
{
	firstLineBuf[5] = secondLineBuf[5] = '>';
	
	firstLineBuf[10] = secondLineBuf[10] = '<';
}

int main(void)
{	
	DisplayLCD::GetInstance().Initialize();
	Time::GetInstance().Configure();
	
	long long lastSwap = Time::GetInstance().GetUS();
	long long lastStep = Time::GetInstance().GetUS();
	unsigned char stepPos = 0;
	int menuPos = 0;
	menuDir = true;
	
    /* Replace with your application code */
    while (1) 
    {		
		long long currentTime = Time::GetInstance().GetUS();
		if(currentTime - lastSwap > 4000000)
		{
			lastSwap = currentTime;
			stepPos = 0;	
			if(menuDir)
			{
				if(menuPos < 2)
					++menuPos;
				else
				{
					menuDir = false;
					--menuPos;
				}
			}
			else
			{
				if(menuPos > 0)
					--menuPos;
				else
				{
					menuDir = true;
					++menuPos;		
				}
			}
		}
		
		switch(menuPos)
		{
			case 0:
			actualMenu1 = menu1;
			actualMenu2 = menu2;
			actualMenu3 = menu3;
			actualMenu4 = menu4;
			break;
			case 1:
			if(menuDir)
			{
				actualMenu1 = menu2;
				actualMenu2 = menu3;
				actualMenu3 = menu4;
				actualMenu4 = menu5;
			}
			else
			{
				actualMenu1 = menu2;
				actualMenu2 = menu3;
				actualMenu3 = menu4;
				actualMenu4 = menu1;
			}
			break;
			case 2:
			actualMenu1 = menu3;
			actualMenu2 = menu4;
			actualMenu3 = menu5;
			actualMenu4 = menu2;
			break;
		}
		
		if(stepPos < 6)
		{
			if(currentTime - lastStep > 1000)
			{
				lastStep = currentTime;
				++stepPos;
			}
		}
		
		if(DisplayLCD::GetInstance().GetAvailableTextLength() > 20)
		{		
			clearBuffs();
			if(menuDir)
			{
				if(line)
				{
					if(stepPos < 4)
						WriteBuf(actualMenu1 + stepPos, firstLineBuf, 4 - stepPos);
					WriteBuf(actualMenu2, firstLineBuf + 6 - stepPos, 4);
					WriteBuf(actualMenu3, firstLineBuf + 12 - stepPos, 4);
					if(stepPos > 2)
						WriteBuf(actualMenu4, firstLineBuf + 18 - stepPos, stepPos - 2);
				}
				else
				{
					if(stepPos < 4)
						WriteBuf(actualMenu1 + stepPos + 4, secondLineBuf, 4 - stepPos);
					WriteBuf(actualMenu2 + 4, secondLineBuf + 6 - stepPos, 4);
					WriteBuf(actualMenu3 + 4, secondLineBuf + 12 - stepPos, 4);
					if(stepPos > 2)
						WriteBuf(actualMenu4 + 4, secondLineBuf + 18 - stepPos, stepPos - 2);
				}
			}
			else
			{
				if(line)
				{
					if(stepPos > 2)
						WriteBuf(actualMenu4 + 6 - stepPos, firstLineBuf, stepPos - 2);
					WriteBuf(actualMenu1, firstLineBuf + stepPos, 4);
					WriteBuf(actualMenu2, firstLineBuf + 6 + stepPos, 4);
					if(stepPos < 4)
						WriteBuf(actualMenu3, firstLineBuf + 12 + stepPos, 4 - stepPos);
				}
				else
				{
					if(stepPos > 2)
						WriteBuf(actualMenu4 + 4 + 6 - stepPos, secondLineBuf, stepPos - 2);
					WriteBuf(actualMenu1 + 4, secondLineBuf + stepPos, 4);
					WriteBuf(actualMenu2 + 4, secondLineBuf + 6 + stepPos, 4);
					if(stepPos < 4)
						WriteBuf(actualMenu3 + 4, secondLineBuf + 12 + stepPos, 4 - stepPos);
				}
			}
			WriteBreces();
			if(line)
				DisplayLCD::GetInstance().WriteText(firstLineBuf, FIRST_LINE_ADDRESS, 16);
			else
				DisplayLCD::GetInstance().WriteText(secondLineBuf, SECOND_LINE_ADDRESS, 16);
			line = !line;
			
			/*char test[1];
			test[0] = menuPos + 48;
			DisplayLCD::GetInstance().WriteText(test, FIRST_LINE_ADDRESS, 1);*/
		}
		
		DisplayLCD::GetInstance().Process();
    }
}