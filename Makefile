# Makefile for RTU DNP3 Reader on macOS

CXX = clang++
CXXFLAGS = -std=c++17 -O2 -Wall
TARGET = rtu_reader
SOURCE = rtu_reader.cpp

# Path to Step Function I/O DNP3 library
DNP3_PATH = ./dnp3
INCLUDE_PATH = $(DNP3_PATH)/ffi/bindings/c++/include
LIB_PATH = $(DNP3_PATH)/target/release
CPP_WRAPPER_SRC = $(DNP3_PATH)/ffi/bindings/c++/src/dnp3.cpp

# Include and library flags for macOS
INCLUDES = -I$(INCLUDE_PATH)
LIBS = -L$(LIB_PATH) -ldnp3_ffi -pthread

# macOS specific flags
LDFLAGS = -framework Foundation -framework Security -Wl,-rpath,$(LIB_PATH)

all: $(TARGET)

$(TARGET): $(SOURCE) $(CPP_WRAPPER_SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(TARGET) $(SOURCE) $(CPP_WRAPPER_SRC) $(LIBS) $(LDFLAGS)

# Build without DNP3 library for testing
test_build: $(SOURCE)
	$(CXX) $(CXXFLAGS) -DTEST_MODE -o $(TARGET)_test $(SOURCE)

# Build the DNP3 library
build_dnp3:
	cd $(DNP3_PATH) && cargo build --release
	cd $(DNP3_PATH) && cargo build --release -p dnp3-ffi

clean:
	rm -f $(TARGET) $(TARGET)_test

install_deps:
	@echo "Installing Rust..."
	curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
	@echo "Installing Xcode command line tools..."
	xcode-select --install

setup_dnp3:
	@echo "Cloning DNP3 library..."
	git clone https://github.com/stepfunc/dnp3.git
	cd dnp3 && cargo build --release

.PHONY: all clean install_deps setup_dnp3 test_build build_dnp3

clean:
	rm -f $(TARGET)

install_deps:
	@echo "Installing Rust..."
	curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
	@echo "Installing Xcode command line tools..."
	xcode-select --install

setup_dnp3:
	@echo "Cloning DNP3 library..."
	git clone https://github.com/stepfunc/dnp3.git
	cd dnp3 && cargo build --release

.PHONY: all clean install_deps setup_dnp3