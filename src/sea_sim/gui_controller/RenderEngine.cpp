#include <sea_sim/gui_controller/RenderEngine.h>

#include <iostream>

namespace gui
{
	void RenderEngine::update_input_interface(std::string module, nlohmann::json data)
	{
		float_fields_storage_[module].clear();

		input_interface_storage_[module] = data;

		for (auto& widget : input_interface_storage_[module])
		{
			auto type = widget["type"].get<std::string>();
			if (type == "input_float")
			{
				auto label = widget["meta"]["label"].get<std::string>();
				float_fields_storage_[module][label] = 0.f;
			}
		}
	}
	void RenderEngine::update_output_interface(std::string module, nlohmann::json data)
	{
		output_interface_storage_[module] = data;
	}

	void RenderEngine::render_scene(sf::RenderTexture& texture)
	{
		using namespace gui::utils;

		sf::Vector2u scene_size = texture.getSize();

		sf::Vector2f rect_size{
			max(0.f, static_cast<float>(scene_size.x) - 50.f),
			max(0.f, static_cast<float>(scene_size.y) - 50.f) };

		sf::RectangleShape rectangle(rect_size);
		rectangle.setFillColor(sf::Color::Cyan);
		rectangle.setPosition(25, 25);

		// render

		texture.clear();
		texture.draw(rectangle);
		texture.display();

		return;
	}
	float a = 0;
	std::optional<channel_packet> RenderEngine::render_inputs(std::string module)
	{
		ImGui::SliderFloat("Percentage", &a, 0, 1);
		ImGui::ProgressBar(a, { 256, 20 });

		if (module.empty() || !input_interface_storage_.contains(module))
			return std::nullopt;

		channel_packet packet = {"core", "gui", ""};
		int gl_id = 0;

		for (auto& widget : input_interface_storage_[module])
		{
			ImGui::PushID(gl_id);

			auto type = widget["type"].get<std::string>();

			if (type == "text")
			{
				auto value = widget["meta"]["value"].get<std::string>();
				ImGui::Text("%s", reinterpret_cast<const char*>(value.c_str()));
			}
			else if (type == "block")
			{
				ImGui::SameLine();
			}
			else if (type == "input_float")
			{
				auto label = widget["meta"]["label"].get<std::string>();
				ImGui::InputFloat("", &float_fields_storage_[module][label]);
			}
			else if (type == "button")
			{
				auto value = widget["meta"]["value"].get<std::string>();
				if (ImGui::Button(reinterpret_cast<const char*>(value.c_str())))
				{
					packet.to = module;
					packet.event = widget["meta"]["label"].get<std::string>();

					for (const auto& [key, value] : int_fields_storage_[module])
						packet.data.push_back({ { key, value } });

					for (const auto& [key, value] : float_fields_storage_[module])
						packet.data.push_back({ { key, value } });

					for (const auto& [key, value] : string_fields_storage_[module])
						packet.data.push_back({ { key, value } });
				}
			}

			ImGui::PopID();
			++gl_id;
		}
		
		if (packet.event != "")
			return std::make_optional(packet);
		else
			return std::nullopt;
	}
	void RenderEngine::render_outputs()
	{

	}

} // namespace gui