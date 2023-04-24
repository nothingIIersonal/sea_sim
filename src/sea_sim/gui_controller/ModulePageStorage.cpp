#include <iostream>

#include <sea_sim/gui_controller/ModulePageStorage.h>

namespace gui
{
	uint32_t ModulePageStorage::gl_id_ = 0;

	ModulePageStorage::ModulePageStorage()
	{
		module_title_ = "Unnamed " + std::to_string(gl_id_++);
	}

	void ModulePageStorage::set_input_interface(nlohmann::json input_interface)
	{
		input_interface_ = input_interface;
		input_widgets_vector_.clear();
		
		for (auto& widget : input_interface_)
		{
			if (widget["type"].get<std::string>() == "module_title")
				module_title_ = widget["widget"]["title"].get<std::string>();
			else
				input_widgets_vector_.push_back(WidgetElement{ widget });
		}
	}

	void ModulePageStorage::set_output_interface(nlohmann::json output_interface)
	{
		output_interface_ = output_interface;
		output_widgets_vector_.clear();

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
		{
			if (auto field = widget.get_field())
			{
				for (auto it = field.value().begin(); it != field.value().end(); ++it)
				{
					fields[it.key()] = it.value();
				}
			}
		}

		return fields;
	}

	std::string& ModulePageStorage::get_title()
	{
		return module_title_;
	}

	ModulePageStorage::WidgetElement::WidgetElement(nlohmann::json widget)
	{
		auto type = widget["type"].get<std::string>();
		widget_info_ = widget["widget"].get<nlohmann::json>();

		if (type == "text")
			{ widget_type_ = WidgetEnum::TEXT;        stored_value_ = 0;     }
		else if (type == "button")
			{ widget_type_ = WidgetEnum::BUTTON;      stored_value_ = 0;     }
		else if (type == "sliderint")
			{ widget_type_ = WidgetEnum::SLIDERINT;   stored_value_ = 0;     }
		else if (type == "sliderfloat")
			{ widget_type_ = WidgetEnum::SLIDERFLOAT; stored_value_ = 0.f;   }
		else if (type == "inputtext")
			{ widget_type_ = WidgetEnum::INPUTTEXT;   stored_value_ = "";    }
		else if (type == "inputint")
			{ widget_type_ = WidgetEnum::INPUTINT;    stored_value_ = 0;     }
		else if (type == "inputfloat")
			{ widget_type_ = WidgetEnum::INPUTFLOAT;  stored_value_ = 0.f;   }
		else if (type == "checkbox")
			{ widget_type_ = WidgetEnum::CHECKBOX;    stored_value_ = false; }
		else if (type == "radiobutton")
			{ widget_type_ = WidgetEnum::RADIOBUTTON; stored_value_ = "";    }
		else if (type == "dropdownlist")
			{ widget_type_ = WidgetEnum::COMBO;       stored_value_ = "";    }
		else if (type == "sameline")
			{ widget_type_ = WidgetEnum::SAMELINE;    stored_value_ = 0;     }
		else
			{ widget_type_ = WidgetEnum::INVALID;     stored_value_ = 0;     }
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

			if (ImGui::BeginCombo(("##combo" + ident).c_str(), selected.c_str()))
			{
				for (auto& entry : vec)
					if (ImGui::Selectable(entry.c_str(), entry == selected))
						stored_value_ = entry;

				ImGui::EndCombo();
			}
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
			return { {{ widget_info_["identifier"].get<std::string>(), std::get<int>(stored_value_) }} };

		case WidgetEnum::SLIDERFLOAT:
		case WidgetEnum::INPUTFLOAT:
			return { {{ widget_info_["identifier"].get<std::string>(), std::get<float>(stored_value_) }} };

		case WidgetEnum::INPUTTEXT:
		case WidgetEnum::CHECKBOX:
		case WidgetEnum::RADIOBUTTON:
		case WidgetEnum::COMBO:
			return { {{ widget_info_["identifier"].get<std::string>(), std::get<std::string>(stored_value_) }} };

		default:
			return std::nullopt;
		}
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
