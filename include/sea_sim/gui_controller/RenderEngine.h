#pragma once
#include <imgui.h>
#include <imgui-SFML.h>
#include <imgui_stdlib.h>
#include <imgui_internal.h>

#include <SFML/Graphics.hpp>

#include <sea_sim/gears/channel_packet.h>
#include <sea_sim/gui_controller/functions.h>
#include <sea_sim/gui_controller/ModulePageStorage.h>
#include <sea_sim/gui_controller/Fonts.h>
#include <sea_sim/gui_controller/GraphicsStorage.h>


namespace gui
{
	class WindowStorage;

	class RenderEngine
	{
	public:
		RenderEngine(WindowStorage* parent);
		~RenderEngine();

		void create_texture(unsigned int x, unsigned int y);

		void update_input_interface(std::string& module, nlohmann::json& data);
		void update_output_interface(std::string& module, nlohmann::json& data);
		void remove_interface(std::string& module);

		void swap_texture();

		void render_scene();

		std::optional<std::string> render_modules_combo();
		std::optional<channel_packet> render_inputs();
		void render_outputs();

		const sf::RenderTexture& get_texture();
		const sf::Vector2u& get_texture_size();

		void set_notification(const std::string& text);
	
	private:
		WindowStorage* parent_ptr_;

		sf::RenderTexture render_texture_;

		GraphicsStorage graphics_storage_;

		std::string selected_module = "";
		std::map<std::string, ModulePageStorage> module_pages;

	};
} // namespace gui
