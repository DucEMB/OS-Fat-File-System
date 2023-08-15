#ifndef FAT_H
#define FAT_H
#include <stdint.h>

/************************************* MACRO **********************************************/
#define READ_EVEN(array,index)\
    ((array[index - 1] >> 4) | array[index] << 4)

#define READ_ODD(array,index)\
    (((array[index] & 0x0F) << 8) | array[index - 1])

/*******************************************************************************************/

/************************************* PROTOTYPE *******************************************/
int Init();
void Read_Boot();
void Print_Boot();
void Read_Root(uint8_t SecNum);
void Read_Sector(uint16_t SecNum);
uint16_t Read_Fat(uint16_t index);
void menu();

/******************************************************************************************/
#endif
