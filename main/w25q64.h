#ifndef MAIN_W25Q64_H_
#define MAIN_W25Q64_H_

#include "driver/spi_master.h"

#define MAX_BLOCKSIZE         128  // ブロック総数
#define MAX_SECTORSIZE        2048 // 総セクタ数

#define CMD_WRITE_ENABLE      0x06
#define CMD_WRITE_DISABLE     0x04
#define CMD_READ_STATUS_R1    0x05
#define CMD_READ_STATUS_R2    0x35
#define CMD_WRITE_STATUS_R    0x01 // 未実装
#define CMD_PAGE_PROGRAM      0x02
#define CMD_QUAD_PAGE_PROGRAM 0x32 // 未実装
#define CMD_BLOCK_ERASE64KB   0xd8
#define CMD_BLOCK_ERASE32KB   0x52
#define CMD_SECTOR_ERASE      0x20
#define CMD_CHIP_ERASE        0xC7
#define CMD_ERASE_SUPPEND     0x75 // 未実装
#define CMD_ERASE_RESUME      0x7A // 未実装
#define CMD_POWER_DOWN        0xB9
#define CMD_HIGH_PERFORM_MODE 0xA3 // 未実装
#define CMD_CNT_READ_MODE_RST 0xFF // 未実装
#define CMD_RELEASE_PDOWN_ID  0xAB // 未実装
#define CMD_MANUFACURER_ID    0x90
#define CMD_READ_UNIQUE_ID    0x4B
#define CMD_JEDEC_ID          0x9f

#define CMD_READ_DATA         0x03
#define CMD_FAST_READ         0x0B
#define CMD_READ_DUAL_OUTPUT  0x3B // 未実装
#define CMD_READ_DUAL_IO      0xBB // 未実装
#define CMD_READ_QUAD_OUTPUT  0x6B // 未実装
#define CMD_READ_QUAD_IO      0xEB // 未実装
#define CMD_WORD_READ         0xE3 // 未実装

#define SR1_BUSY_MASK	0x01
#define SR1_WEN_MASK	0x02

typedef struct {
	uint8_t statusReg1;
	uint8_t statusReg2;
	spi_device_handle_t _SPIHandle;
} W25Q64_t;


void Dump(char *id, int ret, uint8_t *data, int len);
void spi_master_init(W25Q64_t * dev, int GPIO_CS);
esp_err_t W25Q64_readStatusReg1(W25Q64_t * dev, uint8_t * reg1);
esp_err_t W25Q64_readStatusReg2(W25Q64_t * dev, uint8_t * reg2);
esp_err_t W25Q64_readUniqieID(W25Q64_t * dev, uint8_t * id);
esp_err_t W25Q64_readManufacturer(W25Q64_t * dev, uint8_t * id);
bool W25Q64_IsBusy(W25Q64_t * dev);
esp_err_t W25Q64_powerDown(W25Q64_t * dev);
esp_err_t W25Q64_WriteEnable(W25Q64_t * dev);
esp_err_t W25Q64_WriteDisable(W25Q64_t * dev);
uint16_t W25Q64_read(W25Q64_t * dev, uint32_t addr, uint8_t *buf, uint16_t n);
uint16_t W25Q64_fastread(W25Q64_t * dev, uint32_t addr, uint8_t *buf, uint16_t n);
bool W25Q64_eraseSector(W25Q64_t * dev, uint16_t sect_no, bool flgwait);
bool W25Q64_erase64Block(W25Q64_t * dev, uint16_t blk_no, bool flgwait);
bool W25Q64_erase32Block(W25Q64_t * dev, uint16_t blk_no, bool flgwait);
bool W25Q64_eraseAll(W25Q64_t * dev, bool flgwait);
int16_t W25Q64_pageWrite(W25Q64_t * dev, uint16_t sect_no, uint16_t inaddr, uint8_t* buf, int16_t n);

#endif /* MAIN_W25Q64_H_ */

