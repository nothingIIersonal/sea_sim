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
		window_.setFramerateLimit(240);

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
			}
		}

		while (auto response = channel_to_core.TryRead())
		{
			auto& packet_to = response.value().to;
			auto& packet_event = response.value().event;
			auto& packet_data = response.value().data;

			if (packet_to == "gui")
			{
				if (packet_event == "set_input_interface")
				{
					render_engine_.update_input_interface(fields_data_.selected_module, packet_data);
				}
				else if (packet_event == "set_output_interface")
				{

				}
			}
		}
	}
	void WindowStorage::shutdown()
	{
		channel_to_core.SendData({ "core", "gui", "shutdown", {}});
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
		ImGui::SetNextWindowPos({ 0, 0 }, ImGuiCond_Once);
		ImGui::SetNextWindowSize(get_screen_size());
		
		ImGuiWindowFlags main_window_flags =
			ImGuiWindowFlags_NoDocking |
			ImGuiWindowFlags_MenuBar |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoBringToFrontOnFocus;

		if (ImGui::Begin("##MainMenu", NULL, main_window_flags))
		{
			show_main_menu_bar();

			ImGuiID id = ImGui::GetID("MainWindowLayout");
			ImGui::DockSpace(id, { 0, 0 }, ImGuiDockNodeFlags_PassthruCentralNode);

			if (windows_show_state_.reset_docking_layout)
			{
				ImGui_reset_docking_layout();
				windows_show_state_.reset_docking_layout = false;
			}
			
			ImGui::End();
		}
	}
	void WindowStorage::show_exit_popup()
	{
		if (windows_show_state_.exit_popup_new !=
			windows_show_state_.exit_popup)
		{
			windows_show_state_.exit_popup_new ?
				ImGui::OpenPopup(u8"Выход"_C)
				:
				ImGui::CloseCurrentPopup();
			windows_show_state_.exit_popup = windows_show_state_.exit_popup_new;
		}

		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));

		if (ImGui::BeginPopupModal(u8"Выход"_C, NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("%s", u8"Вы уверены?"_C);
			ImGui::NewLine();
			ImGui::Separator();

			if (ImGui::Button(u8"Выйти"_C, ImVec2(120, 0)))
			{
				ImGui::CloseCurrentPopup();
				window_close();
			}
			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();
			if (ImGui::Button(u8"Остаться"_C, ImVec2(120, 0)) || key_hit(sf::Keyboard::Escape))
			{
				windows_show_state_.exit_popup_new = false;
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		if (windows_show_state_.exit_popup == false && key_hit(sf::Keyboard::Escape))
		{
			windows_show_state_.exit_popup_new = true;
		}
	}

	void WindowStorage::show_child_input()
	{
		if (ImGui::Begin(u8"Ввод данных"_C))
		{
			ImGui::Text(u8"%.0f FPS"_C, ImGui::GetIO().Framerate);
			ImGui::Text(u8"Область отрисовки: %ix%i пкс"_C, render_texture_.getSize().x, render_texture_.getSize().y);

			ImGui::Separator();

			if (ImGui::BeginCombo("##modules", fields_data_.selected_module.c_str()))
			{
				if (ImGui::Selectable("m1"))
					fields_data_.selected_module = "m1";
				if (ImGui::Selectable("m2"))
					fields_data_.selected_module = "m2";

				ImGui::EndCombo();
			}

			ImGui::Separator();

			if (auto packet = render_engine_.render_inputs(fields_data_.selected_module))
			{
				send_to_core(packet.value());
			}

			ImGui::End();
		}
	}
	void WindowStorage::show_child_output()
	{
		if (ImGui::Begin(u8"Результаты"_C))
		{
			// ImGui::ShowFontAtlas(font_x20_->ContainerAtlas);
			// ImGui::ShowFontSelector("Fonts");

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
			}
			
			render_engine_.render_scene(render_texture_);

			ImGui::Image(render_texture_, sf::Color::White, sf::Color(70, 70, 70));

			ImGui::End();
		}
	}

	void WindowStorage::show_main_menu_bar()
	{
		if (ImGui::BeginMenuBar())
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
					window_close();
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu(u8"Вид"_C))
			{
				if (ImGui::MenuItem(u8"Восстановить"_C))
				{
					windows_show_state_.reset_docking_layout = true;
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu(u8"Плагины"_C))
			{
				if (ImGui::MenuItem(u8"Добавить"_C))
				{

				}

				ImGui::Separator();

				if (ImGui::MenuItem(u8"Обновить интерфейс"_C))
				{
					if (!fields_data_.selected_module.empty())
						send_to_core("request_interface");
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}
	}

	void WindowStorage::ImGui_reset_docking_layout()
	{
		ImGuiID id = ImGui::GetID("MainWindowLayout");
		ImGui::DockBuilderRemoveNode(id);
		ImGui::DockBuilderAddNode(id);


		ImGuiID dock1 = ImGui::DockBuilderSplitNode(id, ImGuiDir_Left, 1.f, nullptr, &id);

		ImGuiID dock2 = ImGui::DockBuilderSplitNode(dock1, ImGuiDir_Right, 0.25f, nullptr, &dock1);

		ImGuiID dock3 = ImGui::DockBuilderSplitNode(dock2, ImGuiDir_Down, 0.5f, nullptr, &dock2);


		ImGui::DockBuilderDockWindow(u8"Обзор"_C, dock1);
		ImGui::DockBuilderDockWindow(u8"Ввод данных"_C, dock2);
		ImGui::DockBuilderDockWindow(u8"Результаты"_C, dock3);

		ImGui::DockBuilderFinish(id);
	}

	void WindowStorage::send_to_core(std::string event, nlohmann::json data)
	{
		channel_to_core.SendData({ "core", "gui", event, data });
	}
	void WindowStorage::send_to_core(channel_packet& packet)
	{
		channel_to_core.SendData(packet);
	}

} // namespace gui
