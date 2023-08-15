#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "Fat.h"
#include "funt.h"

/************************************* DEFINITION *******************************************/
#pragma pack(push) /* push current alignment to stack */
#pragma pack(1)
typedef struct
{
    uint8_t u8Jump[3];
    uint8_t u8OEM[8];
    uint16_t u16BytePerSec;
    uint8_t u8SecPerClus;
    uint16_t u16Reserve;
    uint8_t u8FatCount;
    uint16_t u16RootRegion;
    uint16_t u16TotalSec;
    uint8_t u8TypeOfDisk;
    uint16_t u16SecPerFat;
    uint16_t u16SecPerTrack;
    uint16_t u16NumOfHead;
    uint32_t u32HiddenSec;
    uint32_t u32TotalSec2;
    /* Extended */
    uint8_t u8DriveNumber;
    uint8_t u8Backup;
    uint8_t signature;
    uint32_t u32Series;
    uint8_t u8VolLabel[11];
    uint8_t u8FatType[8];
    uint8_t garbage[450];
}Boot_Sector;
#pragma pack(pop)

#pragma pack(push) /* push current alignment to stack */
#pragma pack(1)
typedef struct
{
    uint8_t u8Name[11];
    uint8_t u8Attributes;
    uint8_t u8Reserved;
    uint8_t u8CreatedTimeTenths;
    uint16_t u16CreatedTime;
    uint16_t u16CreatedDate;
    uint16_t u16AccessedDate;
    uint16_t u16FirstClusterHigh;
    uint16_t u16ModifiedTime;
    uint16_t u16ModifiedDate;
    uint16_t u16FirstClusterLow;
    uint32_t u32Size;
}DirectoryEntry;
#pragma pack(pop)
/************************************** END OF DIFINITION ****************************************/

/************************************** VARIABLE *******************************************/
FILE * fp;
uint8_t stt = 0;
uint16_t adress[200];
uint16_t cur = 0x00;
uint16_t pre = 0x00;
Boot_Sector g_boot;
DirectoryEntry *g_entry;
uint16_t SecOfRoot;
uint16_t SecOfData;
uint16_t SecOfFat;
uint8_t NumOfEntry;
uint8_t flagentry = 0;

/******************************************************************************************/

/******************************************************************************************
**CODE
******************************************************************************************/

int Init()
{
    int flag = 1;
    fp = fopen("file3.ima","rb");
    if(fp == NULL)
    {
        flag = 0;
    }
    return flag;
}
/*************************************** Brief *******************************************
    Funtion is used to initilize stream for handling file
*****************************************************************************************/

void Read_Boot()
{
    int i;
    fread(&g_boot,1,512,fp);
    SecOfRoot = (g_boot.u16RootRegion * 32)/g_boot.u16BytePerSec;
    SecOfFat = (g_boot.u16SecPerFat * g_boot.u8FatCount);
    SecOfData = (g_boot.u16TotalSec - SecOfFat - SecOfRoot - 1);
}
/*************************************** Brief *******************************************
    Funtion is used to Read the first sector of file (Boot Sector)
*****************************************************************************************/

void Print_Boot()
{
    int i;
    printf("Type Of FAT : ");
    for(i = 0;i < 8;i++)
    {
        printf("%c",g_boot.u8FatType[i]);
    }
    printf("\nTotal Sector: %d",g_boot.u16TotalSec);
    printf("\nByte Per Sector: %d",g_boot.u16BytePerSec);
    printf("\nSector Per Cluster: %d",g_boot.u8SecPerClus);
    printf("\nNum of FAT: %d",g_boot.u8FatCount);
    printf("\nSector Per Fat: %d",g_boot.u16SecPerFat);
    printf("\nNum of Sector in FAT: %d, From Sector 1 to Sector %d",\
    SecOfFat,SecOfFat);
    printf("\nNum of Sector in Root Directory: %d, From Sector %d to \
    Sector %d",SecOfRoot,SecOfFat + 1,SecOfFat + SecOfRoot);
    printf("\nNum of Sector in Data Region: %d, From Sector %d to \
    Sector %d",SecOfData,SecOfFat + SecOfRoot + 1,g_boot.u16TotalSec);
    printf("\n");
}
/*************************************** Brief *******************************************
    Funtion is used to Print Information of Boot Sector to Console
*****************************************************************************************/

void Read_Root(uint8_t SecNum)
{
    uint8_t sector[g_boot.u16BytePerSec];
    uint8_t entry = 0;
    NumOfEntry = g_boot.u16BytePerSec / 32;
    fseek(fp,SecNum*512,SEEK_SET);
    fread(sector,512,1,fp);
    g_entry =(DirectoryEntry *)malloc(sizeof(DirectoryEntry));
    int i = 0;
    int j = 0;
    for(entry = 0;entry < 16;entry++)
    {
        for(i = 0;i < 32;i++)
        {
            ((uint8_t *)g_entry)[i] = sector[i + entry*32];
        }
        if(g_entry->u8Attributes != 0x0F && g_entry->u8Name[0] != 0x00)
        {
            printf("%d\t",stt);
            adress[stt] = g_entry->u16FirstClusterLow;
            for(j = 0;j < 11;j++)
            {
                printf("%c",g_entry->u8Name[j]);
            }
            if(g_entry->u32Size != 0)
            {
                printf("\t %d",g_entry->u32Size);
            }
            else
            {
                printf("\t");
            }
            fat_time_modify(g_entry->u16ModifiedTime,g_entry->u16ModifiedDate);
            fat_time_create(g_entry->u16CreatedTime,g_entry->u16CreatedDate);
            if(g_entry->u8Attributes == 0x10)
            {
                printf("\tDirectory");
            }
            printf("\n");
            stt++;
        }
    }
    pre = 0;
    cur = 0;
    free(g_entry);
    return;
}
/*************************************** Brief *******************************************
    Funtion is used to Read the Root Directory
*****************************************************************************************/

void Read_Sector(uint16_t SecNum)
{
    uint8_t sector[g_boot.u16BytePerSec];
    uint8_t entrytmp[32];
    uint8_t entry = 0;
    NumOfEntry = g_boot.u16BytePerSec / 32;
    fseek(fp,SecNum*512,SEEK_SET);
    fread(sector,512,1,fp);
    int i = 0;
    int j = 0;
    int check = 0;
    g_entry =(DirectoryEntry *)malloc(sizeof(DirectoryEntry));
    for(entry = 0;entry <16;entry++)
    {
        for(i = 0;i < 32;i++)
        {
            ((uint8_t *)g_entry)[i] = sector[i + entry*32];
            entrytmp[i] = sector[i + entry*32];
        }
        if(g_entry->u8Name[0] == 0x00)
        {
            entry = 16;
        }
        else if(g_entry->u8Name[0] == '.' && g_entry->u8Name[1] != '.' && entry == 0)
        {
            cur = g_entry->u16FirstClusterLow;
        }
        else if(g_entry->u8Name[0] == '.'&& g_entry->u8Name[1]== '.')
        {
            pre = g_entry->u16FirstClusterLow;
            flagentry = 1;
        }
        else if((g_entry->u8Attributes == 0x10 || g_entry->u8Attributes ==0x00) \
        && flagentry == 1 && g_entry->u8Name[0] != '.')
        {
            printf("\n");
            printf("%d\t",stt);
            //adress =(uint16_t *)malloc(sizeof(uint16_t));
            adress[stt] = g_entry->u16FirstClusterLow;
            for(j = 0;j < 11;j++)
            {
                printf("%c",g_entry->u8Name[j]);
            }
            printf("\t %d",g_entry->u32Size);
            fat_time_modify(g_entry->u16ModifiedTime,g_entry->u16ModifiedDate);
            fat_time_create(g_entry->u16CreatedTime,g_entry->u16CreatedDate);
            if(g_entry->u8Attributes == 0x10)
            {
                printf("\tDirectory");
            }
            printf("\n");
            stt++;
        }
        else if(g_entry->u8Attributes != 0x0F)
        {
            for(j = 0;j < 32;j++)
            {
                printf("%c",entrytmp[j]);
            }
        }
    }
    free(g_entry);
    return;
}
/*************************************** Brief *******************************************
    Funtion is used to Read 1 Sector in File
*****************************************************************************************/

uint16_t Read_Fat(uint16_t index)
{
    uint8_t * Fat;
    Fat =(uint8_t *)malloc(3*512);
    index = index + 1;
    int seek = index / 768;
    fseek(fp,512 + seek*3*512,SEEK_SET);
    fread(Fat,1,3*512,fp);
    if(seek >= 1)
    {
        index = index - seek*768;
    }
    if(index % 2 == 0)
    {
        index = (index * 3)/2 - 1;
        index = READ_EVEN(Fat,index);
    }
    else
    {
        index = (index * 3)/2;
        index = READ_ODD(Fat,index);
    }
    free(Fat);
    return index;
}
/*************************************** Brief *******************************************
    Funtion is used to Read the Fat Table in File
*****************************************************************************************/

void menu()
{
    int choice;
    int flag = 1;
    uint16_t clus;
    uint16_t NextClus;
    int i = 0;
    while(flag)
    {
        printf("\n------------------------------------------------------------\n\n\n");
        printf("1.In ra thong tin Boot Sector");
        printf("\n2.Vao Root Directory");
        printf("\n3.Mo File");
        printf("\n4.Back");
        printf("\n5.Thoat");
        printf("\nNhap lua chon: ");
        scanf("%d",&choice);
        switch(choice)
        {
            case 1:
            {
                Print_Boot();
                break;
            }
            case 2:
            {
                printf("STT|    |   Name   |    |Size|  |   Modified        |   |   Created         |\t|Attribute|\n");
                for(i = 0;i < SecOfRoot;i++)
                {
                    Read_Root(SecOfFat + 1 + i);
                }
                stt = 0;
                break;
            }
            case 3:
            {
                stt = 0;
                printf("Nhap So Thu Tu Cua File: ");
                scanf("%d",&choice);
                NextClus = adress[choice];
                NextClus = Read_Fat(NextClus);
                printf("STT|    |   Name   |    |Size|  |   Modified        |   |   Created         |\t|Attribute|\n");
                clus = ( adress[choice] - 2) * g_boot.u8SecPerClus + (SecOfRoot\
                + SecOfFat + 1);                
                for(i = 0;i < g_boot.u8SecPerClus;i++)
                {
                    Read_Sector(clus);
                    clus++;
                }
                if(flagentry == 0)
                {
                    pre = cur;
                }
                while(NextClus != 0xfff)
                {
                    clus = (NextClus - 2)*g_boot.u8SecPerClus +(SecOfRoot\
                    + SecOfFat + 1);
                    printf("Fat: %d\n",NextClus);
                    for(i = 0;i < g_boot.u8SecPerClus;i++)
                    {
                        Read_Sector(clus);
                        clus++;
                    }
                    NextClus = Read_Fat(NextClus);
                }
                stt = 0;
                flagentry = 0;
                break;
            }
            case 4:
            {
                if(pre == 0x00)
                {
                    for(i = 0;i < SecOfRoot;i++)
                    {
                        Read_Root(1+SecOfFat + i);
                    }
                }
                else
                {
                    flagentry = 0;
                    NextClus = Read_Fat(pre);
                    clus = (pre - 2)*g_boot.u8SecPerClus + SecOfFat + SecOfRoot + 1;
                    for(i = 0;i < g_boot.u8SecPerClus;i++)
                    {
                        Read_Sector(clus);
                        clus++;
                    }
                    if(flagentry == 0)
                    {
                        pre = cur;
                    }
                    while(NextClus != 0xfff)
                    {
                        clus = (NextClus - 2)*g_boot.u8SecPerClus +(SecOfRoot\
                        + SecOfFat + 1);
                        for(i = 0;i < g_boot.u8SecPerClus;i++)
                        {
                            Read_Sector(clus);
                            clus++;
                        }
                        NextClus = Read_Fat(NextClus);
                    }
                }
                stt = 0;
                flagentry = 0;
                break;
            }
            case 5:
            {
                flag = 0;
                break;
            }
            default:
            {
                printf("\nLua chon khong hop le,Vui long nhap lai...");
                break;
            }
        }
    }
    fclose(fp);
    return;
}
/*************************************** Brief *******************************************
    Funtion is used to create UI and print to console
*****************************************************************************************/
