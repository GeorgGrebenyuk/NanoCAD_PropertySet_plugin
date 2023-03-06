#ifndef __FILESYSTEM_WORKER_HPP_
#define __FILESYSTEM_WORKER_HPP_
#include "stdafx.h"
#include "string"
#include <AtlBase.h>
#include <atlconv.h>
#include <windows.h>
#include <Lmcons.h>
#include <sstream>
#include "aux_functions.h"
#include "DynPropertiesManager.hpp"

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

class Filesystem_worker {
	//https://medium.com/@sshambir/привет-std-filesystem-4c7ed50d5634
public:
	static void initialize() {
		/*Getting file path to psets_folder*/
		stringstream ss;
		ss << get_path_to_temp_folder();
		std::string t_path_to_temp_folder = ss.str();
		//check existense of folder "saves" and "_temp"
		ss.str("");
		ss << t_path_to_temp_folder << "\\_temp";
		const char* path_temp = ss.str().c_str();
		if (!fs::is_directory(path_temp))fs::create_directory(path_temp);
		ss.str("");
		ss << t_path_to_temp_folder << "\\saves";
		const char* path_saves = ss.str().c_str();
		if (!fs::is_directory(path_saves))fs::create_directory(path_saves);
		ss.str("");
	}
	/*Путь для сохранения файла со свойствами; только во временной папке с последующим
	 перемещением */
	static std::string GetSavePath() 
	{
		/*Формирует файл в папке _temp как вложенная папка с именем проекта и внутри имя файла как guid*/
		std::string drawing_path = get_file_path_for_drawing(false);
		stringstream ss;
		ss << drawing_path;
		/*Create an unique name for file*/
		GUID guid3;
		HRESULT hr1 = CoCreateGuid(&guid3);
		std::string guid_new_str = aux_functions::ToStringFromGuid(guid3);
		guid_new_str = guid_new_str.replace(guid_new_str.find("{"), 1, "");
		guid_new_str = guid_new_str.replace(guid_new_str.find("}"), 1, "");
		ss << "\\" << guid_new_str << ".xml";
		return ss.str();
	}
	static std::string GetFilePathForLoading() 
	{
		std::string s_pCurDoc_title = DynPropertiesManager::pDoc_name;
		//DynPropertiesManager
		std::string need_path = "";
		stringstream ss;
		ss << get_path_to_temp_folder() << "\\saves";
		std::string directory_find_path = ss.str();

		//std::wstring w_pCurDoc_title(pDoc->docTitle());
		//std::string s_pCurDoc_title = aux_functions::ToStringFromWString(w_pCurDoc_title, ru_loc);

		for (const auto& entry : fs::directory_iterator(directory_find_path))
		{
			if (fs::is_directory(entry.path()))
			{
				ss.clear();
				//auto dir_entry = fs::directory_entry(entry.path());
				ss << entry.path().c_str();
				size_t index_last_found = ss.str().find_last_of("/\\");
				auto folder_name = ss.str().substr(0, index_last_found);
				ss.clear();
				if (folder_name == s_pCurDoc_title)
				{
					//std::map<unsigned long, std::string>files_info;
					std::chrono::system_clock::time_point that_time = std::chrono::system_clock::now();
					unsigned long selected_time = 0;
					for (const auto& sub_entry : fs::directory_iterator(entry.path()))
					{
						/*TODO: Check smth if this a file?*/
						ss.clear();
						ss << sub_entry.path().c_str();
						auto file_time = fs::last_write_time(ss.str()).time_since_epoch();
						unsigned long seconds_since_epoch =
							std::chrono::duration_cast<std::chrono::seconds>
							(file_time).count();
						if (selected_time == 0)
						{
							selected_time = seconds_since_epoch;
							need_path = ss.str();
						}
						else {
							if (seconds_since_epoch < selected_time) need_path = ss.str();
						}
						//files_info.insert(std::make_pair(seconds_since_epoch, ss.str()));
					}
					break;
					
				}
			}
		}

		return need_path;
	}
	
private:
	/*Получение файлового пути до psets_folder и создание её при отсутствии таковой*/
	static std::string get_path_to_temp_folder() {
		stringstream ss;
		TCHAR username[UNLEN + 1];
		DWORD size = UNLEN + 1;
		GetUserName((TCHAR*)username, &size);
		std::string s_username = aux_functions::ToStringFromWString(username, std::locale("ru_RU.UTF-8"));

		ss << "C:\\Users\\" << s_username << "\\AppData\\Roaming\\psets_folder";

		if (!fs::is_directory(ss.str()))fs::create_directory(ss.str());
		return ss.str();
	}
	/*	is for saving: including to part "_temp"
		is for loading: including to part "saves"
		
		Если данная функция вызывается в контексте AcApDocManagerReactor, то
		 запрашиваются указатели на документы*/
	static std::string get_file_path_for_drawing(bool is_for_save) 
	{
		AcApDocument* doc = acDocManager->curDocument();
		//if (DynPropertiesManager::status_eventhandler_work) doc = DynPropertiesManager::pDoc_pointer;
		//else doc = acDocManager->curDocument();

		std::wstring w_pCurDoc_title(doc->docTitle());
		std::string s_pCurDoc_title = aux_functions::ToStringFromWString(w_pCurDoc_title, ru_loc);
		stringstream ss;
		if (is_for_save) ss << get_path_to_temp_folder() << "\\_temp"; //<< s_pCurDoc_title;
		else ss << get_path_to_temp_folder() << "\\saves";// << s_pCurDoc_title;
		
		/*Create directory for drawing's name*/
		if (!fs::is_directory(ss.str()))
		{
			fs::create_directory(ss.str());
		}
		return ss.str();
	}
};
#endif __FILESYSTEM_WORKER_HPP_