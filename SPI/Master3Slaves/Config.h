#pragma once

#define SCK						13
#define MISO					12
#define MOSI					11
#define SS						10
#define SLAVE_SELECT			SS

#define SLAVE_READY				9
#define DATA_OUT				MOSI
#define DATA_IN					MISO

#define DELAY_100NS				do { asm volatile("nop"); } while (0);
#define DELAY_SPI(X)			{int ii=0; do {asm volatile("nop"); } while (++ii<X); }

#define ROLE_IS_MASTER			true
#define ROLE_IS_SLAVE_RED		false
#define ROLE_IS_SLAVE_GREEN		false
#define ROLE_IS_SLAVE_BLUE		false

#define MASTER_CS_RED_PIN		4
#define MASTER_CS_GREEN_PIN		5
#define MASTER_CS_BLUE_PIN		6
#define MASTER_RED_LED_PIN		A3
#define MASTER_GREEN_LED_PIN	A4
#define MASTER_BLUE_LED_PIN		A5

#define SLAVE_RED_CS_PIN		3
#define SLAVE_RED_LED_PIN		8

#define SLAVE_BLUE_CS_PIN		3
#define SLAVE_BLUE_LED_PIN		8

#define SLAVE_GREEN_CS_PIN		3
#define SLAVE_GREEN_LED_PIN		8

#define FLASH_TIMES				10
#define FLASH_DURATION			50

