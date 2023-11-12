#include "spi_ffs.h"

spiffs fs;
u8_t spiffs_work_buf[SPIFFS_CFG_LOG_PAGE_SZ(ignore)*2];
u8_t spiffs_fds[32*4];
u8_t spiffs_cache_buf[(SPIFFS_CFG_LOG_PAGE_SZ(ignore)+32)*4];

uint8_t buff2[20];

s32_t my_spiffs_read(u32_t addr, u32_t size, u8_t *dst) 
{
    BSP_W25Qx_Read(dst,addr,size);
    return SPIFFS_OK;
}

s32_t my_spiffs_write(u32_t addr, u32_t size, u8_t *src)
{
    BSP_W25Qx_Write(src,addr,size);
    return SPIFFS_OK;
}

s32_t my_spiffs_erase(u32_t addr, u32_t size)
{
    BSP_W25Qx_Erase_Block(addr);
    return SPIFFS_OK;
} 

int my_spiffs_mount(void)
{
    spiffs_config cfg;
    cfg.hal_read_f = my_spiffs_read;
    cfg.hal_write_f = my_spiffs_write;
    cfg.hal_erase_f = my_spiffs_erase;
    
    int res = SPIFFS_mount(&fs,
      &cfg,
      spiffs_work_buf,
      spiffs_fds,
      sizeof(spiffs_fds),
      spiffs_cache_buf,
      sizeof(spiffs_cache_buf),
      0);
    if(res != 0)
    {
       SPIFFS_unmount(&fs);
      // SPIFFS_format(&fs);
       //BSP_W25Qx_Erase_Chip();
      res = SPIFFS_mount(&fs,
      &cfg,
      spiffs_work_buf,
      spiffs_fds,
      sizeof(spiffs_fds),
      spiffs_cache_buf,
      sizeof(spiffs_cache_buf),
      0);
    }
    return res;
}
int spiffs_flash_init(void)
{
    BSP_W25Qx_Init();
    int res = my_spiffs_mount();
    return res;
}
//
void test_spiffs() 
{
// Surely, I've mounted spiffs before entering here

    spiffs_file fd = SPIFFS_open(&fs, "my_file", SPIFFS_CREAT | SPIFFS_TRUNC | SPIFFS_RDWR, 0);
    if (SPIFFS_write(&fs, fd, (u8_t *)"Hello world", 224) < 0) printf("errno %i\n", SPIFFS_errno(&fs));
    SPIFFS_close(&fs, fd); 

    fd = SPIFFS_open(&fs, "my_file", SPIFFS_RDWR, 0);
    if (SPIFFS_read(&fs, fd, (u8_t *)buff2, 12) < 0) printf("errno %i\n", SPIFFS_errno(&fs));
    SPIFFS_close(&fs, fd);

    // printf("--> %s <--\n", buf);
}