#include <application.h>
#include <stdio.h>
#include <string.h>
#include "ESP_AT_command.h"

// LED instance
bc_led_t led;

// Temp instance
bc_tmp112_t temp;
volatile float temperature = 0.0;

volatile char host[]="NAME.tmep.eu";
volatile char url[]="GET /index.php?tempC=";
volatile char SSID[]="\"SSID\",";
volatile char PASSWORD[]="\"PASSWORD\"";

volatile char uart_tx[120]={0};
volatile char uart_rx[100]={0};

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
    // RESET of ESP8266
    bc_gpio_init(BC_GPIO_P6);
    bc_gpio_set_mode(BC_GPIO_P6, BC_GPIO_MODE_OUTPUT);
    bc_gpio_set_output(BC_GPIO_P6, 0);

    // CH_PD of ESP8266 
    bc_gpio_init(BC_GPIO_P8);
    bc_gpio_set_mode(BC_GPIO_P8, BC_GPIO_MODE_OUTPUT);
    bc_gpio_set_output(BC_GPIO_P8, 0);

    bc_led_set_mode(&led, BC_LED_MODE_OFF);
}

void tmp112_event_handler(bc_tmp112_t *self, bc_tmp112_event_t event, void *event_param)
{
    (void) self;
    (void) event_param;

    if (event == BC_TMP112_EVENT_UPDATE)
    {
        bc_tmp112_get_temperature_celsius(&temp, &temperature);
    }
}

static void sendTemperature(void* param)
{    
    bc_tmp112_measure(&temp);

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
    bc_uart_read(BC_UART_UART1, uart_rx, sizeof(uart_rx), 2000);

    /* Send CIPSEND */
    uart_tx[0]='\0';
    strcat(uart_tx, uart_tx_CIPSEND);
    strcat(uart_tx, "62");
    strcat(uart_tx, uart_tx_rn);
    uart_rx[0]='\0';
    bc_uart_write(BC_UART_UART1, uart_tx, strlen(uart_tx));
    bc_uart_read(BC_UART_UART1, uart_rx, sizeof(uart_rx), 5000);

    // Temperature sensor
    bc_tmp112_get_temperature_celsius(&temp, &temperature);
    char buffer[30]={0};
    sprintf(buffer, "%.2f", temperature);

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
    bc_scheduler_plan_current_from_now(30000);
}


void application_init()
{
    // Initialize LED
    bc_led_init(&led, BC_GPIO_LED, false, false);
    bc_led_set_mode(&led, BC_LED_MODE_ON);

    // Initialize UART
    bc_uart_init(BC_UART_UART1, BC_UART_BAUDRATE_115200, BC_UART_SETTING_8N1);

    // Temperature sensor - TMP112
    bc_tmp112_init(&temp, BC_I2C_I2C0, 0x49);

    // set measurement handler (call "tmp112_event_handler()" after measurement)
    bc_tmp112_set_event_handler(&temp, tmp112_event_handler, NULL);

    bc_tmp112_set_update_interval(&temp, 5000);
    //bc_tmp112_measure(&temp);

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
