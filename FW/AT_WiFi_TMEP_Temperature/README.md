WiFi module with built-in temperature TMP112</br>
Temperature TMP112 sensor (I2C0) is built-in BigClown Core module</br>
and the sensor sends temperature to BigClown and then the BigClown </br>
sends the temperature to tmep.eu or tmep.cz by WiFi conection (WiFi module)</br>
CH_EN pin of ESP8266 module is connected GPIO_P8</br>
Reset pin of ESP8266 module is connected to GPIO_P6</br>
Created by Petus (2019)</br>
(czech)</br>
WiFi module https://chiptron.cz/articles.php?article_id=182</br>
(english)</br>
WiFi module https://time4ee.com/articles.php?article_id=113</br>
</br>
Fill in the</br>
static char host[]="HOSTNAME.tmep.eu";</br>
static char SSID[]="\"SSID\",";</br>
static char PASSWORD[]="\"PASSWORD\"";</br>
</br>
FW uses the TMP112 sensor on Core module 
