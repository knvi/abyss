# abyss
A simple Todos List REST API written in C. (*written in 2.5h*)

uses sqlite3, [jansson](https://github.com/akheron/jansson) and [ulfius](https://github.com/babelouest/ulfius/)

to build - linux/mac/other:

```
mkdir build && cd build
cmake ..
make
./abyss
```

to build - windows :

```
mkdir build && cd build
cmake .. -G "MinGW Makefiles"
make
./abyss
```
