#include <sea_sim/gui_controller/window_storage.h>
#include <sea_sim/gui_controller/functions.h>

#include <iostream>


namespace gui
{
	using namespace utils;

	WindowStorage::WindowStorage(const Endpoint& channel_to_core) :
		channel_to_core(channel_to_core),
		font_x20_(nullptr),
		font_x16_(nullptr)
	{
		for (int reset_array = 0; reset_array < sf::Keyboard::KeyCount; ++reset_array)
		{
			keyHit[reset_array] = false;
		}

		render_texture_.create(500, 500);
	}

	void WindowStorage::build_window()
	{
		window_.create(sf::VideoMode(1920, 1080), "Sea Interface");
		window_.setFramerateLimit(60);

		screen_size_ = window_.getSize();
	}
	void WindowStorage::poll_events()
	{
		sf::Event event;
		while (window_.pollEvent(event))
		{
			ImGui::SFML::ProcessEvent(event);

			switch (event.type) {
			case sf::Event::Closed:
				windows_show_state_.exit_popup_new = true;
				break;
			case sf::Event::Resized: {
				sf::FloatRect visibleArea(0.f, 0.f, 
					static_cast<float>(event.size.width), 
					static_cast<float>(event.size.height));
				window_.setView(sf::View(visibleArea));
				screen_size_ = window_.getSize();
				break; }
			case sf::Event::KeyReleased:
				if (event.key.code >= 0 && event.key.code < sf::Keyboard::KeyCount)
					keyHit[event.key.code] = false;
				break;
			default:
				break;
			}
		}

		while (auto response = channel_to_core.TryRead())
		{
			auto& packet_to = response.value().to;
			auto& packet_from = response.value().from;
			auto& packet_event = response.value().event;
			auto& packet_data = response.value().data;

			if (packet_to == "gui")
			{
				if (packet_event == "update_input_interface")
				{
					render_engine_.update_input_interface(packet_from, packet_data);
				}
				else if (packet_event == "update_output_interface")
				{
					render_engine_.update_output_interface(packet_from, packet_data);
				}
				else if (packet_event == "module_loaded")
				{
					auto module_path = packet_data["module_path"].get<std::string>();
					module_dialog_.add_module(module_path);
				}
                else if (packet_event == "module_unloaded")
                {
					auto module_path = packet_data["module_path"].get<std::string>();
					module_dialog_.remove_module(module_path);
                    render_engine_.remove_interface(module_path);
                }
                else if (packet_event == "notify")
                {
					windows_show_state_.exit_popup_new = false;
					set_notification(packet_data["text"].get<std::string>());
                }
				else if (packet_event == "module_error")
				{
					// auto module_path = packet_data["module_path"].get<std::string>();
					// module_dialog_.remove_module(module_path);

					windows_show_state_.exit_popup_new = false;
					set_notification(packet_data["text"].get<std::string>());
				}
                else if (packet_event == "shutdown")
                {
                    window_close();
                }
			}
		}
	}
	void WindowStorage::shutdown()
    {
        channel_packet packet = { "core", "gui", "shutdown", {} };
        send_to_core(packet);

		shutdown_flag_ = true;
    }

	void WindowStorage::ImGui_init()
	{
		if ( ImGui::SFML::Init(window_, false) == false )
			return;

		ImGuiIO& io = ImGui::GetIO();
		font_x20_ = io.Fonts->AddFontFromFileTTF("4-font.ttf", 20, NULL,
			io.Fonts->GetGlyphRangesCyrillic());
		font_x16_ = io.Fonts->AddFontFromFileTTF("4-font.ttf", 16, NULL,
			io.Fonts->GetGlyphRangesCyrillic());

		if ( ImGui::SFML::UpdateFontTexture() == false )
			return;

		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		deltaClock_.restart();
	}
	void WindowStorage::ImGui_update()
	{
		ImGui::SFML::Update(window_, deltaClock_.restart());
	}
	void WindowStorage::ImGui_render()
	{
		ImGui::SFML::Render(window_);
	}
	void WindowStorage::ImGui_shutdown()
	{
		ImGui::SFML::Shutdown();
	}

	void WindowStorage::set_fps_limit(int fps)
    {
        window_.setFramerateLimit(fps);
    }
	bool WindowStorage::window_is_open()
	{
		return window_.isOpen();
	}
	bool WindowStorage::window_has_focus()
	{
		return window_.hasFocus();
	}
	void WindowStorage::window_close()
	{
		window_.close();
	}

	void WindowStorage::window_cls()
	{
		window_.clear(sf::Color(50, 50, 50));
	}
	void WindowStorage::window_flip()
	{
		window_.display();
	}

	ImVec2 WindowStorage::get_screen_size()
	{
		return ImVec2({ static_cast<float>(screen_size_.x), 
			            static_cast<float>(screen_size_.y) });
	}

	int WindowStorage::mouse_X()
	{
		return sf::Mouse::getPosition(window_).x;
	}
	int WindowStorage::mouse_Y()
	{
		return sf::Mouse::getPosition(window_).y;
	}
	int WindowStorage::mouse_down(const sf::Mouse::Button& B)
	{
		return sf::Mouse::isButtonPressed(B);
	}
	int WindowStorage::key_down(const sf::Keyboard::Key& B)
	{
		return sf::Keyboard::isKeyPressed(B);
	}
	int WindowStorage::key_hit(const sf::Keyboard::Key& key)
	{
		if (!keyHit[key]) {
			if (sf::Keyboard::isKeyPressed(key)) {
				keyHit[key] = true;
				return true;
			}
		}
		return false;
	}

	int WindowStorage::block()
	{
		ImGui::SameLine();
		return 1;
	}

	// --- Windows

	void WindowStorage::show_main()
    {
        show_main_menu_bar();
        show_file_dialog();
		show_module_dialog();

        ImGuiID my_dockspace = ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
        if (windows_show_state_.reset_docking_layout)
        {
            ImGui_reset_docking_layout(my_dockspace);
            windows_show_state_.reset_docking_layout = false;
        }
	}

	void WindowStorage::show_exit_popup()
	{
		if (windows_show_state_.exit_popup_new == true &&
			windows_show_state_.exit_popup     == false)
		{
			ImGui::OpenPopup(u8"Выход"_C, ImGuiPopupFlags_NoOpenOverExistingPopup);

			windows_show_state_.exit_popup = true;
		}

		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));

		if (ImGui::BeginPopupModal(u8"Выход"_C, NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("%s", u8"Вы уверены?"_C);
			ImGui::NewLine();
			ImGui::Separator();

			if (ImGui::Button(u8"Выйти"_C, ImVec2(120, 0)) || key_hit(sf::Keyboard::Enter))
			{
				windows_show_state_.exit_popup_new = false;
				ImGui::CloseCurrentPopup();
				shutdown();
			}
			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();
			if (ImGui::Button(u8"Остаться"_C, ImVec2(120, 0)) || key_hit(sf::Keyboard::Escape) ||
				windows_show_state_.exit_popup_new == false)
			{
				windows_show_state_.exit_popup_new = false;
				windows_show_state_.exit_popup = false;
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
		if (!ImGui::IsPopupOpen("", ImGuiPopupFlags_AnyPopupId | ImGuiPopupFlags_AnyPopupLevel))
		{
			if (windows_show_state_.exit_popup == false && key_hit(sf::Keyboard::Escape))
			{
				windows_show_state_.exit_popup_new = true;
			}
		}
	}
	void WindowStorage::show_notification_popup()
	{
		if (windows_show_state_.notification_popup_new == true &&
			windows_show_state_.notification_popup     == false)
		{
			ImGui::OpenPopup(u8"Уведомление"_C, ImGuiPopupFlags_NoOpenOverExistingPopup);

			windows_show_state_.notification_popup = true;
		}

		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));

		if (ImGui::BeginPopupModal(u8"Уведомление"_C, NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("%s", windows_show_state_.notification_text.c_str());
			ImGui::NewLine();
			ImGui::Separator();

			align_for_width(120);

			if (ImGui::Button(u8"Принять"_C, ImVec2(120, 0)) || key_hit(sf::Keyboard::Enter) ||
				windows_show_state_.notification_popup_new == false)
			{
				windows_show_state_.notification_popup_new = false;
				windows_show_state_.notification_popup = false;
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}
	void WindowStorage::set_notification(std::string text)
	{
		windows_show_state_.notification_popup_new = true;
		windows_show_state_.notification_text = text;
	}

	void WindowStorage::show_child_input()
	{
		if (ImGui::Begin(u8"Ввод данных"_C))
		{
			ImGui::Text(u8"%.0f FPS"_C, ImGui::GetIO().Framerate);
			ImGui::Text(u8"Область отрисовки: %ix%i пкс"_C, render_texture_.getSize().x, render_texture_.getSize().y);

			ImGui::Separator();

			if (auto packet = render_engine_.render_inputs())
				send_to_core(packet.value());

			ImGui::End();
		}
	}
	void WindowStorage::show_child_output()
	{
		if (ImGui::Begin(u8"Результаты"_C))
		{
			render_engine_.render_outputs();

			ImGui::End();
		}
	}
	void WindowStorage::show_child_view()
	{
		ImGuiWindowFlags view_window_flags =
			ImGuiWindowFlags_NoCollapse;

		if (ImGui::Begin(u8"Обзор"_C, NULL, view_window_flags))
		{
			ImVec2 view_area = 
				ImGui::GetWindowContentRegionMax() - 
				ImGui::GetWindowContentRegionMin();

			if (view_area != windows_show_state_.render_size)
			{
				windows_show_state_.render_size = view_area;

				view_area.x = max(1, view_area.x - 2);
				view_area.y = max(1, view_area.y - 2);

				render_texture_.create(static_cast<unsigned int>(view_area.x), static_cast<unsigned int>(view_area.y));

				sf::Vector2u scene_size = render_texture_.getSize();
				//send_to_core("view_area_resized", { {"view_area_X", scene_size.x}, {"view_area_Y", scene_size.y} });
			}
			
			render_engine_.render_scene(render_texture_);

			ImGui::Image(render_texture_, sf::Color::White, sf::Color(70, 70, 70));

			ImGui::End();
		}
	}

	void WindowStorage::show_main_menu_bar()
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu(u8"Меню"_C))
			{
				if (ImGui::MenuItem(u8"Открыть"_C))
				{

				}

				if (ImGui::MenuItem(u8"Сохранить"_C))
				{

				}

				ImGui::Separator();

				if (ImGui::MenuItem(u8"Выйти"_C))
				{
					if (windows_show_state_.exit_popup == false)
					{
						windows_show_state_.exit_popup_new = true;
					}
				}

				ImGui::Separator();

				if (ImGui::MenuItem(u8"Форсировать выход"_C))
				{
					send_to_core("force_shutdown");
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu(u8"Вид"_C))
			{
#ifdef WIN32
				if (ImGui::MenuItem(u8"Восстановить"_C))
				{
					windows_show_state_.reset_docking_layout = true;
				}
#endif // WIN32
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu(u8"Плагины"_C))
			{
				if (ImGui::MenuItem(u8"Добавить"_C))
				{
					if (!file_dialog_.is_open())
					{
						file_dialog_.open();
					}
				}

				ImGui::Separator();

				if (ImGui::MenuItem(u8"Менеджер модулей"_C))
				{
					if (!module_dialog_.is_open())
					{
						module_dialog_.open();
					}
				}

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
	}
	void WindowStorage::show_file_dialog()
	{
		if (file_dialog_.is_open())
		{
			if (auto file_path = file_dialog_.render_dialog())
			{
				if (auto file_path = file_dialog_.try_take_files())
				{
					send_to_core("load_module", { {"module_path", file_path.value().front()} });
				}
			}
			if (key_hit(sf::Keyboard::Escape))
				file_dialog_.close();
			else if (key_hit(sf::Keyboard::Enter))
			{
				if (auto file_path = file_dialog_.try_take_files())
				{
					send_to_core("load_module", { {"module_path", file_path.value().front()} });
				}
			}
		}
	}
	void WindowStorage::show_module_dialog()
	{
		if (module_dialog_.is_open())
		{
			if (auto packets = module_dialog_.render_dialog())
				for (auto& packet : packets.value())
					send_to_core(packet);

			if (key_hit(sf::Keyboard::Escape))
				module_dialog_.close();
		}
	}

	void WindowStorage::ImGui_reset_docking_layout(ImGuiID id)
    {
        ImGui::DockBuilderRemoveNode(id);
        ImGui::DockBuilderAddNode(id);
        ImGui::DockBuilderSetNodeSize(id, ImGui::GetMainViewport()->Size);

        ImGuiID dock_view = ImGui::DockBuilderSplitNode(id, ImGuiDir_Left, 0.75f, nullptr, &id);
        ImGuiID dock_input = ImGui::DockBuilderSplitNode(id, ImGuiDir_Right, 0.25f, nullptr, &id);
        ImGuiID dock_output = ImGui::DockBuilderSplitNode(dock_input, ImGuiDir_Down, 0.5f, nullptr, &dock_input);

        ImGui::DockBuilderDockWindow(u8"Обзор"_C, dock_view);
        ImGui::DockBuilderDockWindow(u8"Ввод данных"_C, dock_input);
        ImGui::DockBuilderDockWindow(u8"Результаты"_C, dock_output);

        ImGui::DockBuilderFinish(id);
    }
	void WindowStorage::align_for_width(float width, float alignment)
    {
        float avail = ImGui::GetContentRegionAvail().x;
        float off = (avail - width) * alignment;
        if (off > 0.0f)
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
    }
    float WindowStorage::get_button_width(std::string text, ImGuiStyle& style)
    {
        return ImGui::CalcTextSize(text.c_str()).x + style.FramePadding.x * 2 + style.ItemSpacing.x;
    }

	// --- Communication

    void WindowStorage::send_to_core(std::string event, nlohmann::json data)
    {
        if (!shutdown_flag_)
            channel_to_core.SendData({ "core", "gui", event, data });
    }
    void WindowStorage::send_to_core(channel_packet& packet)
    {
        if (!shutdown_flag_)
            channel_to_core.SendData(packet);
    }

} // namespace gui
