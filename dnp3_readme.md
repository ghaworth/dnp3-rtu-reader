# DNP3 RTU Reader

A C++ client application for reading analog inputs from Siemens RTU devices via DNP3 protocol over TCP/IP.

## Overview

This application uses the [Step Function I/O DNP3 library](https://stepfunc.io/products/libraries/dnp3/) to establish a DNP3 master connection and poll analog input values from an RTU (Remote Terminal Unit).

## Features

- TCP/IP connection to RTU
- Configurable master and station addresses
- Automatic polling of analog inputs every 5 seconds
- Real-time display of analog values with timestamps and flags
- Clean connection handling and error reporting

## Prerequisites

- macOS with Xcode command line tools
- Rust toolchain (for building DNP3 library)
- C++17 compatible compiler (clang++)

## Installation

1. **Install dependencies:**
   ```bash
   make install_deps
   ```

2. **Clone and build the DNP3 library:**
   ```bash
   make setup_dnp3
   ```

3. **Build the DNP3 library:**
   ```bash
   make build_dnp3
   ```

4. **Compile the application:**
   ```bash
   make
   ```

## Configuration

Edit `rtu_reader.cpp` to match your RTU settings:

```cpp
// Network configuration
dnp3::EndpointList endpoints("192.168.0.3:20000");  // RTU IP and port

// DNP3 addresses
dnp3::MasterChannelConfig config(3);  // Master address
channel.add_association(7, ...);      // RTU station address
```

**Default settings:**
- **RTU IP Address:** 192.168.0.3
- **RTU Port:** 20000
- **Master Address:** 3
- **RTU Station Address:** 7

## Usage

1. **Connect RTU to your Mac via Ethernet cable**

2. **Ensure network connectivity:**
   ```bash
   ping 192.168.0.3
   ```

3. **Run the application:**
   ```bash
   ./rtu_reader
   ```

4. **Stop the application:**
   - Press `Ctrl + C` to exit

## Sample Output

```
Simple RTU DNP3 Master
======================
Runtime created
Endpoints configured
Channel created
Association added for station 7
Channel enabled - connecting to RTU...
Client state changed to: Connected

--- Read 1 ---
Read completed successfully!
=== ANALOGUE INPUT ===
Channel: 0
Value: 123.45
Flags: 0x1
Time: 1234567890
======================
```

## Troubleshooting

### Connection Issues

- **"response_timeout" errors:** Check master and station addresses match RTU configuration
- **"Client state: Disconnected":** Verify RTU IP address and network connectivity
- **No analog readings:** Ensure RTU has analog input points configured

### Network Setup

- **Simultaneous Internet + RTU access:** Use a USB-C to Ethernet adapter
  - Connect RTU to USB-C adapter
  - Connect internet to built-in Ethernet port
  - Both connections work simultaneously

### Common Configuration Mismatches

1. **Master Address:** RTU expects specific master address (check RTU web interface)
2. **Station Address:** Must match RTU's configured station address
3. **IP Address:** Ensure RTU is accessible on specified IP and port

## File Structure

```
.
├── rtu_reader.cpp      # Main application source
├── Makefile           # Build configuration
├── README.md          # This file
└── dnp3/             # DNP3 library (created by setup)
```

## Library Information

This project uses the Step Function I/O DNP3 library:
- **Website:** https://stepfunc.io/products/libraries/dnp3/
- **Documentation:** Available on the Step Function I/O website
- **License:** Check the library's license terms

## Contributing

Feel free to submit issues and pull requests to improve this DNP3 client implementation.

## License

This project is provided as-is for educational and development purposes.