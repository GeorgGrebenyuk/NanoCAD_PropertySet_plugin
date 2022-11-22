# Release notes (Заметки о выпуске)

## v1.2
### Добавлено:

- команда `_NCAD_PSETS_LoadFromXmlFile`: загрузка из xml-файла информации о свойствах и значениях свойств у объектов;
- вспомогательные процедуры aux_functions добавлено ToJoinedStringFromVectorString и StringSplit;

### Изменено:

- наименование остальных команд `_NCAD_PSETS_SaveToXmlFile`, `_NCAD_PSETS_LoadPropsFromTextFile`;
- настроена команда aux_functions::GetTempXmlSavePath создающая папку модуля и формирующая файловый путь для сохранения;
- заменена логика работы CategorizedSingleDynProperty::IsPropertyEnabled, для учета объектов нужных классов (логика сравнения строк была изменена);
- для записи в xml информации по объектным классам из конца строки удален лишний разделитель;

### Удалено:

- файл `simpledynprops.vcproj` из ранних файлов, был ненужен;

## v1.1
### Добавлено:

- команда `_NCAD_PSETS_SaveToFile`: сохранение информации по свойствам и значениям объектов в xml-файл. Сохранение пока что производится в каталог %LOCALAPPDATA%\psets_folder без создания оного (надо создать вручную), и файл называется test.xml пока без GUID;
- команда `_NCAD_PSETS_LoadFromFile`: загрузка свойств из внешего текстового файла с разделителями ",". Пример файла см. в файлах к релизу 1.1 https://github.com/GeorgGrebenyuk/NanoCAD_PropertySet_plugin/releases/tag/rel_1.1;

### Изменено:

- добавлен вспомогательный класс aux_functions, куда перенесена логика преобразований строк (std::string -> BSTR и т.д.);
- для CategorizedSingleDynProperty в метод GetCurrentValueData добавлена процедура сохранения значения в памяти (для значений по умолчанию -- при первом назначении свойства);
- в метод DynPropertiesManager::CreateSingleDynProperty добавлена проверка на существование среди кэшированных свойств с таким же GUID, и если таковое есть -- то свойство сбрасывается (prop->Release());
