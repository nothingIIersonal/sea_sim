#include <sea_sim/gui_controller/ModuleDialog.h>

#include <iostream>

namespace gui
{
	using namespace utils;

	void ModuleDialog::open()
	{
		is_open_ = true;
	}
	void ModuleDialog::close()
	{
		is_open_ = false;
	}

	bool ModuleDialog::is_open()
	{
		return is_open_;
	}

	std::optional<std::vector<channel_packet>> ModuleDialog::render_dialog()
	{
		if (!is_open_)
			return std::nullopt;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(400, 250));
		ImGui::SetNextWindowSize(ImVec2(800, 500), ImGuiCond_Once);

		if (ImGui::Begin(u8"Список модулей"_C, NULL, ImGuiWindowFlags_NoDocking))
		{
			if (ImGui::BeginListBox("##modules list", { -1.f, -ImGui::GetFrameHeightWithSpacing() - 4.f }))
			{
				uint32_t gl_id = 0;
				for (auto& order_entry : modules_order_)
				{
					auto& entry = modules_map_.at(order_entry);

					// --- Unload module

					if (ImGui::Button(("X##" + std::to_string(gl_id)).c_str()))
					{

					}

					ImGui::SameLine();

					// --- Move up

					if (gl_id == 0)
						ImGui::BeginDisabled();

					if (ImGui::Button(("U##" + std::to_string(gl_id)).c_str()))
					{
						std::swap<std::string>(modules_order_.at(gl_id), modules_order_.at(gl_id - (uint32_t)1));
					}

					if (gl_id == 0)
						ImGui::EndDisabled();

					ImGui::SameLine();
					
					// --- Move down

					if (gl_id == modules_map_.size() - 1)
						ImGui::BeginDisabled();

					if (ImGui::Button(("D##" + std::to_string(gl_id)).c_str()))
					{
						std::swap<std::string>(modules_order_.at(gl_id), modules_order_.at(gl_id + (uint32_t)1));
					}

					if (gl_id == modules_map_.size() - 1)
						ImGui::EndDisabled();

					ImGui::SameLine();

					// --- Select

					if (ImGui::Checkbox(("##module_checkbox_" + std::to_string(gl_id)).c_str(), &entry))
					{

					}

					ImGui::SameLine();

					// --- Module path

					ImGui::Text(order_entry.c_str());

					++gl_id;
				}

				ImGui::EndListBox();
			}

			if (ImGui::Button(u8"Îòìåíà"_C))
				close();

			ImGui::End();
		}

		ImGui::PopStyleVar();

		return std::nullopt;
	}

	void ModuleDialog::add_module(std::string& module_path)
	{
		modules_order_.push_back(module_path);
		modules_map_.insert({ module_path, false });
	}

	void ModuleDialog::remove_module(std::string& module_path)
	{
		modules_order_.erase(std::find(modules_order_.begin(), modules_order_.end(), module_path));
		modules_map_.erase(module_path);
	}

} // namespace gui
