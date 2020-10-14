# esp-idf-w25q64
SPI Flash Memory W25Q64 Access Library for esp-idf.   
I ported from [here](https://github.com/Tamakichi/Arduino-W25Q64).   

---

# CONFIG
You have to set this config value with menuconfig.   
- CONFIG_SPI
- CONFIG_GPIO_CS   

```
git clone https://github.com/nopnop2002/esp-idf-w25q64
cd esp-idf-w25q64
make menuconfig
make flash
```

![config-1](https://user-images.githubusercontent.com/6020549/96056689-39236f80-0ec2-11eb-9d1d-a3710d8fb2e3.jpg)

- Select SPI2   
![config-2](https://user-images.githubusercontent.com/6020549/96056699-3b85c980-0ec2-11eb-99a6-819c5fe8de9f.jpg)

- Select SPI3   
![config-3](https://user-images.githubusercontent.com/6020549/96056704-3d4f8d00-0ec2-11eb-9ac9-03aba3aa7f37.jpg)

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

|#|W25Q64||ESP32(SPI2)|ESP32(SPI3)
|:-:|:-:|:-:|:-:|:-:|
|1|/CS|--|GPIO15(*)|GPIO5(*)|
|2|MISO|--|GPIO12|GPIO19|
|3|/WP|--|3.3V|3.3V|
|4|GND|--|GND|GND|
|5|MOSI|--|GPIO13|GPIO23|
|6|SCK|--|GPIO14|GPIO18|
|7|/HOLD|--|3.3V|3.3V|
|8|VCC|--|3.3V|3.3V|

(*) You can change any GPIO using menuconfig.   

---

# Winbond

|Device|# of Bytes|Address range|# of 4K-Sectors|# of 32K-Blocks|# of 64K-Blocks|JEDEC ID|
|:---|:---|:---|:---|:---|:---|:---|
|W25Q80|1M|0x0FFFFF|256|32|16|EF-40-14|
|W25Q16|2M|0x1FFFFF|512|64|32|EF-40-15|
|W25Q32|4M|0x3FFFFF|1024|128|64|EF-40-16|
|W25Q64|8M|0x7FFFFF|2048|256|128|EF-40-17|
|W25Q128|16M|0xFFFFFF|4096|512|256|EF-40-18|

- W25Q80   
![W25Q80](https://user-images.githubusercontent.com/6020549/81382267-1342b380-9149-11ea-88bc-bc7cd07501a4.jpg)

Manufacturer:  
Byte1 : ManufacturerID(0xef=Winbond)  
Byte2 : MemoryType(0x40=SPI/0x60=QPI)  
Byte3 : Capacity(2^0x14=2^20=0x100000=1M Byte=8M Bit)  

First 10Byte : ASCII 0-9  
Next 32Byte : ASCII A-Z  

- W25Q16   
![W25Q16](https://user-images.githubusercontent.com/6020549/81403592-b6f28a80-916e-11ea-92ef-1bbac7b79e15.jpg)

- W25Q32   
![W25Q32](https://user-images.githubusercontent.com/6020549/81382304-22296600-9149-11ea-8cbe-8da89123539a.jpg)

- W24Q64   
![W25Q64](https://user-images.githubusercontent.com/6020549/81382272-16d63a80-9149-11ea-90b3-aef92642914f.jpg)

- W25Q128   
![W25Q128](https://user-images.githubusercontent.com/6020549/81382327-2c4b6480-9149-11ea-8b34-dcbd6e43aa37.jpg)

---

# MACRONIX   

|Device|# of Bytes|Address range|# of 4K-Sectors|# of 32K-Blocks|# of 64K-Blocks|JEDEC ID|
|:---|:---|:---|:---|:---|:---|:---|
|MX25L3206E|4M|0x3FFFFF|1024|128|64|C2-20-16|
|MX25L6473E|8M|0x7FFFFF|2048|256|128|C2-20-17|
|MX25L12835F|16M|0xFFFFFF|4096|512|256|C2-20-18|

- MX25L3206E   
![MX25L3206E](https://user-images.githubusercontent.com/6020549/84555332-47685000-ad57-11ea-81a1-db88de013da9.jpg)

Manufacturer:  
Byte1 : ManufacturerID(0xc2=Macronix)  
Byte2 : MemoryType(0x20)  
Byte3 : Capacity(2^0x16=2^22=0x400000=4M Byte=32M Bit)  

- MX25L6473E   
![MX25L6473E](https://user-images.githubusercontent.com/6020549/81383580-32dadb80-914b-11ea-823b-6487b7a7e073.jpg)

- MX25L12835F   
![MX25L12835F](https://user-images.githubusercontent.com/6020549/81383590-353d3580-914b-11ea-913d-ce862c58c36c.jpg)

---

# Note   
I tested these.   
But I couldn't get it working.   
- GD25Q64   
- SST25VF016B   
- SST25VF032B   
