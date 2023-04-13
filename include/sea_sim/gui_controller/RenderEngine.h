#pragma once


#include <imgui.h>
#include <imgui-SFML.h>
#include <imgui_stdlib.h>
#include <imgui_internal.h>

#include <SFML/Graphics.hpp>

#include <sea_sim/gears/channel_packet.h>
#include <sea_sim/gui_controller/functions.h>


namespace gui
{
	class RenderEngine
	{
	public:
		void update_input_interface(std::string module, nlohmann::json data);
		void update_output_interface(std::string module, nlohmann::json data);

		void render_scene(sf::RenderTexture& texture);

		std::optional<channel_packet> render_inputs(std::string module);
		void render_outputs();
	
	private:
		std::map<std::string, nlohmann::json> input_interface_storage_;
		std::map<std::string, nlohmann::json> output_interface_storage_;

		std::map<std::string, std::map<std::string, int        >>    int_fields_storage_;
		std::map<std::string, std::map<std::string, float      >>  float_fields_storage_;
		std::map<std::string, std::map<std::string, std::string>> string_fields_storage_;

	};
} // namespace gui