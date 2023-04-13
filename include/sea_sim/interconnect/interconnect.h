#ifndef INTERCONNECT
#define INTERCONNECT


#include <iostream>
#include <vector>
#include <cstring>


#include <sea_sim/gears/channel_packet.h>


class Interconnect
{
private:
    Endpoint module_endpoint;

public:
    Interconnect() noexcept = delete;
    Interconnect(Endpoint);

    void print_json();
    void send_json(channel_value_type);

    ~Interconnect() noexcept = default;
};


#endif