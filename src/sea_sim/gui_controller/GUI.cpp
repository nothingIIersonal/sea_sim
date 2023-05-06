#include <sea_sim/gui_controller/GUI.h>

#include <iostream>

namespace gui
{
	void launch_GUI(const Endpoint& channel_to_core)
	{
		// --- Init window

		gui::WindowStorage window_storage{ channel_to_core };

		window_storage.build_window();
		window_storage.ImGui_init();

		// --- Main Loop

		while (window_storage.window_is_open())
		{
			// --- Process OS Window

			window_storage.poll_events();     // Process OS and Core calls
			
			if (window_storage.window_has_focus())
				window_storage.set_fps_limit(240); // WORK
			else
				window_storage.set_fps_limit(1);   // IDLE
		
			window_storage.ImGui_update();    // Update GUI
			
			// --- Draw GUI Windows
			
			window_storage.show_main();       // Main window
			window_storage.show_exit_popup(); // Exit PopUp
			window_storage.show_notification_popup();
			
			window_storage.show_child_input();
			window_storage.show_child_output();
			window_storage.show_child_view();
			
			// --- Render
			
			window_storage.window_cls();      // Clear screen
			window_storage.ImGui_render();    // Render GUI
			window_storage.window_flip();     // Flip screen buffer
		}

		window_storage.ImGui_shutdown();

		return;
	}
} // namespace gui
