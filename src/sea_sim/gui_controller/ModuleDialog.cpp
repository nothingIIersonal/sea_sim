#include <sea_sim/gui_controller/ModuleDialog.h>

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

			ImGui::End();
		}

		ImGui::PopStyleVar();

		return std::nullopt;
	}

} // namespace gui
