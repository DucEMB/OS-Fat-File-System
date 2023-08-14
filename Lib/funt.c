#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

void fat_time_modify(uint16_t time,uint16_t date)
{
    int hour, minute, second, day, month, year;
    year = ((date >> 9) & 0x7F) + 1980;
    month = (date >> 5) & 0x0F;
    day = date & 0x1F;
    hour = (time >> 11) & 0x1F;
    minute = (time >> 5) & 0x3F;
    second = (time & 0x1F) * 2;
    
    printf("\t %02d:%02d:%02d %02d/%02d/%d",\
    hour, minute, second, day, month, year);
    return;
}

void fat_time_create(uint16_t time,uint16_t date)
{
    int hour, minute, second, day, month, year;
    hour = (time & 0xF800) >> 11;
    minute = (time & 0x07E0) >> 5;
    second = (time & 0x001F) << 1;
    day = (date & 0x1F);
    month = ((date & 0x01E0) >> 5);
    year = ((date & 0xFE00) >> 9) + 1980;
    printf("\t %02d:%02d:%02d %02d/%02d/%d",\
    hour, minute, second, day, month, year);
    return;
}
