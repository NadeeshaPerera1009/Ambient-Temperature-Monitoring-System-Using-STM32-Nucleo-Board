#ifndef INC_DHT22_H_
#define INC_DHT22_H_

#include "main.h"

typedef struct {
    float Temperature;
    float Humidity;
} DHT22_Data;

uint8_t DHT22_ReadData(DHT22_Data *data);

#endif /* INC_DHT22_H_ */
