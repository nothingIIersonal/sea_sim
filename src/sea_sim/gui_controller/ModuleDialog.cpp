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
			
			if (ImGui::BeginChildFrame(ImGui::GetID("modules list"), { -1.f, -ImGui::GetFrameHeightWithSpacing() * 2 - 4.f }, flags))
			{
				if (modules_order_.empty())
				{
					ImGui::Text(u8"Загрузите модули в меню \"Модули->Добавить\""_C);
				}
				else
				{
					uint32_t gl_id = 0;
					float FontSize = ImGui::GetTextLineHeightWithSpacing();

					for (auto order_entry = modules_order_.begin(); order_entry != modules_order_.end();)
					{
						auto& entry = modules_map_.at(*order_entry);

						// --- Unload module

						if (ImGui::Button((ICON_sea_sim__TIMES + std::string("##") + std::to_string(gl_id)).c_str()))
						{
							if (modules_map_.at(*order_entry).state == ModuleState::LOADED)
								buf.push_back(unload_module(*order_entry));
							else
							{
								if (auto packet = remove_module_from_list(order_entry))
									buf.push_back(packet.value());
								continue;
							}
						}

						ImGui::SameLine();

						// --- Move up

						if (gl_id == 0)
							ImGui::BeginDisabled();

						if (ImGui::Button((ICON_sea_sim__ARROW_UP "##" + std::to_string(gl_id)).c_str()))
						{
							buf.push_back({ "core", "gui", "move_module_up", {{"module_path", *order_entry}} });

							std::swap<std::string>(modules_order_.at(gl_id), modules_order_.at(static_cast<size_t>(gl_id - 1)));
						}

						if (gl_id == 0)
							ImGui::EndDisabled();

						ImGui::SameLine();

						// --- Move down

						if (gl_id == modules_map_.size() - 1)
							ImGui::BeginDisabled();

						if (ImGui::Button((ICON_sea_sim__ARROW_DOWN + std::string("##") + std::to_string(gl_id)).c_str()))
						{
							buf.push_back({ "core", "gui", "move_module_down", {{"module_path", *order_entry}} });

							std::swap<std::string>(modules_order_.at(gl_id), modules_order_.at(static_cast<size_t>(gl_id + 1)));
						}

						if (gl_id == modules_map_.size() - 1)
							ImGui::EndDisabled();

						ImGui::SameLine();

						// --- Select

						ImGui::Checkbox(("##module_checkbox_" + std::to_string(gl_id)).c_str(), &entry.selected);

						ImGui::SameLine();

						// --- Module path

						ImVec2 cursorScreenPos = ImGui::GetCursorScreenPos();
						float width = ImGui::CalcTextSize(order_entry->c_str()).x;

						ImGui::Text("%s", order_entry->c_str());

						if (entry.state == ModuleState::UNLOADED)
						{
							cursorScreenPos.y += static_cast<int>(FontSize * 0.5f);
							ImGui::GetWindowDrawList()->AddLine(
								cursorScreenPos, ImVec2(cursorScreenPos.x + width, cursorScreenPos.y), IM_COL32(255, 255, 255, 255), 1.f);

							cursorScreenPos.y += 1;
							ImGui::GetWindowDrawList()->AddLine(
								cursorScreenPos, ImVec2(cursorScreenPos.x + width, cursorScreenPos.y), IM_COL32(255, 255, 255, 255), 1.f);
						}

						++gl_id;
						++order_entry;
					}
				}

				ImGui::EndChildFrame();
			}

			if (ImGui::Button(u8"Выйти"_C))
				close();

			ImGui::SameLine();

			if (ImGui::Button(u8"Выбрать всё"_C))
				for (auto& elem : modules_map_)
					elem.second.selected = true;

			ImGui::SameLine();

			if (ImGui::Button(u8"Снять выделение"_C))
				for (auto& elem : modules_map_)
					elem.second.selected = false;


			if (ImGui::Button(u8"Загрузить выделенное"_C))
				for (auto& elem : modules_order_)
					if (modules_map_.at(elem).selected == true && modules_map_.at(elem).state == ModuleState::UNLOADED)
						buf.push_back(load_module(elem));

			ImGui::SameLine();

			if (ImGui::Button(u8"Выгрузить выделенное"_C))
				for (auto& elem : modules_order_)
					if (modules_map_.at(elem).selected == true && modules_map_.at(elem).state == ModuleState::LOADED)
						buf.push_back(unload_module(elem));

			ImGui::End();
		}

		ImGui::PopStyleVar();

		if (buf.empty())
			return std::nullopt;
		else
			return buf;
	}

	channel_packet ModuleDialog::load_module(const std::string& module_path)
	{
		return { "core", "gui", "load_module", {{"module_path", module_path}} };
	}

	channel_packet ModuleDialog::unload_module(const std::string& module_path)
	{
		return { "core", "gui", "unload_module", {{"module_path", module_path}} };
	}

	void ModuleDialog::add_module(const std::string& module_path)
	{
		if (modules_map_.contains(module_path))
		{
			if (modules_map_.at(module_path).state == ModuleState::UNLOADED)
				modules_map_.at(module_path).state = ModuleState::LOADED;
			return;
		}

		modules_order_.push_back(module_path);
		modules_map_.insert({ module_path, { module_path } });
	}

	void ModuleDialog::remove_module(const std::string& module_path)
	{
		if (!modules_map_.contains(module_path))
			return;

		modules_map_.at(module_path).state = ModuleState::UNLOADED;
	}

	std::optional<channel_packet> ModuleDialog::remove_module_from_list(std::vector<std::string>::iterator& module)
	{
		if (!modules_map_.contains(*module))
			return std::nullopt;

		if (modules_map_.at(*module).state == ModuleState::LOADED)
			return std::nullopt;

		std::string module_path = *module; // copy: to save the path to return after erasing *module

		modules_map_.erase(*module);
		module = modules_order_.erase(std::find(modules_order_.begin(), modules_order_.end(), *module));

		return { { "core", "gui", "remove_module_from_order", {{"module_path", module_path}} } };
	}

} // namespace gui
