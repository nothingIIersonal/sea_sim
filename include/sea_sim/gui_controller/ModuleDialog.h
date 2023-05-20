#pragma once
#include <imgui.h>
#include <imgui-SFML.h>
#include <imgui_stdlib.h>
#include <imgui_internal.h>

#include <SFML/Graphics.hpp>
#include <optional>

#include <sea_sim/gui_controller/functions.h>
#include <sea_sim/gui_controller/Fonts.h>
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

		channel_packet load_module(const std::string& module_path);
		channel_packet unload_module(const std::string& module_path);

		void add_module(const std::string& module_path);
		void remove_module(const std::string& module_path);

		std::optional<channel_packet> remove_module_from_list(std::vector<std::string>::iterator& module_path);

	private:
		enum class ModuleState
		{
			LOADED = 0,
			UNLOADED = 1
		};

		struct ModuleInfo
		{
			std::string name;
			bool selected = false;
			ModuleState state = ModuleState::LOADED;
		};

		bool is_open_ = false;

		std::map<std::string, ModuleInfo> modules_map_;
		std::vector<std::string> modules_order_;

	};
} // namespace gui
