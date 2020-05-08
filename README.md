# esp-idf-w25q64
SPI Flash Memory W25Q64 Access Library for esp-idf.   
I ported from [here](https://github.com/Tamakichi/Arduino-W25Q64).   

---

# CONFIG
You have to set this config value with menuconfig.   
CONFIG_CS_GPIO   

```
git clone https://github.com/nopnop2002/esp-idf-w25q64
cd esp-idf-w25q64
make menuconfig
make flash
```

![config-1](https://user-images.githubusercontent.com/6020549/59921481-bd014f00-9468-11e9-9e7a-0f2c74c8cca5.jpg)
![config-2](https://user-images.githubusercontent.com/6020549/59921484-becb1280-9468-11e9-994b-9b9e1e9edae6.jpg)

---

# API

// Start Flash  
void spi_master_init(W25Q64_t * dev, int GPIO_CS);  

// Get status register1  
esp_err_t W25Q64_readStatusReg1(W25Q64_t * dev, uint8_t * reg1);  

// Get status register2(Winbond only)  
esp_err_t W25Q64_readStatusReg2(W25Q64_t * dev, uint8_t * reg2);  

// Get Unique ID(Winbond only)  
esp_err_t W25Q64_readUniqieID(W25Q64_t * dev, uint8_t * id);  

// Get JEDEC ID(Manufacture, Memory Type,Capacity)  
esp_err_t W25Q64_readManufacturer(W25Q64_t * dev, uint8_t * id);  

// Check busy  
bool W25Q64_IsBusy(W25Q64_t * dev);  

// Set power down mode  
esp_err_t W25Q64_powerDown(W25Q64_t * dev);;  

// Set write enable  
esp_err_t W25Q64_WriteEnable(W25Q64_t * dev);  

// Set write disable  
esp_err_t W25Q64_WriteDisable(W25Q64_t * dev);  

// Read data from memory  
uint16_t W25Q64_read(W25Q64_t * dev, uint32_t addr, uint8_t *buf, uint16_t n);   

// First read data from memory  
uint16_t W25Q64_fastread(W25Q64_t * dev, uint32_t addr, uint8_t *buf, uint16_t n);  

// Erase data by Sector  
bool W25Q64_eraseSector(W25Q64_t * dev, uint16_t sect_no, bool flgwait);  

// Erase data by block(64KB)  
bool W25Q64_erase64Block(W25Q64_t * dev, uint16_t blk_no, bool flgwait);  

// Erase data by Block(32KB)  
bool W25Q64_erase32Block(W25Q64_t * dev, uint16_t blk_no, bool flgwait);  

// Erase all data  
bool W25Q64_eraseAll(W25Q64_t * dev, bool flgwait);  

// Write data to memory  
int16_t W25Q64_pageWrite(W25Q64_t * dev, uint16_t sect_no, uint16_t inaddr, uint8_t* buf, int16_t n);  

---

# Wireing  

|#|W25Q64||ESP32|
|:-:|:-:|:-:|:-:|
|1|/CS|--|(*)|
|2|MISO|--|GPIO12|
|3|/WP|--|3.3V|
|4|GND|--|GND|
|5|MOSI|--|GPIO13|
|6|SCK|--|GPIO14|
|7|/HOLD|--|3.3V|
|8|VCC|--|3.3V|

(*) You can change any GPIO using menuconfig.   

---

# Winbond

|Device|Address range|Number of 4K-Sectors|Number of 32K-Blocks|Number of 64K-Blocks|
|:---|:---|:---|:---|:---|
|W25Q80|0x0FFFFF|256|32|16|
|W25Q16|0x1FFFFF|512|64|32|
|W25Q32|0x3FFFFF|1024|128|64|
|W25Q64|0x7FFFFF|2048|256|128|
|W25Q128|0xFFFFFF|4096|512|256|

- W24Q64   
![W25Q64](https://user-images.githubusercontent.com/6020549/81382272-16d63a80-9149-11ea-90b3-aef92642914f.jpg)

Manufacturer:  
Byte1 : ManufacturerID(0xef=Winbond)  
Byte2 : MemoryType(0x40=SPI/0x60=QPI)  
Byte3 : Capacity(2^0x17=2^23=0x800000=8M Byte=64M Bit)  

First 10Byte : ASCII 0-9  
Next 32Byte : ASCII A-Z  

- W25Q80   
![W25Q80](https://user-images.githubusercontent.com/6020549/81382267-1342b380-9149-11ea-88bc-bc7cd07501a4.jpg)

- W25Q16   
![W25Q16](https://user-images.githubusercontent.com/6020549/81403592-b6f28a80-916e-11ea-92ef-1bbac7b79e15.jpg)

- W25Q32   
![W25Q32](https://user-images.githubusercontent.com/6020549/81382304-22296600-9149-11ea-8cbe-8da89123539a.jpg)

- W25Q128   
![W25Q128](https://user-images.githubusercontent.com/6020549/81382327-2c4b6480-9149-11ea-8b34-dcbd6e43aa37.jpg)

---

# MACRONIX   

|Device|Address range|Number of 4K-Sectors|Number of 32K-Blocks|Number of 64K-Blocks|
|:---|:---|:---|:---|:---|
|MX25L6473E|0x7FFFFF|2048|256|128|
|MX25L12835F|0xFFFFFF|4096|512|256|

- MX25L6473E   
![MX25L6473E](https://user-images.githubusercontent.com/6020549/81383580-32dadb80-914b-11ea-823b-6487b7a7e073.jpg)

Manufacturer:  
Byte1 : ManufacturerID(0xc2=Macronix)  
Byte2 : MemoryType(0x20)  
Byte3 : Capacity(2^0x17=2^23=0x800000=8M Byte=64M Bit)  

- MX25L12835F   
![MX25L12835F](https://user-images.githubusercontent.com/6020549/81383590-353d3580-914b-11ea-913d-ce862c58c36c.jpg)

---

# Note   
I tested these.   
But I couldn't get it working.   
- GD25Q64   
- SST25VF016B   
- SST25VF032B   
