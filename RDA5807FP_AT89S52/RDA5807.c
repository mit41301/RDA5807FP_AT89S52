/*****************************************************************
*                                                                *
* ME850��Ƭ������ϵͳ��չӦ�� ��  ����ģ����ʾ����               *
*                                                                *
* MCU оƬ: AT89S52      ����Ƶ��: 11.0592MHz                    *
*                                                                *
* �汾��V1.0 (2011/01/15)                                        *
* ���ߣ�gguoqing  (Email: gguoqing@willar.com)                   *
* ��վ��www.sofi-tech.com (˶�ɿƼ�)                             *
*       www.mcusj.com (ΰ�ɵ�Ƭ������)                           *
* ���䣺willar@tom.com                                           *
*                                                                *
*����Ȩ��Copyright(C) ����˶�ɿƼ����޹�˾  All Rights Reserved  *
*���������˳��������ѧϰ��ο���������ע����Ȩ��������Ϣ��      *
*                                                                *
******************************************************************
*                                                                *
*  K1--- ����������̨ K2--- ����������̨                         *
*  K3--- ������       K4--- ������                               *
*                                                                *
*****************************************************************/

#include <reg52.h>
#include <intrins.h>
#include "lcd.h"
#include "IIC.h"
#include "delay.h"

sbit  K1 = P3^2;
sbit  K2 = P3^3;
sbit  K3 = P3^4;
sbit  K4 = P3^1;

unsigned long frequency;

// RDA5807 �Ĵ���
unsigned char  RDA_reg_data[8] =
{
   0xd0,0x00,  // 02H
   0x00,0x00,  // 03H
   0x00,0x40,  // 04H
   0x90,0x88,  // 05H
};

unsigned char code  cdis1[ ] = {"FM STEREO RADIO "};
unsigned char code  cdis2[ ] = {"FM       VOL    "};

char code reserve[3]_at_ 0x3b; //����0x3b��ʼ��3���ֽ�

/**********************************************************

 ����д�Ĵ����Ӻ���

**********************************************************/
void RDA5807_write_reg(void)
{
   uchar i;

   I2C_start();
    // ����ģ��д�����
   I2C_write_byte(0x20);
    // �Ĵ�������д����
   for(i=0; i<8; i++)
   {
     I2C_write_byte(RDA_reg_data[i]);
   }
   I2C_stop();
}

/**********************************************************

 �������Ĵ����Ӻ���

**********************************************************/
void RDA5807_read_reg(uchar *reg_buf)
{
   I2C_start();

    // ����ģ���ȡ����
   I2C_write_byte(0x21);

    // �Ĵ�������������
   reg_buf[0] = I2C_read_byte(I2C_ACK);
   reg_buf[1] = I2C_read_byte(I2C_ACK);
   reg_buf[2] = I2C_read_byte(I2C_ACK);
   reg_buf[3] = I2C_read_byte(I2C_NACK);

   I2C_stop();
}

/**********************************************************

 ģ���ϵ��ʼ���Ӻ���

**********************************************************/
void RDA5807_power(void)
{
   delayms(50);

    // ���������λָ��
   RDA_reg_data[0] = 0x00;
   RDA_reg_data[1] = 0x02;
   RDA5807_write_reg();

   delayms(10);

    // ����ģ��Ĭ�ϲ���
   RDA_reg_data[0] = 0xd0;
   RDA_reg_data[1] = 0x01;
   RDA5807_write_reg();
}
/**********************************************************

 ��������������ģ���Զ�Ѱ̨ģʽ

**********************************************************/
void RDA5807_FM_seek(void)
{
   uint chan;
   uchar  reg_data[4] = {0x00, 0x00, 0x00, 0x00};

   RDA_reg_data[3] &= ~(1 << 4);      //��г����

   // �ڲ��Զ�Ѱ̨ʹ��
   RDA_reg_data[0] |=  (1 << 0);      //SEEKλ��1
   RDA5807_write_reg();

    // �ȴ�STC ��־��λ
   while(0 == (reg_data[0] & 0x40))
   {
     delayms(20);
      // ��ȡ�ڲ�״̬
     RDA5807_read_reg(reg_data);
   }
    // ��ȡ��ǰ����Ƶ��
   chan = reg_data[0] & 0x03;
   chan = reg_data[1] | (chan << 8);
   chan = chan << 6;

    // ���浱ǰ����Ƶ��
   RDA_reg_data[2] = (chan >> 8) & 0xff;
   RDA_reg_data[3] = (chan & 0xff);
}

/**********************************************************

 Ƶ����ʾ�Ӻ���

**********************************************************/
void  show_frequency(void)
{ 
   unsigned char i,display[5];
   unsigned int  temp;

   temp = (RDA_reg_data[2]*256)+(RDA_reg_data[3]&0xc0);	 //����
   temp = temp>>6;
   frequency = (unsigned long)(100*temp+87000)/100;

   for(i=0; i<5; i++)  // ���Դ浥Ԫ
   display[i] = 0x00;

   display[0] = (frequency)/1000 ;     //����ת��
   display[1] = (frequency%1000)/100;
   display[2] = (frequency%100)/10;
   display[3] = 0x2e;                  //С����
   display[4] = (frequency%10);

   if(display[0] == 0)
   { 
     display[0] = display[1]+0x30;
     display[1] = display[2]+0x30;
     display[2] = display[3];
     display[3] = display[4]+0x30;
     display[4] = 0x20;
   }
   else
   { 
     display[0] += 0x30;
     display[1] += 0x30;
     display[2] += 0x30;
     display[4] += 0x30;
   }

   lcd_pos_xy(3,2);                      //Ƶ����ʾ
   lcd_wdat(display[0]);
   lcd_wdat(display[1]);
   lcd_wdat(display[2]);
   lcd_wdat(display[3]);
   lcd_wdat(display[4]);
}

/**********************************************************

 ������ʾ�Ӻ���

**********************************************************/
void show_volume()
{
   unsigned char temp,display[2];

   temp = RDA_reg_data[7] & 0x0f; //ȡ����ֵ

   display[0] = temp/10;
   display[1] = temp%10;

   if(display[0] == 0)            //�����λΪ0
   { 
     display[0] = display[1];     //��λ�Դ����ݽ����λ�Դ�
     display[1] = 0x20;           //��λ����ʾ
   }
   else 
   {
     display[1] += 0x30;
   }
   display[0] += 0x30;

   lcd_pos_xy(13,2);              //����ֵ��ʾ
   lcd_wdat(display[0]);
   lcd_wdat(display[1]); 
}

/**********************************************************

 ������

**********************************************************/
void  main(void)
{  
   P0 = 0xff;
   P1 = 0xff;
   P2 = 0xff;

   lcd_init(); 
   lcd_w_string(0,1,cdis1,0);
   lcd_w_string(0,2,cdis2,0);

   RDA5807_power();

   while(1)
   {
     if(K1 == 0)
     {
       delayms(20);
       if(K1 == 0)
       {
         RDA_reg_data[0] |= (1 << 1); 	//SEEK UP	 
         RDA5807_FM_seek();		  
         while(K1 == 0);
       }
     }

     if(K2 == 0)
     {
       delayms(20);
       if(K2 == 0)
       {
         RDA_reg_data[0] &= ~(1 << 1);  //SEEK DOWN		 
         RDA5807_FM_seek();
         while(K2 == 0);
       }
     }

     if(K3 == 0)
     {
       delayms(20);
       if(K3 == 0)
       {
         if((RDA_reg_data[7] & 0x0f) < 0x0f)
         {
           RDA_reg_data[0] = 0xd0;
           RDA_reg_data[1] = 0x01;
           RDA_reg_data[3] &= ~(1 << 4);

           RDA_reg_data[7]++;	 // ��������
           RDA5807_write_reg();
		   while(K3 == 0);
         }
       }
     }

     if(K4 == 0)
     {
       delayms(20);
       if(K4 == 0)
       {
         if((RDA_reg_data[7] & 0x0f) > 0x00)
         {
           RDA_reg_data[0] = 0xd0;
           RDA_reg_data[1] = 0x01;
           RDA_reg_data[3] &= ~(1 << 4);
           
           RDA_reg_data[7]--;	 // �����ݼ�
           RDA5807_write_reg();
           while(K4 == 0);
         }
       }
     }

     show_volume();
     show_frequency();
   }
}

/*********************************************************/

