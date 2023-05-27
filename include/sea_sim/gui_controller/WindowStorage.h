#pragma once

#include <queue>

#include <sea_sim/gui_controller/Functions.h>
#include <sea_sim/gui_controller/RenderEngine.h>
#include <sea_sim/gui_controller/FileDialog.h>
#include <sea_sim/gui_controller/ModuleDialog.h>
#include <sea_sim/gui_controller/Fonts.h>
#include <sea_sim/gears/channel_packet.h>


namespace gui
{
	using namespace utils;

	class WindowStorage
	{
	public:
		WindowStorage(const Endpoint& channel_to_core);

		void build_window();
		void poll_events();
		void shutdown();

		void ImGui_init();
		void ImGui_update();
		void ImGui_render();
		void ImGui_shutdown();

		void set_fps_limit(int fps);
		bool window_is_open();
		bool window_has_focus();
		void window_close();

		void window_cls();
		void window_flip();

		ImVec2 get_screen_size();

		int mouse_X();                        // Get X coord of Mouse
		int mouse_Y();                        // Get Y coord of Mouse

		// Windows
		void show_main();
		void show_exit_popup();
        void show_notification_popup();
        void set_notification(const std::string& text);

		void show_child_input();
		void show_child_output();
		void show_child_view();

		void send_to_core(std::string event, nlohmann::json data = {});
		void send_to_core(channel_packet& packet);

	private:
		void show_main_menu_bar();
		void show_file_dialog();
		void show_module_dialog();

		void ImGui_reset_docking_layout(ImGuiID id);
		float get_button_width(const std::string& text, ImGuiStyle& style);
        void align_for_width(float width, float alignment = 0.5f);

		sf::RenderWindow window_;
		sf::Vector2u screen_size_;
		sf::Clock deltaClock_;

		ImFont* font_x20_;

		RenderEngine render_engine_;
		FileDialog file_dialog_;
		ModuleDialog module_dialog_;

		Endpoint channel_to_core;

		struct WindowsShowState
		{
			bool show_main = true;
			bool exit_popup = false;
			bool exit_popup_new = false;

            bool notification_popup = false;
            bool notification_popup_new = false;
            std::string notification_text = u8""_C;
			std::queue<std::string> notification_queue = {};

			bool show_file_dialog = false;
			bool show_module_dialog = false;

			bool reset_docking_layout = true;
			ImVec2 render_size = { 500, 500 };

			int8_t sim_speed = 0;
			bool frame_pause = true;
		} windows_show_state_;

		bool shutdown_flag_ = false;
	};
} // namespace gui
