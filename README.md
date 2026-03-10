# ZPHR-STM32-0007

* Board: Blackpill + display
* MCU: STM32F411RE

# Zephyr

* Zephyr Version: v4.3.0
* Zephyr SDK: v0.17.4

# Peripherals

* Display: ILI9341 (320*240)
* sensor Touch: FT6236

## Display Pinout (2.8" TFT SPI 240x320 Capacitive Touch V1.0)

| No | Display Pin | MCU Pin | Function                 |
|----|-------------|---------|--------------------------|
| 14 | SD_CS       | -       | SD card (not used)       |
| 13 | CTP_INT     | PB6     | Touch interrupt          |
| 12 | CTP_SDA     | PB3     | I2C2 SDA (touch)         |
| 11 | CTP_RST     | PB2     | Touch reset              |
| 10 | CTP_SCL     | PB10    | I2C2 SCL (touch)         |
| 9  | SDO(MISO)   | PA6     | SPI1 MISO                |
| 8  | LED         | PB7     | Backlight (TIM4 CH2 PWM) |
| 7  | SCK         | PA5     | SPI1 SCK                 |
| 6  | SDI(MOSI)   | PA7     | SPI1 MOSI                |
| 5  | LCD_RS      | PB0     | Data/Command (DC)        |
| 4  | LCD_RST     | PB1     | Display reset            |
| 3  | LCD_CS      | PA15     | SPI1 CS (chip select)    |
| 2  | GND         | GND     | Ground                   |
| 1  | VCC         | 3.3V    | Power                    |
