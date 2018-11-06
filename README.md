# remote-imaging-server
client usage: client.exe --imgs-path=..\..\resources\photos\ --dest-imgs-path=..\..\res\ --address=127.0.0.1 --port=7777
server usage: qt_server.exe

## Текущее состояние
*Клиент*
- считывание и обработка аргументов коммандной строки boost::program_options
- 2 потока producer/consumer чтения картинок из директории и отправки RAW-представления на сервер

*Qt-диалог сервера*
- старт/стоп сервера на 127.0.0.1:7777
- Периодический poll задач на соединении (1000 мс)
- echo обратно прочтённой информации

**TODO**
- асинхронное установление connection-a и сохранение его в пуле соединений (здесь застрял)
- в рамках каждого соединения принимать сперва id-картинки и размер => последовательное чтение отправленных частей (размер части <= размер буфера отправки)
- конкатенация данных и возврат клиенту
