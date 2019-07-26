#include "stm32l4xx.h"
#include "Buffer.h"
#include "myPrint.h"

#include <stdbool.h>
#include <string.h>

#define NEXT(index)   ((index+1)%SERIAL_BUF_SIZE)
#define CommandSize 20

extern UART_HandleTypeDef huart2;

uint8_t print[30] = "";
uint8_t bend[] = "\r\n";

bool result = false;
rb RXRingBuf;
rb TXRingBuf;

/**************************RXBuffer*****************************/
bool RingBufferInit()		//RXBuffer initialize
{
  RXRingBuf.head = 0;
  RXRingBuf.tail = 0;
	RXRingBuf.length = 0;
  RXRingBuf.Data[0] = '\0';
	result = true;

	return result;
}

uint8_t RingBufferLength()		//Unoutputed data in RXBuffer
{
	RXRingBuf.length = RXRingBuf.tail - RXRingBuf.head;
	
  return RXRingBuf.length;
}

/*Check RX Buffer State*/
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

/*Pushed data in RXBuffer*/
void RingBufferPush(uint8_t data)
{
	if(IsFull()){
		memcpy(print,"Buffer is full\r\n",sizeof("Buffer is full\r\n"));
		TXRingBufferPush(print);
		OutputBuffer(&huart2);
		
		return;
	}
	RXRingBuf.Data[RXRingBuf.tail] = data;
	RXRingBuf.tail = NEXT(RXRingBuf.tail);
	
	return;
}

/*Poped data in RXBuffer*/
char RingBufferPop()
{
	char re = 0;
    if (IsEmpty())
    {
			memcpy(print,"Buffer is empty\r\n",sizeof("Buffer is empty\r\n"));
			TXRingBufferPush(print);
			OutputBuffer(&huart2);
			
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

/*Print of all of data in RXBuffer*/
void PrintBuffer(){
	memcpy(print,"**********BUFFER**********\r\n",sizeof("**********BUFFER**********\r\n"));
	TXRingBufferPush(print);
	TXRingBufferPush(RXRingBuf.Data);
	TXRingBufferPush(bend);
	memcpy(print,"**************************\r\n",sizeof("**************************\r\n"));
	TXRingBufferPush(print);
	OutputBuffer(&huart2);
	
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
bool TXRingBufferInit() //TXBuffer initialize
{
  TXRingBuf.head = 0;
  TXRingBuf.tail = 0;
	TXRingBuf.length = 0;
  TXRingBuf.Data[0] = '\0';
	result = true;

	return result;
}

uint8_t TXRingBufferLength()		//Unoutputed data in TXBuffer
{
	TXRingBuf.length = TXRingBuf.tail - TXRingBuf.head;
	
  return TXRingBuf.length;
}

/*Check TX Buffer State*/
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

/*Pushed data in TXBuffer*/
void TXRingBufferPush(uint8_t *data){	
	uint8_t inputDATA = *data;
	
	if(TXIsFull()){
			memcpy(print,"TX Buffer is full\r\n",sizeof("TX Buffer is full\r\n"));
			TXRingBufferPush(print);
			OutputBuffer(&huart2);
		
			return;
		}
	else{
		while(inputDATA != NULL){
			inputDATA = *data;
			
			TXRingBuf.Data[TXRingBuf.tail] = inputDATA;
			TXRingBuf.tail = NEXT(TXRingBuf.tail);
			
			data ++;
		}
	}
	
/*	uint8_t inputDATA;
		for(int i = 0;i < datasize; i++){
		if(TXIsFull()){
			memcpy(print,"TX Buffer is full\r\n",sizeof("TX Buffer is full\r\n"));
			
			HAL_Delay(10);
			HAL_UART_Transmit_IT(&huart2, print, sizeof(print));
		
//		printf("Buffer is full \n");
		
			return;
		}
		inputDATA = *data;
		data ++;
		
		TXRingBuf.Data[TXRingBuf.tail] = inputDATA;
		TXRingBuf.tail = NEXT(TXRingBuf.tail);
	}
*/	
		return;
}

/*Poped data in TXBuffer*/
char TXRingBufferPop()
{
	uint8_t re = 0;
    if (TXIsEmpty()){
			memcpy(print,"TX Buffer is empty\r\n",sizeof("TX Buffer is empty\r\n"));
			TXRingBufferPush(print);
			OutputBuffer(&huart2);
			
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

/*Print of all of data in TXBuffer*/
void TXPrintBuffer(){
	memcpy(print,"**********TX BUFFER**********\r\n",sizeof("**********TX BUFFER**********\r\n"));
	TXRingBufferPush(print);
	TXRingBufferPush(TXRingBuf.Data);
	TXRingBufferPush(bend);
	memcpy(print,"**************************\r\n",sizeof("**************************\r\n"));
	TXRingBufferPush(print);
	OutputBuffer(&huart2);
		
/*	printf("********BUFFER**********\n");
	for(int i = 0; i < SERIAL_BUF_SIZE; i ++){
		printf("%c", RingBuf.Data[i]);
	}
	printf("\n");
	printf("**************************\n");
*/	
	return;
}



