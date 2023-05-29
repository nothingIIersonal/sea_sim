#pragma once
#include <filesystem>
#include <algorithm>

#include <sea_sim/gui_controller/Functions.h>
#include <sea_sim/gui_controller/Fonts.h>
#include <sea_sim/gears/channel_packet.h>



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

	class WindowStorage;
	class FileDialog;

	class FileInfo
	{
	public:
		enum class FileTypeEnum
		{
			DIRECTORY = 0,
			FILE,
			INVALID
		};

		FileInfo(FileDialog* parent);
		FileInfo(FileDialog* parent, fs::path absolute_path_in, FileTypeEnum file_type_in);

		bool isDir();
		bool isFile();
		bool update_from_name(fs::path& current_path);

		void clear();

		void set_notification(const std::string& text);

		FileTypeEnum file_type;

		fs::path absolute_path;
		std::string name;
		std::string ext;

		std::string name_optimized;

	private:
		std::string get_short_name();

		FileDialog* parent_ptr_;
	};

	class FileInfoContaier
	{
	public:
		bool contains(const FileInfo& file);

		void push(const FileInfo& file);
		void pop(const FileInfo& file);
		std::vector<FileInfo>::iterator at(const FileInfo& file);

		void swap(const FileInfo& file, int32_t position);

		std::vector<FileInfo>& get_files();

	private:
		std::vector<FileInfo> files;

	};

	class FileDialog
	{
	public:
		FileDialog(WindowStorage* parent, const fs::path& starting_path = "");
		~FileDialog();

		void open(const fs::path& starting_path = "");
		void close();
		std::optional<std::vector<std::string>> render_dialog();
		std::optional<std::vector<std::string>> try_take_files();

		void set_notification(const std::string& text);

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


		WindowStorage* parent_ptr_;

		std::optional<std::vector<std::string>> return_files();

		bool is_open_ = false;

		fs::path current_path;
		std::vector<std::string> drives_;
		std::vector<std::string> current_path_tokens;
		std::string selected_file_extension;
		SortingTypeEnum selected_sorting_type;

		std::vector<FileInfo> current_directory_content;
		FileInfoContaier selected_files;
	};

	std::string get_FileTypeName(FileInfo::FileTypeEnum type);
	std::string get_SortingType(FileDialog::SortingTypeEnum type);

	std::vector<std::string> split_string_to_vector(const std::string& text, char delimiter);
	std::vector<std::string> get_drives();
} // namespace gui
