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

		std::vector<channel_packet> buf = {};

		ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(400, 250));
		ImGui::SetNextWindowSize(ImVec2(800, 500), ImGuiCond_Once);

		if (ImGui::Begin(u8"Список модулей"_C, &is_open_, ImGuiWindowFlags_NoDocking))
		{
			ImGuiWindowFlags_ flags = ImGuiWindowFlags_HorizontalScrollbar;
			
			if (ImGui::BeginChildFrame(ImGui::GetID("modules list"), { -1.f, -ImGui::GetFrameHeightWithSpacing() - 4.f }, flags))
			{
				uint32_t gl_id = 0;
				float FontSize = ImGui::GetTextLineHeightWithSpacing();

				for (auto& order_entry : modules_order_)
				{
					auto& entry = modules_map_.at(order_entry);

					// --- Unload module

					if (ImGui::Button((ICON_sea_sim__TIMES + std::string("##") + std::to_string(gl_id)).c_str()))
					{
						buf.push_back({ "core", "gui", "unload_module", {{"module_path", order_entry}} });
					}

					ImGui::SameLine();

					// --- Move up

					if (gl_id == 0)
						ImGui::BeginDisabled();

					if (ImGui::Button((ICON_sea_sim__ARROW_UP "##" + std::to_string(gl_id)).c_str()))
					{
						std::swap<std::string>(modules_order_.at(gl_id), modules_order_.at(static_cast<size_t>(gl_id - 1)));

						buf.push_back({ "core", "gui", "move_module_up", {{"module_path", order_entry}} });
					}

					if (gl_id == 0)
						ImGui::EndDisabled();

					ImGui::SameLine();
					
					// --- Move down

					if (gl_id == modules_map_.size() - 1)
						ImGui::BeginDisabled();

					if (ImGui::Button((ICON_sea_sim__ARROW_DOWN + std::string("##") + std::to_string(gl_id)).c_str()))
					{
						std::swap<std::string>(modules_order_.at(gl_id), modules_order_.at(static_cast<size_t>(gl_id + 1)));

						buf.push_back({ "core", "gui", "move_module_down", {{"module_path", order_entry}} });
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
					
					ImVec2 cursorScreenPos = ImGui::GetCursorScreenPos();
					float width = ImGui::CalcTextSize(order_entry.c_str()).x;

					ImGui::Text("%s", order_entry.c_str());

					if (entry)
					{
						cursorScreenPos.y += static_cast<int>(FontSize * 0.5f);
						ImGui::GetWindowDrawList()->AddLine(
							cursorScreenPos, ImVec2(cursorScreenPos.x + width, cursorScreenPos.y), IM_COL32(255, 255, 255, 255), 1.f);

						cursorScreenPos.y += 1;
						ImGui::GetWindowDrawList()->AddLine(
							cursorScreenPos, ImVec2(cursorScreenPos.x + width, cursorScreenPos.y), IM_COL32(255, 255, 255, 255), 1.f);
					}

					++gl_id;
				}

				ImGui::EndChildFrame();
			}

			if (ImGui::Button(u8"Выйти"_C))
				close();

			ImGui::End();
		}

		ImGui::PopStyleVar();

		if (buf.empty())
			return std::nullopt;
		else
			return buf;
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
