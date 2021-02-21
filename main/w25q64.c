#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <driver/spi_master.h>
#include <driver/gpio.h>
#include "esp_log.h"

#include "w25q64.h"

#define tag	"W25Q64"
#define _DEBUG_	0

#if CONFIG_IDF_TARGET_ESP32
#define LCD_HOST    HSPI_HOST
#define DMA_CHAN    2

#elif CONFIG_IDF_TARGET_ESP32S2
#define LCD_HOST    SPI2_HOST
#define DMA_CHAN    LCD_HOST
#endif

//static const int SPI_Command_Mode = 0;
//static const int SPI_Data_Mode = 1;
static const int SPI_Frequency = 1000000;

void W25Q64_dump(char *id, int ret, uint8_t *data, int len)
{
	int i;
	printf("[%s] = %d\n",id, ret);
	for(i=0;i<len;i++) {
		printf("%0x ",data[i]);
		if ( (i % 10) == 9) printf("\n");
	}
	printf("\n");
}


void spi_master_init(W25Q64_t * dev, int GPIO_CS, int GPIO_MISO, int GPIO_MOSI, int GPIO_SCLK)
{
	esp_err_t ret;

	gpio_pad_select_gpio( GPIO_CS );
	gpio_set_direction( GPIO_CS, GPIO_MODE_OUTPUT );
	gpio_set_level( GPIO_CS, 0 );

	spi_bus_config_t spi_bus_config = {
		.sclk_io_num = GPIO_SCLK,
		.mosi_io_num = GPIO_MOSI,
		.miso_io_num = GPIO_MISO,
		.quadwp_io_num = -1,
		.quadhd_io_num = -1
	};

	ret = spi_bus_initialize( LCD_HOST, &spi_bus_config, DMA_CHAN );
	if(_DEBUG_)ESP_LOGI(tag, "spi_bus_initialize=%d",ret);
	assert(ret==ESP_OK);

	spi_device_interface_config_t devcfg;
	memset( &devcfg, 0, sizeof( spi_device_interface_config_t ) );
	devcfg.clock_speed_hz = SPI_Frequency;
	devcfg.spics_io_num = GPIO_CS;
	devcfg.queue_size = 1;

	spi_device_handle_t handle;
	ret = spi_bus_add_device( LCD_HOST, &devcfg, &handle);
	if(_DEBUG_)ESP_LOGI(tag, "spi_bus_add_device=%d",ret);
	assert(ret==ESP_OK);
	dev->_SPIHandle = handle;
}

//
// ステータスレジスタ1の値取得
// reg1(out):ステータスレジスタ1の値
//
esp_err_t W25Q64_readStatusReg1(W25Q64_t * dev, uint8_t * reg1)
{
	spi_transaction_t SPITransaction;
	uint8_t data[2];
	data[0] = CMD_READ_STATUS_R1;
	memset( &SPITransaction, 0, sizeof( spi_transaction_t ) );
	SPITransaction.length = 2 * 8;
	SPITransaction.tx_buffer = data;
	SPITransaction.rx_buffer = data;
	esp_err_t ret = spi_device_transmit( dev->_SPIHandle, &SPITransaction );
	assert(ret==ESP_OK);
	if(_DEBUG_)ESP_LOGI(tag, "W25Q64_readStatusReg1=%x",data[1]);
	*reg1 = data[1];
	return ret;
}

//
// ステータスレジスタ2の値取得
// reg2(out):ステータスレジスタ2の値
//
esp_err_t W25Q64_readStatusReg2(W25Q64_t * dev, uint8_t * reg2)
{
	spi_transaction_t SPITransaction;
	uint8_t data[2];
	data[0] = CMD_READ_STATUS_R2;
	memset( &SPITransaction, 0, sizeof( spi_transaction_t ) );
	SPITransaction.length = 2 * 8;
	SPITransaction.tx_buffer = data;
	SPITransaction.rx_buffer = data;
	esp_err_t ret = spi_device_transmit( dev->_SPIHandle, &SPITransaction );
	assert(ret==ESP_OK);
	if(_DEBUG_)ESP_LOGI(tag, "W25Q64_readStatusReg2=%x",data[1]);
	*reg2 = data[1];
	return ret;
}

//
// Unique IDの取得
// id(out):Unique ID 8バイトを返す  
//
esp_err_t W25Q64_readUniqieID(W25Q64_t * dev, uint8_t * id)
{
	spi_transaction_t SPITransaction;
	uint8_t data[13];
	data[0] = CMD_READ_UNIQUE_ID;
	memset( &SPITransaction, 0, sizeof( spi_transaction_t ) );
	SPITransaction.length = 13 * 8;
	SPITransaction.tx_buffer = data;
	SPITransaction.rx_buffer = data;
	esp_err_t ret = spi_device_transmit( dev->_SPIHandle, &SPITransaction );
	assert(ret==ESP_OK);
	if(_DEBUG_)W25Q64_dump("readUniqieID", ret, data, 13);
	memcpy(id, &data[5], 8);
	return ret ;
}

//
// JEDEC ID(Manufacture, Memory Type,Capacity)の取得
// d(out) :Manufacture, Memory Type,Capacityの3バイトを格納する
//
esp_err_t W25Q64_readManufacturer(W25Q64_t * dev, uint8_t * id)
{
	spi_transaction_t SPITransaction;
	uint8_t data[4];
	data[0] = CMD_JEDEC_ID;
	memset( &SPITransaction, 0, sizeof( spi_transaction_t ) );
	SPITransaction.length = 4 * 8;
	SPITransaction.tx_buffer = data;
	SPITransaction.rx_buffer = data;
	esp_err_t ret = spi_device_transmit( dev->_SPIHandle, &SPITransaction );
	assert(ret==ESP_OK);
	if(_DEBUG_)W25Q64_dump("readManufacturer", ret, data, 4);
	memcpy(id, &data[1], 3);
	return ret ;
}

//
// 書込み等の処理中チェック
// 戻り値: true:作業 、false:アイドル中
//
bool W25Q64_IsBusy(W25Q64_t * dev)
{
	spi_transaction_t SPITransaction;
	uint8_t data[2];
	data[0] = CMD_READ_STATUS_R1;
	memset( &SPITransaction, 0, sizeof( spi_transaction_t ) );
	SPITransaction.length = 2 * 8;
	SPITransaction.tx_buffer = data;
	SPITransaction.rx_buffer = data;
	esp_err_t ret = spi_device_transmit( dev->_SPIHandle, &SPITransaction );
	assert(ret==ESP_OK);
	if (ret != ESP_OK) return false;
	if( (data[1] & SR1_BUSY_MASK) != 0) return true;
	return false;
}


//
//　パワーダウン指定 
//
esp_err_t W25Q64_powerDown(W25Q64_t * dev)
{
	spi_transaction_t SPITransaction;
	uint8_t data[1];
	data[0] = CMD_POWER_DOWN;
	memset( &SPITransaction, 0, sizeof( spi_transaction_t ) );
	SPITransaction.length = 1 * 8;
	SPITransaction.tx_buffer = data;
	SPITransaction.rx_buffer = data;
	esp_err_t ret = spi_device_transmit( dev->_SPIHandle, &SPITransaction );
	assert(ret==ESP_OK);
	return ret;
}


//
// 書込み許可設定
//
esp_err_t W25Q64_WriteEnable(W25Q64_t * dev)
{
	spi_transaction_t SPITransaction;
	uint8_t data[1];
	data[0] = CMD_WRITE_ENABLE;
	memset( &SPITransaction, 0, sizeof( spi_transaction_t ) );
	SPITransaction.length = 1 * 8;
	SPITransaction.tx_buffer = data;
	SPITransaction.rx_buffer = data;
	esp_err_t ret = spi_device_transmit( dev->_SPIHandle, &SPITransaction );
	assert(ret==ESP_OK);
	return ret;
}


//
// 書込み禁止設定
//
esp_err_t W25Q64_WriteDisable(W25Q64_t * dev)
{
	spi_transaction_t SPITransaction;
	uint8_t data[1];
	data[0] = CMD_WRITE_DISABLE;
	memset( &SPITransaction, 0, sizeof( spi_transaction_t ) );
	SPITransaction.length = 1 * 8;
	SPITransaction.tx_buffer = data;
	SPITransaction.rx_buffer = data;
	esp_err_t ret = spi_device_transmit( dev->_SPIHandle, &SPITransaction );
	assert(ret==ESP_OK);
	return ret;
}

//
// データの読み込み
// addr(in): 読込開始アドレス (24ビット 0x000000 - 0xFFFFFF)
// n(in):読込データ数
//
uint16_t W25Q64_read(W25Q64_t * dev, uint32_t addr, uint8_t *buf, uint16_t n)
{ 
	spi_transaction_t SPITransaction;
	uint8_t *data;
	data = (uint8_t *)malloc(n+4);
	data[0] = CMD_READ_DATA;
	data[1] = (addr>>16) & 0xFF;     // A23-A16
	data[2] = (addr>>8) & 0xFF;      // A15-A08
	data[3] = addr & 0xFF;           // A07-A00
	memset( &SPITransaction, 0, sizeof( spi_transaction_t ) );
	SPITransaction.length = (n+4) * 8;
	SPITransaction.tx_buffer = data;
	SPITransaction.rx_buffer = data;
	esp_err_t ret = spi_device_transmit( dev->_SPIHandle, &SPITransaction );
	assert(ret==ESP_OK);
	memcpy(buf, &data[4], n);
	free(data);
	if (ret != ESP_OK) return 0;
	return n;
}

//
// 高速データの読み込み
// addr(in): 読込開始アドレス (24ビット 0x00000 - 0xFFFFF)
// n(in):読込データ数
//
uint16_t W25Q64_fastread(W25Q64_t * dev, uint32_t addr, uint8_t *buf, uint16_t n)
{
	spi_transaction_t SPITransaction;
	uint8_t *data;
	data = (uint8_t *)malloc(n+5);
	data[0] = CMD_FAST_READ;
	data[1] = (addr>>16) & 0xFF;     // A23-A16
	data[2] = (addr>>8) & 0xFF;      // A15-A08
	data[3] = addr & 0xFF;           // A07-A00
	data[4] = 0;
	memset( &SPITransaction, 0, sizeof( spi_transaction_t ) );
	SPITransaction.length = (n+5) * 8;
	SPITransaction.tx_buffer = data;
	SPITransaction.rx_buffer = data;
	esp_err_t ret = spi_device_transmit( dev->_SPIHandle, &SPITransaction );
	assert(ret==ESP_OK);
	memcpy(buf, &data[5], n);
	free(data);
	if (ret != ESP_OK) return 0;
	return n;
}


//
// セクタ単位消去(4kb空間単位でデータの消去を行う)
// sect_no(in) セクタ番号(0 - 2048)
// flgwait(in) true:処理待ちを行う false:待ち無し
// 戻り値: true:正常終了 false:失敗
//  補足： データシートでは消去に通常 30ms 、最大400msかかると記載されている
//         アドレス23ビットのうち上位 11ビットがセクタ番号の相当する。下位12ビットはセクタ内アドレスとなる。
//
bool W25Q64_eraseSector(W25Q64_t * dev, uint16_t sect_no, bool flgwait)
{
	spi_transaction_t SPITransaction;
	uint8_t data[4];
	uint32_t addr = sect_no;
	addr<<=12;

	// 書込み許可設定
	esp_err_t ret;
	ret = W25Q64_WriteEnable(dev);
	if (ret != ESP_OK) return false;

	data[0] = CMD_SECTOR_ERASE;
	data[1] = (addr>>16) & 0xff;
	data[2] = (addr>>8) & 0xff;
	data[3] = addr & 0xff;
	memset( &SPITransaction, 0, sizeof( spi_transaction_t ) );
	SPITransaction.length = 4 * 8;
	SPITransaction.tx_buffer = data;
	SPITransaction.rx_buffer = data;
	ret = spi_device_transmit( dev->_SPIHandle, &SPITransaction );
	assert(ret==ESP_OK);
	if (ret != ESP_OK) return false;

	// 処理待ち
	while( W25Q64_IsBusy(dev) & flgwait) {
		vTaskDelay(1);
	}
	return true;
}

//
// 64KBブロック単位消去(64kb空間単位でデータの消去を行う)
// blk_no(in) ブロック番号(0 - 127)
// flgwait(in) true:処理待ちを行う false:待ち無し
// 戻り値: true:正常終了 false:失敗
//   補足: データシートでは消去に通常 150ms 、最大1000msかかると記載されている
//         アドレス23ビットのうち上位 7ビットがブロックの相当する。下位16ビットはブロック内アドレスとなる。
//
bool W25Q64_erase64Block(W25Q64_t * dev, uint16_t blk_no, bool flgwait)
{
	spi_transaction_t SPITransaction;
	uint8_t data[4];
	uint32_t addr = blk_no;
	addr<<=16;

	// 書込み許可設定
	esp_err_t ret;
	ret = W25Q64_WriteEnable(dev);
	if (ret != ESP_OK) return false;

	data[0] = CMD_BLOCK_ERASE64KB;
	data[1] = (addr>>16) & 0xff;
	data[2] = (addr>>8) & 0xff;
	data[3] = addr & 0xff;
	memset( &SPITransaction, 0, sizeof( spi_transaction_t ) );
	SPITransaction.length = 4 * 8;
	SPITransaction.tx_buffer = data;
	SPITransaction.rx_buffer = data;
	ret = spi_device_transmit( dev->_SPIHandle, &SPITransaction );
	assert(ret==ESP_OK);
	if (ret != ESP_OK) return false;

	// 処理待ち
	while( W25Q64_IsBusy(dev) & flgwait) {
		vTaskDelay(1);
	}
	return true;
}

//
// 32KBブロック単位消去(32kb空間単位でデータの消去を行う)
// blk_no(in) ブロック番号(0 - 255)
// flgwait(in) true:処理待ちを行う false:待ち無し
// 戻り値: true:正常終了 false:失敗
//   補足: データシートでは消去に通常 120ms 、最大800msかかると記載されている
//         アドレス23ビットのうち上位 8ビットがブロックの相当する。下位15ビットはブロック内アドレスとなる。
//
bool W25Q64_erase32Block(W25Q64_t * dev, uint16_t blk_no, bool flgwait)
{
	spi_transaction_t SPITransaction;
	uint8_t data[4];
	uint32_t addr = blk_no;
	addr<<=15;

	// 書込み許可設定
	esp_err_t ret;
	ret = W25Q64_WriteEnable(dev);
	if (ret != ESP_OK) return false;

	data[0] = CMD_BLOCK_ERASE32KB;
	data[1] = (addr>>16) & 0xff;
	data[2] = (addr>>8) & 0xff;
	data[3] = addr & 0xff;
	memset( &SPITransaction, 0, sizeof( spi_transaction_t ) );
	SPITransaction.length = 4 * 8;
	SPITransaction.tx_buffer = data;
	SPITransaction.rx_buffer = data;
	ret = spi_device_transmit( dev->_SPIHandle, &SPITransaction );
	assert(ret==ESP_OK);
	if (ret != ESP_OK) return false;

	// 処理待ち
	while( W25Q64_IsBusy(dev) & flgwait) {
		vTaskDelay(1);
	}
	return true;
}


//
// 全領域の消去
// flgwait(in) true:処理待ちを行う false:待ち無し
// 戻り値: true:正常終了 false:失敗
//   補足: データシートでは消去に通常 15s 、最大30sかかると記載されている
//
bool W25Q64_eraseAll(W25Q64_t * dev, bool flgwait)
{
	spi_transaction_t SPITransaction;
	uint8_t data[1];

	// 書込み許可設定
	esp_err_t ret;
	ret = W25Q64_WriteEnable(dev);
	if (ret != ESP_OK) return false;

	data[0] = CMD_CHIP_ERASE;
	memset( &SPITransaction, 0, sizeof( spi_transaction_t ) );
	SPITransaction.length = 1 * 8;
	SPITransaction.tx_buffer = data;
	SPITransaction.rx_buffer = data;
	ret = spi_device_transmit( dev->_SPIHandle, &SPITransaction );
	assert(ret==ESP_OK);
	if (ret != ESP_OK) return false;

	// 処理待ち
	while( W25Q64_IsBusy(dev) & flgwait) {
		vTaskDelay(1);
	}
	return true;
}

//
// データの書き込み
// sect_no(in) : セクタ番号(0x00 - 0x7FF) 
// inaddr(in)  : セクタ内アドレス(0x00-0xFFF)
// data(in)    : 書込みデータ格納アドレス
// n(in)       : 書込みバイト数(0～256)
//
int16_t W25Q64_pageWrite(W25Q64_t * dev, uint16_t sect_no, uint16_t inaddr, uint8_t* buf, int16_t n)
{
	if (n > 256) return 0;
	spi_transaction_t SPITransaction;
	uint8_t *data;

	uint32_t addr = sect_no;
	addr<<=12;
	addr += inaddr;

	// 書込み許可設定
	esp_err_t ret;
	ret = W25Q64_WriteEnable(dev);
	if (ret != ESP_OK) return 0;

	// 書込み等の処理中チェック
	if (W25Q64_IsBusy(dev)) return 0;  

	data = (unsigned char*)malloc(n+4);
	data[0] = CMD_PAGE_PROGRAM;
	data[1] = (addr>>16) & 0xff;
	data[2] = (addr>>8) & 0xff;
	data[3] = addr & 0xFF;
	memcpy( &data[4], buf, n );
	memset( &SPITransaction, 0, sizeof( spi_transaction_t ) );
	SPITransaction.length = (n+4) * 8;
	SPITransaction.tx_buffer = data;
	SPITransaction.rx_buffer = data;
	ret = spi_device_transmit( dev->_SPIHandle, &SPITransaction );
	free(data);
	assert(ret==ESP_OK);
	if (ret != ESP_OK) return 0;

	// 処理待ち
	while( W25Q64_IsBusy(dev) ) {
		vTaskDelay(1);
	}
	return n;
}

