#pragma once
#include <imgui.h>
#include <imgui-SFML.h>
#include <imgui_stdlib.h>
#include <imgui_internal.h>

#include <SFML/Graphics.hpp>

#include <sea_sim/json/json.hpp>
#include <optional>
#include <variant>


namespace gui
{
	class ModulePageStorage
	{
	public:
		ModulePageStorage();

		void set_input_interface(nlohmann::json input_interface);
		void set_output_interface(nlohmann::json output_interface);

		std::optional<nlohmann::json> render_input_interface();
		void render_output_interface();

		nlohmann::json get_fields();
		std::string& get_title();

	private:
		class WidgetElement
		{
		public:
			WidgetElement(nlohmann::json widget);

			std::optional<std::string> render_widget(std::string ident);

			std::optional<nlohmann::json> get_field();

		private:
			enum class WidgetEnum
			{
				TEXT = 0,
				BUTTON,
				SLIDERINT,
				SLIDERFLOAT,
				INPUTTEXT,
				INPUTINT,
				INPUTFLOAT,
				CHECKBOX,
				RADIOBUTTON,
				COMBO,

				SAMELINE,

				INVALID
			};

			WidgetEnum widget_type_;
			nlohmann::json widget_info_;
			std::variant<bool, int, float, std::string> stored_value_;

		public:
			std::string get_WidgetType(WidgetEnum& widget_type);

		};

		std::string module_title_;

		nlohmann::json input_interface_ = {};
		nlohmann::json output_interface_ = {};

		std::vector<WidgetElement> input_widgets_vector_ = {};
		std::vector<WidgetElement> output_widgets_vector_ = {};

		static uint32_t gl_id_;
	};

} // namespace gui
