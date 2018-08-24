#include <application.h>

// LED instance
bc_led_t led;

// Button instance
bc_button_t button;

void button_event_handler(bc_button_t *self, bc_button_event_t event, void *event_param)
{
    if (event == BC_BUTTON_EVENT_PRESS)
    {
        bc_led_set_mode(&led, BC_LED_MODE_TOGGLE);
        bc_uart_init(BC_UART_UART1, BC_UART_BAUDRATE_115200, BC_UART_SETTING_8N1);
        char uart_tx[] = "AT\r\n";
        bc_uart_write(BC_UART_UART1, uart_tx, strlen(uart_tx));
    }
}

void application_init(void)
{
    // Initialize LED
    bc_led_init(&led, BC_GPIO_LED, false, false);
    bc_led_set_mode(&led, BC_LED_MODE_ON);

    // RESET of ESP8266
    bc_gpio_init(BC_GPIO_P6);
    bc_gpio_set_mode(BC_GPIO_P6, BC_GPIO_MODE_OUTPUT);
    bc_gpio_set_output(BC_GPIO_P6, 1);

    //CH_PD of ESP8266 
    bc_gpio_init(BC_GPIO_P8);
    bc_gpio_set_mode(BC_GPIO_P8, BC_GPIO_MODE_OUTPUT);
    bc_gpio_set_output(BC_GPIO_P8, 1);

    // Initialize button
    bc_button_init(&button, BC_GPIO_BUTTON, BC_GPIO_PULL_DOWN, false);
    bc_button_set_event_handler(&button, button_event_handler, NULL);

    // Initialize UART
    bc_uart_init(BC_UART_UART0, BC_UART_BAUDRATE_115200, BC_UART_SETTING_8N1);
}
