#ifndef INTERCONNECT
#define INTERCONNECT


#include <iostream>
#include <vector>
#include <cstring>
#include <optional>


#include <sea_sim/gears/channel_packet.h>
#include <sea_sim/toolkit/object_showcase/object_showcase.h>
#include <sea_sim/toolkit/graphics/color.hpp>
#include <sea_sim/toolkit/controllers/mouse.hpp>
#include <shared_mutex>


typedef struct environment_t
{
    geom::Vector2u view_area = {500u, 500u};
    geom::Vector2u mouse_position = {0u, 0u};
    uint8_t mouse_buttons = 0;
    int8_t sim_speed = 0;
    int map_scale = 1;
    bool paused = true;
} environment_t;


typedef struct shared_ic_objects_t
{
    std::map<std::string, Ship>& ship_storage;
    std::shared_mutex& ship_storage_mutex;

    std::map<std::string, Isle>& isle_storage;
    std::shared_mutex& isle_storage_mutex;

    environment_t environment;
} shared_ic_objects_t;


class Interconnect
{
private:
    Endpoint module_endpoint;
    std::string module_name;
    nlohmann::json ui_fields;
    std::string ui_trigger;

    std::map<std::string, Ship>& ship_storage;
    std::map<std::string, Ship> ::iterator ship_storage_it;
    std::shared_mutex& ship_storage_mutex;

    std::map<std::string, Isle>& isle_storage;
    std::map<std::string, Isle> ::iterator isle_storage_it;
    std::shared_mutex& isle_storage_mutex;

    class Environment
    {
    private:
        environment_t environment;
    public:
        explicit Environment(const environment_t& environment);

        geom::Vector2u get_view_area() const;
        geom::Vector2u get_mouse_position() const;
        uint8_t get_mouse_buttons() const;
        bool get_mouse_button(controllers::MouseButtonEnum key) const;
        int get_map_scale() const;
        bool is_paused() const;
    };

    class Render
    {
    private:
        nlohmann::json graphics_output;
    public:
        void send();

		void set_fill_color(graphics::Color color);
		void set_outline_color(graphics::Color color);

		void draw_line(geom::Vector2f a, geom::Vector2f b, float width = 1.f);
		void draw_circle(geom::Vector2f pos, float radius, float border_width = 1.f);
		void draw_triangle(geom::Vector2f a, geom::Vector2f b, geom::Vector2f c, float border_width = 1.f);

		void draw_ship(const Ship& ship);
    };

    class WGTI
    {
    private:
        nlohmann::json ui_input;
    public:
        void send();
        void set_module_title(const std::string& title);
        void sameline();
        void add_text(const std::string& text);
        void add_button(const std::string& identifier, const std::string& text);
        void add_sliderint(const std::string& identifier, int64_t from, int64_t to, bool keep_value = false);
        void add_sliderfloat(const std::string& identifier, float from, float to, uint64_t precision, bool keep_value = false);
        void add_inputtext(const std::string& identifier, const std::string& placeholder, bool keep_value = false);
        void add_inputint(const std::string& identifier, bool keep_value = false);
        void add_inputfloat(const std::string& identifier, bool keep_value = false);
        void add_checkbox(const std::string& identifier, const std::string& text, bool keep_value = false);
        void add_radiobutton(const std::string& identifier, const std::vector<std::string>& elements, bool keep_value = false);
        void add_dropdownlist(const std::string& identifier, const std::vector<std::string>& elements, bool keep_value = false);
    };

    class WGTO
    {
    private:
        nlohmann::json ui_output;
    public:
        void send();
        void sameline();
        void add_text(const std::string& text);
    };

    class Ships
    {
    public:
        void create(const std::string& identifier, geom::Vector2f position, graphics::Color fill_color, graphics::Color outline_color, float angle, float desired_angle, float speed, float rotation_speed);
        void set_position(const std::string& identifier, geom::Vector2f position);
        void set_angle(const std::string& identifier, float angle);
        void set_desired_angle(const std::string& identifier, float desired_angle);
        void set_speed(const std::string& identifier, float speed);
        void set_rotation_speed(const std::string& identifier, float rotation_speed);
        std::optional<Ship> get_by_id(const std::string& identifier);
        std::vector<Ship> get_all();
    };

    class Isles
    {
    public:
        Isles() noexcept = default;
        ~Isles() noexcept = default;
    };

protected:
    Interconnect() noexcept = delete;
    Interconnect(const Interconnect&) = delete;
    void operator=(const Interconnect&) = delete;

public:
    explicit Interconnect(const Endpoint& module_endpoint, const std::string& module_name, const shared_ic_objects_t& shared_ic_objects_t, const environment_t& environment);
    ~Interconnect() noexcept = default;

    Environment environment;
    Render render;
    WGTI wgti;
    WGTO wgto;
    Ships ships;
    Isle isle;

    const std::string& get_trigger();

    std::optional<int> get_field_int(const std::string& field_name);
    std::optional<bool> get_field_bool(const std::string& field_name);
    std::optional<float> get_field_float(const std::string& field_name);
    std::optional<std::string> get_field_string(const std::string& field_name);
};


extern "C"
{

int sea_module_init(Interconnect &&ic); // on load module
int sea_module_exec(Interconnect &&ic); // on one cycle (at action)
int sea_module_hotf(Interconnect &&ic); // on every cycle (always)
int sea_module_exit(Interconnect &&ic); // on unload module

}


#endif