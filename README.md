# reverseCAN
Improved CAN bus analysis tool. Currently works with SocketCAN on Linux. 

### Screenshots not included to comply with manufacturer policy

Compile with 

g++ canreverse.cpp -o <OUTPUTFILE> 



## Usage
The utility collects background CAN data until interrupted.
This collected data is used to generate a set of ignorable can_frame_id <-> bit_index pairs.
The utility then prints CAN data to stdout, ignoring ignorable data.
Extremely high-frequency data is filtered out gradually

Notes
- Designed for discrete data (window actuators, UI controls)
- For continuous data, refer to Brent Nolan's project on GitHub
- The longer you run the system, the fewer signals will pop up

Tested on the
- Tesla Model Y
- Tesla Model X AWD, 75D, P90D

Hardware & Software Used:
- Debian on Beaglebone Black

Warning:
Asserting signals on the CAN bus is risky if you don't know what you're doing.
Conflicting signals between the car's input and your assertions may result in erratic behavior.
Be very careful especially when controlling the steering mechanism.

## Test it out

Setup a virtual device on your system - let's call it vcan0

Install the cangen utility and run it to push random CAN bus data to vcan0

Set the bus flag in the code to vcan0

