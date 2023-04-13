#pragma once


#include <sea_sim/gears/channel_packet.h>
#include <sea_sim/gui_controller/RenderEngine.h>


namespace gui
{
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


		bool window_is_open();
		bool window_has_focus();
		void window_close();

		void window_cls();
		void window_flip();

		ImVec2 get_screen_size();

		int mouse_X();                        // Get X coord of Mouse
		int mouse_Y();                        // Get Y coord of Mouse
		int mouse_down(const sf::Mouse::Button& B);
		int key_down(const sf::Keyboard::Key& B);
		int key_hit(const sf::Keyboard::Key& key);

		int block();                          // ImGUI::Sameline();

		// Windows
		void show_main();
		void show_exit_popup();

		void show_child_input();
		void show_child_output();
		void show_child_view();

	private:
		void show_main_menu_bar();

		void ImGui_reset_docking_layout();

		void send_to_core(std::string event, nlohmann::json data = {});
		void send_to_core(channel_packet& packet);

		sf::RenderWindow window_;
		sf::Vector2u screen_size_;
		sf::Clock deltaClock_;

		ImFont* font_x20_;
		ImFont* font_x16_;

		sf::RenderTexture render_texture_;
		RenderEngine render_engine_;

		Endpoint channel_to_core;

		bool keyHit[sf::Keyboard::KeyCount];

		struct WindowsShowState
		{
			bool show_main = true;
			bool exit_popup = false;
			bool exit_popup_new = false;

			bool reset_docking_layout = true;
			ImVec2 render_size = { 500, 500 };
		} windows_show_state_;

		struct FieldsData
		{
			std::string selected_module = "";
		} fields_data_;
	};
} // namespace gui
