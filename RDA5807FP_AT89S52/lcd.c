/**********************************************************

File: lcd.c

Compiler:  gguoqing

**********************************************************/
#include <reg52.h>
#include <intrins.h>
#include "lcd.h"
#include "delay.h"
	
#define uchar unsigned char
#define uint  unsigned int

#define DATA_PORT  P0

sbit LCD_RS = P2^7;
sbit LCD_RW = P2^6;
sbit LCD_EN = P2^5;


/*******************************************************************

  ���LCDæ״̬  
  lcd_busyΪ1ʱ��æ���ȴ���lcd-busyΪ0ʱ,�У���дָ�������ݡ�

*******************************************************************/ 
bit lcd_busy()
 {
    bit result;
    LCD_RS = 0;
    LCD_RW = 1;
    LCD_EN = 1;
    delayNOP();
    result = (bit)(DATA_PORT&0x80);
    LCD_EN = 0;
    return(result); 
 }

/*******************************************************************

  дָ�����ݵ�LCD       
  RS=L��RW=L��E=�����壬D0-D7=ָ���롣
  Check=1������æ��⡣

*******************************************************************/
void lcd_wcmd(uchar cmd, bit Check)
{   
    if(Check)
    while(lcd_busy());

    LCD_RS = 0;
    LCD_RW = 0;
    LCD_EN = 0;
    delayNOP();
    DATA_PORT = cmd;
    delayNOP();
    LCD_EN = 1;
    delayNOP();
    LCD_EN = 0;
}

/*******************************************************************

  д��ʾ���ݵ�LCD
  RS=H��RW=L��E=�����壬D0-D7=���ݡ�

*******************************************************************/
void lcd_wdat(uchar dat)
{                          
    while(lcd_busy());      //����æ���

    LCD_RS = 1;
    LCD_RW = 0;
    LCD_EN = 0;
    DATA_PORT = dat;
    delayNOP();
    LCD_EN = 1;
    delayNOP();
    LCD_EN = 0;
}

/*******************************************************************

  LCD��ʼ���趨

*******************************************************************/
void lcd_init()
{ 
    delayms(50);
    lcd_wcmd(0x38,0);      //16*2��ʾ��5*7����8λ����
    delayms(5);
    lcd_wcmd(0x38,0);      //������æ��⣬ǿ��ִ�С�
    delayms(5);
    lcd_wcmd(0x38,0);
    delayms(5);

	lcd_wcmd(0x38,1);      //����æ��� 
    delayms(5);
    lcd_wcmd(0x0c,1);      //��ʾ�����ع��
    delayms(5);
    lcd_wcmd(0x06,1);      //�ƶ����
    delayms(5);
    lcd_wcmd(0x01,1);      //���LCD����ʾ����
    delayms(5);
}

/*******************************************************************

  �趨��ʾλ��

*******************************************************************/
void lcd_pos_xy(uchar pos_x,uchar pos_y)
{                          
   uchar  pos_temp;

   pos_temp=pos_x & 0x0f;
   pos_y &= 0x0f;

   if(pos_y==0x01)
   pos_temp |= 0x80;
   else 
   if(pos_y==0x02)
   pos_temp |= 0xc0;

   lcd_wcmd(pos_temp,1);
}

/*******************************************************************

  д�ַ����Ӻ��� 

*******************************************************************/
void  lcd_w_string(uchar pos_x,uchar pos_y,uchar code *str,uint times)
{
   uchar m;
   lcd_pos_xy(pos_x,pos_y);      //�趨��ʼλ��
   for(m=0; m<16; m++)
   {
     lcd_wdat(str[m]);
	 delayms(times);
   }
}

/*****************************************************************

  pos_x ȡֵ��Χ�� 0 - 15

  pos_y ȡֵ��Χ�� 1 - 2

******************************************************************/
