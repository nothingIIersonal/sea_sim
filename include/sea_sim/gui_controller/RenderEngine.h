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


namespace gui
{
	class WindowStorage;

	class RenderEngine
	{
	public:
		RenderEngine(WindowStorage* parent);
		~RenderEngine();

		void update_input_interface(std::string& module, nlohmann::json& data);
		void update_output_interface(std::string& module, nlohmann::json& data);
		void remove_interface(std::string& module);

		void render_scene(sf::RenderTexture& texture);

		std::optional<std::string> render_modules_combo();
		std::optional<channel_packet> render_inputs();
		void render_outputs();

		void set_notification(const std::string& text);
	
	private:
		WindowStorage* parent_ptr_;

		std::string selected_module = "";
		std::map<std::string, ModulePageStorage> module_pages;

	};
} // namespace gui