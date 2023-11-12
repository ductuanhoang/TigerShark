#include "ui_func_com.h"

ui_screen_active_t screen_active_flag;

void ui_convert_to_sensor(uint8_t *array, uint16_t value)
{
  array[0] = '0' + value%1000 / 100;
  array[1] = '.';
  array[2] = '0' + value%100 / 10;
  array[3] = '0' + value%10 / 1;
  array[4] = '\0'; 
}

void ui_convert_to_compass(uint8_t *array, uint16_t value)
{
    if(value >= 100)
    {
        array[0] = '0' + value/100;
        array[1] = '0' + value%100 /10;
        array[2] = '0' + value%10;
        array[3] = '\0';
    }
    else if(value >= 10)
    {
        array[0] = '0' + value/10;
        array[1] = '0' + value%10;
        array[2] = '\0';
    }
    else
    {
        array[0] = '0' + value;
        array[1] = '\0';
    }
}

void ui_convert_to_diagnotics_sensor(uint8_t *array, uint16_t value)
{
  array[0] = '0' + value%1000 / 100;
  array[1] = '0' + value%100 / 10;
  array[2] = '.';
  array[3] = '0' + value%10 / 1;
  array[4] = '\0';  
}

uint8_t ui_convert_to_distance(uint8_t *array, uint16_t value)
{
  uint8_t index = 0;
  array[index++] = '0' + value/ 10;
  array[index++] = '.';
  array[index++] = '0' + value%10;
 // array[index++] = '\0';
    return index;
}

void ui_convert_setpoint(uint8_t *array, uint16_t value,uint16_t mode)
{
  if(mode == 0)
  {
    array[0] = '0' + value%1000 / 100;
    array[1] = '.';
    array[2] = '0' + value%100 / 10;
    array[3] = '0' + value%10 / 1;
    array[4] = '\0';
  }
  else
  {
    array[0] = '-';
    array[1] = 'O';
    array[2] = 'C' ;
    array[3] = '-' ;
    array[4] = '\0';
  }
  
}
uint8_t ui_convert_to_int3(uint8_t *array, int16_t value)
{
  uint8_t index = 0;
  if(value < 0)
  {
    value = value * (-1); 
    array[index++] = '-';
    if(value > 99)
    {    
       array[index++] = '0' + value%1000 / 100;
       
    }
    if(value > 9)
    {
        array[index++] = '0' + value%100 / 10;
        
    }
    array[index++] = '0' + value%10 / 1;
    array[index] = 0; 
  }
  else
  {
    if(value > 999)
        array[index++] = '0' + value%10000 / 1000;
    if(value > 99)
        array[index++] = '0' + value%1000 / 100;
    if(value > 9)
        array[index++] = '0' + value%100 / 10;
    array[index++] = '0' + value%10 / 1;
    array[index] = 0;
  }
  return index;
}
uint8_t ui_convert_to_diving_time(uint8_t *array, uint32_t value)
{
     uint8_t index = 0;
     uint8_t hour = 0;
     uint8_t minu = 0;
     uint8_t sec = 0;
     hour = value/3600;
     minu = (value/60) % 60;
     sec = value%60;
     {
        if(hour/10)
        {
          array[index++] = '0' + hour/10;  
        }
        if(hour >0)
        {
          array[index++] = '0' + hour%10;  
          array[index++] = ':';
          array[index++] = '0' + minu/10;
          array[index++] = '0' + minu/10;
        }
        else
        {
            if((minu <10) && (hour==0))
            {
              //array[index++] = '0' + minu/10;  
            }
            else
            {
             array[index++] = '0' + minu/10;   
            }
            array[index++] = '0' + minu%10;
            array[index++] = ':';
            
            array[index++] = '0' + sec/ 10;
            array[index++] = '0' + sec%10;
        }
        array[index] = 0;       
     }
   return index;
}
uint8_t ui_convert_to_dive_time(uint8_t *array, uint32_t value)
{
     uint8_t index = 0;
     uint16_t hour = 0;
     uint16_t minu = 0;
     hour = value/3600;
     minu = (value/60) % 60;
     if(value >= 60)
     {
       if(hour)
       {
         
           if(hour/100)
           {
              array[index++] = '0' + (hour/100)%10;  
           }
           
           if(hour/10)
           {
              array[index++] = '0' + (hour/10)%10;  
           }
              array[index++] = '0' + hour%10;
              array[index++] = ':';
             array[index++] = '0' + minu/10;   
            array[index++] = '0' + minu%10;
            array[index] = 0;
       }
       else
       {
         if(minu/10)
         {
          array[index++] = '0' + minu/10; 
         }
          array[index++] = '0' + minu%10;
          array[index] = 0;
       }
         
     }
     else
     {
        array[index++] = '0';
        array[index] = 0; 
     }
   return index;
}