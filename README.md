## Очень простое облако для хранения файлов.

Хранилище представляет несколько узлов (считаем, что они друг про друга знают заранее). Пользователь может загружать файлы в систему и скачивать их.
Каждый узел хранит некоторый набор файлов в локальном репозитории, каждый файл должен храниться на двух узлах.
При отключении какого-то узла нужно создавать дополнительные копии, лишние копии — удалять. 

### Со стороны клиента :
Запуск с параметром - ip:port; Порт установлен как 2288
./client 127.0.0.1:2288
 
download filename - загрузить файл с сервера 
upload filename - загрузить файл на сервер
getlist - получить список файлов на облаке

### Со стороны сервера 
Запуск с параметрами - места для хранения файлов.
./run ~bla/dir1 ~bla/dir2 ~bla/dir3 ...

