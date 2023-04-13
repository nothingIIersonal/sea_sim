#pragma once


#include <string>
#include <sea_sim/json/json.hpp>
#include <sea_sim/gears/channels.hpp>


struct channel_packet
{
	std::string to;
	std::string from;
	std::string event;
	nlohmann::json data = {};
};


using channel_value_type = channel_packet;


namespace fdx = gears::fdx;
using Endpoint = fdx::ChannelEndpoint<channel_value_type>;