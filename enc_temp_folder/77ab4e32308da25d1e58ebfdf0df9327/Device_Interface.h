#pragma once
#include "pch.h"

using string_map = std::unordered_map<std::string, int>;
using ordered_string_map = std::map<std::string, int>;

struct Device_Command {
	std::string command_name;
	int value;

public:
	Device_Command(std::string command_name_, int value_);
};

class Device_Interface
{
public:
	Device_Interface() : object_name_("default_device_constructor") {};
	//void Device_Interface();
	//Device_Interface(const char* object_name);
	std::thread start_thread();

protected:
	//Device_Interface(const char* object_name) : object_name_(object_name) {};
	Device_Interface(const char* object_name);
	
	static const int sleep_time_ms = 10;
	const char* object_name_;// = "generic_device_interface";
	long maximum_timeout = 1;

	std::atomic<bool> run_thread;
	std::mutex mtx;
	std::queue<Device_Command> command_queue;

	static const size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);
	//std::thread start_thread(std::atomic<bool>& run_flag, void (*fh), Device_Interface& caller);
	
	//void start_thread();
	const void synchronize();
	//const void synchronize(Device_Interface* caller);
	const void enqueue_command(const Device_Command& command);
	//const void synchronize();
	virtual void synchronize_states();
	virtual void handle_command(const Device_Command& command);
	virtual void read_state();

 };