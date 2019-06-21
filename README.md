# esp-idf-w25q64
SPI Flash Memory W25Q64 Access Library for esp-idf.   
I ported from here.   

https://github.com/Tamakichi/Arduino-W25Q64

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

// Get status register2  
esp_err_t W25Q64_readStatusReg2(W25Q64_t * dev, uint8_t * reg2);  

// Get Unique ID  
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

|W25Q64||ESP32|
|:-:|:-:|:-:|
|VCC|--|3.3V|
|GND|--|GND|
|CS|--|(*)|
|MISI|--|GPIO12|
|MOSI|--|GPIO13|
|SCK|--|GPIO14|

(*) You can change any GPIO using menuconfig.   

---

# Output

![w25q64-1](https://user-images.githubusercontent.com/6020549/59921463-b1ae2380-9468-11e9-9353-0ba07faecfdd.jpg)

Manufacturer:  
Byte1 : ManufacturerID(0xef=Winbond)  
Byte2 : MemoryType(0x40=SPI/0x60=QPI)  
Byte3 : Capacity(2^0x17=2^23=0x800000=8M Byte=64M Bit)  

First 10Byte : ASCII 0-9  
Next 32Byte : ASCII A-Z  


