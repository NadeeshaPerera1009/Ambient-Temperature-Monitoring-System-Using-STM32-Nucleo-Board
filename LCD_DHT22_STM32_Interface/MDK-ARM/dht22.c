#include "dht22.h"

extern TIM_HandleTypeDef htim6;
#define DHT22_PORT GPIOA
#define DHT22_PIN GPIO_PIN_1

void delay_us(uint16_t us) {
    __HAL_TIM_SET_COUNTER(&htim6, 0);
    while (__HAL_TIM_GET_COUNTER(&htim6) < us);
}

void DHT22_SetPinOutput(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = DHT22_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(DHT22_PORT, &GPIO_InitStruct);
}

void DHT22_SetPinInput(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = DHT22_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(DHT22_PORT, &GPIO_InitStruct);
}

uint8_t DHT22_ReadData(DHT22_Data *data) {
    uint8_t bits[5] = {0};
    uint32_t rawHumidity, rawTemperature;

    DHT22_SetPinOutput();
    HAL_GPIO_WritePin(DHT22_PORT, DHT22_PIN, GPIO_PIN_RESET);
    HAL_Delay(1);  // at least 1ms
    HAL_GPIO_WritePin(DHT22_PORT, DHT22_PIN, GPIO_PIN_SET);
    delay_us(30);
    DHT22_SetPinInput();

    if (HAL_GPIO_ReadPin(DHT22_PORT, DHT22_PIN)) return 1;
    while (!HAL_GPIO_ReadPin(DHT22_PORT, DHT22_PIN)); // wait high
    while (HAL_GPIO_ReadPin(DHT22_PORT, DHT22_PIN));  // wait low

    for (int j = 0; j < 5; j++) {
        for (int i = 0; i < 8; i++) {
            while (!HAL_GPIO_ReadPin(DHT22_PORT, DHT22_PIN));
            delay_us(40);
            if (HAL_GPIO_ReadPin(DHT22_PORT, DHT22_PIN))
                bits[j] |= (1 << (7 - i));
            while (HAL_GPIO_ReadPin(DHT22_PORT, DHT22_PIN));
        }
    }

    rawHumidity = (bits[0] << 8) | bits[1];
    rawTemperature = (bits[2] << 8) | bits[3];
    if (rawTemperature & 0x8000) {
        rawTemperature = rawTemperature & 0x7FFF;
        data->Temperature = (float)rawTemperature / 10.0 * -1.0;
    } else {
        data->Temperature = (float)rawTemperature / 10.0;
    }
    data->Humidity = (float)rawHumidity / 10.0;

    uint8_t checksum = bits[0] + bits[1] + bits[2] + bits[3];
    if (checksum != bits[4]) return 2;

    return 0;
}
