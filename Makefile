# Cross-platform Makefile for RTU DNP3 Reader

# Detect operating system
ifeq ($(OS),Windows_NT)
    # Windows settings
    CXX = g++
    TARGET = rtu_reader.exe
    LIBS = -L$(LIB_PATH) -ldnp3_ffi -lws2_32 -lbcrypt -lntdll -luserenv -pthread
    LDFLAGS = -static-libgcc -static-libstdc++
    CLEAN_CMD = del /Q $(TARGET) 2>NUL || echo "No files to clean"
else
    # macOS/Linux settings
    CXX = clang++
    TARGET = rtu_reader
    LIBS = -L$(LIB_PATH) -ldnp3_ffi -pthread -framework Foundation -framework Security
    LDFLAGS = -Wl,-rpath,$(LIB_PATH)
    CLEAN_CMD = rm -f $(TARGET)
endif

# Common settings
CXXFLAGS = -std=c++17 -O2 -Wall
SOURCE = rtu_reader.cpp

# Path to Step Function I/O DNP3 library
DNP3_PATH = ./dnp3
INCLUDE_PATH = $(DNP3_PATH)/ffi/bindings/c++/include
LIB_PATH = $(DNP3_PATH)/target/release
CPP_WRAPPER_SRC = $(DNP3_PATH)/ffi/bindings/c++/src/dnp3.cpp

# Include flags
INCLUDES = -I$(INCLUDE_PATH)

all: $(TARGET)

$(TARGET): $(SOURCE) $(CPP_WRAPPER_SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(TARGET) $(SOURCE) $(CPP_WRAPPER_SRC) $(LIBS) $(LDFLAGS)

clean:
	$(CLEAN_CMD)

install_deps:
ifeq ($(OS),Windows_NT)
	@echo "Install MSYS2 from https://www.msys2.org/"
	@echo "Then run: pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-make"
	@echo "Install Rust from https://rustup.rs/"
else
	@echo "Installing Rust..."
	curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
	@echo "Installing Xcode command line tools..."
	xcode-select --install
endif

setup_dnp3:
	@echo "Cloning DNP3 library..."
	git clone https://github.com/stepfunc/dnp3.git
	cd dnp3 && cargo build --release

.PHONY: all clean install_deps setup_dnp3