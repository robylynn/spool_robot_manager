// vcpp_manager.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
//#include <string>
//#include <vector>

//#include <curlpp/curlpp.hpp>
//#include <curlpp/Easy.hpp>
//#include <curlpp/Options.hpp>
//#include <json.hpp>
//#include "WISEUtilities.h"
//#include "ManagerFunctions.h"
#include "WISE_Interface.h"
#include "MiR_Interface.h"

using namespace std;
//using json = nlohmann::json;

//static std::string readBuffer;

//unsigned int write_cb(char* in, unsigned int size, unsigned int nmemb, string& out) {
//	out.append(in);
//	return size * nmemb;
//}

//static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
//{
//	((std::string*)userp)->append((char*)contents, size * nmemb);
//	return size * nmemb;
//}

//CURLcode read_WISE_inputs(std::string& readBuffer, json& GET_buffer, CURL *request_handle, int input_state[])
//{
//	curl_easy_setopt(&request_handle, CURLOPT_URL, "utd4_door_east_ethernet_iomodule/di_value/slot_0");
//	curl_easy_setopt(&request_handle, CURLOPT_HTTPGET, (long)1);
//
//	GET_buffer = json::parse(readBuffer);
//
//	parse_digital_inputs(GET_buffer, input_state);
//
//	return curl_easy_perform(&request_handle);
//}

//CURLcode read_WISE_state(std::string& readBuffer, json& GET_buffer, CURL *request_handle, int state[], const char* module_address)
//{
//	curl_easy_setopt(&request_handle, CURLOPT_URL, "utd4_door_east_ethernet_iomodule/di_value/slot_0");
//	curl_easy_setopt(&request_handle, CURLOPT_HTTPGET, (long)1);
//
//	GET_buffer = json::parse(readBuffer);
//
//	parse_digital_inputs(GET_buffer, input_state);
//
//	return curl_easy_perform(&request_handle);
//}

//class WISE_Interface
//{
//	//const int IO_direction;
//	const char network_address;
//
//	public:
//		static int state[4];
//		//static CURL* request_handle;
//
//		CURLcode read_WISE_state()
//		{
//			curl_easy_setopt(this->request_handle, CURLOPT_URL, this->URL);
//			curl_easy_setopt(this->request_handle, CURLOPT_HTTPGET, (long)1);
//			curl_easy_perform(this->request_handle);
//
//			this->HTTP_result = json::parse(this->read_buffer);
//			parse_digital_inputs(this->HTTP_result, this->state);
//
//			//return curl_easy_perform(this->request_handle);
//		}
//
//		//void update_state()
//		//{
//
//		//}
//
//		WISE_Interface(const char address[]) : network_address{ *address }//, IO_direction{ direction }
//		{
//			this->request_handle = curl_easy_init();
//			//this->URL = this->build_URL();
//			this->input_URL = build_URL(INPUT_DIRECTION);
//			this->output_URL = build_URL(OUTPUT_DIRECTION);
//		}
//		/*{
//			this->network_address = network_address;
//			this->IO_direction = IO_direction;
//		}*/
//
//	private:
//		static CURL* request_handle;
//		static std::string read_buffer;
//		static json HTTP_result;
//		static const char* URL;
//
//		static const char* input_URL;
//		static const char* output_URL;
//
//		const char* build_URL(int IO_direction)
//		{
//			vector<string> address_vector;
//			address_vector.push_back(&(this->network_address));
//			//address_vector.push_back((std::string) "/");
//			address_vector.push_back(DIRECTION_ADDRESSES[IO_direction]);
//			address_vector.push_back("slot_0");
//
//			std::string address;
//			for (vector<string>::const_iterator p = address_vector.begin(); p != address_vector.end(); ++p)
//			{
//				address += *p;
//				if (p != address_vector.end() - 1)
//				{
//					address += "/";
//				}
//			}
//			return address.c_str();
//		}
//
//		void set_cURL_options()
//		{
//			curl_easy_setopt(this->request_handle, CURLOPT_USERPWD, "root:00000000");
//			curl_easy_setopt(this->request_handle, CURLOPT_HTTPGET, (long)1);
//			curl_easy_setopt(this->request_handle, CURLOPT_WRITEFUNCTION, WriteCallback);
//			curl_easy_setopt(this->request_handle, CURLOPT_WRITEDATA, this->read_buffer);
//		}
//};
		//CURLcode read_WISE_state(std::string& readBuffer, json& GET_buffer, CURL *request_handle, int state[], const char* module_address)
		
			
		//return std::string.join(this.address, 
//void parse_digital_inputs(json& GET_buffer, int input_state[])
//{
//	int channel_number, input_value;
//	//int input_value;
//	for (auto channel : GET_buffer["DIVal"])
//	{
//		//std::cout << "range 2 value: " << channel << "\n";
//		//auto channel_number = channel.find("Ch");
//		channel["Ch"].get_to(channel_number);
//		channel["Val"].get_to(input_value);
//		//std::cout << "channel number is: " << channel_number << "\n";
//		input_state[channel_number] = input_value;
//	}
//}
//
//void parse_digital_outputs(json& GET_buffer, int output_state[])
//{
//	int channel_number, output_value;
//	for (auto channel : GET_buffer["DOVal"])
//	{
//		channel["Ch"].get_to(channel_number);
//		channel["Val"].get_to(output_value);
//		output_state[channel_number] = output_value;
//	}
//}



int main(int argc, char** argv)
{
	CURL *curl;
	CURLcode res;
	std::string readBuffer;
	//json j;
	
	static int input_state[4];
	static int output_state[4];

	static json http_GET_result;

	curl_global_init(CURL_GLOBAL_ALL);

	WISE_Interface East_Door_IO = WISE_Interface("utd4_door_east_ethernet_iomodule");
	//std::mutex East_Door_mutex;

	WISE_Interface West_Door_IO = WISE_Interface("utd4_door_west_ethernet_iomodule");
	//std::mutex West_Door_mutex;

	//East_Door_IO.read_internal_state();
	//West_Door_IO.read_internal_state();
	
	/*
	MiR_Interface UTD_Robot = MiR_Interface("utd_robot", "draw_mir");
	
	//MiR_Interface UTD_Robot = MiR_Interface("10.1.27.60", "draw_mir");
	
	std::unordered_map<std::string, int> register_state;
	UTD_Robot.synchronize_states(register_state);
	*/

	std::thread edthread = East_Door_IO.start_thread();
	std::thread wdthread = West_Door_IO.start_thread();

	int east_input_state_copy[4] = { 0,0,0,0 };
	int east_output_state_copy[4] = { 0,0,0,0 };
	//int(&state_copy)[];
	East_Door_IO.get_state(east_input_state_copy, east_output_state_copy);

	edthread.join();
	wdthread.join();

	//std::thread t_East_Door_IO(WISE_Interface::WISE_Interface("utd4_door_east_ethernet_iomodule"), NULL);
	
	//int state_vector[] = { 0, 0, 0, 1 };
	//East_Door_IO.write_state(state_vector);

	/*state_vector[3] = 0;
	East_Door_IO.write_state(state_vector);

	state_vector[3] = 1;
	East_Door_IO.write_state(state_vector);

	state_vector[3] = 0;
	East_Door_IO.write_state(state_vector);*/

	//East_Door_IO.read_state();

	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, "utd4_door_east_ethernet_iomodule/di_value/slot_0");
		curl_easy_setopt(curl, CURLOPT_USERPWD, "root:00000000");
		curl_easy_setopt(curl, CURLOPT_HTTPGET, (long)1);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		//curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
		//curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "

		res = curl_easy_perform(curl);
		std::cout << "curl result: " << readBuffer;

		http_GET_result = json::parse(readBuffer);

		//read_WISE_inputs(readBuffer, http_GET_result, curl, input_state);
		//string ts = http_get_result["DIVal"];
		//auto dival = http_get_result.find("DIVal");
		
		//int channel1val;
		//http_get_result["DIVal"][0]["Val"].get_to(channel1val);
		//std::cout << "channel 1 value is: " << channel1val;

		//parse_digital_inputs(http_GET_result, input_state);
		//parse_digital_outputs(http_GET_result, output_state);

		for (auto rack : http_GET_result)
		{
			std::cout << "range value: " << rack << "\n";
			for (auto channel : rack)
			{
				int channel_number;
				std::cout << "range 2 value: " << channel << "\n";
				//auto channel_number = channel.find("Ch");
				channel["Ch"].get_to(channel_number);
				std::cout << "channel number is: " << channel_number << "\n";
			}
			
		}

		//for (auto rack = http_get_result.begin(); rack != http_get_result.end(); ++rack)
		//{
		//	std:cout << "key: " << rack.key() << ", value: " << rack.value() << "\n";
		//	for (auto channel : rack.begin(); )
		//	{
		//		std::cout << "ch key: " << channel.key() << ", value: " << channel.value() << "\n";
		//	}
		//}

		//auto a = dival["Ch"]
		//auto chval = dival.find("Ch");
		//auto ts = http_get_result["DIVal"].get <std::string>();
		if (res != CURLE_OK) fprintf(stderr, "curl_easy_perform failed with code: %s\n", curl_easy_strerror(res));

		curl_easy_cleanup(curl);
	}
	curl_global_cleanup();

	std::cout << "Hello World!\n";
	std::cout << readBuffer << std::endl;
	
	return 1;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
