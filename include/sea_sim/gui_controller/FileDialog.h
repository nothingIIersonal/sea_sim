#pragma once
#include <imgui.h>
#include <imgui-SFML.h>
#include <imgui_stdlib.h>
#include <imgui_internal.h>

#include <SFML/Graphics.hpp>
#include <filesystem>
#include <algorithm>
#include <optional>

#include <sea_sim/gui_controller/functions.h>
#include <sea_sim/gui_controller/Fonts.h>


#ifdef WIN32
#include <Windows.h>

#define LIB_EXT ".dll"
#define STOP_POP_OF_PATH_AT 2
#else
#define LIB_EXT ".so"
#define STOP_POP_OF_PATH_AT 1
#endif // WIN32

namespace gui
{
	namespace fs = ::std::filesystem;

	class FileInfo
	{
	public:
		enum class FileTypeEnum
		{
			DIRECTORY = 0,
			FILE,
			INVALID
		};

		FileInfo();
		FileInfo(fs::path absolute_path_in, FileTypeEnum file_type_in);

		bool isDir();
		bool isFile();
		bool update_from_name(fs::path current_path);

		void clear();

		FileTypeEnum file_type;

		fs::path absolute_path;
		std::string name;
		std::string ext;

		std::string name_optimized;

	private:
		std::string get_short_name();

		void SelectableColor(ImU32 color);
		
	};

	class FileDialog
	{
	public:
		FileDialog(fs::path starting_path = "");

		void open(fs::path starting_path = "");
		void close();
		std::optional<std::vector<std::string>> render_dialog();
		std::optional<std::vector<std::string>> try_take_files();

		bool is_open();

		enum class SortingTypeEnum
		{
			byFileName = 0,
			byExtension
		};

	private:
		void update_current_path_tokens();
		void update_directory_content();
		void sort_directory_content_by(SortingTypeEnum type);

		std::string get_current_path();
		void set_current_path(std::string& path_str);
		void set_current_path(fs::path& path);
		void set_current_path_from_tokens();

		void new_path_update();

		std::optional<std::vector<std::string>> return_files();

		bool is_open_ = false;

		fs::path current_path;
		std::vector<std::string> drives_;
		std::vector<std::string> current_path_tokens;
		std::string selected_file_extension;
		SortingTypeEnum selected_sorting_type;

		std::vector<FileInfo> current_directory_content;
		FileInfo selected_file;
	};

	bool directory_content_sorter_name(FileInfo const& lhs, FileInfo const& rhs);
	bool directory_content_sorter_ext(FileInfo const& lhs, FileInfo const& rhs);

	std::string get_FileTypeName(FileInfo::FileTypeEnum type);
	std::string get_SortingType(FileDialog::SortingTypeEnum type);

	float get_button_width(std::string text, ImGuiStyle& style);

	std::vector<std::string> split_string_to_vector(const std::string& text, char delimiter);
	std::vector<std::string> get_drives();
} // namespace gui
