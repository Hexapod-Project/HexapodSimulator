PORJECT_NAME = HexapodSimulator
BUILD_PATH = build

all:
	clear
	cmake -B $(BUILD_PATH)
	cd $(BUILD_PATH) && make
	$(BUILD_PATH)/Debug/$(PORJECT_NAME)/$(PORJECT_NAME)