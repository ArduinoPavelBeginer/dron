# Dron Квадрокоптер

## Как запустить акселерометр

- stty -F /dev/ttyUSB0 cs8 9600 ignbrk -brkint -icrnl -imaxbel -opost -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke noflsh -ixon -crtscts raw
- su 
- chmod a+rw /dev/ttyUSB0
- запуск
- tail -f /dev/ttyUSB0
