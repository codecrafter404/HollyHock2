#ifndef _SDK_SYSTEM_MCS_H
#define _SDK_SYSTEM_MCS_H

/**
 * Copies one region of memory to another. Equivalent to the C standard library
 * function with the same name.
 * 
 * Copies @p num bytes from @p source to @p destination.
 * 
 * @param destination A pointer to the destination of the copy.
 * @param source A pointer to the source for the copy.
 * @param num The number of bytes to copy.
 * @return @p destination
 */
void *memcpy(void *destination, void *source, int num);

/**
 * Sets a region of memory to a specific value. Equivalent to the C standard
 * library function with the same name.
 * 
 * Fills the region pointed to by @p ptr with @p num bytes of value @p value
 * (zero-extended to a byte).
 *
 * @param ptr A pointer to the region of memory to fill.
 * @param value The value to fill the memory region with.
 * @param num The number of bytes to fill.
 * @return @p ptr
 */
void *memset(void *ptr, int value, int num);

#endif