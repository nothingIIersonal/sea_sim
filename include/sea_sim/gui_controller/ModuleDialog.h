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

	private:
		bool is_open_ = false;

	};
} // namespace gui
