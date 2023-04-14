#include <sea_sim/interconnect/interconnect.h>

Interconnect::Interconnect(Endpoint module_endpoint, std::string module_name) : module_endpoint(module_endpoint), module_name(module_name) {}

void Interconnect::print_json()
{
    while (const auto &val = module_endpoint.TryRead())
    {
        std::cout << "Received packet:\n";
        std::cout << "\tTo: "    << val.value().to    << "\n";
        std::cout << "\tFrom: "  << val.value().from  << "\n";
        std::cout << "\tEvent: " << val.value().event << "\n";
        std::cout << "\tData: "  << val.value().data  << "\n";
    }
    std::cout << std::endl;
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

void Interconnect::wgti_add_sliderint(std::string identifier, int64_t from, int64_t to, int64_t step)
{
    this->ui_input.push_back(
        {
            {"type", "sliderint"},
            {"widget",
                {
                    {"identifier", identifier},
                    {"from", from},
                    {"to", to},
                    {"step", step}
                }
            }
        }
    );
}

void Interconnect::wgti_add_sliderfloat(std::string identifier, float from, float to, std::uint64_t precision)
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

void Interconnect::wgti_add_inputint(std::string identifier, std::string placeholder)
{
    this->ui_input.push_back(
        {
            {"type", "inputint"},
            {"widget",
                {
                    {"identifier", identifier},
                    {"placeholder", placeholder}
                }
            }
        }
    );
}

void Interconnect::wgti_add_inputfloat(std::string identifier, std::string placeholder)
{
    this->ui_input.push_back(
        {
            {"type", "inputfloat"},
            {"widget",
                {
                    {"identifier", identifier},
                    {"placeholder", placeholder}
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