#pragma once
#include <imgui.h>
#include <imgui-SFML.h>
#include <imgui_stdlib.h>
#include <imgui_internal.h>

#include <SFML/Graphics.hpp>
#include <optional>

#include <sea_sim/gui_controller/functions.h>
#include <sea_sim/gears/channel_packet.h>


namespace gui
{
	class ModuleDialog
	{
	public:
		void open();
		void close();

		bool is_open();

		std::optional<std::vector<channel_packet>> render_dialog();

		void add_module(std::string& module_path);
		void remove_module(std::string& module_path);

	private:
		bool is_open_ = false;

		std::map<std::string, bool> modules_map_;
		std::vector<std::string> modules_order_;

	};
} // namespace gui
