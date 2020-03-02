#pragma once
#include "pch.h"

class Device_Interface
{
public:
	Device_Interface() : object_name_("default_device_constructor") {};
	//void Device_Interface();
	//Device_Interface(const char* object_name);

protected:
	//Device_Interface(const char* object_name) : object_name_(object_name) {};
	Device_Interface(const char* object_name);
	
	static const int sleep_time_ms = 10;
	const char* object_name_;// = "generic_device_interface";
	long maximum_timeout = 1;

	std::atomic<bool> run_thread;
	std::mutex mtx;

	static const size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);
	std::thread start_thread(std::atomic<bool>& run_flag, void (*fh), Device_Interface& caller);
	//void start_thread();
	const void synchronize(Device_Interface* caller);
	//const void synchronize();
	virtual void synchronize_states();

 };