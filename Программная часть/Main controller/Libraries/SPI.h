/*
 * SPI.h
 *
 * Created: 05.06.2020 17:55:24
 *  Author: GaD_Bogdan
 */ 


#ifndef SPI_H_
#define SPI_H_

#include "main.h"

//===================================================================================================================
/* ����������� �����, �� ������� ����� ������ ������� ����������*/
#define SPI_slaves_port		PORTC			// ����, � �������� ���������� ����� SS ������� ���������
#define SPI_slave1_pin		0				// ����� � ������ ������� �����������
#define SPI_slave2_pin		1				// ����� �� ������ ������� �����������
#define SPI_slave3_pin		2				// ����� � ������� ������� �����������
#define SPI_slave4_pin		3				// ����� � ��������� ������� �����������

//===================================================================================================================

/*������������� SPI ���������� � �������� �������*/
void SPI_MS_init(void);

//===================================================================================================================

/*������������� SPI ���������� � �������� �������� ����������*/
void SPI_SL_init(void);

//===================================================================================================================

/*�������� �������� ����� ������. ���������� ���������� �� �������� ����*/
uint8_t SPI_MS_send_byte(uint8_t byte);

//===================================================================================================================

/*�������� �������� 4 ����� ������. ���������� ���������� �� �������� ����*/
void SPI_MS_send_uint32_t(uint8_t * p8_data32, uint8_t slaves_pin);

//===================================================================================================================



#endif /* SPI_H_ */