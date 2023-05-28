#pragma once
#include <sea_sim/gui_controller/functions.h>
#include <sea_sim/gui_controller/ModulePageStorage.h>
#include <sea_sim/gui_controller/Fonts.h>
#include <sea_sim/gui_controller/GraphicsStorage.h>
#include <sea_sim/gears/channel_packet.h>
#include <sea_sim/toolkit/geom/geom.hpp>


namespace gui
{
	class WindowStorage;

	class RenderEngine
	{
	public:
		RenderEngine(WindowStorage* parent);
		~RenderEngine();

		void create_texture(const sf::Vector2u& size);

		void update_input_interface(std::string& module, nlohmann::json& data);
		void update_output_interface(std::string& module, nlohmann::json& data);
		void remove_interface(std::string& module);

		void draw_from_json(nlohmann::json& data);

		void swap_texture();


		std::optional<std::string> render_modules_combo();
		std::optional<channel_packet> render_inputs();
		void render_outputs();

		sf::RenderTexture& get_texture(bool get_writing_texture = false);
		sf::Vector2u get_texture_size(bool current_texture = false);

		sf::Font& get_font();

		void set_notification(const std::string& text);
	
	private:
		WindowStorage* parent_ptr_;

		struct GraphicBuffer
		{
			sf::RenderTexture render_texture_[2];
			bool writing_buffer = 0;

			sf::Vector2u size_to_set = {};
		} graphic_buffer_;

		GraphicsStorage graphics_storage_;

		std::string selected_module = "";
		std::map<std::string, ModulePageStorage> module_pages;

		sf::Font cyrillic_font;

	};
} // namespace gui

namespace nlohmann {
	template <>
	struct adl_serializer<sf::Vector2f> {
		static void to_json(nlohmann::json&, const sf::Vector2f&);
		static void from_json(const nlohmann::json&, sf::Vector2f&);
	};

	template <>
	struct adl_serializer<sf::Vector2u> {
		static void to_json(nlohmann::json&, const sf::Vector2u&);
		static void from_json(const nlohmann::json&, sf::Vector2u&);
	};

	template <>
	struct adl_serializer<sf::Color> {
		static void to_json(nlohmann::json&, const sf::Color&);
		static void from_json(const nlohmann::json&, sf::Color&);
	};
} // namespace nlohmann


