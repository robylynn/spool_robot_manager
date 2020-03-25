#include "pch.h"
#include "Device_Interface.h"

//Device_Interface::sleep_time_ms = 10;

Device_Command::Device_Command(std::string command_name_, int value_) : command_name{ command_name_ }, value{ value_ } {};

Device_Interface::Device_Interface(const char* object_name) : object_name_(object_name) {};

const size_t Device_Interface::WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

//std::thread Device_Interface::start_thread(std::atomic<bool>& run_flag, void (*synchronize_fh), Device_Interface& caller)
std::thread Device_Interface::start_thread()
{
	//run_flag = true;
	this->run_thread = true;
	//std::thread sync_thread = std::thread(&synchronize_fh, caller);
	//void *fh = &(this->synchronize_states);
	//synchronization_fcn *fh = &Device_Interface::synchronize_states;
	std::thread sync_thread = std::thread(&Device_Interface::synchronize, this);
	//std::thread sync_thread = std::thread(&(this->synchronize), this);
	return sync_thread;
}

//void Device_Interface::start_thread()
//{
//	return;
//}
const void Device_Interface::enqueue_command(const Device_Command& command)
{
	this->command_queue.push(command);
}

void Device_Interface::handle_command(const Device_Command& command)
{
	std::cout << "handling command: " << command.command_name;
	return;
}

//const void Device_Interface::synchronize(Device_Interface* caller)
//{
//	while (caller->run_thread)
//	{
//		if (!this->command_queue.empty())
//		{
//			Device_Command command = this->command_queue.front();
//			//caller->synchronize_states();
//			caller->read_state();
//			caller->handle_command(command);
//			this->command_queue.pop();
//		}
//		
//		//this->read_module_states();
//		//this->write_state(this->output_commands);
//		//std::cout << "test\n";
//		std::this_thread::sleep_for(std::chrono::milliseconds(caller->sleep_time_ms));
//	}
//	std::cout << "exiting thread " << caller->object_name_ << "\n";
//}

const void Device_Interface::synchronize()
{
	while (this->run_thread)
	{
		this->read_state();
		if (!this->command_queue.empty())
		{
			Device_Command command = this->command_queue.front();
			//caller->synchronize_states();
			//this->read_state();
			this->handle_command(command);			
			this->command_queue.pop();
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(this->sleep_time_ms));
	}
	std::cout << "exiting thread " << this->object_name_ << "\n";
}

//const void Device_Interface::synchronize()
//{
//	return;
//}

void Device_Interface::synchronize_states() {};

void Device_Interface::read_state() {};