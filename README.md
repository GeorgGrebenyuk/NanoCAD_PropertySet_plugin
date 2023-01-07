# NanoCAD_PropertySet_plugin
[В разработке!].Встраиваемый плагин на NRX (C++) под платформу NanoCAD для добавления пользовательских свойств в модель к объектам.

# Описание
## Функциональные возможности
Плагин (загружаемый nrx-модуль) позволяет создавать со стороны Пользователя новые объектные свойства (так называемые "динамические свойства"), которые становятся доступными у объектов модели. Эти свойства могут быть редактируемыми, но (пока) недоступными из других языков программирования и подгруженных плагинов не на NRX.

На текущий момент, поддерживаются только 3 типа данных: строки, действительные числа и целые числа. Далее планируется также ввести поддержку BOOLEAN и списков.

Импорт новых определений может осуществляться при помощи команды `_NCAD_PSETS_LoadPropsFromTextFile`. Пока что это единственный вариант (кроме команды `_NCAD_PSETS_LoadFromXmlFile`, импортирующей их из xml-файла)

## Логика работы с модулем

При работе над моделью необходимо после ее окончания (перед закрытием файла) делать выгрузку свойств и значений во внешний файл (команда `_NCAD_PSETS_SaveToXmlFile`), и соответственно - перед началом работы -- импортировать свойства и значения из файла (команда `_NCAD_PSETS_LoadFromXmlFile`) либо делать с "нуля". 

## Команды модуля:

`_NCAD_PSETS_LoadPropsFromTextFile`: загрузка новых свойств из внешнего текстового файла и регистрация их в модели при условии, что их GUID'ы не совпадают с уже имеющимися в модели;

`_NCAD_PSETS_SaveToXmlFile`: сохранение в xml-файл (по умолчанию в директорию `%LOCALAPPDATA%\psets_folder`) информации по свойствам и значениям объектов модели по этим свойстам (через handle);

`_NCAD_PSETS_LoadFromXmlFile`: загрузка из xml-файла информации о свойствах и значениях свойств у объектов; файл импорта **обязательно** должен быть пересохранен в кодировку ANSI (удаобнее это сделать например через Notepad++), так как при чтении через tinyxml2 русские символы почему-то читаются вместо utf-8 в коридровке Windows-1251;

## Демо-видео:

https://disk.yandex.ru/i/SEc8gZDemmRjqg

## Обновления модуля

[См. файл](https://github.com/GeorgGrebenyuk/NanoCAD_PropertySet_plugin/blob/main/release_notes.md)

# Внешние зависимости
- NRX SDK (ООО "Нанософт разработка");
- tinyxml2 (через vcpkg);
# Дальнейшая работа (TODO)

- расширение числа поддерживаемых типов данных (как минимум, BOOLEAN и списки);
- добавление оконных форм (обозреватель свойств, окно редактирования и добавления новых свойств), на QT;
