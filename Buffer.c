#include "stm32l4xx.h"
#include "Buffer.h"

#include <stdbool.h>
#include <string.h>

#define NEXT(index)   ((index+1)%SERIAL_BUF_SIZE)
#define CommandSize 20

extern UART_HandleTypeDef huart2;

uint8_t bprint[30] = "";
uint8_t bend[] = "\r\n";

bool result = false;
rb RXRingBuf;
rb TXRingBuf;

/**************************RXBuffer*****************************/
bool RingBufferInit()
{
  RXRingBuf.head = 0;
  RXRingBuf.tail = 0;
	RXRingBuf.length = 0;
  RXRingBuf.Data[0] = '\0';
	result = true;

	return result;
}

uint8_t RingBufferLength()
{
	RXRingBuf.length = RXRingBuf.tail - RXRingBuf.head;
	
  return RXRingBuf.length;
}

//false : not full / true : full
bool IsFull()
{
    return NEXT(RXRingBuf.tail) == RXRingBuf.head;
}

// false : not empty / true : empty
bool IsEmpty()
{
    return RXRingBuf.head == RXRingBuf.tail;   
}

void RingBufferPush(uint8_t data)
{
	if(IsFull()){
		memcpy(bprint,"Buffer is full\r\n",sizeof("Buffer is full\r\n"));
			
		HAL_Delay(10);
		HAL_UART_Transmit_IT(&huart2, bprint, sizeof(bprint));
		
//		printf("Buffer is full \n");
		
		return;
	}
	RXRingBuf.Data[RXRingBuf.tail] = data;
	RXRingBuf.tail = NEXT(RXRingBuf.tail);
	
	return;
}

char RingBufferPop()
{
	char re = 0;
    if (IsEmpty())
    {
			memcpy(bprint,"Buffer is empty\r\n",sizeof("Buffer is empty\r\n"));
			
			HAL_Delay(10);
			HAL_UART_Transmit_IT(&huart2, bprint, sizeof(bprint));
			
//      printf("Buffer is empty \n");
			
        return re;
    }
    re = RXRingBuf.Data[RXRingBuf.head];
    RXRingBuf.head = NEXT(RXRingBuf.head);
		
    return re;
	
	/*
    uint8_t ret = 0;
    if (RingBuf.length > 0) {
        if (RingBuf.tail == SERIAL_BUF_SIZE) {
            RingBuf.tail = 0;
        }
        ret = RingBuf.Data[RingBuf.tail++];
        RingBuf.length--;
    }
    return ret;
	*/
	
}

void PrintBuffer(){
	memcpy(bprint,"**********BUFFER**********\r\n",sizeof("**********BUFFER**********\r\n"));
	
	HAL_UART_Transmit_IT(&huart2, bprint, sizeof(bprint));
	HAL_Delay(10);
	
	HAL_UART_Transmit_IT(&huart2, RXRingBuf.Data, SERIAL_BUF_SIZE);
	HAL_Delay(10);
	
	HAL_UART_Transmit_IT(&huart2, bend, 3);
	HAL_Delay(10);
	memcpy(bprint,"**************************\r\n",sizeof("**************************\r\n"));
	
	HAL_UART_Transmit_IT(&huart2, bprint, sizeof(bprint));
	HAL_Delay(10);
	
	
/*	printf("********BUFFER**********\n");
	for(int i = 0; i < SERIAL_BUF_SIZE; i ++){
		printf("%c", RingBuf.Data[i]);
	}
	printf("\n");
	printf("**************************\n");
*/	
	return;
}

/**************************TXBuffer*****************************/
bool TXRingBufferInit()
{
  TXRingBuf.head = 0;
  TXRingBuf.tail = 0;
	TXRingBuf.length = 0;
  TXRingBuf.Data[0] = '\0';
	result = true;

	return result;
}

uint8_t TXRingBufferLength()
{
	TXRingBuf.length = TXRingBuf.tail - TXRingBuf.head;
	
  return TXRingBuf.length;
}

//false : not full / true : full
bool TXIsFull()
{
    return NEXT(TXRingBuf.tail) == TXRingBuf.head;
}

// false : not empty / true : empty
bool TXIsEmpty()
{
    return TXRingBuf.head == TXRingBuf.tail;   
}

void TXRingBufferPush(uint8_t *data,uint8_t datasize)
{
	uint8_t inputDATA;
		for(int i = 0;i < datasize; i++){
		if(TXIsFull()){
			memcpy(bprint,"TX Buffer is full\r\n",sizeof("TX Buffer is full\r\n"));
			
			HAL_Delay(10);
			HAL_UART_Transmit_IT(&huart2, bprint, sizeof(bprint));
		
//		printf("Buffer is full \n");
		
			return;
		}
		inputDATA = *data;
		data ++;
		
		TXRingBuf.Data[TXRingBuf.tail] = inputDATA;
		TXRingBuf.tail = NEXT(TXRingBuf.tail);
	}
		return;
}

char TXRingBufferPop()
{
	uint8_t re = 0;
    if (TXIsEmpty())
    {
			memcpy(bprint,"TX Buffer is empty\r\n",sizeof("TX Buffer is empty\r\n"));
			HAL_UART_Transmit_IT(&huart2, bprint, sizeof(bprint));
			
//      printf("Buffer is empty \n");
			
        return re;
    }
    re = TXRingBuf.Data[TXRingBuf.head];
    TXRingBuf.head = NEXT(TXRingBuf.head);
		
    return re;
	
	/*
    uint8_t ret = 0;
    if (RingBuf.length > 0) {
        if (RingBuf.tail == SERIAL_BUF_SIZE) {
            RingBuf.tail = 0;
        }
        ret = RingBuf.Data[RingBuf.tail++];
        RingBuf.length--;
    }
    return ret;
	*/
	
}

void TXPrintBuffer(){
	memcpy(bprint,"**********TX BUFFER**********\r\n",sizeof("**********TX BUFFER**********\r\n"));
	
	HAL_UART_Transmit_IT(&huart2, bprint, sizeof(bprint));
	HAL_Delay(10);
	
	HAL_UART_Transmit_IT(&huart2, TXRingBuf.Data, SERIAL_BUF_SIZE);
	HAL_Delay(10);
	
	HAL_UART_Transmit_IT(&huart2, bend, 3);
	HAL_Delay(10);
	memcpy(bprint,"**************************\r\n",sizeof("**************************\r\n"));
	
	HAL_UART_Transmit_IT(&huart2, bprint, sizeof(bprint));
	HAL_Delay(10);
	
	
/*	printf("********BUFFER**********\n");
	for(int i = 0; i < SERIAL_BUF_SIZE; i ++){
		printf("%c", RingBuf.Data[i]);
	}
	printf("\n");
	printf("**************************\n");
*/	
	return;
}
