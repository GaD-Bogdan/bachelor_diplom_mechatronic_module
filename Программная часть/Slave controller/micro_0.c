/*
 * Microcontroller_0.c
 *
 * Created: 04.06.2020 11:16:09
 * Author : GaD_Bogdan
 */ 
#include "Libraries/main.h"
//===================================================================================================================
#define MOTOR_SPEED 255
//===================================================================================================================
uint8_t SPI_byte_counter = 0;					// ���������� ��� ����� �������� ������ �� SPI
uint8_t stop_flag = 0;							// ����������, ��� �������� ��������� ���
uint8_t motor_dir;								// ����, ��� ����������� ����������� �������� ���������
uint32_t data32;								// ���������� ��� ���������� ������ � ���������������� �����������
uint32_t encoder_counter = 0;					// ������� 32-�������� ���������� ��� ������� ��� ��������.
int32_t delta = 0;								// ���������� ��� �������� ������
uint8_t * p_data32;								// ��������� �� ������ ���� ���� ������
char str_array[12];								// ��������� ������ ��� ������ �� LCD �������
//===================================================================================================================
void ext_INTERRUPT_init(void);					// �������� �������, ��� ����������� �� ������
void device_is_busy(uint8_t is_busy);			// ������� ��� ��������� ���������������� ����������� � ���������
void motor_driver_init(void);					// ������� ������������� ���������� ���������
void motor_driver_start(uint8_t OCR0A_value, uint8_t direction);	// ������� ������� ���������
void motor_driver_stop(void);					// ������� ��������� ���������
void motor_calibration(void);					// ������� ���������� ���������
//===================================================================================================================
int main(void)
{
	device_is_busy(1);					// �������, ��� ���������� ������ ��������������

 	p_data32 = (uint8_t *) &data32;		// �������������� ��������� �� ������� ������ � data32
	ext_INTERRUPT_init();				// ������������� ������� ����������
	motor_driver_init();				// ������������� �������
	sei();								// ��������� ����������
	motor_calibration();				// ���������� �������
 	SPI_SL_init();						// �������������� SPI � �������� ��������
//	LCD_init();							// ���������� LCD-������� ��� �������

  	device_is_busy(0);					// ���������� ������ � ������
    while (1) 
    {
//-------------------------------------------------------------------------------------------------------------------
		if (stop_flag)						// ���� �������� ���, ��� �������������
		{
			motor_driver_stop();			// ���� �������� ��� ������������� ���������
			device_is_busy(1);					// �������, ��� ���������� ������
			continue;
		}
//-------------------------------------------------------------------------------------------------------------------	
		if (SPI_byte_counter == 4)				// ���� ���������� ����� ������...
		{
			delta = data32 - encoder_counter;	// ����������� ������
			if (delta < 7 && delta > -7)		// ���� ������ � �������� �����������, ��
			{
				motor_driver_stop();			// ������������� ���������
				SPI_byte_counter = 0;			// �������� ������� SPI
				device_is_busy(0);				// �������, ��� ���������� ������ � �������
				continue;
			}
			else if (delta > 0)					// ���� ������ ������ ����, ��...
			{
				motor_driver_start(MOTOR_SPEED, 1);	// ������� � ������������� �����������
			}
			else								// ���� ������ ������ ����, ��...
			{
				motor_driver_start(MOTOR_SPEED, 0);	// ������� � ������������� �����������
			}
		}
	}
}
//===================================================================================================================
ISR(INT0_vect)						// ���������� �������� ���������� INT0
{
	stop_flag = 1;					// ������������� ���� ��������� �������
}
//===================================================================================================================
ISR(INT1_vect)						// ���������� �������� ���������� INT1
{
	if (BitIsSet(PIND, 4)) encoder_counter++;				// ���� ������ ����� �������� � 1, ������ ����������
	else encoder_counter--;									// ���� ������ ����� �������� � 0, ������ ��������
	
}
//===================================================================================================================
ISR(SPI_STC_vect)					// ���������� ���������� �� ������ ����� ������
{
	*(p_data32 + SPI_byte_counter) = SPDR;								// ���������� ���������� ���� � ������
	SPI_byte_counter++;													// ����������� ������� ������
	if (SPI_byte_counter == 4) 
	{
		device_is_busy(1);			// ���� ������� 4 �����, �� ���������� � ���������� ��������
	}
}
//===================================================================================================================
void motor_calibration(void)
{
	motor_driver_start(MOTOR_SPEED, 0);		// ��������� ��������� � �������� �����������
	while(1)
	{
		_delay_us(1);
		if (stop_flag)						// ���� �������� ���...
		{
			motor_driver_stop();			// ������������� ���������
			stop_flag = 0;					// ���������� � 0 ���� ��������
			encoder_counter = 0;			// ��������� ����������-������� ���������
			break;							// ���������� ���������
		}
		else								// ���� ���, �� ����
		{
			continue;
		}
	}
}
//===================================================================================================================
void motor_driver_init(void)
{
	SetBit(DDRD, 7);												// ����� �������� "dir" �� �����
	SetBit(DDRD, 6);												// OC0A �� �����
	TCCR0A = (0<<COM0A1)|(1<<COM0A0)|(1<<WGM01)|(0<<WGM00);			// �������� ����� ��� � ������������ ����� OC0A ��� ����������
}
void motor_driver_start(uint8_t OCR0A_value, uint8_t direction)
{
	if (direction) SetBit(PORTD, 7);								// ���� direction = 1, �� ��� dir � ������� ���������
	else ClearBit(PORTD, 7);										// ����� � ������
	OCR0A = OCR0A_value;											// ������ ��������
	TCCR0B = (1<<CS02)|(0<<CS01)|(1<<CS00);							// ��������� ������, ������������ 1024
}
void motor_driver_stop(void)
{
	TCCR0B = (0<<CS02)|(0<<CS01)|(0<<CS00);							// ������������� ������
	TCNT0 = 0;														// �������� ������� �������
}
//===================================================================================================================
void device_is_busy(uint8_t is_busy)
{
	if (is_busy)
	{
		SetBit(DDRD, 0);					// �������� ������-����, �������, ��� ���������� ������
		ClearBit(PORTD, 0);	
	} 
	else
	{
		ClearBit(DDRD, 0);					// ��������� ������-����, �������, ��� ���������� ������ � ������ �������
		SetBit(PORTD, 0);					// ����������� �� � 1 ����� ��������
	}
}
//===================================================================================================================
void ext_INTERRUPT_init(void)					// ������������� ������� ���������� INT0 � INT1
{
	ClearBit(DDRD, 2);							// ����������� ����� � ����������� �� ������������ ��� �� ����
	SetBit(PORTD, 2);							// �������� � 1
	EICRA = (1<<ISC01)|(0<<ISC00);				// ���������� ��� ��������� ������ �� PD2
	SetBit(EIMSK, INT0);						// ��������� ���������� INT1
	
	ClearBit(DDRD, 3);							// ����������� ����� �� ����
	SetBit(PORTD, 3);							// �������� � 1
	EICRA = (1<<ISC11)|(0<<ISC10);				// ���������� ��� ��������� ������ �� PD3
	SetBit(EIMSK, INT1);						// ��������� ���������� INT1
}
//===================================================================================================================

