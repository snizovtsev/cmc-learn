ФИО: Низовцев Сергей Николевич
Группа: 322

Задание: 2 / Выделение пешеходов
Подавление немаксимумов [+]
Bootstrapping  [+]
Нелинейный классификатор [+]
Скользящий контроль [+]

Система: Qt Creator 2.3.1
ОС: Linux (Ubuntu 11.10)
Аппаратура: Intel Core 2 Duo T5870, 2GB RAM

Комментарии:

Обучение: learn <idl файл> <файл модели>

Детектирование: predict <файл модели> <файлы или директории> ...
	* Можно указать опцию --label для рисования зелёных рамок в файлах *-labeled.png
	* Для сохранения результата используйте перенаправление	stdout коммандной оболочки.

Оценка качества: rate <idl файл ответов> [результат predict]

Примеры:
	predict model.txt tests > result.txt
	rate tests/test-processed.idl results.txt
	predict model.txt tests/*.png | rate tests/test-processed.idl
	predict model.txt --label tests	
