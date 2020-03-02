#pragma once
#include "pch.h"
//#include <json.hpp>
//#include <curlpp/curlpp.hpp>
//#include <curlpp/Easy.hpp>
//#include <curlpp/Options.hpp>

#define INPUT_DIRECTION 0
#define OUTPUT_DIRECTION 1

const char* DIRECTION_ADDRESSES[];// = { "di_value", "do_value" };
const char* JSON_DIRECTION_ADDRESSES[];// = { "DIVal", "DOVal" };

using json = nlohmann::json;
using namespace std;

void parse_digital_inputs(json& GET_buffer, int input_state[]);
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

void parse_digital_outputs(json& GET_buffer, int output_state[]);
//{
//	int channel_number, output_value;
//	for (auto channel : GET_buffer["DOVal"])
//	{
//		channel["Ch"].get_to(channel_number);
//		channel["Val"].get_to(output_value);
//		output_state[channel_number] = output_value;
//	}
//}