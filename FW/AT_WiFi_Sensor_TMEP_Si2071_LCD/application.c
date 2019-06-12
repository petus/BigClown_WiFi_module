#include <application.h>
#include <stdio.h>
#include <string.h>
#include "ESP_AT_command.h"

#define BLACK true

// LED instance
bc_led_t led;

// Temp
volatile float temperature = 0.0;
float voltage_value = 0;

static char host[]="HOSTNAME.tmep.cz";
static char url[]="GET /index.php?tempC=";
static char SSID[]="\"SSID\",";
static char PASSWORD[]="\"PASSWORD\"";

volatile char uart_tx[120]={0};
volatile char uart_rx[100]={0};

bc_i2c_transfer_t rx_transfer;
bc_i2c_transfer_t tx_transfer;
uint8_t tx_buffer[] = {0xE3}; // request for measurement

void turnOnESP(void)
{
    // CH_PD of ESP8266 
    bc_gpio_init(BC_GPIO_P8);
    bc_gpio_set_mode(BC_GPIO_P8, BC_GPIO_MODE_OUTPUT);
    bc_gpio_set_output(BC_GPIO_P8, 1);

    // RESET of ESP8266
    bc_gpio_init(BC_GPIO_P6);
    bc_gpio_set_mode(BC_GPIO_P6, BC_GPIO_MODE_OUTPUT);
    bc_gpio_set_output(BC_GPIO_P6, 1);

    bc_led_set_mode(&led, BC_LED_MODE_ON);
}

void turnOffESP(void)
{
    // CH_PD of ESP8266 
    bc_gpio_init(BC_GPIO_P8);
    bc_gpio_set_mode(BC_GPIO_P8, BC_GPIO_MODE_OUTPUT);
    bc_gpio_set_output(BC_GPIO_P8, 0);

    bc_led_set_mode(&led, BC_LED_MODE_OFF);
}

static void sendTemperature(void* param)
{    
    uint8_t rx_buffer[2] = {0x00};
    float temperature = 0x00;
    uint8_t str[50] = {0x00};

    // Toggle LED
    bc_led_set_mode(&led, BC_LED_MODE_TOGGLE);

    /********************************************
    *                                           *
    *               Temperature                 *
    *                                           *
    *********************************************/

    tx_transfer.device_address = 0x40;      // slave address
    tx_transfer.buffer = tx_buffer;         // 0xE3 - request for measurement
    tx_transfer.length = sizeof(tx_buffer);

    bc_i2c_write(BC_I2C_I2C0, &tx_transfer); // write

    rx_transfer.device_address = 0x40;      // slave address
    rx_transfer.buffer = rx_buffer;         // rx buffer - temperature
    rx_transfer.length = sizeof(rx_buffer);

    bc_i2c_read(BC_I2C_I2C0, &rx_transfer); // read

    temperature = ((rx_buffer[0] << 8) | rx_buffer[1]);    // sum MSB and LSB values

	temperature = ((175.72 * temperature)/65536) - 46.85;   // formula from datasheet

    char buffer[30]={0};
    char bufferBat[30]={0};
    sprintf(buffer, "%.2f", temperature);


    /********************************************
    *                                           *
    *                   LCD                     *
    *                                           *
    *********************************************/
    
    // clear LCD module
    bc_module_lcd_clear();

    // Init default font
    bc_module_lcd_set_font(&bc_font_ubuntu_33);

    // Temperature
    bc_module_lcd_draw_string(5, 30, &buffer[0], BLACK);

    // Init default font
    bc_module_lcd_set_font(&bc_font_ubuntu_15);

    //Temperature
    bc_module_lcd_draw_string(5, 10, "Teplota ", BLACK);
    bc_module_lcd_draw_string(5, 60, "°C", BLACK);


    // Battery
    sprintf(bufferBat, "%02.2f", voltage_value);
    bc_module_lcd_draw_string(3, 90, "Bat: ", BLACK);
    bc_module_lcd_draw_string(45, 90, &bufferBat[0], BLACK);
    bc_module_lcd_draw_string(85, 90, "V", BLACK);

    bc_module_lcd_draw_line(0, 108, 128, 108, BLACK);
    bc_module_lcd_draw_string(25, 110, "chiptron.cz", BLACK);

    bc_module_lcd_update();

    bc_uart_write(BC_UART_UART2, &buffer, strlen(buffer)); // send rx buffer through UART2

    turnOnESP();
    
    for(uint32_t i=0; i<0xFFFFF; i++)
    { ; }

    /* Send AT */
    uart_tx[0]='\0';
    strcat(uart_tx, uart_tx_AT);
    strcat(uart_tx, uart_tx_rn);
    uart_rx[0]='\0';
    bc_uart_write(BC_UART_UART1, uart_tx, strlen(uart_tx));
    bc_uart_read(BC_UART_UART1, uart_rx, sizeof(uart_rx), 1000);
    if(memcmp(uart_tx, uart_rx, sizeof(uart_tx)) != 0)
    {
        //turnOffESP();
    }
    else
    {
        
    }
    
    
    /* Send CIPSTART */
    uart_tx[0]='\0';
    strcat(uart_tx, uart_tx_CIPSTART);
    strcat(uart_tx, "\"TCP\",\"");
    strcat(uart_tx, host);
    strcat(uart_tx, "\",80");
    strcat(uart_tx, uart_tx_rn);
    uart_rx[0]='\0';
    bc_uart_write(BC_UART_UART1, uart_tx, strlen(uart_tx));
    bc_uart_read(BC_UART_UART1, uart_rx, sizeof(uart_rx), 5000);

    /* Send CIPSEND */
    uart_tx[0]='\0';
    strcat(uart_tx, uart_tx_CIPSEND);
    strcat(uart_tx, "62");
    strcat(uart_tx, uart_tx_rn);
    uart_rx[0]='\0';
    bc_uart_write(BC_UART_UART1, uart_tx, strlen(uart_tx));
    bc_uart_read(BC_UART_UART1, uart_rx, sizeof(uart_rx), 5000);

    /* Send HTTP GET request */
    uart_tx[0]='\0';
    strcpy(uart_tx, url);
    strcat(uart_tx, buffer);
    strcat(uart_tx, uart_tx_http_header);
    strcat(uart_tx, host);
    strcat(uart_tx, uart_tx_rnrnrn);
    
    uart_rx[0]='\0';
    bc_uart_write(BC_UART_UART1, uart_tx, strlen(uart_tx));
    bc_uart_read(BC_UART_UART1, uart_rx, sizeof(uart_rx), 2000);

    /* Send CIPCLOSE */
    uart_tx[0]='\0';
    strcat(uart_tx, uart_tx_CIPCLOSE);
    strcat(uart_tx, uart_tx_rn);
    uart_rx[0]='\0';
    bc_uart_write(BC_UART_UART1, uart_tx, strlen(uart_tx));
    bc_uart_read(BC_UART_UART1, uart_rx, sizeof(uart_rx), 1000);

    turnOffESP();

    // Execute the task
    bc_scheduler_plan_current_from_now(60000);
}


void application_init()
{
    // Initialize LED
    bc_led_init(&led, BC_GPIO_LED, false, false);
    bc_led_set_mode(&led, BC_LED_MODE_ON);

    // Initialize UART
    bc_uart_init(BC_UART_UART1, BC_UART_BAUDRATE_115200, BC_UART_SETTING_8N1);

    // Initialize UART
    bc_uart_init(BC_UART_UART2, BC_UART_BAUDRATE_115200, BC_UART_SETTING_8N1);

    // Initialize I2C0
    bc_i2c_init(BC_I2C_I2C0, BC_I2C_SPEED_100_KHZ);

    // Initialize LCD
    bc_module_lcd_init();

    // Initialize battery
    bc_module_battery_init();
    bc_module_battery_set_update_interval(10000);

    turnOnESP();
    
    for(uint32_t i=0; i<0xFFFFF; i++)
    { ; }

    /* Send CWMODE */
    uart_tx[0]='\0';
    strcat(uart_tx, uart_tx_CWMODE);
    strcat(uart_tx, uart_tx_rn);
    uart_rx[0]='\0';
    bc_uart_write(BC_UART_UART1, uart_tx, strlen(uart_tx));
    bc_uart_read(BC_UART_UART1, uart_rx, sizeof(uart_rx), 2000);

    /* Send CIPMUX */
    uart_tx[0]='\0';
    strcat(uart_tx, uart_tx_CIPMUX);
    strcat(uart_tx, uart_tx_rn);
    uart_rx[0]='\0';
    bc_uart_write(BC_UART_UART1, uart_tx, strlen(uart_tx));
    bc_uart_read(BC_UART_UART1, uart_rx, sizeof(uart_rx), 2000);

    /* Send CWJAP */
    uart_tx[0]='\0';
    strcat(uart_tx, uart_tx_CWJAP);
    strcat(uart_tx, SSID);
    strcat(uart_tx, PASSWORD);
    strcat(uart_tx, uart_tx_rn);
    uart_rx[0]='\0';
    bc_uart_write(BC_UART_UART1, uart_tx, strlen(uart_tx));
    bc_uart_read(BC_UART_UART1, uart_rx, sizeof(uart_rx), 5000);

    turnOffESP();

    // Timming
    bc_scheduler_register(sendTemperature, NULL, bc_tick_get() + 3000);
}
