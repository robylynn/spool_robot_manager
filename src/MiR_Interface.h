#pragma once

#ifndef MIRINTERFACE_H
#define MIRINTERFACE_H

#include "pch.h"
#include "Device_Interface.h"

#define UT3_MODULE_ALIVE 3
#define UT4_MODULE_ALIVE 4
#define UT3_DOOR_OPEN_CMD 5
#define UT3_DOOR_CLOSE_CMD 6
#define UT4_DOOR_OPEN_CMD 7
#define UT4_DOOR_CLOSE_CMD 8
#define UT3_DOOR_OPENED 9
#define UT4_DOOR_OPENED 10
#define UT3_DOOR_CLOSING 11
#define UT4_DOOR_CLOSING 12

#define UT3_DOOR_OPEN_CMD_STRING "open_ut3_door"
#define UT4_DOOR_OPEN_CMD_STRING "open_ut4_door"
#define UT3_DOOR_CLOSE_CMD_STRING "close_ut3_door"
#define UT4_DOOR_CLOSE_CMD_STRING "close_ut4_door"
#define UT3_MODULE_ALIVE_STRING "ut3_module_alive"
#define UT4_MODULE_ALIVE_STRING "ut4_module_alive"
#define UT3_DOOR_OPENED_STRING "ut3_door_opened"
#define UT4_DOOR_OPENED_STRING "ut4_door_opened"
#define UT3_DOOR_CLOSING_STRING "ut3_door_closing"
#define UT4_DOOR_CLOSING_STRING "ut4_door_closing"

#define DOOR_STATE 0
#define ROBOT_COMMAND 1

#define ONBOARD_WISE_IO_NAME "Onboard Wise IO"

using namespace std;
using json = nlohmann::json;
using multi_level_map = unordered_map<string, unordered_map<string, int>>;
//size_t WriteCallback2(void *contents, size_t size, size_t nmemb, void *userp);

struct MiR_Register {
	int id = 0;
	int value = 0;
	std::string label = "";
	int type = -1;

public:
	MiR_Register(int id_, int value_, std::string label_, int type_);
};

struct MiR_IO_Module {
	string guid = "";
	string type = "";
	string name = "";
	string URL = "";

public:
	MiR_IO_Module();
	MiR_IO_Module(string name_, string type_, string guid_);
};

//const MiR_Register register_configuration[] = {
//	MiR_Register(UT3_DOOR_OPEN_CMD, 0.0, UT3_DOOR_OPEN_CMD_STRING),
//	MiR_Register(UT4_DOOR_OPEN_CMD, 0.0, UT4_DOOR_OPEN_CMD_STRING),
//	MiR_Register(UT3_DOOR_CLOSE_CMD, 0.0, UT3_DOOR_CLOSE_CMD_STRING),
//	MiR_Register(UT4_DOOR_CLOSE_CMD, 0.0, UT4_DOOR_CLOSE_CMD_STRING),
//	MiR_Register(UT3_MODULE_ALIVE, 0.0, UT3_MODULE_ALIVE_STRING),
//	MiR_Register(UT4_MODULE_ALIVE, 0.0, UT4_MODULE_ALIVE_STRING),
//	MiR_Register(UT3_DOOR_OPENED, 0.0, UT3_DOOR_OPENED_STRING),
//	MiR_Register(UT4_DOOR_OPENED, 0.0, UT4_DOOR_OPENED_STRING),
//	MiR_Register(UT3_DOOR_CLOSING, 0.0, UT3_DOOR_CLOSING_STRING),
//	MiR_Register(UT4_DOOR_CLOSING, 0.0, UT4_DOOR_CLOSING_STRING)
//};

const std::vector<MiR_Register> register_configuration = {
	MiR_Register(UT3_DOOR_OPEN_CMD, 0, UT3_DOOR_OPEN_CMD_STRING, ROBOT_COMMAND),
	MiR_Register(UT4_DOOR_OPEN_CMD, 0, UT4_DOOR_OPEN_CMD_STRING, ROBOT_COMMAND),
	MiR_Register(UT3_DOOR_CLOSE_CMD, 0, UT3_DOOR_CLOSE_CMD_STRING, ROBOT_COMMAND),
	MiR_Register(UT4_DOOR_CLOSE_CMD, 0, UT4_DOOR_CLOSE_CMD_STRING, ROBOT_COMMAND),
	MiR_Register(UT3_MODULE_ALIVE, 0, UT3_MODULE_ALIVE_STRING, DOOR_STATE),
	MiR_Register(UT4_MODULE_ALIVE, 0, UT4_MODULE_ALIVE_STRING, DOOR_STATE),
	MiR_Register(UT3_DOOR_OPENED, 0, UT3_DOOR_OPENED_STRING, DOOR_STATE),
	MiR_Register(UT4_DOOR_OPENED, 0, UT4_DOOR_OPENED_STRING, DOOR_STATE),
	MiR_Register(UT3_DOOR_CLOSING, 0, UT3_DOOR_CLOSING_STRING, DOOR_STATE),
	MiR_Register(UT4_DOOR_CLOSING, 0, UT4_DOOR_CLOSING_STRING, DOOR_STATE)
};
//register_configuration[0] = MiR_Register(0, 0.0, UT3_DOOR_OPEN_CMD_STRING, "");

using IO_map = unordered_map<string, MiR_IO_Module>;

class MiR_Interface: public Device_Interface
{
	const char* network_address_;
	const char* b64authorization_ = "ZGlzdHJpYnV0b3I6NjJmMmYwZjFlZmYxMGQzMTUyYzk1ZjZmMDU5NjU3NmU0ODJiYjhlNDQ4MDY0MzNmNGNmOTI5NzkyODM0YjAxNA==";
	//const char* b64authorization_header_ = strcat((char*) "Authorization: Basic ", "ZGlzdHJpYnV0b3I6NjJmMmYwZjFlZmYxMGQzMTUyYzk1ZjZmMDU5NjU3NmU0ODJiYjhlNDQ4MDY0MzNmNGNmOTI5NzkyODM0YjAxNA");
	std::string _header_ = ((std::string) "Authorization: Basic " + (std::string) "ZGlzdHJpYnV0b3I6NjJmMmYwZjFlZmYxMGQzMTUyYzk1ZjZmMDU5NjU3NmU0ODJiYjhlNDQ4MDY0MzNmNGNmOTI5NzkyODM0YjAxNA==");
	const char* b64authorization_header_ = _header_.c_str();
	const char* content_type_header_ = "Content-Type: application/json";
	const char* accept_language_header_ = "Accept-Language: en_US";
	const char* api_address_ = "api/v2.0.0";
	
	typedef void (MiR_Interface::*synchronization_fcn)();

public:
	MiR_Interface(const MiR_Interface&);
	MiR_Interface(const char* address, const char* object_name);

	//std::thread start_thread();
	void synchronize_states(std::unordered_map<std::string, int>& door_state_map);

	std::unordered_map<std::string, float> register_state;
	IO_map internal_IO_configuration;

	//static multi_level_map IO_device_state_map;
	static string_map IO_device_state_map;
	void read_internal_WISE_IO_config();

private:
	CURL* request_handle;
	std::string read_buffer;
	json HTTP_result;
	const char* URL;
	atomic<bool> run_thread;
	
	string register_bank_URL;
	string io_modules_URL;

	void build_URL(const char* endpoint, std::string& target_URL);
	void build_register_URL(int register_address, std::string& register_URL);
	
	//void synchronize();
	void set_cURL_options();
	void GET(const char* URL, CURLcode& res);
	void PUT(const char* URL, CURLcode& res, const json& register_data);
	void handle_command(Device_Command& command);
	void read_register_state(const int register_address, std::unordered_map<std::string, int>* register_map);
	bool read_all_registers(std::unordered_map<std::string, int>* register_map);
	bool write_register_state(const MiR_Register* register_state);
	void parse_registers(const json& GET_buffer, std::unordered_map<std::string, int>& result);
	void setup_registers(const std::vector<MiR_Register>& register_config);
	void read_command_registers();
	void write_state_registers(std::unordered_map<std::string, int>& door_state_map);
	void copy_robot_state(string_map* robot_state_map);
	int lookup_state_value(string& point_id);
	void read_state();
	//void parse_io_config(const json& GET_buffer, );
	void parse_io_state(const json& GET_buffer, string_map& result);
	
	void read_IO_module_state(string module_name, string_map& io_map);
	//void synchronize();
	
	

};

#endif