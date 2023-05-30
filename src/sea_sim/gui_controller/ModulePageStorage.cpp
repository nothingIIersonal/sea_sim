#include <iostream>

#include <sea_sim/gui_controller/ModulePageStorage.h>


namespace gui
{
	using namespace utils;

	uint32_t ModulePageStorage::gl_id_ = 0;

	ModulePageStorage::ModulePageStorage()
	{
		module_title_ = u8"Без имени "_C + std::to_string(gl_id_++);
	}

	void ModulePageStorage::set_input_interface(nlohmann::json& input_interface)
	{
		input_interface_ = input_interface;

		std::vector<WidgetElement> input_widgets_vector_buf;
		
		uint32_t placeholder_id = 0;
		for (auto& widget : input_interface_)
		{
			auto widget_type = widget["type"].get<std::string>();
			if (widget_type == "module_title")
				module_title_ = widget["widget"]["title"].get<std::string>();
			else
			{
				if (widget_type != "sameline" && widget_type != "text" && widget_type != "button")
				{
					auto keep_value = widget["widget"]["keep_value"].get<bool>();

					if (keep_value)
					{
						auto identifier = widget["widget"]["identifier"].get<std::string>();

						if (auto field = get_field_by_identifier(identifier))
						{
							input_widgets_vector_buf.push_back(WidgetElement{ widget, field.value() });
							continue;
						}
					}
				}
				input_widgets_vector_buf.push_back(WidgetElement{ widget });
			}
		}

		input_widgets_vector_ = std::move(input_widgets_vector_buf);
	}

	void ModulePageStorage::set_output_interface(nlohmann::json& output_interface)
	{
		output_interface_ = output_interface;
		output_widgets_vector_.clear();

		uint32_t placeholder_id = 0;
		for (auto& widget : output_interface_)
			output_widgets_vector_.push_back(WidgetElement{ widget });
	}

	std::optional<nlohmann::json> ModulePageStorage::render_input_interface()
	{
		std::optional<std::string> event_str = std::nullopt;

		uint32_t gl_id = 0;
		for (auto& widget : input_widgets_vector_)
		{
			if (auto event_res = widget.render_widget(std::to_string(gl_id++)))
			{
				event_str = event_res;
			}
		}

		if (event_str)
		{
			return nlohmann::json{ {"trigger", event_str.value()}, {"fields", get_fields()} };
		}

		return std::nullopt;
	}

	void ModulePageStorage::render_output_interface()
	{
		uint32_t gl_id = 2048;
		for (auto& widget : output_widgets_vector_)
			widget.render_widget(std::to_string(gl_id++));
	}

	nlohmann::json ModulePageStorage::get_fields()
	{
		nlohmann::json fields = {};

		for (auto& widget : input_widgets_vector_)
			if (auto field = widget.get_field())
				fields[widget.get_identifier().value()] = field.value()["value"];

		return fields;
	}

	std::string& ModulePageStorage::get_title()
	{
		return module_title_;
	}

	std::optional<nlohmann::json> ModulePageStorage::get_field_by_identifier(const std::string& identifier)
	{
		for (auto& widget : input_widgets_vector_)
			if (auto cur_identifier = widget.get_identifier())
				if (cur_identifier == identifier)
					return widget.get_field();
		return std::nullopt;
	}


	ModulePageStorage::WidgetElement::WidgetElement(nlohmann::json& widget, nlohmann::json stored_value)
	{
		auto type = widget["type"].get<std::string>();
		widget_info_ = widget["widget"];

		if (type == "text")
			{ widget_type_ = WidgetEnum::TEXT;        stored_value_ = stored_value.empty() ? 0     : stored_value["value"].get<int>();         }
		else if (type == "button")
			{ widget_type_ = WidgetEnum::BUTTON;      stored_value_ = stored_value.empty() ? 0     : stored_value["value"].get<int>();         }
		else if (type == "sliderint")
			{ widget_type_ = WidgetEnum::SLIDERINT;   stored_value_ = stored_value.empty() ? 0     : stored_value["value"].get<int>();         }
		else if (type == "sliderfloat")
			{ widget_type_ = WidgetEnum::SLIDERFLOAT; stored_value_ = stored_value.empty() ? 0.f   : stored_value["value"].get<float>();       }
		else if (type == "inputtext")
			{ widget_type_ = WidgetEnum::INPUTTEXT;   stored_value_ = stored_value.empty() ? ""    : stored_value["value"].get<std::string>(); }
		else if (type == "inputint")
			{ widget_type_ = WidgetEnum::INPUTINT;    stored_value_ = stored_value.empty() ? 0     : stored_value["value"].get<int>();         }
		else if (type == "inputfloat")
			{ widget_type_ = WidgetEnum::INPUTFLOAT;  stored_value_ = stored_value.empty() ? 0.f   : stored_value["value"].get<float>();       }
		else if (type == "checkbox")
			{ widget_type_ = WidgetEnum::CHECKBOX;    stored_value_ = stored_value.empty() ? false : stored_value["value"].get<bool>();        }
		else if (type == "radiobutton")
			{ widget_type_ = WidgetEnum::RADIOBUTTON; stored_value_ = stored_value.empty() ? ""    : stored_value["value"].get<std::string>(); }
		else if (type == "dropdownlist")
			{ widget_type_ = WidgetEnum::COMBO;       stored_value_ = stored_value.empty() ? ""    : stored_value["value"].get<std::string>(); }
		else if (type == "sameline")
			{ widget_type_ = WidgetEnum::SAMELINE;    stored_value_ = stored_value.empty() ? 0     : stored_value["value"].get<int>();         }
		else
			{ widget_type_ = WidgetEnum::INVALID;     stored_value_ = stored_value.empty() ? 0     : stored_value["value"].get<int>();         }
	}


	std::optional<std::string> ModulePageStorage::WidgetElement::render_widget(std::string ident)
	{
		std::optional<std::string> to_ret = std::nullopt;

		switch (widget_type_)
		{
		case gui::ModulePageStorage::WidgetElement::WidgetEnum::TEXT:
			ImGui::TextWrapped("%s", widget_info_["text"].get<std::string>().c_str());
			break;
		case gui::ModulePageStorage::WidgetElement::WidgetEnum::BUTTON:
			if (ImGui::Button(widget_info_["text"].get<std::string>().c_str()))
				to_ret = { widget_info_["identifier"].get<std::string>() };
			break;
		case gui::ModulePageStorage::WidgetElement::WidgetEnum::SLIDERINT:
			ImGui::SliderInt(("##slider_int" + ident).c_str(), &std::get<int>(stored_value_),
				widget_info_["from"].get<int>(), widget_info_["to"].get<int>());
			break;
		case gui::ModulePageStorage::WidgetElement::WidgetEnum::SLIDERFLOAT:
			ImGui::SliderFloat(("##slider_float" + ident).c_str(), &std::get<float>(stored_value_),
				widget_info_["from"].get<float>(), widget_info_["to"].get<float>());
			break;
		case gui::ModulePageStorage::WidgetElement::WidgetEnum::INPUTTEXT:
			ImGui::InputTextWithHint(("##input_text_with_hint" + ident).c_str(), 
				widget_info_["placeholder"].get<std::string>().c_str(),
				&std::get<std::string>(stored_value_));
			break;
		case gui::ModulePageStorage::WidgetElement::WidgetEnum::INPUTINT:
			ImGui::InputInt(("##input_int" + ident).c_str(), &std::get<int>(stored_value_));
			break;
		case gui::ModulePageStorage::WidgetElement::WidgetEnum::INPUTFLOAT:
			ImGui::InputFloat(("##input_float" + ident).c_str(), &std::get<float>(stored_value_));
			break;
		case gui::ModulePageStorage::WidgetElement::WidgetEnum::CHECKBOX:
			ImGui::Checkbox(widget_info_["text"].get<std::string>().c_str(),
				&std::get<bool>(stored_value_));
			break;
		case gui::ModulePageStorage::WidgetElement::WidgetEnum::RADIOBUTTON:
		{
			auto vec = widget_info_["elements"].get<std::vector<std::string>>();
			auto& selected = std::get<std::string>(stored_value_);
			for (auto& entry : vec)
				if (ImGui::RadioButton(entry.c_str(), entry == selected))
					selected = entry;
		}
			break;
		case gui::ModulePageStorage::WidgetElement::WidgetEnum::COMBO:
		{
			auto vec = widget_info_["elements"].get<std::vector<std::string>>();
			auto& selected = std::get<std::string>(stored_value_);

			ImGui::PushItemWidth(200);
			if (ImGui::BeginCombo(("##combo" + ident).c_str(), selected.c_str()))
			{
				for (auto& entry : vec)
					if (ImGui::Selectable(entry.c_str(), entry == selected))
						stored_value_ = entry;

				ImGui::EndCombo();
			}
			ImGui::PopItemWidth();
		}
			break;
		case gui::ModulePageStorage::WidgetElement::WidgetEnum::SAMELINE:
			ImGui::SameLine();
			break;
		case gui::ModulePageStorage::WidgetElement::WidgetEnum::INVALID:
			break;
		default:
			break;
		}

		return to_ret;
	}

	std::optional<nlohmann::json> ModulePageStorage::WidgetElement::get_field()
	{
		switch (widget_type_)
		{
		case WidgetEnum::SLIDERINT:
		case WidgetEnum::INPUTINT:
			return { {{ "value", std::get<int>(stored_value_) }} };

		case WidgetEnum::CHECKBOX:
			return { {{ "value", std::get<bool>(stored_value_) }} };

		case WidgetEnum::SLIDERFLOAT:
		case WidgetEnum::INPUTFLOAT:
			return { {{ "value", std::get<float>(stored_value_) }} };

		case WidgetEnum::INPUTTEXT:
		case WidgetEnum::RADIOBUTTON:
		case WidgetEnum::COMBO:
			return { {{ "value", std::get<std::string>(stored_value_) }} };

		default:
			return std::nullopt;
		}
	}

	std::optional<std::string> ModulePageStorage::WidgetElement::get_identifier()
	{
		if (widget_info_.contains("identifier"))
			return widget_info_["identifier"].get<std::string>();
		return std::nullopt;
	}

	std::string ModulePageStorage::WidgetElement::get_WidgetType(WidgetEnum& widget_type)
	{
		switch (widget_type)
		{
		case gui::ModulePageStorage::WidgetElement::WidgetEnum::TEXT:        return "Text";
		case gui::ModulePageStorage::WidgetElement::WidgetEnum::BUTTON:		 return "Button";
		case gui::ModulePageStorage::WidgetElement::WidgetEnum::SLIDERINT:	 return "SliderInt";
		case gui::ModulePageStorage::WidgetElement::WidgetEnum::SLIDERFLOAT: return "SliderFloat";
		case gui::ModulePageStorage::WidgetElement::WidgetEnum::INPUTTEXT:	 return "InputText";
		case gui::ModulePageStorage::WidgetElement::WidgetEnum::INPUTINT:	 return "InputInt";
		case gui::ModulePageStorage::WidgetElement::WidgetEnum::INPUTFLOAT:	 return "InputFloat";
		case gui::ModulePageStorage::WidgetElement::WidgetEnum::CHECKBOX:	 return "Checkbox";
		case gui::ModulePageStorage::WidgetElement::WidgetEnum::RADIOBUTTON: return "RadioButton";
		case gui::ModulePageStorage::WidgetElement::WidgetEnum::COMBO:		 return "Combo";
		case gui::ModulePageStorage::WidgetElement::WidgetEnum::SAMELINE:	 return "Sameline";
		case gui::ModulePageStorage::WidgetElement::WidgetEnum::INVALID:	 return "Invalid";
		default:															 return "Invalid";
		}
	}

} // namespace gui
