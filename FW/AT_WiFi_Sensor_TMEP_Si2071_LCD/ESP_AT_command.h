#ifndef _ESP_AT_command_H
#define _ESP_AT_command_H

static char uart_tx_rn[]="\r\n";

static char uart_tx_AT[]="AT";
static char uart_tx_CIPSTART[]="AT+CIPSTART=";
static char uart_tx_CIPSEND[]="AT+CIPSEND=";
static char uart_tx_CIPCLOSE[]="AT+CIPCLOSE";

static char uart_tx_CWMODE[]="AT+CWMODE=3";
static char uart_tx_CIPMUX[]="AT+CIPMUX=0";
static char uart_tx_CWJAP[]="AT+CWJAP=";

static char uart_tx_http_header[]=" HTTP/1.1\r\nHost: ";
static char uart_tx_rnrnrn[]="\r\n\r\n\r\n"; 

#endif // _ESP_AT_command_H