#include <iostream>

#include <sea_sim/gui_controller/FileDialog.h>
#include <sea_sim/gui_controller/WindowStorage.h>


namespace gui
{
	namespace fs = ::std::filesystem;
	using namespace utils;

	// --- File Info

	FileInfo::FileInfo(FileDialog* parent)
		: parent_ptr_(parent)
	{
		file_type = FileTypeEnum::INVALID;

		absolute_path = "";
		name = "";
		name_optimized = "";
		ext = "";
	}
	FileInfo::FileInfo(FileDialog* parent, fs::path absolute_path_in, FileTypeEnum file_type_in)
		: parent_ptr_(parent)
	{
		file_type = file_type_in;

		absolute_path = absolute_path_in;
		name = reinterpret_cast<const char*>(absolute_path.filename().u8string().c_str());
		name_optimized = get_short_name();

		if (file_type == FileTypeEnum::FILE)
			ext = absolute_path.extension().string();
		else
			ext = "";
	}

	bool FileInfo::isDir()
	{
		return file_type == FileTypeEnum::DIRECTORY;
	}
	bool FileInfo::isFile()
	{
		return file_type == FileTypeEnum::FILE;
	}

	void FileInfo::clear()
	{
		file_type = FileTypeEnum::INVALID;

		absolute_path = "";
		name = "";
		name_optimized = "";
		ext = "";
	}

	void FileInfo::set_notification(const std::string& text)
	{
		if (parent_ptr_ == nullptr)
			return;

		parent_ptr_->set_notification(text);
	}

	std::string FileInfo::get_short_name()
	{
		switch (file_type)
		{
		case gui::FileInfo::FileTypeEnum::DIRECTORY:
			return ICON_sea_sim__FOLDER_OPEN " " + name;
		case gui::FileInfo::FileTypeEnum::FILE:
			return ICON_sea_sim__FILE_O " " + name;
		default:
			return ICON_sea_sim__FILE_O " " + name;
		}
	}

	bool FileInfo::update_from_name(fs::path& current_path)
	{
		fs::path path_buf = absolute_path;
		try
		{
			path_buf = path_buf.parent_path();

			if (path_buf.is_relative())
				path_buf = current_path.append(path_buf.string());

			path_buf.append(std::u8string(name.begin(), name.end()));


			if (!fs::exists(path_buf))
				return 0;

			path_buf = fs::canonical(path_buf);
		}
		catch (const std::filesystem::filesystem_error& err)
		{
			switch (err.code().value())
			{
			case 5:
				set_notification(u8"Отказано в доступе к: \""_C + err.path1().string() + u8"\""_C);
				break;
			default:
				set_notification(err.what());
				break;
			}
		}

		absolute_path = path_buf;

		if (fs::is_directory(absolute_path))
		{
			file_type = FileTypeEnum::DIRECTORY;
			ext = "";
		}
		else if (fs::is_regular_file(absolute_path))
		{
			file_type = FileTypeEnum::FILE;
			ext = absolute_path.extension().string();
		}
		else
		{
			file_type = FileTypeEnum::INVALID;
			ext = "";
		}

		name_optimized = get_short_name();

		return 1;
	}

	// --- File Info Container

	bool FileInfoContaier::contains(const FileInfo& file)
	{
		return std::find(files.begin(), files.end(), file) != files.end();
	}
	void FileInfoContaier::push(const FileInfo& file)
	{
		files.push_back(file);
	}
	void FileInfoContaier::pop(const FileInfo& file)
	{
		auto it = std::find(files.begin(), files.end(), file);

		if (it != files.end())
			files.erase(it);
	}

	std::vector<FileInfo>::iterator FileInfoContaier::at(const FileInfo& file)
	{
		return std::find(files.begin(), files.end(), file);
	}
	void FileInfoContaier::swap(const FileInfo& file, int32_t position)
	{
		if (position > files.size())
			files.push_back(file);
		else if (position > 0)
		{
			if (!contains(file))
				files.push_back(file);

			std::iter_swap(at(file), files.at(position));
		}
		else
			files.insert(files.begin(), file);
	}
	std::vector<FileInfo>& FileInfoContaier::get_files()
	{
		return files;
	}

	// --- File Dialog

	FileDialog::FileDialog(WindowStorage* parent, const fs::path& starting_path)
		: parent_ptr_(parent),
		  selected_file(this)
	{
		drives_ = get_drives();
		open(starting_path);
		is_open_ = false;
	}
	FileDialog::~FileDialog()
	{
		parent_ptr_ = nullptr;
	}

	void FileDialog::open(const fs::path& starting_path)
	{
		if (starting_path.empty())
			current_path = fs::current_path();
		else
			current_path = starting_path;

		selected_file.clear();
		selected_sorting_type = SortingTypeEnum::byExtension;

		update_current_path_tokens();

		update_directory_content();
		sort_directory_content_by(selected_sorting_type);

		selected_file_extension = LIB_EXT;

		is_open_ = true;
	}

	void FileDialog::close()
	{
		is_open_ = false;
	}

	std::optional<std::vector<std::string>> FileDialog::render_dialog()
	{
		if (!is_open_)
			return std::nullopt;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(400, 250));
		ImGui::SetNextWindowSize(ImVec2(800, 500), ImGuiCond_Once);

		if (ImGui::Begin(u8"Открыть файл"_C, &is_open_, ImGuiWindowFlags_NoDocking))
		{
			auto& style = ImGui::GetStyle();

			// Navigation and drives
			if (ImGui::Button(ICON_sea_sim__ARROW_UP))
			{
				if (current_path_tokens.size() > STOP_POP_OF_PATH_AT)
				{
					current_path_tokens.pop_back();
					new_path_update();

					selected_file.clear();
				}
			}

			ImGui::SameLine();

			if (ImGui::Button(ICON_sea_sim__REFRESH))
			{
				update_directory_content();
				sort_directory_content_by(selected_sorting_type);
			}

			ImGui::SameLine();

			ImGui::PushItemWidth(130);
			if (ImGui::BeginCombo("##select_sort", get_SortingType(selected_sorting_type).c_str()))
			{
				if (ImGui::Selectable(get_SortingType(SortingTypeEnum::byFileName).c_str()))
					sort_directory_content_by(SortingTypeEnum::byFileName);
				if (ImGui::Selectable(get_SortingType(SortingTypeEnum::byExtension).c_str()))
					sort_directory_content_by(SortingTypeEnum::byExtension);

				ImGui::EndCombo();
			}
			ImGui::PopItemWidth();

#ifdef WIN32
			ImGui::SameLine();
			ImGui::PushItemWidth(100);
			if (ImGui::BeginCombo("##select_drive", current_path_tokens.front().c_str()))
			{
				if (drives_.empty())
					drives_ = get_drives();

				for (auto& drive : drives_)
				{
					if (ImGui::Selectable(drive.c_str()))
					{
						set_current_path(drive);
						selected_file.clear();
					}
				}

				ImGui::EndCombo();
			}
			ImGui::PopItemWidth();
#endif
			ImGui::Separator();

			// Path as tokens
			uint32_t how_much_to_pop = 0;

			for (auto& entry : current_path_tokens)
			{
#ifdef WIN32
				if (entry == "\\")
					continue;
#endif	
				if (how_much_to_pop != 0)
					++how_much_to_pop;

				if (ImGui::GetContentRegionAvail().x < get_button_width(entry.c_str(), style) + ImGui::CalcTextSize("\\").x)
					ImGui::NewLine();

				if (ImGui::Button(entry.c_str()))
					how_much_to_pop = 1;

				ImGui::SameLine();
				ImGui::Text("\\");
				ImGui::SameLine();

			}
			ImGui::NewLine();

			if (how_much_to_pop) {
				while (how_much_to_pop-- > 1)
					current_path_tokens.pop_back();

				new_path_update();
			}

			ImGui::Separator();
			
			// Files list
			if (ImGui::BeginListBox("##directory content", {-1.f, - ImGui::GetFrameHeightWithSpacing() - 4.f})) 
			{
				ImDrawList* draw_list = ImGui::GetWindowDrawList();

				for (auto& entry : current_directory_content)
				{
					if (selected_file_extension != ".*" && entry.isFile() && entry.ext != selected_file_extension ||
						entry.name_optimized.empty() || entry.name_optimized.size() > 255)
						continue;

					draw_list->ChannelsSplit(2);
					draw_list->ChannelsSetCurrent(1);

					if (ImGui::Selectable(entry.name_optimized.c_str(), selected_file.name_optimized == entry.name_optimized))
					{
						if (selected_file.name_optimized == entry.name_optimized)
						{
							if (entry.isDir())
							{
								current_path_tokens.push_back(entry.name);
								new_path_update();
								selected_file.clear();
							}
							else
							{
								close();
								ImGui::EndListBox();
								ImGui::PopStyleVar();
								ImGui::End();
								return return_files();
							}
						}
						else
							selected_file = entry;
					}

					if (entry.ext == LIB_EXT)
					{
						draw_list->ChannelsSetCurrent(0);
						SelectableColor(IM_COL32(100.f, 140.f, 100.f, 100.f));
					}

					draw_list->ChannelsMerge();
				}

				ImGui::EndListBox();
			}

			ImGui::Separator();

			// File path
			ImGui::PushItemWidth(
				ImGui::GetContentRegionAvail().x // All space
				- get_button_width(u8"Открыть"_C, style)
				- get_button_width(u8"Отмена"_C, style)
				- 101 - style.ItemSpacing.x // Combo list with extensions
				- style.ScrollbarSize // Allign with scrollbar
			);

			ImGui::InputText("##file_input", &selected_file.name);
			ImGui::PopItemWidth();

			ImGui::SameLine();

			if (ImGui::Button(u8"Открыть"_C) && !selected_file.absolute_path.empty() && !current_directory_content.empty())
			{
				if (selected_file.update_from_name(current_path))
				{
					if (selected_file.isDir())
					{
						set_current_path(selected_file.absolute_path);
						selected_file.clear();
					}
					else
					{
						ImGui::PopStyleVar();
						ImGui::End();
						return return_files();
					}
				}
				selected_file.name = "";
			}

			ImGui::SameLine();

			if (ImGui::Button(u8"Отмена"_C))
			{
				close();
			}

			ImGui::SameLine();

			ImGui::PushItemWidth(100);
			if (ImGui::BeginCombo("##file_ext", selected_file_extension.c_str()))
			{
				if (ImGui::Selectable(".*"))
				{
					selected_file_extension = ".*";

					update_directory_content();
					sort_directory_content_by(selected_sorting_type);
				}
				if (ImGui::Selectable(LIB_EXT))
				{
					selected_file_extension = LIB_EXT;

					update_directory_content();
					sort_directory_content_by(selected_sorting_type);
				}
				ImGui::EndCombo();
			}
			ImGui::PopItemWidth();

			ImGui::End();
		}

		ImGui::PopStyleVar();

		return std::nullopt;
	}

	std::optional<std::vector<std::string>> FileDialog::try_take_files()
	{
		return return_files();
	}

	void FileDialog::set_notification(const std::string& text)
	{
		if (parent_ptr_ == nullptr)
			return;

		parent_ptr_->set_notification(text);
	}

	bool FileDialog::is_open()
	{
		return is_open_;
	}

	void FileDialog::update_current_path_tokens()
	{
		current_path_tokens.clear();
		for (const auto& entry : current_path)
			current_path_tokens.push_back(entry.string());
	}

	void FileDialog::update_directory_content()
	{
		current_directory_content.clear();

		try
		{
			for (const auto& entry : fs::directory_iterator(current_path))
			{
				fs::perms perm = fs::status(entry.path()).permissions();
				if ((perm & fs::perms::others_read) == fs::perms::none)
					continue;

				FileInfo::FileTypeEnum file_type =
					(entry.is_directory()) ? FileInfo::FileTypeEnum::DIRECTORY :
					(entry.is_regular_file()) ? FileInfo::FileTypeEnum::FILE : FileInfo::FileTypeEnum::INVALID;

				current_directory_content.push_back({ this, entry.path(), file_type });
			}
		}
		catch (const std::filesystem::filesystem_error& err)
		{
			switch (err.code().value())
			{
			case 5:
				set_notification(u8"Отказано в доступе к: \""_C + err.path1().string() + u8"\""_C);
				break;
			default:
				set_notification(err.what());
				break;
			}
		}
	}

	void FileDialog::sort_directory_content_by(SortingTypeEnum type)
	{
		update_directory_content();

		selected_sorting_type = type;

		auto directory_content_sorter_name = [](const FileInfo& lhs, const FileInfo& rhs) {
			return std::tie(lhs.file_type, lhs.name) <
			       std::tie(rhs.file_type, rhs.name); };

		auto directory_content_sorter_ext = [](const FileInfo& lhs, const FileInfo& rhs) {
			return std::tie(lhs.file_type, lhs.ext, lhs.name) <
				   std::tie(rhs.file_type, rhs.ext, rhs.name); };

		switch (selected_sorting_type)
		{
		case gui::FileDialog::SortingTypeEnum::byFileName:
			std::sort(current_directory_content.begin(), current_directory_content.end(), directory_content_sorter_name);
			break;
		case gui::FileDialog::SortingTypeEnum::byExtension:
			std::sort(current_directory_content.begin(), current_directory_content.end(), directory_content_sorter_ext);
			break;
		default:
			std::sort(current_directory_content.begin(), current_directory_content.end(), directory_content_sorter_name);
			break;
		}
	}

	std::string FileDialog::get_current_path()
	{
		return "";
	}

	void FileDialog::set_current_path(std::string& path_str)
	{
		fs::path buf = path_str;
		set_current_path(buf);
	}
	void FileDialog::set_current_path(fs::path& path)
	{
		if (path.is_relative()) {
			current_path += path;
			current_path = fs::canonical(current_path);
		}
		else
			current_path = path;

		update_current_path_tokens();

		update_directory_content();

		sort_directory_content_by(selected_sorting_type);
	}
	void FileDialog::set_current_path_from_tokens()
	{
		current_path.clear();
		for (auto& item : current_path_tokens)
			current_path.append(std::u8string(item.begin(), item.end()));
	}

	void FileDialog::new_path_update()
	{
		set_current_path_from_tokens();
		update_directory_content();
		sort_directory_content_by(selected_sorting_type);
	}

	std::optional<std::vector<std::string>> FileDialog::return_files()
	{
		if (!current_directory_content.empty() && !selected_file.absolute_path.empty() && selected_file.update_from_name(current_path))
		{
			if (selected_file.isDir())
			{
				set_current_path(selected_file.absolute_path);
				selected_file.clear();
			}
			else if (!selected_file.absolute_path.empty())
			{
				close();
				return { { selected_file.absolute_path.string() } };
			}
		}
		return std::nullopt;
	}

	// --- Additional functions

	std::string get_FileTypeName(FileInfo::FileTypeEnum type)
	{
		switch (type)
		{
		case FileInfo::FileTypeEnum::DIRECTORY: return "DIR";
		case FileInfo::FileTypeEnum::FILE:      return "FILE";
		case FileInfo::FileTypeEnum::INVALID:   return "INV";
		default:                                return "ERR";
		}
	}
	std::string get_SortingType(FileDialog::SortingTypeEnum type)
	{
		switch (type)
		{
		case FileDialog::SortingTypeEnum::byExtension: return u8"по типу"_C;
		case FileDialog::SortingTypeEnum::byFileName:  return u8"по имени"_C;
		default:								       return "ERR";
		}
	}

	std::vector<std::string> split_string_to_vector(const std::string& text, char delimiter)
	{
		std::vector<std::string> arr;

		if (!text.empty())
		{
			std::string::size_type start = 0;
			std::string::size_type end = text.find(delimiter, start);
			while (end != std::string::npos)
			{
				std::string token = text.substr(start, end - start);
				if (!token.empty())
					arr.push_back(token);
				start = end + 1;
				end = text.find(delimiter, start);
			}
			std::string token = text.substr(start);
			if (!token.empty())
				arr.push_back(token);
		}
		return arr;
	}
	std::vector<std::string> get_drives()
	{
		std::vector<std::string> res;
#ifdef WIN32
		const DWORD mydrives = 2048;
		char lpBuffer[2048];
#define mini(a,b) (((a) < (b)) ? (a) : (b))
		const DWORD countChars = mini(GetLogicalDriveStringsA(mydrives, lpBuffer), 2047);
#undef mini
		if (countChars > 0U && countChars < 2049U)
		{
			std::string var = std::string(lpBuffer, (size_t)countChars);

			res = split_string_to_vector(var, '\0');
		}
#endif // WIN32
		return res;
	}
	
} // namespace gui
