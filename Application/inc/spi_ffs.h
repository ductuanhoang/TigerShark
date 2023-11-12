#ifndef __SPI_FFS_H__
#define __SPI_FFS_H__

#include "main.h"
#include "W25Qx.h"
#include "spiffs.h"
#include "spiffs.h"

int spiffs_flash_init(void);
void test_spiffs() ;

extern spiffs fs;
#endif