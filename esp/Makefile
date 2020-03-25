PORT=/dev/ttyUSB0
AMPY=ampy --port $(PORT)
ESP_BIN=esp32-idf3-20200324-v1.12-270-g38ccb4c64.bin

upload:
	$(AMPY) put boot.py
	$(AMPY) put main.py
	$(AMPY) run main.py

upload_micropython:
	esptool.py --chip esp32 --port $(PORT) write_flash -z 0x1000 $(ESP_BIN)
