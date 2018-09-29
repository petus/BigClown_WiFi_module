#ifndef _ESP_AT_command_H
#define _ESP_AT_command_H

volatile char uart_tx_rn[]="\r\n";

volatile char uart_tx_AT[]="AT";
volatile char uart_tx_CIPSTART[]="AT+CIPSTART=";
volatile char uart_tx_CIPSEND[]="AT+CIPSEND=";
volatile char uart_tx_CIPCLOSE[]="AT+CIPCLOSE";

volatile char uart_tx_CWMODE[]="AT+CWMODE=3";
volatile char uart_tx_CIPMUX[]="AT+CIPMUX=0";
volatile char uart_tx_CWJAP[]="AT+CWJAP=";

volatile char uart_tx_http_header[]=" HTTP/1.1\r\nHost: ";
volatile char uart_tx_rnrnrn[]="\r\n\r\n\r\n"; 

#endif // _ESP_AT_command_H