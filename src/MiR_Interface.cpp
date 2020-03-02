#include "pch.h"
#include "MiR_Interface.h"

MiR_Register::MiR_Register(int id_, int value_, std::string label_, int type_) : id{ id_ }, value{ value_ }, label{ label_ }, type{ type_ } {};

MiR_Interface::MiR_Interface(const MiR_Interface&) {};

MiR_Interface::MiR_Interface(const char* address, const char* object_name) : network_address_{ address }, Device_Interface{ object_name }
{
	this->request_handle = curl_easy_init();
	if (this->request_handle) {}
	else
	{
		throw ERROR;
	}
	//this->URL = this->build_URL();
	//std::string built_URL;
	this->build_URL(this->register_bank_URL);
	//this->register_bank_URL = this->build_URL());
	//this->output_URL = this->build_URL(OUTPUT_DIRECTION);
	this->set_cURL_options();
	this->setup_registers(register_configuration);

	//this->setup_modules();
	//std::string URL = (std::string) address;
	//return;
}

void MiR_Interface::build_URL(std::string& URL)
{
	vector<string> address_vector;
	address_vector.push_back(this->network_address_);
	address_vector.push_back(this->api_address_);
	address_vector.push_back("registers");

	std::string address;
	for (vector<string>::const_iterator p = address_vector.begin(); p != address_vector.end(); ++p)
	{
		address += *p;
		if (p != address_vector.end() - 1)
		{
			address += "/";
		}
	}
	//return address;
	URL = address;
}

void MiR_Interface::set_cURL_options()
{
	//curl_easy_setopt(this->request_handle, CURLOPT_USERPWD, "root:00000000");
	struct curl_slist *header_list = NULL;
	header_list = curl_slist_append(header_list, this->b64authorization_header_);
	header_list = curl_slist_append(header_list, this->content_type_header_);
	header_list = curl_slist_append(header_list, this->accept_language_header_);
	
	curl_easy_setopt(this->request_handle, CURLOPT_HTTPHEADER, header_list);
	
	//curl_easy_setopt(this->request_handle, CURLOPT_WRITEFUNCTION, (void *) this->WriteCallback);
	curl_easy_setopt(this->request_handle, CURLOPT_WRITEFUNCTION, this->WriteCallback);
	curl_easy_setopt(this->request_handle, CURLOPT_WRITEDATA, &(this->read_buffer));
	curl_easy_setopt(this->request_handle, CURLOPT_TIMEOUT, this->maximum_timeout);
	
	//curl_slist_free_all(header_list);
	
	//header_list = curl_slist_append(header_list, "Expect: 100 - continue");
}

void MiR_Interface::GET(const char* URL, CURLcode& res)
{
	curl_easy_setopt(this->request_handle, CURLOPT_CUSTOMREQUEST, NULL);
	curl_easy_setopt(this->request_handle, CURLOPT_HTTPGET, 1L);
	curl_easy_setopt(this->request_handle, CURLOPT_URL, URL);
	curl_easy_setopt(this->request_handle, CURLOPT_TIMEOUT, this->maximum_timeout);
	res = curl_easy_perform(this->request_handle);
	
	if (res != CURLE_OK) fprintf(stderr, "curl_easy_perform failed with code: %s\n", curl_easy_strerror(res));

	std::cout << "GET result from " << this->network_address_ << ": " << this->read_buffer << "\n";

	this->HTTP_result = json::parse(this->read_buffer);
	this->read_buffer.clear();
}

void MiR_Interface::parse_registers(const json& GET_buffer, std::unordered_map<std::string, int>& result)
{
	for (const auto &register_data : GET_buffer)
	{
		std::string register_label = register_data["label"];
		//const char* register_label_c = register_label.c_str();
		float register_value = register_data["value"];
		result[register_label.c_str()] = register_value;
	}
}

void MiR_Interface::PUT(const char* URL, CURLcode& res, const json& register_data)
{
	curl_easy_setopt(this->request_handle, CURLOPT_CUSTOMREQUEST, "PUT");
	//curl_easy_setopt(this->request_handle, CURLOPT_POST, 1L);
	curl_easy_setopt(this->request_handle, CURLOPT_URL, URL);
	//curl_easy_setopt(this->request_handle, CURLOPT_TIMEOUT, this->maximum_timeout);
	
	std::string put_data_string = register_data.dump();
	curl_easy_setopt(this->request_handle, CURLOPT_POSTFIELDS, put_data_string.c_str());
	
	res = curl_easy_perform(this->request_handle);

	if (res != CURLE_OK) fprintf(stderr, "curl_easy_perform failed with code: %s\n", curl_easy_strerror(res));
	//std::cout << "attempted to post: " << register_map.dump() << "\n";

	std::cout << "PUT result from " << this->network_address_ << ": " << this->read_buffer << "\n";
	this->read_buffer.clear();
}


void MiR_Interface::setup_registers(const std::vector<MiR_Register>& register_config)
{
	bool write_success = true;
	for (const MiR_Register &reg : register_config)
	{
		write_success &= this->write_register_state(&reg);
	}
}

void MiR_Interface::read_register_state(const int register_address, std::unordered_map<std::string, int>* register_map)
{
	//std::string URL = (std::string) address;
	//return;
	CURLcode register_read_res;
	std::string register_address_URL;
	
	this->build_register_URL(register_address, register_address_URL);
	this->GET(register_address_URL.c_str(), register_read_res);

}

bool MiR_Interface::read_all_registers(std::unordered_map<std::string, int>* register_map)
{
	CURLcode register_read_res;
	this->GET(this->register_bank_URL.c_str(), register_read_res);
	this->parse_registers(this->HTTP_result, *register_map);

	return (register_read_res == CURLE_OK);
}

bool MiR_Interface::write_register_state(const MiR_Register* register_state)
{
	json post_data;
	post_data["label"] = (*register_state).label.c_str();
	//post_data["value"] = (*register_state).value;
	//post_data["id"] = (*register_state).id;
	std::string register_URL;
	this->build_register_URL((*register_state).id, register_URL);
	
	CURLcode res;
	this->PUT(register_URL.c_str(), res, post_data);

	return (res == CURLE_OK);
}

//void MiR_Interface::write_all_registers(std::unordered_map<std::string, float>& register_map)
//{
//
//}

void MiR_Interface::read_command_registers()
{
	std::unordered_map<std::string, int> current_register_state;
	this->read_all_registers(&current_register_state);

	//const std::lock_guard<std::mutex> lock(this->mtx);
	for (const auto &reg : register_configuration)
	{
		if ((&reg)->type == ROBOT_COMMAND)
		{
			std::string label = (&reg)->label;
			this->register_state[label] = current_register_state[label];
		}
		else if ((&reg)->type == DOOR_STATE)
		{
			continue;
		}
	}
}

void MiR_Interface::write_state_registers(std::unordered_map<std::string, int> &door_state_map)
{
	//const std::lock_guard<std::mutex> lock(this->mtx);
	for (const auto &reg : register_configuration)
	{
		if ((&reg)->type == ROBOT_COMMAND)
		{
			continue;
		}
		else if ((&reg)->type == DOOR_STATE)
		{
			std::string label = (&reg)->label;
			this->register_state[label] = door_state_map[label];
		}
	}
}

void MiR_Interface::synchronize_states(std::unordered_map<std::string, int>& door_state_map)
{
	//std::unordered_map<std::string, int> register_map;
	//this->read_register_state(this->network_address_, &register_map);
	//this->read_all_registers(&register_map);
	const std::lock_guard<std::mutex> lock(this->mtx);
	this->read_command_registers();
	this->write_state_registers(door_state_map);
	//this->write_register_state(nullptr);

}

void MiR_Interface::copy_robot_state(string_map* robot_state_map)
{
	const std::lock_guard<std::mutex> lock(this->mtx);
	memcpy(robot_state_map, &(this->register_state), sizeof(this->register_state));
}

void MiR_Interface::build_register_URL(int register_address, std::string& register_URL)
{
	register_URL += this->register_bank_URL;
	register_URL += "/";
	register_URL += std::to_string(register_address);
}

//void MiR_Interface::synchronize()
//{
//	while (this->run_thread)
//	{
//		this->read_module_states();
//		this->write_state(this->output_commands);
//		//std::cout << "test\n";
//		std::this_thread::sleep_for(std::chrono::milliseconds(10));
//	}
//	std::cout << "exiting thread\n";
//}
//
//std::thread MiR_Interface::start_thread()
//{
//	this->run_thread = true;
//	std::thread sync_thread = std::thread(&WISE_Interface::synchronize, this);
//	return sync_thread;
//}

//size_t WriteCallback2(void *contents, size_t size, size_t nmemb, void *userp)
//{
//	((std::string*)userp)->append((char*)contents, size * nmemb);
//	return size * nmemb;
//}