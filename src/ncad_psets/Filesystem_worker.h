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
		const char* root_dir = "C:\\ProgramData\\TBS";
		if (!fs::is_directory(root_dir))fs::create_directory(root_dir);

		const char* plugin_dir = "C:\\ProgramData\\TBS\\ncad_psets";
		const char* plugin_dir_temp = "C:\\ProgramData\\TBS\\ncad_psets\\_temp";
		const char* plugin_dir_save = "C:\\ProgramData\\TBS\\ncad_psets\\saves";
		if (!fs::is_directory(plugin_dir))fs::create_directory(plugin_dir);
		if (!fs::is_directory(plugin_dir_temp))fs::create_directory(plugin_dir_temp);
		if (!fs::is_directory(plugin_dir_save))fs::create_directory(plugin_dir_save);

	}
	/*Путь для сохранения файла со свойствами; только во временной папке с последующим
	 перемещением */
	static std::string GetSavePath() 
	{
		/*Формирует файл в папке _temp c FingerprintGuid базы данных чертежа*/
		std::string drawing_path = get_file_path_for_drawing(false);
		stringstream ss;
		ss << drawing_path;
		/*Create an unique name for file*/
		
		ss << "\\" << aux_functions::NewGuidToCharString(true) << ".xml";
		//ss << ".xml";
		return ss.str();
	}
	static std::string GetFilePathForLoading() 
	{
		auto pCurDoc_id_temp = aux_functions::get_doc_id();

		//DynPropertiesManager
		std::string need_path = "";
		stringstream ss;
		ss << get_path_to_temp_folder() << "\\saves";
		std::string directory_find_path = ss.str();

		std::string pCurDoc_id_temp_str = aux_functions::ToStringFromWString(pCurDoc_id_temp, ru_loc);

		/*В папке saves перебираем подкаталоги с именем FingerprintGuid 
		и из них загружаем последний по дате изменения файл*/
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
				if (folder_name == pCurDoc_id_temp_str)
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
	static std::string encoding_convert(const char* pszCode)
	{
		BSTR    bstrWide;
		char* pszAnsi;
		int     nLength;
		//const char* pszCode = doc_data;

		nLength = MultiByteToWideChar(CP_UTF8, 0, pszCode, strlen(pszCode) + 1, NULL, NULL);
		bstrWide = SysAllocStringLen(NULL, nLength);

		MultiByteToWideChar(CP_UTF8, 0, pszCode, strlen(pszCode) + 1, bstrWide, nLength);

		nLength = WideCharToMultiByte(CP_ACP, 0, bstrWide, -1, NULL, 0, NULL, NULL);
		pszAnsi = new char[nLength];

		WideCharToMultiByte(CP_ACP, 0, bstrWide, -1, pszAnsi, nLength, NULL, NULL);
		SysFreeString(bstrWide);

		string r(pszAnsi);
		delete[] pszAnsi;
		return r;
	}
	static void encoding_file_convert(const char* source_file_path)
	{
		std::ifstream in(source_file_path);
		std::stringstream buffer;
		buffer << in.rdbuf();
		in.close();

		std::string re_encoded_data = encoding_convert(buffer.str().c_str());
		fs::remove_all(source_file_path);

		//fs::path new_save_path = fs::temp_directory_path();
		std::ofstream out;
		out.open(source_file_path);
		if (out.is_open())
		{
			out << re_encoded_data << std::endl;
		}
		out.close();
	}

private:
	/*Получение файлового пути до psets_folder и создание её при отсутствии таковой*/
	static std::string get_path_to_temp_folder() {
		return  "C:\\ProgramData\\TBS\\ncad_psets";
	}
	/*
		Если данная функция вызывается в контексте AcApDocManagerReactor, то
		 запрашиваются указатели на документы*/
	/**\brief 
	* \param[in] is_for_save True: _temp; False: saves
	*/
	static std::string get_file_path_for_drawing(bool is_for_save) 
	{
		auto pCurDoc_id_temp = aux_functions::ToStringFromWString(aux_functions::get_doc_id(), en_loc);
		stringstream ss;
		//путь = файл
		if (is_for_save) ss << get_path_to_temp_folder() << "\\_temp\\" << pCurDoc_id_temp;
		//путь = папка
		else 
		{
			ss << get_path_to_temp_folder() << "\\saves\\" << pCurDoc_id_temp;
			if (!fs::is_directory(ss.str()))
			{
				fs::create_directory(ss.str());
			}
		}

		return ss.str();
	}
	
};
#endif __FILESYSTEM_WORKER_HPP_