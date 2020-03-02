#include "pch.h"
#include "Device_Interface.h"

//Device_Interface::sleep_time_ms = 10;

Device_Interface::Device_Interface(const char* object_name) : object_name_(object_name) {};

const size_t Device_Interface::WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

std::thread Device_Interface::start_thread(std::atomic<bool>& run_flag, void (*synchronize_fh), Device_Interface& caller)
{
	run_flag = true;
	//std::thread sync_thread = std::thread(&synchronize_fh, caller);
	//void *fh = &(this->synchronize_states);
	//synchronization_fcn *fh = &Device_Interface::synchronize_states;
	std::thread sync_thread = std::thread(&Device_Interface::synchronize_states, this);
	return sync_thread;
}

//void Device_Interface::start_thread()
//{
//	return;
//}

const void Device_Interface::synchronize(Device_Interface* caller)
{
	while (caller->run_thread)
	{
		caller->synchronize_states();
		//this->read_module_states();
		//this->write_state(this->output_commands);
		//std::cout << "test\n";
		std::this_thread::sleep_for(std::chrono::milliseconds(caller->sleep_time_ms));
	}
	std::cout << "exiting thread " << caller->object_name_ << "\n";
}

//const void Device_Interface::synchronize()
//{
//	return;
//}

void Device_Interface::synchronize_states() {};