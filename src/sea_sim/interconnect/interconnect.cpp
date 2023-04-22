#include <sea_sim/interconnect/interconnect.h>

Interconnect::Interconnect(Endpoint module_endpoint, std::string module_name) : module_endpoint(module_endpoint), module_name(module_name)
{
    if (const auto &packet = this->module_endpoint.TryRead())
    {
        this->ui_fields = packet.value().data["fields"];
        this->ui_trigger = packet.value().data["trigger"].get<std::string>();
    }
    else
    {
        this->ui_trigger = "";
    }
}

Interconnect::~Interconnect()
{
    this->module_endpoint.SendData({ "core", this->module_name, "close_channel", {  } });
}

void Interconnect::wgti_send()
{
    module_endpoint.SendData({ "gui", this->module_name, "update_input_interface", this->ui_input });
}

void Interconnect::wgto_send()
{
    module_endpoint.SendData({ "gui", this->module_name, "update_output_interface", this->ui_output });
}

void Interconnect::wgti_set_module_title(std::string title)
{
    this->ui_input.push_back(
        {
            {"type", "module_title"},
            {"widget",
                {
                    {"title", title}
                }
            }
        }
    );
}

void Interconnect::wgti_sameline()
{
    this->ui_input.push_back(
        {
            {"type", "sameline"},
            {"widget",
                {

                }
            }
        }
    );
}

void Interconnect::wgti_add_text(std::string text)
{
    this->ui_input.push_back(
        {
            {"type", "text"},
            {"widget",
                {
                    {"text", text}
                }
            }
        }
    );
}

void Interconnect::wgti_add_button(std::string identifier, std::string text)
{
    this->ui_input.push_back(
        {
            {"type", "button"},
            {"widget",
                {
                    {"identifier", identifier},
                    {"text", text}
                }
            }
        }
    );
}

void Interconnect::wgti_add_sliderint(std::string identifier, int64_t from, int64_t to)
{
    this->ui_input.push_back(
        {
            {"type", "sliderint"},
            {"widget",
                {
                    {"identifier", identifier},
                    {"from", from},
                    {"to", to},
                }
            }
        }
    );
}

void Interconnect::wgti_add_sliderfloat(std::string identifier, float from, float to, uint64_t precision)
{
    this->ui_input.push_back(
        {
            {"type", "sliderfloat"},
            {"widget",
                {
                    {"identifier", identifier},
                    {"from", from},
                    {"to", to},
                    {"precision", precision}
                }
            }
        }
    );
}

void Interconnect::wgti_add_inputtext(std::string identifier, std::string placeholder)
{
    this->ui_input.push_back(
        {
            {"type", "inputtext"},
            {"widget",
                {
                    {"identifier", identifier},
                    {"placeholder", placeholder}
                }
            }
        }
    );
}

void Interconnect::wgti_add_inputint(std::string identifier)
{
    this->ui_input.push_back(
        {
            {"type", "inputint"},
            {"widget",
                {
                    {"identifier", identifier}
                }
            }
        }
    );
}

void Interconnect::wgti_add_inputfloat(std::string identifier)
{
    this->ui_input.push_back(
        {
            {"type", "inputfloat"},
            {"widget",
                {
                    {"identifier", identifier}
                }
            }
        }
    );
}

void Interconnect::wgti_add_checkbox(std::string identifier, std::string text)
{
    this->ui_input.push_back(
        {
            {"type", "checkbox"},
            {"widget",
                {
                    {"identifier", identifier},
                    {"text", text}
                }
            }
        }
    );
}

void Interconnect::wgti_add_radiobutton(std::string identifier, std::vector<std::string> elements)
{
    this->ui_input.push_back(
        {
            {"type", "radiobutton"},
            {"widget",
                {
                    {"identifier", identifier},
                    {"elements", elements}
                }
            }
        }
    );
}

void Interconnect::wgti_add_dropdownlist(std::string identifier, std::vector<std::string> elements)
{
    this->ui_input.push_back(
        {
            {"type", "dropdownlist"},
            {"widget",
                {
                    {"identifier", identifier},
                    {"elements", elements}
                }
            }
        }
    );
}

void Interconnect::wgto_sameline()
{
    this->ui_output.push_back(
        {
            {"type", "sameline"},
            {"widget",
                {
                    
                }
            }
        }
    );
}

void Interconnect::wgto_add_text(std::string text)
{
    this->ui_output.push_back(
        {
            {"type", "text"},
            {"widget",
                {
                    {"text", text}
                }
            }
        }
    );
}

std::string Interconnect::get_trigger()
{
    return this->ui_trigger;
}

std::optional<int> Interconnect::get_field_int(std::string field_name)
{
    if (this->ui_fields.contains(field_name) )
        return this->ui_fields[field_name].get<int>();
    return std::nullopt;
}

std::optional<float> Interconnect::get_field_float(std::string field_name)
{
    if (this->ui_fields.contains(field_name) )
        return this->ui_fields[field_name].get<float>();
    return std::nullopt;
}

std::optional<std::string> Interconnect::get_field_string(std::string field_name)
{
    if (this->ui_fields.contains(field_name) )
        return this->ui_fields[field_name].get<std::string>();
    return std::nullopt;
}