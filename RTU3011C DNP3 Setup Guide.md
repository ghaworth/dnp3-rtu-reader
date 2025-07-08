# RTU3011C DNP3 Setup Guide
## Overview
This guide walks through setting up a Siemens RTU3011C to send analogue input data via DNP3, first testing with KEPServerEX, then transitioning to custom C++ code.
- - -
## Hardware Setup
### Physical Connections
- Connect computer directly to RTU with an ethernet cable, if this doesn’t work try connecting via a network switch. 
- - -
## RTU Configuration for DNP3
### Access RTU Web Interface
- Out of the box the RTU has IP address 192.168.0.3, to access it you will need to put your computer on the same subnet.
- To do this (using Windows) navigate to System Settings > Network & Internet > Ethernet then find the Ethernet port that you wish to use to connect to the RTU. Click Edit:
![](RTU3011C%20DNP3%20Setup%20Guide/image.png)<!-- {"width":436} -->
- Change your IP address so that it is on the same subnet as the RTU, 192.168.0.4 should work fine:
![](RTU3011C%20DNP3%20Setup%20Guide/image%20%281%29.png)<!-- {"width":406} -->
- Open web browser and navigate to RTU IP address; i.e. write 192.168.0.3 in the URL bar:
![](RTU3011C%20DNP3%20Setup%20Guide/Screenshot%202025-07-07%20053046.png)<!-- {"width":416} -->
- Your browser will most likely give a warning that the connection isn’t private. Click ‘Advanced’:
![](RTU3011C%20DNP3%20Setup%20Guide/Screenshot%202025-07-07%20054601.png)<!-- {"width":416} -->
- Then click ‘Proceed to 192.168.0.3 (unsafe)’:
![](RTU3011C%20DNP3%20Setup%20Guide/Screenshot%202025-07-07%20054634.png)<!-- {"width":426} -->
- The login screen for the RTU looks like this. Straight out of the box the username and password are both ‘admin’. You will be asked to change the password when you first log in:
![](RTU3011C%20DNP3%20Setup%20Guide/image%20%282%29.png)<!-- {"width":426} -->
- You should now see the Web Based Management (WBM) interface:
![](RTU3011C%20DNP3%20Setup%20Guide/Screenshot%202025-07-07%20054753.png)<!-- {"width":436} -->
- - -
### Install DNP3 Firmware
- You will now need to install the DNP3 firmware. This can be downloaded from the Siemens website here: [SIOS](https://support.industry.siemens.com/cs/document/109974343/sales-release-for-simatic-rtu3051c-and-rtu3011c-with-firmware-v6-0-17-including-download-?dti=0&lc=en-CN)
  ![](RTU3011C%20DNP3%20Setup%20Guide/Screenshot%202025-07-07%20at%2006.03.27.png)<!-- {"width":426} -->
- When downloaded, extract the contents of the zip folder, there should be an SWF file.
- Navigate to the ‘Maintenance’ page and the ‘Firmware’ tab, click the ‘Search’ button:  
![](RTU3011C%20DNP3%20Setup%20Guide/Screenshot%202025-07-07%20061645.png)
- Navigate to the SWF file and click ‘Load on device’:
![](RTU3011C%20DNP3%20Setup%20Guide/Screenshot%202025-07-07%20062133.png)
- - -
### Activate Analogue Inputs
- Navigate to the ‘Tags’ page and the ‘Analog inputs’ tab:  
![](RTU3011C%20DNP3%20Setup%20Guide/Screenshot%202025-07-07%20062906.png)<!-- {"width":566} -->
- Apply the appropriate configurations for the sensors being used, here I will be testing with a Fluke 789 Processmeter so I have selected a 4…20mA 2-wire connector. Give the input a name. In this case I changed the Process value range to be from 0.0 to 100.0, by changing ‘Process value (interpolation point 1)’ and ‘Process value (interpolation point 2)’; these will be the values that actually get sent via DNP3 corresponding to the 4 to 20mA signal (i.e. 4mA = 0.0, 20mA =100.0). Check the ‘Active’ box and click ‘Apply’:  
![](RTU3011C%20DNP3%20Setup%20Guide/Screenshot%202025-07-07%20063650.png)<!-- {"width":607} -->
- If you now go to the ‘Overview’ tab in the ‘Tags’ page, the input that you just activated should show green: 
![](RTU3011C%20DNP3%20Setup%20Guide/Screenshot%202025-07-07%20064620.png)
- It is possible (and advisable) at this stage to test that the analogue input with an actual input. Here I am sourcing 10mA with a Fluke 789 Processmeter:
  ![](RTU3011C%20DNP3%20Setup%20Guide/IMG_0966.HEIC)<!-- {"width":446} -->
- If you click the ‘Read’ button on the ‘Analog inputs’ tab, it will update the actual reading at that input:
  ![](RTU3011C%20DNP3%20Setup%20Guide/Screenshot%202025-07-07%20075534.png)
- - -
### DNP3 Station Settings
- Navigate to the ‘Telecontrol’ page and the ‘DNP3’ tab:
![](RTU3011C%20DNP3%20Setup%20Guide/Screenshot%202025-07-07%20083928.png)
- Set ‘IP address master station’ to the IP address of your computer.
- Set ‘Port number’ to 20000.
- Set ‘Station address’ and ‘Master station address’ to a unique address (example: 7 and 3).
- Check the ‘Active’ box.
- - -
### DNP3 Data Points Setup
- In order to send the analogue reading via DNP3, the data point needs to be configured. Navigate to the ‘Telecontrol’ page and the ‘Data points’ tab:
![](RTU3011C%20DNP3%20Setup%20Guide/Screenshot%202025-07-07%20080652.png)
- The analogue input that was just activated should appear here, by default ‘Type of transfer’ is set to ‘Only internal use’. This means that the point is not available via DNP3. This needs to be set to either Class 1, 2 or 3:
  **Class 1:** High priority events (critical alarms, important changes)
  **Class 2:** Medium priority events (normal operational data)
  **Class 3:** Low priority events (non-critical data, historical logs)
- Set the ‘Index’ appropriately; this is the DNP3 reference number that the DNP3 client shall refer to. This can be any integer value but here I will simply use 0 for AI0.
- If ‘Trigger’ is set to ‘Change’ this will update the value sent to a DNP3 client whenever the analogue reading changes more than the deadband specified.
- If ‘Trigger’ is set to ‘Cyclic’ this will update the value sent to a DNP3 client at the time interval specified in the ‘Update cycle of this input’ setting for the analogue input (found in the ‘Analog inputs’ tab in the ‘Tags’ page).
- ’Trigger’ can also be set to ‘Both’.
- - -
### RTU Time Synch	
- The system time for the RTU cannot be synchronised with an NTP server. 
- When you activate telecontrol communication, the time-of-day synch is activated automatically and set by the telecontrol master station.
- If time-of-day synchronization is already activated, the currently active method is maintained.
- When you deactivate telecontrol communication and the time-of-day synchronization is activated by the telecontrol master station, the time-of-day synchronization is automatically deactivated.
- The ’System time’ tab of the System page in the RTU web interface should look like this:
![](RTU3011C%20DNP3%20Setup%20Guide/Screenshot%202025-07-08%20at%2023.58.27.png)
- When the DNP3 client is run for the very first time it may be necessary for ’Time-of-day sychronization’ to be deactivated and reactivated in the RTU if it was already running when the DNP3 client started.  ![](RTU3011C%20DNP3%20Setup%20Guide/Screenshot%202025-07-09%20at%2000.01.20.png)
- In order to check if time-of-day synch is working, deactivate it, then set the system time manually
  ![](RTU3011C%20DNP3%20Setup%20Guide/Screenshot%202025-07-09%20at%2000.04.09.png)
- Then reactivate ‘Time-of-day sychronization’ with the DNP3 client already running.
- Keep an eye on whether the time in the top right hand corner changes:
  ![](RTU3011C%20DNP3%20Setup%20Guide/Screenshot%202025-07-09%20at%2000.07.48.png)
- - -
## Testing with KEPServerEX
- Download and install KEPServerEX Demo: [Download the KEPServerEX demo](https://www.ptc.com/en/products/kepware/kepserverex/demo-download)
### KEPServerEX Channel Setup
- Open KEPServerEX and right click on ‘Connectivity’, then click ‘New Channel’:
![](RTU3011C%20DNP3%20Setup%20Guide/Screenshot%202025-07-07%20085102.png)
- Select channel type as ‘DNP Client Ethernet’ and give it a name such as ‘RTU_Channel’.
- Leave ‘Virtual Network’ set as ‘None’ and ‘Transactions per Cycle’ set to 1.
- Set the appropriate network adapter for the port on your computer the RTU is connected to.
- Leave ‘Optimization Method’ and ‘Duty Cycle’ as default.
- Set floating-point values to be sent ‘Unmodfiied’.
- Leave the communication protocol set as TCP.
- Ensure that the destination IP address and port match those set in the RTU (192.168.0.3 and 20000): 
  ![](RTU3011C%20DNP3%20Setup%20Guide/Screenshot%202025-07-07%20090254.png)
- Leave ‘Connection Timeout’, ‘Response Timeout’ and ‘Max Link Layer Retries’ as default.
- Click ‘Finish’.
- - -
### Device Configuration
- The RTU_Channel you just created will now appear and have an option to add a device, click on this:
![](RTU3011C%20DNP3%20Setup%20Guide/Screenshot%202025-07-07%20091931.png)

- Give it a name e.g. Siemens_RTU.
- Leave ‘Scan Mode’ and ‘Initial Updates from Cache’ as default.
- Leave ‘Demote on Failure’ as ‘Disable’.
- Leave all the next page settings as default.
- Make sure that the DNP Client (your computer) and Server (the RTU) match the settings we set in the RTU. In this case 3 and 7:![](RTU3011C%20DNP3%20Setup%20Guide/Screenshot%202025-07-07%20092452.png)
- Set ‘Request Timeout’ to 5000ms. Leave all other settings on this page and the next as default.
- Enable unsolicited messaging for all data classes.
- Leave all other settings as default.
- Click ‘Finish’.
- - -
### Tag Creation
- The RTU_Channel will now have a device associated with it, and this will have an option to add a tag. Click on this:
![](RTU3011C%20DNP3%20Setup%20Guide/Screenshot%202025-07-07%20091627.png) 
- Populate the settings for the tag like this:
![](RTU3011C%20DNP3%20Setup%20Guide/Screenshot%202025-07-07%20093612.png)
- Address *30.0.0.Value* corresponds to analogue input 0, *30.0.1.Value* for input 1 etc.
- - -
### Testing Procedure
1. **Start KEPServerEX Runtime**
2. **Open OPC Quick Client**
3. **Browse to tags:** KEPServerEX > RTU_Channel > Siemens_RTU > AI_Channel_0
![](RTU3011C%20DNP3%20Setup%20Guide/Screenshot%202025-07-07%20094241.png)
4. **Testing with Fluke 789:**
> [!NOTE] 4mA
> ![](RTU3011C%20DNP3%20Setup%20Guide/IMG_0968.HEIC)<!-- {"width":328} -->
> ![](RTU3011C%20DNP3%20Setup%20Guide/Screenshot%202025-07-07%20102650.png)<!-- {"width":588} -->

> [!NOTE] 10mA
> ![](RTU3011C%20DNP3%20Setup%20Guide/IMG_0967.HEIC)<!-- {"width":328} -->
> ![](RTU3011C%20DNP3%20Setup%20Guide/Screenshot%202025-07-07%20102613.png)

> [!NOTE] 20mA
> ![](RTU3011C%20DNP3%20Setup%20Guide/IMG_0969.HEIC)<!-- {"width":328} -->
> ![](RTU3011C%20DNP3%20Setup%20Guide/Screenshot%202025-07-07%20102734.png)
- - -
## Transition to C++ Implementation
I have created a C++ client for reading analog inputs from Siemens RTU via DNP3 protocol here:
https://github.com/ghaworth/dnp3-rtu-reader

### Step Function I/O DNP3 Library
- **Download:** https://github.com/stepfunc/dnp3
- **Language:** Use C++ bindings for integration
- **Installation:** Follow library documentation for C++ setup

### C++ DNP3 Master Configuration
Use the same proven settings from KEPServerEX:

```cpp
// Connection parameters (same as KEPServerEX)
std::string rtu_ip = "192.168.0.3";
uint16_t port = 20000;
uint16_t station_address = 7;

// Analogue input addressing (same as KEPServerEX tags)
// Group 30 = Analogue Inputs
// Index 0, 1, 2 = Your configured channels
```
### Data Integration
Replace existing TCP/serial parsing in data logger with DNP3 calls:
- **Connect** to RTU using proven parameters
- **Request** Group 30 analogue inputs (Class 2)
- **Handle** both polled data and unsolicited events
- **Convert** DNP3 analogue values to proprietary format
- - -
## Performance Characteristics

### Communication Modes
- **Polling:** 1-10 Hz typical for DNP3 (avoid 60Hz polling)
- **Unsolicited Events:** 10-50ms response time to changes
- **Hybrid Approach:** Combine slow polling (integrity) with fast events (responsiveness)

### Event Configuration
- **Small deadbands** for sensitive change detection
- **Class 2** for normal operational data
- **Change trigger** for immediate event reporting
- - -
## Troubleshooting
### Common Issues
1. **No Connection:** Check IP addresses, port numbers, firewall settings
2. **No Data:** Verify DNP3 is enabled, check station addresses match
3. **Incorrect Values:** Check analogue input scaling and engineering units
4. **Slow Updates:** Verify unsolicited messaging is enabled, check deadband settings

### Verification Steps
1. **Network:** Ping RTU IP address
2. **DNP3:** Use KEPServerEX diagnostics or DNP3 protocol analyser
3. **Values:** Cross-check with Fluke 789 output current
4. **Events:** Monitor for unsolicited DNP3 events when changing input
- - -
## Summary
The KEPServerEX testing phase validates all configuration before moving to custom code, ensuring a smooth transition to the final implementation.