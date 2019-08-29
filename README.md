# Квадрокоптер

## Как запустить акселерометр

- для теста переименуйте sensortest.cpp
- `stty -F /dev/ttyUSB0 cs8 9600 ignbrk -brkint -icrnl -imaxbel -opost -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke noflsh -ixon -crtscts raw`
- переименуйте другие файлы 
- запуск
- tail -f /dev/ttyUSB0
