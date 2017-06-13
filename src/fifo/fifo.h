#ifndef __FIFO_H__
#define __FIFO_H__

#include <stdbool.h>
#include <stdint.h>

/*!
 * FIFO structure
 */
typedef struct {
    __IO uint16_t begin;
    __IO uint16_t end;
    uint16_t maxSize;
    __IO uint16_t size;
    uint8_t data[];
} Fifo_t;

/*!
 * Initializes the FIFO structure
 *
 * \param [IN] fifo   Pointer to the FIFO object
 * \param [IN] buffer Buffer to be used as FIFO
 * \param [IN] maxSize   MaxSize of the buffer
 */
void FifoInit(Fifo_t *fifo, uint16_t maxSize);

/*!
 * Pushes data to the FIFO
 *
 * \param [IN] fifo Pointer to the FIFO object
 * \param [IN] data Data to be pushed into the FIFO
 */
void FifoPush(Fifo_t *fifo, uint8_t data);

/*!
 * Pops data from the FIFO
 *
 * \param [IN] fifo Pointer to the FIFO object
 * \retval data     Data popped from the FIFO
 */
uint8_t FifoPop(Fifo_t *fifo);

/*!
 * Clears the FIFO
 *
 * \param [IN] fifo   Pointer to the FIFO object
 */
void FifoClear(Fifo_t *fifo);

/*!
 * Checks if the FIFO is empty
 *
 * \param [IN] fifo   Pointer to the FIFO object
 * \retval isEmpty    true: FIFO is empty, false FIFO is not empty
 */
bool IsFifoEmpty(Fifo_t *fifo);

/*!
 * Checks if the FIFO is full
 *
 * \param [IN] fifo   Pointer to the FIFO object
 * \retval isFull     true: FIFO is full, false FIFO is not full
 */
bool IsFifoFull(Fifo_t *fifo);

/* gets the current fifo size */
uint16_t FifoSize(Fifo_t *fifo);

#endif // __FIFO_H__
