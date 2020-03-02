#include "pch.h"
#include "WISE_Interface.h"

//WISE_Interface::WISE_Interface() {};
const char* DIRECTION_ADDRESSES[] = { "di_value", "do_value" };
const char* JSON_DIRECTION_ADDRESSES[] = { "DIVal", "DOVal" };

WISE_IO_Point::WISE_IO_Point(int IO_direction_, int channel_, std::string label_) : IO_direction{ IO_direction_ }, channel{ channel_ }, label{ label_ } {};
WISE_IO_Point::WISE_IO_Point() : IO_direction{ 0 }, channel{ 0 }, label{ "" } {};

WISE_Interface::WISE_Interface(std::string network_address_) : network_address{ network_address_ }
{
	this->request_handle = curl_easy_init();
	if (this->request_handle) {}
	else
	{
		throw ERROR;
	}
	//this->URL = this->build_URL();
	this->initialize_IO_state();
	this->input_URL = this->build_URL(INPUT_DIRECTION);
	this->output_URL = this->build_URL(OUTPUT_DIRECTION);
	this->set_cURL_options();
	this->setup_modules();
}

//WISE_Interface::WISE_Interface(const WISE_Interface &WI) : WISE_Interface(WI, std::lock_guard<std::mutex>(WI.mtx)) {}
WISE_Interface::WISE_Interface(const WISE_Interface &WI) {}
//{
//	return;
//}

WISE_Interface::WISE_Interface(const WISE_Interface &WI, const std::lock_guard<std::mutex> &) : input_URL(WI.input_URL) {}

void WISE_Interface::GET(const char* URL, CURLcode& res)
{
	struct curl_slist *header_list = NULL;
	curl_easy_setopt(this->request_handle, CURLOPT_CUSTOMREQUEST, NULL);
	curl_easy_setopt(this->request_handle, CURLOPT_HTTPGET, 1L);
	curl_easy_setopt(this->request_handle, CURLOPT_URL, URL);
	//curl_easy_setopt(this->request_handle, CURLOPT_POSTFIELDS, NULL);
	//curl_easy_setopt(this->request_handle, CURLOPT_HTTPGET, (long)1);
	res = curl_easy_perform(this->request_handle);
	curl_slist_free_all(header_list);
	//res = curl_easy_perform(this->request_handle);
	
	if (res != CURLE_OK) fprintf(stderr, "curl_easy_perform failed with code: %s\n", curl_easy_strerror(res));

	std::cout << "GET result from " << this->network_address << ": " << this->read_buffer << "\n";

	//this->read_buffer.clear();
	//res = curl_easy_perform(this->request_handle);

	this->HTTP_result = json::parse(this->read_buffer);
	this->read_buffer.clear();
}

void WISE_Interface::initialize_IO_state()
{
	for (auto &io_point : Door_IO_Configuration)
	{
		memcpy(&(this->IO_state[io_point.label]), &io_point, sizeof(io_point));
		//this->IO_state[io_point.label] = io_point;
	}
}

bool WISE_Interface::read_module_states()
{
	CURLcode input_read_res;
	CURLcode output_read_res;
	
	//First read inputs
	//curl_easy_setopt(this->request_handle, CURLOPT_URL, this->input_URL.c_str());
	this->GET(this->input_URL.c_str(), input_read_res);
	parse_io_data(this->HTTP_result, this->IO_state);
	//parse_digital_inputs(this->HTTP_result, this->input_state);

	this->GET(this->output_URL.c_str(), output_read_res);
	parse_io_data(this->HTTP_result, this->IO_state);

	//if (res != CURLE_OK) fprintf(stderr, "curl_easy_perform failed with code: %s\n", curl_easy_strerror(res));
	return (input_read_res == CURLE_OK) && (output_read_res == CURLE_OK);
}

void WISE_Interface::synchronize()
{
	while (this->run_thread)
	{
		this->read_module_states();
		//this->write_state(this->output_commands);
		this->write_command_state(this->output_commands);
		//std::cout << "test\n";
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	std::cout << "exiting thread\n";
}

std::thread WISE_Interface::start_thread()
{
	//this->sync_thread = std::thread(&WISE_Interface::synchronize, this);
	this->run_thread = true;
	std::thread sync_thread = std::thread(&WISE_Interface::synchronize, this);
	//this->sync_thread = sync_thread;
	//this->run_thread = false;
	//sync_thread.join();
	//this->sync_thread = sync_thread;
	return sync_thread;
}


//void WISE_Interface::get_state(const int*(&state_copy)[])
void WISE_Interface::get_state(int* input_state_copy, int* output_state_copy)
{
	//int state_copy[4];
	const std::lock_guard<std::mutex> lock(this->mtx);
	memcpy((void *)input_state_copy, this->input_state, sizeof(this->input_state));
	memcpy((void *)output_state_copy, this->output_state, sizeof(this->output_state));
	//cout << "test";
	//memcpy(&state_copy, this->input_state, sizeof(this->input_state));
}

void WISE_Interface::stop_thread()
{
	this->run_thread = false;
}

void WISE_Interface::PATCH(const char* URL, CURLcode& res, json& patch_data)
{
	std::string patch_data_string = patch_data.dump();
	std::string header = "Content-Length: " + std::to_string(patch_data_string.length());
	
	struct curl_slist *header_list = NULL;
	header_list = curl_slist_append(header_list, header.c_str());

	curl_easy_setopt(this->request_handle, CURLOPT_URL, URL);
	//curl_easy_setopt(this->request_handle, CURLOPT_HTTPHEADER, header_list);
	//curl_easy_setopt(this->request_handle, CURLOPT_HTTPPATCH, (long)1);
	curl_easy_setopt(this->request_handle, CURLOPT_CUSTOMREQUEST, "PATCH");
	curl_easy_setopt(this->request_handle, CURLOPT_POSTFIELDS, patch_data_string.c_str());
	res = curl_easy_perform(this->request_handle);
	curl_slist_free_all(header_list);
	if (res != CURLE_OK) fprintf(stderr, "curl_easy_perform failed with code: %s\n", curl_easy_strerror(res));

}

bool WISE_Interface::write_command_state(std::unordered_map<std::string, int>& command_state)
{
	json patch_data;
	CURLcode write_state_res;

	for (auto& point : command_state)
	{
		this->output_commands[point.label] = point;
		patch_data["DOVal"][point.channel]["Ch"] = point.channel;
		patch_data["DOVal"][point.channel]["Val"] = point.value;
	}

	this->PATCH(this->output_URL.c_str(), write_state_res, patch_data);
	return write_state_res == CURLE_OK;
}

WISE_IO_Point& WISE_Interface::lookup_IO_point(std::string& label)
{

}

//bool WISE_Interface::write_state(int state[])
//{
//	CURLcode write_state_res;
//
//	json output_state;
//	for (int state_ndx = 0; state_ndx < sizeof(state); ++state_ndx)
//	{
//		output_state["DOVal"][state_ndx]["Ch"] = state_ndx;
//		output_state["DOVal"][state_ndx]["Val"] = state[state_ndx];
//		std::cout << output_state.dump() << "\n";
//	}
//
//	this->PATCH(this->output_URL.c_str(), write_state_res, output_state);
//	return write_state_res == CURLE_OK;
//}

bool WISE_Interface::setup_modules()
{
	CURLcode input_setup_res;
	CURLcode output_setup_res;

	this->setup_module_inputs(this->input_URL, input_setup_res);
	this->setup_module_outputs(this->output_URL, output_setup_res);

	return (input_setup_res == CURLE_OK) && (output_setup_res == CURLE_OK);
}

void WISE_Interface::setup_module_inputs(std::string& URL, CURLcode& res)
{
	json module_settings;
	for (int point_ndx = 0; point_ndx < 4; ++point_ndx)
	{
		module_settings["DIVal"][point_ndx]["Ch"] = point_ndx;
		module_settings["DIVal"][point_ndx]["Md"] = 0;
	}
	this->PATCH(URL.c_str(), res, module_settings);
}

void WISE_Interface::setup_module_outputs(std::string& URL, CURLcode& res)
{
	json module_settings;
	for (int point_ndx = 0; point_ndx < 4; ++point_ndx)
	{
		module_settings["DOVal"][point_ndx]["Ch"] = point_ndx;
		module_settings["DOVal"][point_ndx]["Md"] = 3;
		module_settings["DOVal"][point_ndx]["PsCtn"] = 0;
		module_settings["DOVal"][point_ndx]["Val"] = 0;
	}
	this->PATCH(URL.c_str(), res, module_settings);
}

std::string WISE_Interface::build_URL(int IO_direction)
{
	vector<string> address_vector;
	//address_vector.push_back("http:/");
	address_vector.push_back((this->network_address));
	//address_vector.push_back((std::string) "/");
	address_vector.push_back(DIRECTION_ADDRESSES[IO_direction]);
	address_vector.push_back("slot_0");

	std::string address;
	for (vector<string>::const_iterator p = address_vector.begin(); p != address_vector.end(); ++p)
	{
		address += *p;
		if (p != address_vector.end() - 1)
		{
			address += "/";
		}
	}
	return address;//.c_str();
}

void WISE_Interface::set_cURL_options()
{
	curl_easy_setopt(this->request_handle, CURLOPT_USERPWD, "root:00000000");
	curl_easy_setopt(this->request_handle, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(this->request_handle, CURLOPT_WRITEDATA, &(this->read_buffer));
}

void WISE_Interface::parse_io_data(json& GET_buffer, std::unordered_map<std::string, WISE_IO_Point> &io_map)
{
	int channel_number, value;
	//const char* directions [2][6]  = { "DIVal", "DOVal" };
	//std::string directions[2] = { "DIVal", "DOVal" };
	for (int dir_index = 0; dir_index < (sizeof(JSON_DIRECTION_ADDRESSES)/ sizeof(JSON_DIRECTION_ADDRESSES[0])); dir_index++)//(const char* dir : JSON_DIRECTION_ADDRESSES)
	{
		const char* dir_label = JSON_DIRECTION_ADDRESSES[dir_index];

		//Assume this will be run once per IO direction
		if (GET_buffer.count(dir_label) > 0)
		{
			for (auto point : Door_IO_Configuration)
			{
				if (point.IO_direction == dir_index)
				{
					int point_value = GET_buffer[dir_label][point.channel]["Val"];
					io_map[point.label].value = point_value;
				}

			}
		}
	}
}
//	if (GET_buffer.count("DIVal") > 0)
//	{
//
//	}
//	else if (GET_buffer.count("DOVal") > 0)
//	{
//		for (auto point : Door_IO_Configuration)
//		{
//			int point_value = GET_buffer["DOVal"][point.channel]["Val"];
//			io_map[point.label].value = point_value;
//		}
//		/*for (auto channel : GET_buffer["DOVal"])
//		{
//			channel["Ch"].get_to(channel_number);
//			channel["Val"].get_to(value);
//			io_map[channel_number] = input_value;
//		}*/
//	}
//	else
//	{
//		return false;
//	}
//}

void parse_digital_inputs(json& GET_buffer, int input_state[])
{
	int channel_number, input_value;
	for (auto channel : GET_buffer["DIVal"])
	{
		channel["Ch"].get_to(channel_number);
		channel["Val"].get_to(input_value);
		input_state[channel_number] = input_value;
	}
}

void parse_digital_outputs(json& GET_buffer, int output_state[])
{
	int channel_number, output_value;
	for (auto channel : GET_buffer["DOVal"])
	{
		channel["Ch"].get_to(channel_number);
		channel["Val"].get_to(output_value);
		output_state[channel_number] = output_value;
	}
}

size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}