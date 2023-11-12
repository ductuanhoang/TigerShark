/**
 * @file lv_port_disp_templ.c
 *
 */

 /*Copy this file as "lv_port_disp.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_disp_template.h"
#include "stm32l476xx.h"
/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void disp_init(void);

static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_disp_init(void)
{
    /*-------------------------
     * Initialize your display
     * -----------------------*/
    disp_init();

    /*-----------------------------
     * Create a buffer for drawing
     *----------------------------*/

    /* LVGL requires a buffer where it internally draws the widgets.
     * Later this buffer will passed your display drivers `flush_cb` to copy its content to your display.
     * The buffer has to be greater than 1 display row
     *
     * There are three buffering configurations:
     * 1. Create ONE buffer with some rows:
     *      LVGL will draw the display's content here and writes it to your display
     *
     * 2. Create TWO buffer with some rows:
     *      LVGL will draw the display's content to a buffer and writes it your display.
     *      You should use DMA to write the buffer's content to the display.
     *      It will enable LVGL to draw the next part of the screen to the other buffer while
     *      the data is being sent form the first buffer. It makes rendering and flushing parallel.
     *
     * 3. Create TWO screen-sized buffer:
     *      Similar to 2) but the buffer have to be screen sized. When LVGL is ready it will give the
     *      whole frame to display. This way you only need to change the frame buffer's address instead of
     *      copying the pixels.
     * */

    /* Example for 1) */
    // static lv_disp_buf_t draw_buf_dsc_1;
    // static lv_color_t draw_buf_1[LV_HOR_RES_MAX * 32];                          /*A buffer for 32 rows*/
    // lv_disp_buf_init(&draw_buf_dsc_1, draw_buf_1, NULL, LV_HOR_RES_MAX * 32);   /*Initialize the display buffer*/

    /* Example for 2) */       
    #define draw_buf_2_1      (lv_color_t*)(SRAM2_BASE)
    #define draw_buf_2_2      (lv_color_t*)(SRAM2_BASE +0x4000)
    static lv_disp_buf_t draw_buf_dsc_2;
    //static lv_color_t draw_buf_2_1[LV_HOR_RES_MAX * 32];                        /*A buffer for 32 rows*/
   // static lv_color_t draw_buf_2_2[LV_HOR_RES_MAX * 32];                        /*An other buffer for 32 rows*/
    lv_disp_buf_init(&draw_buf_dsc_2, draw_buf_2_1, draw_buf_2_2, LV_HOR_RES_MAX * 32);   /*Initialize the display buffer*/

    /* Example for 3) */
    // static lv_disp_buf_t draw_buf_dsc_3;
    // static lv_color_t draw_buf_3_1[LV_HOR_RES_MAX * LV_VER_RES_MAX];            /*A screen sized buffer*/
    // static lv_color_t draw_buf_3_2[LV_HOR_RES_MAX * LV_VER_RES_MAX];            /*An other screen sized buffer*/
    // lv_disp_buf_init(&draw_buf_dsc_3, draw_buf_3_1, draw_buf_3_2, LV_HOR_RES_MAX * LV_VER_RES_MAX);   /*Initialize the display buffer*/

    /*-----------------------------------
     * Register the display in LVGL
     *----------------------------------*/

    lv_disp_drv_t disp_drv;                         /*Descriptor of a display driver*/
    lv_disp_drv_init(&disp_drv);                    /*Basic initialization*/

    /*Set up the functions to access to your display*/

    /*Set the resolution of the display*/
    disp_drv.hor_res = LV_HOR_RES_MAX;
    disp_drv.ver_res = LV_VER_RES_MAX;
    disp_drv.rotated = LV_DISP_ROT_NONE;
    disp_drv.sw_rotate = 1;

    /*Used to copy the buffer's content to the display*/
    disp_drv.flush_cb = disp_flush;
    disp_drv.set_px_cb = 0;

    /*Set a display buffer*/
    disp_drv.buffer = &draw_buf_dsc_2;

    /*Finally register the driver*/
    lv_disp_drv_register(&disp_drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/* Initialize your display and the required peripherals. */
static void disp_init(void)
{
    /*You code here*/
    extern void ILI9341_Initial(void);
    ILI9341_Initial();
}

/* Flush the content of the internal buffer the specific area on the display
 * You can use DMA or any hardware acceleration to do this operation in the background but
 * 'lv_disp_flush_ready()' has to be called when finished. */
static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    /*The most simple case (but also the slowest) to put all pixels to the screen one-by-one*/
//    typedef union
//    {
//        struct
//        {
//            uint16_t blue : 6;
//            uint16_t green : 6;
//            uint16_t red : 6;
//            uint16_t padding : 14;
//        };
//        uint32_t full;
//    }lv_color18_t;
//
    int32_t x;
    int32_t y;
    extern void LCD_SetPos(unsigned char x0,unsigned char x1,unsigned int y0,unsigned int y1);
    extern void  Write_Data_U16(unsigned int data);

    if(area->x2 < 0) return;
    if(area->y2 < 0) return;
    if(area->x1 > LV_HOR_RES_MAX - 1) return;
    if(area->y1 > LV_VER_RES_MAX - 1) return;

    //SCB_CleanInvalidateDCache();
   // SCB_InvalidateICache();

    LCD_SetPos(area->x1, area->x2, area->y1, area->y2);
    for(y = area->y1; y <= area->y2; y++) {
        for(x = area->x1; x <= area->x2; x++) {
//            lv_color18_t pixel;
//            pixel.full = 0;
//
//            pixel.green = (uint16_t)(color_p->ch.green & 0x3F);
//            pixel.red = (uint16_t)((color_p->ch.red & 0x1F) << 1 | (color_p->ch.red & 0x1F) >> 4);
//            pixel.blue = (uint16_t)((color_p->ch.blue & 0x1F) << 1 | (color_p->ch.blue & 0x1F) >> 4);
//
//            pixel.full = pixel.full & 0x3FFFF;
            Write_Data_U16(color_p->full);
            color_p++;
        }
    }

    /* IMPORTANT!!!
     * Inform the graphics library that you are ready with the flushing*/
    lv_disp_flush_ready(disp_drv);
}
#else /* Enable this file at the top */

/* This dummy typedef exists purely to silence -Wpedantic. */
typedef int keep_pedantic_happy;
#endif
