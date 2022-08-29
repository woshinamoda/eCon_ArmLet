/**********************************************************************
 * PB6 ---->  Color for Green
 * PB7 ---->  Color for Blue
 * PB8 ---->  Color for Red
 *
 *==========��ɫ�߼�=============
 * ��ɫ����	�� ������
 * ��ɫ��˸	�� ���ڳ��
 * ��ɫ����	�� ��������
 * ��ɫ����	�� ��������
 * ��ɫ��˸	�� ����΢�����뼰ʱ���
 *
 *
 **********************************************************************/
#include <led.h>

//�򿪶�Ӧ��ɫ��RGB����
//param 0:Open all   1: Green  2: Blue  3: Red  4: yellow  5: purple
void Open_LED(uint8_t color)
{
	switch(color)
	{
	case 0:
		PBout(6) = 0;
		PBout(7) = 0;
		PBout(8) = 0;
		break;			//������RGB��
	case 1:
		PBout(6) = 0;
		break;			//Open Gree Color
	case 2:
		PBout(7) = 0;
		break;			//Open Blue Color
	case 3:
		PBout(8) = 0;
		break;			//Open Rend
	case 4:
		PBout(8) = 0;
		PBout(6) = 0;
		break;			//Open yellow
	case 5:
		PBout(8) = 0;
		PBout(7) = 0;
		break;			//Open purple
	}
}

//�رն�Ӧ��ɫ��RGB����
//param 0��Close all	  1: Green  2: Blue  3: Red  4: yellow  5: purple
void Close_LED(uint8_t color)
{
	switch(color)
	{
	case 0:
		PBout(6) = 1;
		PBout(7) = 1;
		PBout(8) = 1;
		break;			//�ر�����RGB��
	case 1:
		PBout(6) = 1;
		break;			//close Gree Color
	case 2:
		PBout(7) = 1;
		break;			//close Blue Color
	case 3:
		PBout(8) = 1;
		break;			//close Red
	case 4:
		PBout(8) = 1;
		PBout(6) = 1;
		break;			//close yellow
	case 5:
		PBout(8) = 1;
		PBout(7) = 1;
		break;			//close purple
	case 6:
		PBout(8) = 1;
		PBout(7) = 1;
		PBout(6) = 1;	
		break;			//close purple	
	}
}

//��Ӧ����ָʾ��״̬
//param 0������		1���ر�
void Booth_LED(uint8_t state)
{
	switch(state)
	{
	case 0:
		PBout(12) = 1;
		break;
	case 1:
		PBout(12) = 0;
		break;
	}
}














