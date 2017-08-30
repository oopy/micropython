#include "py/mpconfig.h"
#if MICROPY_BOARD_ESPY8285

#include <stdio.h>

#include "py/runtime.h"
#include "modmachine.h"
#include "esp_mphal.h"
#include "hspi.h"
#include "esphc595.h"

#define HC595_BAUDRATE  (1000000L)
#define GPIO_MODE_OUTPUT (1)

STATIC uint8_t global_value;

void esp_hc595_init(){
    printf("esp_hc595_init function begin\n");
    uint32_t divider = 40000000L / HC595_BAUDRATE; 
    uint32_t prediv = MIN(divider, SPI_CLKDIV_PRE + 1);
    uint16_t cntdiv = (divider / prediv) * 2;
    if (cntdiv > SPI_CLKCNT_N + 1 || cntdiv == 0 || prediv == 0){
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError,
                    "hc595 error baudrate"));
    }
    //uint32_t baudrate = 80000000L / (prediv * cntdiv);
    spi_init_gpio(HSPI, SPI_CLK_USE_DIV);
    spi_clock(HSPI, prediv, cntdiv);
    spi_tx_byte_order(HSPI, SPI_BYTE_ORDER_HIGH_TO_LOW);
    spi_rx_byte_order(HSPI, SPI_BYTE_ORDER_HIGH_TO_LOW);
    CLEAR_PERI_REG_MASK(SPI_USER(HSPI), SPI_FLASH_MODE | SPI_USR_MISO |
                        SPI_USR_ADDR | SPI_USR_COMMAND | SPI_USR_DUMMY);
    // Clear Dual or Quad lines transmission mode
    CLEAR_PERI_REG_MASK(SPI_CTRL(HSPI), SPI_QIO_MODE | SPI_DIO_MODE | SPI_DOUT_MODE | SPI_QOUT_MODE);
    spi_mode(HSPI, 0, 0);
    esp_hc595_write(0x00);
    printf("esp_hc595_init function end\n");
}

void esp_hc595_test(){
}
void esp_hc595_write(uint8_t value){
    global_value = value;
    spi_tx8fast(HSPI, global_value);
    pin_set(RCK, 0);
    mp_hal_delay_us(10);
    pin_set(RCK, 1);
    mp_hal_delay_us(10);
}

uint8_t esp_hc595_read(void){
    return global_value;
}

void esp_hc595_bit_write(uint8_t bit, uint8_t value){
    if (value == 0){
        global_value &= ~(1<<bit);
    }else if (value == 1){
        global_value |= (1<<bit);
    }
    esp_hc595_write(global_value);
    printf("global_value = %d\n", value);
}

uint8_t esp_hc595_bit_read(uint8_t bit){
    return (global_value >> bit)&0x01;
}

#endif
