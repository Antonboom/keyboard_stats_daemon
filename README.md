##Задача 
Реализовать демон, снимающий статистику с клавиатуры и записывающий ее в файл. Например, частоту нажатий клавиш за период в 5 минут.
 - события с клавиатуры можно брать тут: /dev/input/eventX (или удобнее: /dev/input/by-path/… - там будет понятно, что из этого клавиатура)
 - описание событий: https://www.kernel.org/doc/Documentation/input/input.txt
 - демон реализовать двумя способами: самостоятельно через fork() и через systemd
 - про первый способ посмотреть можно тут:http://www.netzmafia.de/skripten/unix/linux-daemon-howto.html
 - про второй: https://www.freedesktop.org/software/systemd/man/systemd.service.html
 
### Вариант через fork

```bash
gcc keyboard_stats_daemon1.c -o keyboard_daemon.out
sudo ./keyboard_daemon.out
```
 
```
cat /var/log/keyboard.stats
```
```
TOTAL PRESS COUNT: 31
KEY		PRESS_COUNT		PERCENT
KEY_BACKSPACE       		13		41.94
KEY_ENTER           		4		12.90
KEY_S               		3		9.68
KEY_D               		3		9.68
KEY_F               		3		9.68
KEY_UP              		5		16.13
```

```
cat /var/log/keyboard_stats_daemon.log
```
```
Key press | KEY_F           | Time: 1479384254.408482, Type: 1, Code: 33, Value: 1
Key press | KEY_BACKSPACE   | Time: 1479384254.595177, Type: 1, Code: 14, Value: 1
Key press | KEY_F           | Time: 1479384254.708307, Type: 1, Code: 33, Value: 1
Key press | KEY_G           | Time: 1479384254.713506, Type: 1, Code: 34, Value: 1
Key press | KEY_BACKSPACE   | Time: 1479384254.734747, Type: 1, Code: 14, Value: 1
Key press | KEY_BACKSPACE   | Time: 1479384254.889798, Type: 1, Code: 14, Value: 1
Key press | KEY_R           | Time: 1479384254.900347, Type: 1, Code: 19, Value: 1
Key press | KEY_BACKSPACE   | Time: 1479384255.37673, Type: 1, Code: 14, Value: 1
```

```
sudo killall keyboard_daemon.out
 
