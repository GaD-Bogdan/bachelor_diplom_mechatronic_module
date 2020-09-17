/*
 * SPI.c
 *
 * Created: 05.06.2020 15:23:18
 *  Author: GaD_Bogdan
 */ 

#include "SPI.h"

#define delay_between_bytes_us 100
//===================================================================================================================
void SPI_MS_init(void)
{
	DDRB |= (1<<PORTB3)|(1<<PORTB5);					// MOSI, SCK
	PORTB &= ~((1<<PORTB3)|(1<<PORTB5));				// ������ ������� �� MOSI, SCK
	DDR(SPI_slaves_port) |= (1<<SPI_slave1_pin)|(1<<SPI_slave2_pin)|(1<<SPI_slave3_pin)|(1<<SPI_slave4_pin);
	// �� ����� �����, � ������� ���������� ��� SS ������� ���������
	SPI_slaves_port |= (1<<SPI_slave1_pin)|(1<<SPI_slave2_pin)|(1<<SPI_slave3_pin)|(1<<SPI_slave4_pin);
	// ������� ������� �� ���� ������ � SS	
	ClearBit(DDRB, 4);									// MISO �� ����
	SetBit(SPSR, SPI2X);								// �������� �������� ������� ������������ SPI
	SPCR = (1<<SPE)|(1<<MSTR);							// �������� SPI �������� ���������� � �������� �������
}
//===================================================================================================================
void SPI_SL_init(void)
{
	DDRB &= ~((1<<PORTB2)|(1<<PORTB3)|(1<<PORTB5));		// MOSI, SCK, SS �� ����
	SetBit(DDRB, 4);									// MISO �� �����
	ClearBit(PORTB, 4);									// 0 �� MISO
	SetBit(SPSR, SPI2X);								// �������� �������� ������� ������������ SPI
	SPCR = (1<<SPIE)|(1<<SPE)|(0<<MSTR);				// �������� SPI, ����������, �������� ���������� � �������� ��������
}
//===================================================================================================================
uint8_t SPI_MS_send_byte(uint8_t byte)
{
	SPDR = byte;										// �������� �������� ������
	while(BitIsClear(SPSR, SPIF));						// ���� ���� ���������� �������� ������
	return SPDR;										// ���������� ���������� ����
}
//===================================================================================================================
void SPI_MS_send_uint32_t(uint8_t * p8_data32, uint8_t slaves_pin)
{
	ClearBit(SPI_slaves_port, slaves_pin);			// �������, � ����� ����������� �� ����� ��������
	SPI_MS_send_byte(*p8_data32++);					// ���������� ������ ����
	_delay_us(delay_between_bytes_us);									// ��������, ����� ���������� ����� ���������� ������
	SPI_MS_send_byte(*(p8_data32++));				// ���������� ������ ����
	_delay_us(delay_between_bytes_us);									// ��������, ����� ���������� ����� ���������� ������
	SPI_MS_send_byte(*(p8_data32++));				// ���������� ������ ����
	_delay_us(delay_between_bytes_us);									// ��������, ����� ���������� ����� ���������� ������
	SPI_MS_send_byte(*(p8_data32));	// ���������� ��������� ����
	SetBit(SPI_slaves_port, slaves_pin);			// ���������� ������� ������� �� ����� SS
}