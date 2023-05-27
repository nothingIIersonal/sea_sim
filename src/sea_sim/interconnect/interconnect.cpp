#include <sea_sim/interconnect/interconnect.h>


#define container_of(ptr, type, member) ((type *)((size_t)(ptr) - ((size_t)&(((type *)0)->member))))


Interconnect::Interconnect(const Endpoint& module_endpoint, const std::string& module_name, const shared_ic_objects_t& shared_ic_objects, const environment_t& environment) 
                    : module_endpoint(module_endpoint), module_name(module_name),
                      ship_storage(shared_ic_objects.ship_storage), ship_storage_mutex(shared_ic_objects.ship_storage_mutex),
                      isle_storage(shared_ic_objects.isle_storage), isle_storage_mutex(shared_ic_objects.isle_storage_mutex),
                      environment(environment)
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

Interconnect::Environment::Environment(const environment_t& environment) : environment(environment) {}

geom::Vector2u Interconnect::Environment::get_view_area() const
{
    return this->environment.view_area;
}

geom::Vector2u Interconnect::Environment::get_mouse_position() const
{
    return this->environment.mouse_position;
}

int Interconnect::Environment::get_map_scale() const
{
    return this->environment.map_scale;
}

void Interconnect::Render::send()
{
    Interconnect *ic = container_of(this, Interconnect, render);
    ic->module_endpoint.SendData({ "gui", ic->module_name, "draw", this->graphics_output });
}

void Interconnect::Render::set_fill_color(graphics::Color color)
{
    this->graphics_output.push_back(
        {
            {"type", "setFillColor"},
            {"settings",
                {
                    {"color", color}
                }
            }
        }
    );
}

void Interconnect::Render::set_outline_color(graphics::Color color)
{
    this->graphics_output.push_back(
        {
            {"type", "setOutlineColor"},
            {"settings",
                {
                    {"color", color}
                }
            }
        }
    );
}

void Interconnect::Render::draw_line(geom::Vector2f a, geom::Vector2f b, float width)
{
    this->graphics_output.push_back(
        {
            {"type", "line"},
            {"settings",
                {
                    {"a", a},
                    {"b", b},
                    {"width", width}
                }
            }
        }
    );
}

void Interconnect::Render::draw_circle(geom::Vector2f pos, float radius, float border_width)
{
    this->graphics_output.push_back(
        {
            {"type", "circle"},
            {"settings",
                {
                    {"pos", pos},
                    {"radius", radius},
                    {"border_width", border_width}
                }
            }
        }
    );
}

void Interconnect::Render::draw_triangle(geom::Vector2f a, geom::Vector2f b, geom::Vector2f c, float border_width)
{
    this->graphics_output.push_back(
        {
            {"type", "triangle"},
            {"settings",
                {
                    {"a", a},
                    {"b", b},
                    {"c", c},
                    {"border_width", border_width}
                }
            }
        }
    );
}

void Interconnect::Render::draw_ship(const Ship& ship)
{
    this->graphics_output.push_back(
        {
            {"type", "ship"},
            {"settings",
                {
                    {"ship", ship}
                }
            }
        }
    );
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

void Interconnect::Ships::create(const std::string& identifier, geom::Vector2f position, float angle)
{
    Interconnect *ic = container_of(this, Interconnect, ships);

    std::unique_lock lock(ic->ship_storage_mutex);

    ic->ship_storage.insert_or_assign(identifier, Ship{identifier, position, angle} );
}

void Interconnect::Ships::set_position(const std::string& identifier, geom::Vector2f position)
{
    Interconnect *ic = container_of(this, Interconnect, ships);

    std::unique_lock lock(ic->ship_storage_mutex);

    if ( ic->ship_storage.contains(identifier) )
        ic->ship_storage.at(identifier).set_position(position);
}

void Interconnect::Ships::set_angle(const std::string& identifier, float angle)
{
    Interconnect *ic = container_of(this, Interconnect, ships);

    std::unique_lock lock(ic->ship_storage_mutex);

    if ( ic->ship_storage.contains(identifier) )
        ic->ship_storage.at(identifier).set_angle(angle);
}

std::optional<Ship> Interconnect::Ships::get_by_id(const std::string& identifier)
{
    Interconnect *ic = container_of(this, Interconnect, ships);

    std::shared_lock lock(ic->ship_storage_mutex);

    if ( ic->ship_storage.contains(identifier) )
        return ic->ship_storage.at(identifier);

    return std::nullopt;
}

std::vector<Ship> Interconnect::Ships::get_all()
{
    Interconnect *ic = container_of(this, Interconnect, ships);

    std::vector<Ship> ships;

    std::shared_lock lock(ic->ship_storage_mutex);

    std::for_each(ic->ship_storage.begin(), ic->ship_storage.end(), [&](auto & element) { ships.push_back(element.second); });

    return ships;
}
