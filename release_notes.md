# Release notes (Заметки о выпуске)

## v1.1
### Добавлено:

- команда `_NCAD_PSETS_SaveToFile`: сохранение информации по свойствам и значениям объектов в xml-файл. Сохранение пока что производится в каталог %LOCALAPPDATA%\psets_folder без создания оного (надо создать вручную), и файл называется test.xml пока без GUID;
- команда `_NCAD_PSETS_LoadFromFile`: загрузка свойств из внешего текстового файла с разделителями ",". Пример файла см. в файлах к релизу 1.1 https://github.com/GeorgGrebenyuk/NanoCAD_PropertySet_plugin/releases/tag/rel_1.1;

### Изменено:

- добавлен вспомогательный класс aux_functions, куда перенесена логика преобразований строк (std::string -> BSTR и т.д.);
- для CategorizedSingleDynProperty в метод GetCurrentValueData добавлена процедура сохранения значения в памяти (для значений по умолчанию -- при первом назначении свойства);
- в метод DynPropertiesManager::CreateSingleDynProperty добавлена проверка на существование среди кэшированных свойств с таким же GUID, и если таковое есть -- то свойство сбрасывается (prop->Release());
