# reverseCAN
Improved CAN bus analysis tool. Currently works with SocketCAN on Linux. 

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

