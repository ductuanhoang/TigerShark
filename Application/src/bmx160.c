#include "bmx160.h"

#include "i2c.h"

#define BMX160_SLAVE_ADDR 0x68

#define BMX160_REG_DATA 0x04
#define BMX160_REG_MAG_CONF 0x44
#define BMX160_REG_MAG_IF_0 0x4c
#define BMX160_REG_MAG_IF_1 0x4d
#define BMX160_REG_MAG_IF_2 0x4e
#define BMX160_REG_MAG_IF_3 0x4f
#define BMX160_REG_CMD 0x7e

#define BMX160_CMD_SOFT_RESET 0xb6
#define BMX160_CMD_ACC_NORMAL 0x11
#define BMX160_CMD_GYR_NORMAL 0x15
#define BMX160_CMD_MAG_NORMAL 0x19
#define BMX160_CMD_MAG_LOW_POWER 0x1A

#define BMX160_MAG_RES_X 0.0351 // +- 1150 uT / 16 bits
#define BMX160_MAG_RES_Y 0.0351 // +- 1150 uT / 16 bits
#define BMX160_MAG_RES_Z 0.0763 // +- 2500 uT / 16 bits

#define BMX160_ACC_RES 0.000061035 // +- 2g / 16 bits

#define BMX160_GYR_RES 0.061035 // +- 2000dps / 16 bits

#define BMX160_DATA_SIZE 20 // Tamaño del buffer de datos

/**
 * @brief Escribe un registro n (1 byte) en el sensor
 * 
 * @param addr Dirección del registro
 * @param byte Dato a escribir
 */
void bmx160_write(uint8_t addr, uint8_t byte) 
{
    HAL_I2C_Master_Transmit(&hi2c1,addr,&byte,1,100);
}

/**
 * @brief Ejecuta un comando en el sensor
 * 
 * @param cmd Comando a ejecutar
 */
void bmx160_write_cmd(uint8_t cmd) {
    bmx160_write(BMX160_REG_CMD, cmd);
}

/**
 * @brief Inicializa el sensor BMX160, actualmente solo se utiliza el magnetómetro.
 * 
 */
void bmx160_init() {    
    // Soft reset del sensor
    bmx160_write_cmd(BMX160_CMD_SOFT_RESET);
    HAL_Delay(1);

    // Habilitar acelerómetro
    bmx160_write_cmd(BMX160_CMD_ACC_NORMAL);
    HAL_Delay(1);

    // Habilitar giroscopio
    bmx160_write_cmd(BMX160_CMD_GYR_NORMAL);
    HAL_Delay(1);

    // Habilitar magnetómetro
    bmx160_write_cmd(BMX160_CMD_MAG_NORMAL);
   HAL_Delay(1);

    // Configurar magnetómetro (pag. 25 datasheet)
    bmx160_write(BMX160_REG_MAG_IF_0, 0x80);
    HAL_Delay(1);
    // Sleep mode
    bmx160_write(BMX160_REG_MAG_IF_3, 0x01);
    bmx160_write(BMX160_REG_MAG_IF_2, 0x4b);
    // REPXY regular preset
    bmx160_write(BMX160_REG_MAG_IF_3, 0x04);
    bmx160_write(BMX160_REG_MAG_IF_2, 0x51);
    // REPZ regular preset
    bmx160_write(BMX160_REG_MAG_IF_3, 0x0E);
    bmx160_write(BMX160_REG_MAG_IF_2, 0x52);
    // Prepare for data mode
    bmx160_write(BMX160_REG_MAG_IF_3, 0x02);
    bmx160_write(BMX160_REG_MAG_IF_2, 0x4C);
    bmx160_write(BMX160_REG_MAG_IF_1, 0x42);
    // Frecuencia de trabajo 12.5Hz
    bmx160_write(BMX160_REG_MAG_CONF, 0x05);
    // Data mode
    bmx160_write(BMX160_REG_MAG_IF_0, 0x00);
    // Pasar a low-power
    bmx160_write_cmd(BMX160_CMD_MAG_NORMAL);
    HAL_Delay(1);
}

/**
 * @brief Lee los datos en crudo del sensor
 * 
 * @param buffer Buffer donde guardar los datos (tamaño mínimo BMX160_DATA_SIZE)
 */
void bmx160_read_raw(uint8_t *buffer) 
{
  uint8_t reg = BMX160_REG_DATA;
   HAL_I2C_Master_Transmit(&hi2c1,BMX160_SLAVE_ADDR,&reg,1,100);
    // Realizar lectura
   HAL_I2C_Master_Receive(&hi2c1,BMX160_SLAVE_ADDR,buffer,BMX160_DATA_SIZE,100);
}

/**
 * @brief Lee los datos del sensor (procesados y convertidos)
 * 
 * @return sensor_t Datos leidos
 */
bmx160_data_t bmx160_read() {
    // Leer datos crudos del sensor
    uint8_t buffer[BMX160_DATA_SIZE];
    bmx160_read_raw(buffer);

    // Convertir datos a estructura
    bmx160_data_t data;

    int16_t x, y, z;

    // Magnetómetro
    x = (int16_t) (((uint16_t)buffer[1] << 8) | buffer[0]);
    y = (int16_t) (((uint16_t)buffer[3] << 8) | buffer[2]);
    z = (int16_t) (((uint16_t)buffer[5] << 8) | buffer[4]);

    data.mag.x = x * BMX160_MAG_RES_X;
    data.mag.y = y * BMX160_MAG_RES_Y;
    data.mag.z = z * BMX160_MAG_RES_Z;

    // Sensor Hall
    // @todo
    data.hall = 0.0;

    // Magnetómetro
    x = (int16_t) (((uint16_t)buffer[9] << 8) | buffer[8]);
    y = (int16_t) (((uint16_t)buffer[11] << 8) | buffer[10]);
    z = (int16_t) (((uint16_t)buffer[13] << 8) | buffer[12]);

    data.mag.x = x * BMX160_ACC_RES;
    data.mag.y = y * BMX160_ACC_RES;
    data.mag.z = z * BMX160_ACC_RES;

    // Magnetómetro
    x = (int16_t) (((uint16_t)buffer[15] << 8) | buffer[14]);
    y = (int16_t) (((uint16_t)buffer[17] << 8) | buffer[16]);
    z = (int16_t) (((uint16_t)buffer[19] << 8) | buffer[18]);

    data.mag.x = x * BMX160_GYR_RES;
    data.mag.y = y * BMX160_GYR_RES;
    data.mag.z = z * BMX160_GYR_RES;

    return data;
}