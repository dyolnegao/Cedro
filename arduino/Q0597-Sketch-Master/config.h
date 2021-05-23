/************************ Adafruit IO Config *******************************/

// visit io.adafruit.com if you need to create an account,
// or if you need your Adafruit IO key.
#define IO_USERNAME   "dyoljordan"
#define IO_KEY        "aio_FKPa71BHgb9EK1xCt5vZl9AjNqcx"

/******************************* WIFI **************************************/

#define WIFI_SSID   "Dyol.net_2G"
#define WIFI_PASS   "Famengo09"

#include "AdafruitIO_WiFi.h"

AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);

/****************************************************************************/
