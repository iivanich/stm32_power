//#include "main.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_nucleo.h"
#include "fifo.h"


//------------------------------------------------------------------------------
void
FifoInit(Fifo_t *fifo, uint16_t maxSize) {
    fifo->begin = 0;
    fifo->end = 0;
    fifo->size = 0;
    fifo->maxSize = maxSize;
}

//------------------------------------------------------------------------------
void
FifoPush(Fifo_t *fifo, uint8_t data) {
    if(fifo->size < fifo->maxSize) {
        fifo->size ++;
        fifo->data[fifo->end] = data;
        fifo->end = (fifo->end + 1) % fifo->maxSize;
    }
}

//------------------------------------------------------------------------------
uint8_t
FifoPop(Fifo_t *fifo) {
    if(fifo->size > 0) {
        uint8_t data = fifo->data[fifo->begin];

        fifo->begin = (fifo->begin + 1) % fifo->maxSize;
        fifo->size --;

        return data;
    } else
        return 0xff;
}

//------------------------------------------------------------------------------
void
FifoFlush(Fifo_t *fifo) {
    fifo->begin = 0;
    fifo->end = 0;
    fifo->size = 0;
}

//------------------------------------------------------------------------------
uint16_t
FifoSize(Fifo_t *fifo) {
    return fifo->size;
}

//------------------------------------------------------------------------------
bool
IsFifoEmpty(Fifo_t *fifo) {
    return (fifo->size == 0);
}

//------------------------------------------------------------------------------
bool
IsFifoFull(Fifo_t *fifo) {
    return (fifo->size == fifo->maxSize);
}

//------------------------------------------------------------------------------
