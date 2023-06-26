#include <iostream>

#include <stdio.h>

#include <vector>

#include <unordered_set>

#include <stdlib.h>

#include <string.h>

#include <unistd.h>

#include <net/if.h>

#include <sys/types.h>

#include <sys/socket.h>

#include <sys/ioctl.h>

#include <unordered_map>

#include <linux/can.h>

#include <linux/can/raw.h>

#include <signal.h>

#include <csignal>

/*
std::vector<float> get_variance(std::vector<std::vector<int>>) {


  
}
std::unordered_map<int,std::set<int>> get_ignorables(std::unordered_map<int,std::vector<std::vector<int>>> raw_data) {

  

  for (std::unordered_map<int,std::vector<std::vector<<int>>>>::iterator iterator = raw_data.begin(); iterator != raw_data.end(); iterator++) {

    int key = iterator -> first;
    std::vector<std::vector<int>> payloads = iterator -> second;

    std::cout << key << std::endl;

  }

}*/

bool interrupted = false;

void setup_network(int * s) {

  struct sockaddr_can addr;
  struct ifreq ifr;

  const char * ifname = "vcan0";

  if (( * s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
    perror("Error while opening socket");
  }
  
  strcpy(ifr.ifr_name, ifname);
  ioctl( * s, SIOCGIFINDEX, & ifr);
  
  addr.can_family = AF_CAN;
  addr.can_ifindex = ifr.ifr_ifindex;

  printf("%s at index %d\n", ifname, ifr.ifr_ifindex);

  if (bind( * s, (struct sockaddr * ) & addr, sizeof(addr)) < 0) {
    perror("Error in socket bind");
  }

}

void get_data(int s, can_frame * frame) {

  int nbytes;
  nbytes = read(s, frame, sizeof(struct can_frame));

  if (nbytes < 0) {
    perror("can raw socket read");
  }

  if (nbytes < sizeof(struct can_frame)) {
    fprintf(stderr, "read: incomplete CAN frame\n");
  }

}

std::vector < int > byte_to_binary_array(unsigned char byte) {
  std::vector < int > bit_array(8);

  for (int i = 7; i >= 0; i--) {
    bit_array[i] = byte % 2;
    byte = byte / 2;
  }

  return bit_array;

}

std::unordered_set<int> get_positive_variance_columns(std::vector<std::vector<int>> payload) {
	
	std::unordered_map<int, std::unordered_set<int>> found_values; //Store found values for each column. If there are > 1 unique values, we can assume that specific bit changed
	std::unordered_set<int> columns;
	
	for (int i = 0; i < payload.size(); i++) {
		for (int j = 0; j < payload[0].size(); j++) {

			if (columns.find(j) == columns.end()) { //If the value is already known to have changed, there is nothing further to do
								//
				if (found_values.find(j) != found_values.end()) { //If the key already exists in the dictionary,

					if (found_values[j].find(payload[i][j]) == found_values[j].end()) { //Check if the new value is unique

						columns.insert(j);
					} 
				} else {
				
					std::unordered_set<int> temp;
					temp.insert(payload[i][j]);
					found_values[j] = temp;
				}

			}
		}
	}

	return columns;
}


std::unordered_map<int, std::vector<std::vector<int>>> create_payload_dictionary(int s) {
	
  struct can_frame frame;
  //Process data
  //

  std::unordered_map< int, std::vector < std::vector < int >>> data;

  std::unordered_set < int > keys;

  while (!interrupted) {
    get_data(s, & frame);

    std::vector < int > bin_arr;

    for (int i = 0; i < frame.can_dlc; i++) {


      std::vector < int > temp = byte_to_binary_array(frame.data[i]);

      bin_arr.insert(std::end(bin_arr), std::begin(temp), std::end(temp));

    }

    if (keys.find(frame.can_id) != keys.end()) {

      data[frame.can_id].push_back(bin_arr);

    } else {

      keys.insert(frame.can_id);

      std::vector < std::vector < int >> temp;

      temp.push_back(bin_arr);

      data[frame.can_id] = temp;

    }
	

  }


  return data;
}
std::unordered_map<int, std::unordered_set<int>> generate_ignorables(int s) {
	
	std::unordered_map<int, std::vector<std::vector<int>>> data = create_payload_dictionary(s);

	std::unordered_map<int, std::unordered_set<int>> ignorables;

	for (const auto& payload : data) {

		std::unordered_set<int> columns = get_positive_variance_columns(payload.second);	

		if (columns.size() > 0) {

			ignorables[payload.first] = columns;

		}

	}

	return ignorables;

}


void handle_interrupt(int signal) {
	


	interrupted = true;


}

void sniffer(int s, std::unordered_map<int, std::unordered_set<int>> ignorables) {

	std::unordered_map<int, std::vector<int>> values;

	struct can_frame frame;

	while (!interrupted) {
		get_data(s, & frame);

		std::vector < int > bin_arr;

		for (int i = 0; i < frame.can_dlc; i++) {


			std::vector < int > temp = byte_to_binary_array(frame.data[i]);

			bin_arr.insert(std::end(bin_arr), std::begin(temp), std::end(temp));

		}

		if (values.find(frame.can_id) != values.end()) {

			for (int i = 0; i < bin_arr.size(); i++) {
				if (bin_arr[i] != values[frame.can_id][i]) {
					std::cout << "frame "<< frame.can_id<< " bit "<< i<< "" << std::endl;
				}


			}
		}

		values[frame.can_id] = bin_arr;


	}


}

int main(void)

{

  std::signal(SIGINT, handle_interrupt);
  int s;
  struct can_frame frame;
  //Open connection to can socket
  setup_network( & s);

  std::unordered_map<int, std::unordered_set<int>> ignorables = generate_ignorables(s);

  for (const auto& pair: ignorables) {

	  std::cout << pair.first << std::endl;
  }


  interrupted = false;

	
  sniffer(s, ignorables);


  return 0;
}
