/* The example of ESP-IDF
 *
 * This sample code is in the public domain.
 */
#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

// You have to set these CONFIG value using menuconfig.
#if CONFIG_SPI2
static const int GPIO_MISO = 12;
static const int GPIO_MOSI = 13;
static const int GPIO_SCLK = 14;
#endif

#if CONFIG_SPI3
static const int GPIO_MISO = 19;
static const int GPIO_MOSI = 23;
static const int GPIO_SCLK = 18;
#endif

#include "w25q64.h"

#define tag "W25Q64"

//
// 書込みデータのダンプリスト
// dt(in) : データ格納先頭アドレス
// n(in)  : 表示データ数
//
void dump(uint8_t *dt, int n)
{
	uint16_t clm = 0;
	uint8_t data;
	uint8_t sum;
	uint8_t vsum[16];
	uint8_t total =0;
	uint32_t saddr =0;
	uint32_t eaddr =n-1;

	printf("----------------------------------------------------------\n");
	uint16_t i;
	for (i=0;i<16;i++) vsum[i]=0;  
	uint32_t addr;
	for (addr = saddr; addr <= eaddr; addr++) {
		data = dt[addr];
		if (clm == 0) {
			sum =0;
			printf("%05x: ",addr);
		}

		sum+=data;
		vsum[addr % 16]+=data;

		printf("%02x ",data);
		clm++;
		if (clm == 16) {
			printf("|%02x \n",sum);
			clm = 0;
		}
	}
	printf("----------------------------------------------------------\n");
	printf("       ");
	for (i=0; i<16;i++) {
		total+=vsum[i];
		printf("%02x ",vsum[i]);
	}
	printf("|%02x \n\n",total);
}

void app_main()
{
	ESP_LOGI(tag, "CONFIG_GPIO_CS=%d", CONFIG_GPIO_CS);
	W25Q64_t dev;
	spi_master_init(&dev, CONFIG_GPIO_CS, GPIO_MISO, GPIO_MOSI, GPIO_SCLK);

	// ステータスレジスタ1の取得
	// Get fron Status Register1
	uint8_t reg1;
	esp_err_t ret;
	ret = W25Q64_readStatusReg1(&dev, &reg1);
	if (ret != ESP_OK) {
		ESP_LOGE(tag, "readStatusReg1 fail %d",ret);
		while(1) { vTaskDelay(1); }
	} 
	ESP_LOGI(tag, "readStatusReg1 : %x", reg1);
	
	// ステータスレジスタ2の取得
	// Get fron Status Register2
	uint8_t reg2;
	ret = W25Q64_readStatusReg2(&dev, &reg2);
	if (ret != ESP_OK) {
		ESP_LOGE(tag, "readStatusReg2 fail %d",ret);
		while(1) { vTaskDelay(1); }
	}
	ESP_LOGI(tag, "readStatusReg2 : %x", reg2);

	// Unique IDの取得テスト
	// Get Unique ID
	uint8_t uid[8];
	ret = W25Q64_readUniqieID(&dev, uid);
	if (ret != ESP_OK) {
		ESP_LOGE(tag, "readUniqieID fail %d",ret);
		while(1) { vTaskDelay(1); }
	}
	ESP_LOGI(tag, "readUniqieID : %x-%x-%x-%x-%x-%x-%x-%x",
		 uid[0], uid[1], uid[2], uid[3], uid[4], uid[5], uid[6], uid[7]);

	// JEDEC IDの取得テスト
	// Get JEDEC ID
	uint8_t jid[3];
	ret = W25Q64_readManufacturer(&dev, jid);
	if (ret != ESP_OK) {
		ESP_LOGE(tag, "readManufacturer fail %d",ret);
		while(1) { vTaskDelay(1); }
	}
	ESP_LOGI(tag, "readManufacturer : %x-%x-%x",
		 jid[0], jid[1], jid[2]);

	// データの読み込み(アドレス0から256バイト取得)
	// Read 256 byte data from Address=0
	uint8_t rbuf[256];    // 取得データ
	int len;
	memset(rbuf, 0, 256);
	len =  W25Q64_fastread(&dev, 0, rbuf, 256);
	if (len != 256) {
		ESP_LOGE(tag, "fastread fail");
		while(1) { vTaskDelay(1); }
	}
	ESP_LOGI(tag, "Fast Read Data: len=%d", len);
	dump(rbuf, 256);

	// セクタ単位の削除
	// Erase data by Sector
	bool flag = W25Q64_eraseSector(&dev, 0, true);
	if (flag == false) {
		ESP_LOGE(tag, "eraseSector fail %d",ret);
		while(1) { vTaskDelay(1); }
	}
	memset(rbuf, 0, 256);
	len =  W25Q64_read(&dev, 0, rbuf, 256);
	if (len != 256) {
		ESP_LOGE(tag, "read fail");
		while(1) { vTaskDelay(1); }
	}
	ESP_LOGI(tag, "Read Data: len=%d", len);
	dump(rbuf, 256);

	// データ書き込みテスト
	// Write data to Sector=0 Address=10
	uint8_t wdata[26];    // 書込みデータ
	for (int i=0; i<26; i++) {
		wdata[i]='A'+i; // A-Z     
	}  
	len =  W25Q64_pageWrite(&dev, 0, 10, wdata, 26);
	if (len != 26) {
		ESP_LOGE(tag, "pageWrite fail");
		while(1) { vTaskDelay(1); }
	}
	ESP_LOGI(tag, "Page Write(Sector=0 Address=10) len=%d", len);

	// 高速データの読み込み(アドレス0から256バイト取得)
	// First read 256 byte data from Address=0
	memset(rbuf, 0, 256);
	len =  W25Q64_fastread(&dev, 0, rbuf, 256);
	if (len != 256) {
		ESP_LOGE(tag, "fastread fail");
		while(1) { vTaskDelay(1); }
	}
	ESP_LOGI(tag, "Fast Read Data: len=%d", len);
	dump(rbuf, 256);

	// データ書き込みテスト
	// Write data to Sector=0 Address=0
	for (int i=0; i < 10;i++) {
		wdata[i]='0'+i; // 0-9     
	}  
	len =  W25Q64_pageWrite(&dev, 0, 0, wdata, 10);
	if (len != 10) {
		ESP_LOGE(tag, "pageWrite fail");
		while(1) { vTaskDelay(1); }
	}
	ESP_LOGI(tag, "Page Write(Sector=0 Address=0) len=%d", len);

	// 高速データの読み込み(アドレス0から256バイト取得)
	// First read 256 byte data from Address=0
	memset(rbuf, 0, 256);
	len =  W25Q64_fastread(&dev, 0, rbuf, 256);
	if (len != 256) {
		ESP_LOGE(tag, "fastread fail");
		while(1) { vTaskDelay(1); }
	}
	ESP_LOGI(tag, "Fast Read Data: len=%d", len);
	dump(rbuf, 256);

	ESP_LOGI(tag, "Success All Test");
}

