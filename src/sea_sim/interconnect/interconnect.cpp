#include <sea_sim/interconnect/interconnect.h>


#define container_of(ptr, type, member) ((type *)((size_t)(ptr) - ((size_t)&(((type *)0)->member))))


Interconnect::Interconnect(const Endpoint& module_endpoint, const std::string& module_name, const shared_ic_objects_t& shared_ic_objects) 
                    : module_endpoint(module_endpoint), module_name(module_name),
                      ship_storage(shared_ic_objects.ship_storage), ship_storage_mutex(shared_ic_objects.ship_storage_mutex),
                      isle_storage(shared_ic_objects.isle_storage), isle_storage_mutex(shared_ic_objects.isle_storage_mutex)
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

void Interconnect::WGTI::send()
{
    Interconnect *ic = container_of(this, Interconnect, wgti);
    ic->module_endpoint.SendData({ "gui", ic->module_name, "update_input_interface", this->ui_input });
}

void Interconnect::WGTO::send()
{
    Interconnect *ic = container_of(this, Interconnect, wgto);
    ic->module_endpoint.SendData({ "gui", ic->module_name, "update_output_interface", this->ui_output });
}

void Interconnect::WGTI::set_module_title(const std::string& title)
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

void Interconnect::WGTI::sameline()
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

void Interconnect::WGTI::add_text(const std::string& text)
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

void Interconnect::WGTI::add_button(const std::string& identifier, const std::string& text)
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

void Interconnect::WGTI::add_sliderint(const std::string& identifier, int64_t from, int64_t to, bool keep_value)
{
    this->ui_input.push_back(
        {
            {"type", "sliderint"},
            {"widget",
                {
                    {"keep_value", keep_value},
                    {"identifier", identifier},
                    {"from", from},
                    {"to", to},
                }
            }
        }
    );
}

void Interconnect::WGTI::add_sliderfloat(const std::string& identifier, float from, float to, uint64_t precision, bool keep_value)
{
    this->ui_input.push_back(
        {
            {"type", "sliderfloat"},
            {"widget",
                {
                    {"keep_value", keep_value},
                    {"identifier", identifier},
                    {"from", from},
                    {"to", to},
                    {"precision", precision}
                }
            }
        }
    );
}

void Interconnect::WGTI::add_inputtext(const std::string& identifier, const std::string& placeholder, bool keep_value)
{
    this->ui_input.push_back(
        {
            {"type", "inputtext"},
            {"widget",
                {
                    {"keep_value", keep_value},
                    {"identifier", identifier},
                    {"placeholder", placeholder}
                }
            }
        }
    );
}

void Interconnect::WGTI::add_inputint(const std::string& identifier, bool keep_value)
{
    this->ui_input.push_back(
        {
            {"type", "inputint"},
            {"widget",
                {
                    {"keep_value", keep_value},
                    {"identifier", identifier}
                }
            }
        }
    );
}

void Interconnect::WGTI::add_inputfloat(const std::string& identifier, bool keep_value)
{
    this->ui_input.push_back(
        {
            {"type", "inputfloat"},
            {"widget",
                {
                    {"keep_value", keep_value},
                    {"identifier", identifier}
                }
            }
        }
    );
}

void Interconnect::WGTI::add_checkbox(const std::string& identifier, const std::string& text, bool keep_value)
{
    this->ui_input.push_back(
        {
            {"type", "checkbox"},
            {"widget",
                {
                    {"keep_value", keep_value},
                    {"identifier", identifier},
                    {"text", text}
                }
            }
        }
    );
}

void Interconnect::WGTI::add_radiobutton(const std::string& identifier, const std::vector<std::string>& elements, bool keep_value)
{
    this->ui_input.push_back(
        {
            {"type", "radiobutton"},
            {"widget",
                {
                    {"keep_value", keep_value},
                    {"identifier", identifier},
                    {"elements", elements}
                }
            }
        }
    );
}

void Interconnect::WGTI::add_dropdownlist(const std::string& identifier, const std::vector<std::string>& elements, bool keep_value)
{
    this->ui_input.push_back(
        {
            {"type", "dropdownlist"},
            {"widget",
                {
                    {"keep_value", keep_value},
                    {"identifier", identifier},
                    {"elements", elements}
                }
            }
        }
    );
}

void Interconnect::WGTO::sameline()
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

void Interconnect::WGTO::add_text(const std::string& text)
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

const std::string& Interconnect::get_trigger()
{
    return this->ui_trigger;
}

std::optional<int> Interconnect::get_field_int(const std::string& field_name)
{
    if (this->ui_fields.contains(field_name) )
        return this->ui_fields[field_name].get<int>();
    return std::nullopt;
}

std::optional<float> Interconnect::get_field_float(const std::string& field_name)
{
    if (this->ui_fields.contains(field_name) )
        return this->ui_fields[field_name].get<float>();
    return std::nullopt;
}

std::optional<std::string> Interconnect::get_field_string(const std::string& field_name)
{
    if (this->ui_fields.contains(field_name) )
        return this->ui_fields[field_name].get<std::string>();
    return std::nullopt;
}

int Interconnect::object_ship_set(const std::string& identifier, float x, float y, std::vector<std::string> staff)
{
    std::unique_lock lock(this->ship_storage_mutex);

    this->ship_storage.insert_or_assign(identifier, Ship{identifier, x, y, staff} );

    return 0;
}

std::optional<Ship> Interconnect::object_ship_get(const std::string& identifier)
{
    std::shared_lock lock(this->ship_storage_mutex);

    if ( this->ship_storage.contains(identifier) )
        return this->ship_storage.at(identifier);

    return std::nullopt;
}

std::optional<Ship> Interconnect::object_ship_get_next()
{
    std::shared_lock lock(this->ship_storage_mutex);

    if ( this->ship_storage_it == this->ship_storage.end() )
        return std::nullopt;

    return (this->ship_storage_it++)->second;
}

std::optional<float> Interconnect::object_ship_get_x(const std::string& identifier)
{
    std::shared_lock lock(this->ship_storage_mutex);

    if ( this->ship_storage.contains(identifier) )
        return this->ship_storage.at(identifier).get_x();

    return std::nullopt;
}

std::optional<float> Interconnect::object_ship_get_y(const std::string& identifier)
{
    std::shared_lock lock(this->ship_storage_mutex);

    if ( this->ship_storage.contains(identifier) )
        return this->ship_storage.at(identifier).get_y();

    return std::nullopt;
}

std::optional<std::vector<std::string>> Interconnect::object_ship_get_staff(const std::string& identifier)
{
    std::shared_lock lock(this->ship_storage_mutex);

    if ( this->ship_storage.contains(identifier) )
        return this->ship_storage.at(identifier).get_staff();

    return std::nullopt;
}

void Interconnect::object_ship_iterator_reset()
{
    this->ship_storage_it = this->ship_storage.begin();
}
