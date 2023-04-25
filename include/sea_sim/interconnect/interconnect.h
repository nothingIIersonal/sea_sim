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
    Interconnect(const Endpoint& module_endpoint, const std::string& module_name);
    ~Interconnect();

    void wgti_send();
    void wgto_send();

    void wgti_set_module_title(const std::string& title);

    void wgti_sameline();
    void wgti_add_text(const std::string& text);
    void wgti_add_button(const std::string& identifier, const std::string& text);
    void wgti_add_sliderint(const std::string& identifier, int64_t from, int64_t to);
    void wgti_add_sliderfloat(const std::string& identifier, float from, float to, uint64_t precision);
    void wgti_add_inputtext(const std::string& identifier, const std::string& placeholder);
    void wgti_add_inputint(const std::string& identifier);
    void wgti_add_inputfloat(const std::string& identifier);
    void wgti_add_checkbox(const std::string& identifier, const std::string& text);
    void wgti_add_radiobutton(const std::string& identifier, const std::vector<std::string>& elements);
    void wgti_add_dropdownlist(const std::string& identifier, const std::vector<std::string>& elements);

    void wgto_sameline();
    void wgto_add_text(const std::string& text);

    const std::string& get_trigger();

    std::optional<int> get_field_int(const std::string& field_name);
    std::optional<float> get_field_float(const std::string& field_name);
    std::optional<std::string> get_field_string(const std::string& field_name);
};


#endif