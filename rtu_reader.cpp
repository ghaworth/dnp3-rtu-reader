#include "dnp3.hpp"
#include <iostream>
#include <memory>
#include <thread>
#include <chrono>

// =============================================================================
// CONFIGURATION PARAMETERS
// =============================================================================

// Network configuration
const std::string RTU_IP = "192.168.0.3";
const uint16_t RTU_PORT = 20000;

// DNP3 address configuration
const uint16_t MASTER_ADDRESS = 3;      // Master station address
const uint16_t RTU_STATION_ADDRESS = 7; // RTU station address

// Polling configuration
const int POLL_INTERVAL_SECONDS = 5;    // Time between reads
const int TOTAL_READS = 150;            // Number of reads to perform (750 seconds = 12.5 minutes)
const int CONNECTION_WAIT_SECONDS = 3;  // Wait time for initial connection

// Request configuration
const auto ANALOG_INPUT_VARIATION = dnp3::Variation::group30_var0; // All analog inputs

// Timeout configuration
const int RESPONSE_TIMEOUT_SECONDS = 10; // DNP3 response timeout

// =============================================================================
// DNP3 HANDLER CLASSES
// =============================================================================

// Simple read handler based on the official example
class SimpleReadHandler : public dnp3::ReadHandler {
    void handle_analog_input(const dnp3::HeaderInfo &info, dnp3::AnalogInputIterator &it) override
    {
        while (it.next()) {
            const auto value = it.get();
            std::cout << "=== ANALOGUE INPUT ===" << std::endl;
            std::cout << "Channel: " << value.index << std::endl;
            std::cout << "Value: " << value.value << std::endl;
            std::cout << "Flags: 0x" << std::hex << (int)value.flags.value << std::dec << std::endl;
            std::cout << "Time: " << value.time.value << std::endl;
            std::cout << "======================" << std::endl;
        }
    }
};

// Simple association handler
class SimpleAssociationHandler : public dnp3::AssociationHandler {
public:
    dnp3::UtcTimestamp get_current_time() override
    {
        const auto time_since_epoch = std::chrono::system_clock::now().time_since_epoch();
        auto timestamp = dnp3::UtcTimestamp::valid(std::chrono::duration_cast<std::chrono::milliseconds>(time_since_epoch).count());
        
        // Debug: Log when time is requested
        std::cout << "DNP3 time sync requested - providing timestamp: " << timestamp.value << std::endl;
        
        return timestamp;
    }
};

class SimpleAssociationInformation : public dnp3::AssociationInformation {
    void task_start(dnp3::TaskType task_type, dnp3::FunctionCode function_code, uint8_t seq) override {}
    void task_success(dnp3::TaskType task_type, dnp3::FunctionCode function_code, uint8_t seq) override {}
    void task_fail(dnp3::TaskType task_type, dnp3::TaskError error) override {}
    void unsolicited_response(bool is_duplicate, uint8_t seq) override {}
};

class SimpleClientStateListener : public dnp3::ClientStateListener {
    void on_change(dnp3::ClientState state) override {
        std::cout << "Client state changed to: " << dnp3::to_string(state) << std::endl;
        if (state == dnp3::ClientState::connected) {
            std::cout << "Successfully connected to RTU at " << RTU_IP << ":" << RTU_PORT << "!" << std::endl;
        }
    }
};

class SimpleReadTaskCallback : public dnp3::ReadTaskCallback {
    void on_complete(dnp3::Nothing result) override {
        std::cout << "Read completed successfully!" << std::endl;
    }
    void on_failure(dnp3::ReadError error) override {
        std::cout << "Read failed: " << dnp3::to_string(error) << std::endl;
    }
};

// =============================================================================
// CONFIGURATION FUNCTIONS
// =============================================================================

dnp3::MasterChannelConfig get_master_config() {
    dnp3::MasterChannelConfig config(MASTER_ADDRESS);
    // Note: response_timeout is not available in this version of the library
    // config.response_timeout = std::chrono::seconds(RESPONSE_TIMEOUT_SECONDS);
    return config;
}

dnp3::AssociationConfig get_association_config() {
    dnp3::AssociationConfig config(
        dnp3::EventClasses::all(),
        dnp3::EventClasses::all(), 
        dnp3::Classes::all(),
        dnp3::EventClasses::none()
    );
    
    // Enable automatic time synchronization if available
    config.auto_time_sync = dnp3::AutoTimeSync::lan;
    
    return config;
}

std::string get_endpoint_string() {
    return RTU_IP + ":" + std::to_string(RTU_PORT);
}

// =============================================================================
// MAIN FUNCTION
// =============================================================================

int main() {
    std::cout << "Simple RTU DNP3 Master" << std::endl;
    std::cout << "======================" << std::endl;
    std::cout << "Configuration:" << std::endl;
    std::cout << "  RTU Address: " << RTU_IP << ":" << RTU_PORT << std::endl;
    std::cout << "  Master Address: " << MASTER_ADDRESS << std::endl;
    std::cout << "  RTU Station Address: " << RTU_STATION_ADDRESS << std::endl;
    std::cout << "  Poll Interval: " << POLL_INTERVAL_SECONDS << " seconds" << std::endl;
    std::cout << "  Total Reads: " << TOTAL_READS << std::endl;
    std::cout << "======================" << std::endl;
    
    try {
        // Create runtime (same as official example)
        auto runtime = dnp3::Runtime(dnp3::RuntimeConfig());
        std::cout << "Runtime created" << std::endl;
        
        // Create endpoint list for our RTU
        dnp3::EndpointList endpoints(get_endpoint_string());
        std::cout << "Endpoints configured: " << get_endpoint_string() << std::endl;
        
        // Create TCP channel (same pattern as official example)
        auto channel = dnp3::MasterChannel::create_tcp_channel(
            runtime,
            dnp3::LinkErrorMode::close,
            get_master_config(),
            endpoints,
            dnp3::ConnectStrategy(),
            std::make_unique<SimpleClientStateListener>()
        );
        std::cout << "Channel created" << std::endl;
        
        // Add association for RTU station
        auto assoc = channel.add_association(
            RTU_STATION_ADDRESS,
            get_association_config(),
            std::make_unique<SimpleReadHandler>(),
            std::make_unique<SimpleAssociationHandler>(),
            std::make_unique<SimpleAssociationInformation>()
        );
        std::cout << "Association added for station " << RTU_STATION_ADDRESS << std::endl;
        
        // Enable the channel
        channel.enable();
        std::cout << "Channel enabled - connecting to RTU..." << std::endl;
        
        // Wait for connection
        std::this_thread::sleep_for(std::chrono::seconds(CONNECTION_WAIT_SECONDS));
        
        // Perform an initial integrity poll (like Kepware would do)
        std::cout << "\n--- Performing Initial Integrity Poll ---" << std::endl;
        dnp3::Request integrity_request;
        integrity_request.add_all_objects_header(dnp3::Variation::group1_var0);  // Binary inputs
        integrity_request.add_all_objects_header(dnp3::Variation::group30_var0); // Analog inputs
        integrity_request.add_all_objects_header(dnp3::Variation::group20_var0); // Counters
        channel.read(assoc, integrity_request, std::make_unique<SimpleReadTaskCallback>());
        
        std::this_thread::sleep_for(std::chrono::seconds(5));
        
        std::this_thread::sleep_for(std::chrono::seconds(2));
        
        // Perform reads at specified interval
        for (int i = 0; i < TOTAL_READS; i++) {
            std::cout << "\n--- Read " << (i+1) << " of " << TOTAL_READS << " ---" << std::endl;
            
            // Create request for analog inputs
            dnp3::Request request;
            request.add_all_objects_header(ANALOG_INPUT_VARIATION);
            
            // Perform read
            channel.read(assoc, request, std::make_unique<SimpleReadTaskCallback>());
            
            // Wait before next read (except for the last one)
            if (i < TOTAL_READS - 1) {
                std::this_thread::sleep_for(std::chrono::seconds(POLL_INTERVAL_SECONDS));
            }
        }
        
        std::cout << "\nCompleted all " << TOTAL_READS << " reads. Exiting..." << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}