#include "main.h"
#include "nrf24l01.h"

extern SPI_HandleTypeDef hspi1;

static void error() {
    while (true) {
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        HAL_Delay(100);
    }
}

static const uint8_t rx_address[5] = {1, 2, 3, 4, 5};
static const uint8_t tx_address[5] = {1, 2, 3, 4, 6};

void example() {
    nrf24l01 nrf;
    uint32_t rx_data;

    {
        nrf24l01_config config;
        config.data_rate        = NRF_DATA_RATE_1MBPS;
        config.tx_power         = NRF_TX_PWR_0dBm;
        config.crc_width        = NRF_CRC_WIDTH_1B;
        config.addr_width       = NRF_ADDR_WIDTH_5;
        config.payload_length   = 4;    // maximum is 32 bytes
        config.retransmit_count = 15;   // maximum is 15 times
        config.retransmit_delay = 0x0F; // 4000us, LSB:250us
        config.rf_channel       = 0;
        config.rx_address       = rx_address;
        config.tx_address       = tx_address;
        config.rx_buffer        = (uint8_t*)&rx_data;

        config.spi         = &hspi1;
        config.spi_timeout = 10; // milliseconds
        config.ce_port     = GPIOA;
        config.ce_pin      = GPIO_PIN_3;
        config.irq_port    = GPIOA;
        config.irq_pin     = GPIO_PIN_4;

        nrf_init(&nrf, &config);
    }

    uint32_t tx_data = 0xDEADBEEF;

    nrf_send_packet(&nrf, (uint8_t*)&tx_data);

    while (true) {
        nrf_receive_packet(&nrf);
        if (tx_data != rx_data) error();
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        HAL_Delay(500);
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    }
}
