#pragma once
#include "pch.h"
#include "Device_Interface.h"

//#include "WISE_Utilities.h"
//#include "ManagerFunctions.h"

using namespace std;

#ifndef WISEINTERFACE_H
#define WISEINTERFACE_H

//#include "pch.h"
//#include <json.hpp>
//#include <curlpp/curlpp.hpp>
//#include <curlpp/Easy.hpp>
//#include <curlpp/Options.hpp>

#define INPUT_DIRECTION 0
#define OUTPUT_DIRECTION 1

#define OUT_OPEN_DOOR_CMD 2
#define OUT_CLOSE_DOOR_CMD 1
#define IN_DOOR_OPEN_ST 0
#define IN_DOOR_CLOSING_ST 1

#define OUT_OPEN_DOOR_CMD_STRING "door_open_command"
#define OUT_CLOSE_DOOR_CMD_STRING "door_close_command"
#define IN_DOOR_OPEN_STATE_STRING "door_open_state"
#define IN_DOOR_CLOSING_STATE_STRING "door_closing_state"

//#define OPEN_DOOR_CMD_OBJECT open_door_cmd({ { OUT_OPEN_DOOR_CMD_STRING , 1 }, { OUT_CLOSE_DOOR_CMD_STRING , 0 } })
//#define CLOSE_DOOR_CMD_OBJECT const string_map ({ { OUT_OPEN_DOOR_CMD_STRING , 0 }, { OUT_CLOSE_DOOR_CMD_STRING , 1 } })

const ordered_string_map OPEN_DOOR_CMD_OBJECT({ { OUT_CLOSE_DOOR_CMD_STRING , 0 }, { OUT_OPEN_DOOR_CMD_STRING , 1 } });
const ordered_string_map CLOSE_DOOR_CMD_OBJECT({ { OUT_OPEN_DOOR_CMD_STRING , 0 }, { OUT_CLOSE_DOOR_CMD_STRING , 1 } });

#define ARRAYSIZE(x) ( sizeof(x)/sizeof(x[0]) )

//const char* DIRECTION_ADDRESSES[];// = { "di_value", "do_value" };
//const char* JSON_DIRECTION_ADDRESSES[];// = { "DIVal", "DOVal" };

//const char* DIRECTION_ADDRESSES[] = { "di_value", "do_value" };
//const char* JSON_DIRECTION_ADDRESSES[] = { "DIVal", "DOVal" };

using json = nlohmann::json;
using namespace std;

void parse_digital_inputs(json& GET_buffer, int input_state[]);

void parse_digital_outputs(json& GET_buffer, int output_state[]);

struct WISE_IO_Point {
	int slot = 0;
	int channel;
	int value = 0;
	int IO_direction = -1;
	std::string label;

public:
	WISE_IO_Point(int IO_direction, int channel_, std::string label_);
	WISE_IO_Point();
};

const std::vector<WISE_IO_Point> Door_IO_Configuration = {
	WISE_IO_Point(INPUT_DIRECTION, IN_DOOR_OPEN_ST, IN_DOOR_OPEN_STATE_STRING),
	WISE_IO_Point(INPUT_DIRECTION, IN_DOOR_CLOSING_ST, IN_DOOR_CLOSING_STATE_STRING),
	WISE_IO_Point(OUTPUT_DIRECTION, OUT_OPEN_DOOR_CMD, OUT_OPEN_DOOR_CMD_STRING),
	WISE_IO_Point(OUTPUT_DIRECTION, OUT_CLOSE_DOOR_CMD, OUT_CLOSE_DOOR_CMD_STRING)
};

class WISE_Interface : public Device_Interface
{
	//const int IO_direction;
	//const char* network_address;

public:
	std::unordered_map<std::string, WISE_IO_Point> IO_state;
	//std::unordered_map<std::string, WISE_IO_Point> output_commands;
	
	
	int input_state[4];
	int output_state[4];
	//int output_commands[4] = { 0, 0, 0, 0 };
	std::thread sync_thread;
	
	//static CURL* request_handle;

	
	//void get_state(const int(&state_copy)[]);
	void get_state(int* input_state_copy, int* output_state_copy);
	void stage_commands(const ordered_string_map& cmomand_map);
	//bool write_state(int state[]);
	
	bool setup_modules();
	//std::thread start_thread();
	void stop_thread();
	//void synchronize_states();

	WISE_Interface(const std::string network_address_);// : network_address{ *address } {};//, IO_direction{ direction }
	WISE_Interface(const WISE_Interface& WI);
	

private:
	CURL* request_handle;
	std::string read_buffer;
	//std::string read_buffer;
	json HTTP_result;
	const char* URL;

	std::string network_address;
	std::string input_URL;
	std::string output_URL;

	std::string build_URL(int IO_direction);

	std::unordered_map<std::string, int> output_commands;
	
	void GET(const char* URL, CURLcode& res);
	void PATCH(const char* URL, CURLcode& res, json& patch_data);
	bool synchronous_PATCH(const char* GET_URL, const char* PATCH_URL, CURLcode& res, json& PATCH_data);
	//void setup_module(const char* URL, CURLcode& res, int options[]);
	void setup_module_inputs(std::string& URL, CURLcode& res);
	void setup_module_outputs(std::string& URL, CURLcode& res);
	void set_cURL_options();
	bool read_module_states();
	bool write_command_state_to_module(const ordered_string_map& command_state);
	void handle_command(Device_Command& command);
	//void synchronize();
	void initialize_IO_state();
	void parse_io_data(json& GET_buffer, std::unordered_map<std::string, WISE_IO_Point> &io_map);
	std::unique_ptr<WISE_IO_Point> lookup_IO_point(const std::string& label);
	WISE_Interface(const WISE_Interface &WI, const std::lock_guard<std::mutex> &);
	void read_state();
};

size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);

#endif
