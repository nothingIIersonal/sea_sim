﻿#include <sea_sim/gui_controller/RenderEngine.h>
#include <sea_sim/gui_controller/window_storage.h>

#include <iostream>

namespace gui
{
	RenderEngine::RenderEngine(WindowStorage* parent)
		: parent_ptr_(parent), graphics_storage_(this) {}
	RenderEngine::~RenderEngine()
	{
		parent_ptr_ = nullptr;
	}

	void RenderEngine::create_texture(unsigned int x, unsigned int y)
	{
		get_texture(true).create(x, y);
	}

	void RenderEngine::update_input_interface(std::string& module, nlohmann::json& data)
	{
		if (!module_pages.contains(module))
			selected_module = module;

		module_pages[module].set_input_interface(data);
	}
	void RenderEngine::update_output_interface(std::string& module, nlohmann::json& data)
	{
		module_pages[module].set_output_interface(data);
	}
	void RenderEngine::remove_interface(std::string& module)
    {
		if (module_pages.contains(module))
        {
            module_pages.erase(module);

            if (module_pages.empty())
                selected_module = "";
            else
                selected_module = module_pages.begin()->first;
        }
    }

	void RenderEngine::swap_texture()
	{
		graphic_buffer_.writing_buffer = !graphic_buffer_.writing_buffer;

		if (get_texture_size(true) != get_texture_size(false))
			create_texture(get_texture_size(false).x, get_texture_size(false).y);
	}

	void RenderEngine::render_scene()
	{
		using namespace gui::utils;

		sf::Vector2u scene_size = get_texture(true).getSize();

		sf::Vector2f rect_size{
			max(0.f, static_cast<float>(scene_size.x) - 50.f),
			max(0.f, static_cast<float>(scene_size.y) - 50.f) };

		sf::RectangleShape rectangle(rect_size);
		rectangle.setFillColor(sf::Color::Cyan);
		rectangle.setPosition(25, 25);

		// render

		get_texture(true).clear();
		get_texture(true).draw(rectangle);
		get_texture(true).display();

		return;
	}

	std::optional<std::string> RenderEngine::render_modules_combo()
	{
		using namespace utils;
		
		ImGui::Text("%s", u8"Ìîäóëü:"_C);

		ImGui::SameLine();

		std::string title = (module_pages.contains(selected_module)) ?
			module_pages[selected_module].get_title() : "";

		ImGui::PushItemWidth(250);

		if (ImGui::BeginCombo("##module_selection_combo", title.c_str()))
		{
			for (auto& module : module_pages)
			{
				auto& title = module.second.get_title();
				if (ImGui::Selectable(title.c_str(), module.first == selected_module))
					selected_module = module.first;
			}

			ImGui::EndCombo();
		}

		ImGui::PopItemWidth();

        ImGui::SameLine();

        if (ImGui::Button(ICON_sea_sim__TIMES) && !selected_module.empty())
        {
            std::string buf = selected_module;

            return buf;
        }

        return std::nullopt;
	}

	std::optional<channel_packet> RenderEngine::render_inputs()
	{
        if (auto remove_module = render_modules_combo())
        {
            return channel_packet{ "core", "gui", "unload_module", {{ "module_path", remove_module.value() }} };
        }

        ImGui::Separator();

        if (selected_module.empty() || !module_pages.contains(selected_module))
            return std::nullopt;

        if (auto packet = module_pages[selected_module].render_input_interface())
        {
            packet.value()["module_path"] = selected_module;
            return channel_packet{ "core", "gui", "exec_module", packet.value() };
        }

        return std::nullopt;
	}
	void RenderEngine::render_outputs()
	{
		if (selected_module.empty() || !module_pages.contains(selected_module))
			return;

		module_pages[selected_module].render_output_interface();
	}

	sf::RenderTexture& RenderEngine::get_texture(bool get_writing_texture)
	{
		return graphic_buffer_.render_texture_[get_writing_texture == graphic_buffer_.writing_buffer];
	}

	sf::Vector2u RenderEngine::get_texture_size(bool get_writing_texture)
	{
		return get_texture(get_writing_texture).getSize();
	}

	void RenderEngine::set_notification(const std::string& text)
	{
		if (parent_ptr_ == nullptr)
			return;

		parent_ptr_->set_notification(text);
	}

} // namespace gui
