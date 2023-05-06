#ifndef INTERCONNECT
#define INTERCONNECT


#include <iostream>
#include <vector>
#include <cstring>
#include <optional>


#include <sea_sim/gears/channel_packet.h>


class Interconnect
{
private:
    Endpoint module_endpoint;
    std::string module_name;
    nlohmann::json ui_input, ui_output;
    nlohmann::json ui_fields;
    std::string ui_trigger;

public:
    Interconnect() noexcept = delete;
    Interconnect(const Interconnect &) noexcept = delete;
    void operator=(const Interconnect&) noexcept = delete;
    Interconnect(Endpoint, std::string);
    ~Interconnect();

    void wgti_send();
    void wgto_send();

    void wgti_set_module_title(std::string);

    void wgti_sameline();
    void wgti_add_text(std::string);
    void wgti_add_button(std::string, std::string);
    void wgti_add_sliderint(std::string, int64_t, int64_t);
    void wgti_add_sliderfloat(std::string, float, float, std::uint64_t);
    void wgti_add_inputtext(std::string, std::string);
    void wgti_add_inputint(std::string);
    void wgti_add_inputfloat(std::string);
    void wgti_add_checkbox(std::string, std::string);
    void wgti_add_radiobutton(std::string, std::vector<std::string>);
    void wgti_add_dropdownlist(std::string, std::vector<std::string>);

    void wgto_sameline();
    void wgto_add_text(std::string);

    std::string get_trigger();

    std::optional<int> get_field_int(std::string);
    std::optional<float> get_field_float(std::string);
    std::optional<std::string> get_field_string(std::string);
};


#endif