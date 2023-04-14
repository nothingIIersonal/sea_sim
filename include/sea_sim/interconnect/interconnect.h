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
    std::string module_name;
    nlohmann::json ui_input, ui_output;

public:
    Interconnect() noexcept = delete;
    Interconnect(Endpoint, std::string);

    void print_json();
    void wgti_send();
    void wgto_send();

    void wgti_set_module_title(std::string);

    void wgti_sameline();
    void wgti_add_text(std::string);
    void wgti_add_button(std::string, std::string);
    void wgti_add_sliderint(std::string, int64_t, int64_t, int64_t);
    void wgti_add_sliderfloat(std::string, float, float, std::uint64_t);
    void wgti_add_inputtext(std::string, std::string);
    void wgti_add_inputint(std::string, std::string);
    void wgti_add_inputfloat(std::string, std::string);
    void wgti_add_checkbox(std::string, std::string);
    void wgti_add_radiobutton(std::string, std::vector<std::string>);
    void wgti_add_dropdownlist(std::string, std::vector<std::string>);

    void wgto_sameline();
    void wgto_add_text(std::string);

    ~Interconnect() noexcept = default;
};


#endif