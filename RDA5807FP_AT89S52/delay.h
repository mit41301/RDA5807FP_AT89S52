
/**********************************************************

 file: delay.h

**********************************************************/
#ifndef  __delay_h__             //�������ָ��
#define  __delay_h__             //�����ظ�����

#include <intrins.h>
	
#define  NOP  _nop_()

extern  void delayNOP();
extern  void delayms( unsigned int ms); 

#endif                           //����������

/**********************************************************/
