﻿#include <sea_sim/gui_controller/WindowStorage.h>

#include <iostream>


extern bool keyHit[];

namespace gui
{
	using namespace utils;

	WindowStorage::WindowStorage(const Endpoint& channel_to_core) :
		channel_to_core(channel_to_core),
		font_x20_(nullptr),
		render_engine_(this),
		file_dialog_(this)
	{
		for (int reset_array = 0; reset_array < sf::Keyboard::KeyCount; ++reset_array)
		{
			keyHit[reset_array] = false;
		}

		time_manipulations_.clock = std::chrono::system_clock::now();
		time_manipulations_.last_elapsed = std::chrono::system_clock::now();
		
		render_engine_.create_texture({ 500u, 500u });
		render_engine_.swap_texture();

		sf::Vector2u scene_size = render_engine_.get_texture_size();
		send_to_core("view_area_resized", { { "view_area", scene_size} });
	}

	void WindowStorage::build_window()
	{
		window_.create(sf::VideoMode(1920, 1080), "Sea Interface");

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
                else if (packet_event == "notify" || packet_event == "module_error")
                {
					windows_show_state_.exit_popup_new = false;
					set_notification(packet_data["text"].get<std::string>());
                }
				else if (packet_event == "draw")
				{
					render_engine_.draw_from_json(packet_data);
				}
				else if (packet_event == "swap_texture")
				{
					render_engine_.swap_texture();
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
		font_x20_ = io.Fonts->AddFontFromFileTTF(FONT_CYRILLIC_FILE_NAME_sea_sim_, 20, NULL,
			io.Fonts->GetGlyphRangesCyrillic());

		static const ImWchar icons_ranges[] = { ICON_MIN_sea_sim_, ICON_MAX_sea_sim_, 0 };
		ImFontConfig icons_config; icons_config.MergeMode = true; icons_config.PixelSnapH = true;
		
		io.Fonts->AddFontFromFileTTF(FONT_ICON_FILE_NAME_sea_sim_, 20, &icons_config, icons_ranges);

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
				if (windows_show_state_.notification_queue.empty())
				{
					windows_show_state_.notification_popup_new = false;
					windows_show_state_.notification_popup = false;
					ImGui::CloseCurrentPopup();
				}
				else
				{
					windows_show_state_.notification_text = windows_show_state_.notification_queue.front();
					windows_show_state_.notification_queue.pop();
				}
			}

			ImGui::EndPopup();
		}
	}
	void WindowStorage::set_notification(const std::string& text)
	{
		if (windows_show_state_.notification_popup     == true ||
			windows_show_state_.notification_popup_new == true)
			windows_show_state_.notification_queue.push(text);
		else
		{
			windows_show_state_.notification_popup_new = true;
			windows_show_state_.notification_text = text;
		}
	}

	void WindowStorage::show_child_input()
	{
		ImGuiWindowFlags_ flags = ImGuiWindowFlags_HorizontalScrollbar;

		ImGui::Begin(u8"Ввод данных"_C, NULL, flags);

		ImGui::Text(u8"%.0f FPS"_C, ImGui::GetIO().Framerate);
		ImGui::Text(u8"Область отрисовки: %ix%i пкс"_C, render_engine_.get_texture_size().x, render_engine_.get_texture_size().y);

		ImGui::Separator();

		if (auto packet = render_engine_.render_inputs())
			send_to_core(packet.value());

		ImGui::End();
	}
	void WindowStorage::show_child_output()
	{
		ImGuiWindowFlags_ flags = ImGuiWindowFlags_HorizontalScrollbar;

		ImGui::Begin(u8"Результаты"_C, NULL, flags);
		
		render_engine_.render_outputs();

		ImGui::End();
	}
	void WindowStorage::show_child_view()
	{
		// --- Time

		auto current_time = std::chrono::system_clock::now();
		auto elapsed = current_time - time_manipulations_.last_elapsed;
		time_manipulations_.last_elapsed = current_time;

		if (time_manipulations_.frame_pause)
			elapsed *= 0;
		if (time_manipulations_.sim_speed >= 0)
			elapsed *= int64_t(pow(2, time_manipulations_.sim_speed));
		else
			elapsed /= int64_t(pow(2, abs(time_manipulations_.sim_speed)));

		time_manipulations_.clock += elapsed;

		// convert time_stamp to string

		struct tm newtime;
#ifdef WIN32
		const auto nowAsTimeT = std::chrono::system_clock::to_time_t(time_manipulations_.clock);
		localtime_s(&newtime, &nowAsTimeT);
#else
		auto nowAsTimeT = std::chrono::system_clock::to_time_t(time_manipulations_.clock);
		newtime = *localtime(&nowAsTimeT);
#endif

		std::stringstream nowSs;
		nowSs << std::put_time(&newtime, "%H:%M:%S");

		// --- Window
		
		ImGui::Begin(u8"Обзор"_C, NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_MenuBar);

		if (ImGui::BeginMenuBar())
		{
			ImGui::Separator();


			ImGui::Text("%s", u8"Время: "_C);

			int32_t width = static_cast<int32_t>(get_button_width(nowSs.str(), ImGui::GetStyle()));
			ImGui::BeginChildFrame(ImGui::GetID("view_time_area"), {(width / 25 + 1) * 25.f, 0.f});

			ImGui::Text("%s", nowSs.str().c_str());

			ImGui::EndChildFrame();


			ImGui::Separator();

			if ((ImGui::Button(ICON_sea_sim__BACKWARD) || key_hit(sf::Keyboard::PageDown)) 
				&& time_manipulations_.sim_speed >= -4)
			{
				--time_manipulations_.sim_speed;
				send_to_core("speed_down", {});
			}

			ImGui::Separator();

			bool keyboard_is_captured = ImGui::GetIO().WantCaptureKeyboard;
			bool space_hit = key_hit(sf::Keyboard::Space);
			bool flip_pause = !keyboard_is_captured && space_hit;
			bool pause_on = flip_pause && !time_manipulations_.frame_pause;
			bool pause_off = flip_pause && time_manipulations_.frame_pause;

			if (ImGui::Button(ICON_sea_sim__PAUSE) || pause_on)
			{
				time_manipulations_.frame_pause = true;
				send_to_core("pause_on", {});
			}

			ImGui::Separator();

			if (ImGui::Button(ICON_sea_sim__PLAY) || pause_off)
			{
				time_manipulations_.frame_pause = false;
				send_to_core("pause_off", {});
			}

			ImGui::Separator();

			if ((ImGui::Button(ICON_sea_sim__FORWARD) || key_hit(sf::Keyboard::PageUp))
				&& time_manipulations_.sim_speed <= 4)
			{
				++time_manipulations_.sim_speed;
				send_to_core("speed_up", {});
			}

			ImGui::Separator();

			if (time_manipulations_.sim_speed >= 0)
				ImGui::Text("x%i", static_cast<int>(pow(2, time_manipulations_.sim_speed)));
			else
				ImGui::Text("1/%i", static_cast<int>(pow(2, abs(time_manipulations_.sim_speed))));

			ImGui::Separator();

			if (time_manipulations_.frame_pause)
			{
				ImGui::Text("%s", u8"Пауза"_C);

				ImGui::Separator();
			}

			ImGui::EndMenuBar();
		}

		ImVec2 view_area = ImGui::GetWindowContentRegionMax() -
			               ImGui::GetWindowContentRegionMin();

		if (view_area != windows_show_state_.render_size)
		{
			windows_show_state_.render_size = view_area;

			view_area.x = max(1, view_area.x - 2);
			view_area.y = max(1, view_area.y - 2);

			render_engine_.create_texture(view_area);

			sf::Vector2u scene_size = render_engine_.get_texture_size();
		}


		ImVec2 relative_mouse_pos = ImGui::GetMousePos() - ImGui::GetCursorScreenPos();

		ImGui::Image(render_engine_.get_texture(), ImVec2(render_engine_.get_texture_size()), sf::Color::White, sf::Color(70, 70, 70));

		if (ImGui::IsItemHovered())
		{
			if (button_cache_.last_mouse_pos != relative_mouse_pos)
			{
				button_cache_.last_mouse_pos = relative_mouse_pos;

				send_to_core("mouse_position_changed", { { "mouse_position", sf::Vector2u(relative_mouse_pos)} });
			}

			uint8_t hash =
				(mouse_down(sf::Mouse::Left)   << 0) +
				(mouse_down(sf::Mouse::Middle) << 1) +
				(mouse_down(sf::Mouse::Right)  << 2);

			if (button_cache_.last_mouse_buttons != hash)
			{
				button_cache_.last_mouse_buttons = hash;

				send_to_core("mouse_buttons_changed", { { "mouse_buttons", hash } });
			}
		}

		ImGui::End();
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

			if (ImGui::BeginMenu(u8"Модули"_C))
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
			bool keyboard_is_captured = ImGui::GetIO().WantCaptureKeyboard;

			if (auto file_paths = file_dialog_.render_dialog())
			{
				for (auto& file : file_paths.value())
					send_to_core("load_module", { {"module_path", file} });
			}
			else if (key_hit(sf::Keyboard::Escape))
				file_dialog_.close();
			else if (key_hit(sf::Keyboard::Enter) && !keyboard_is_captured)
			{
				if (auto file_paths = file_dialog_.try_take_files())
				{
					for (auto& file : file_paths.value())
						send_to_core("load_module", { {"module_path", file} });
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
    float WindowStorage::get_button_width(const std::string& text, ImGuiStyle& style)
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
