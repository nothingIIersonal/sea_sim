# Разработчики:
## https://github.com/nothingIIersonal/ - работа с модулями
## https://github.com/nikikust - GUI, концепция
## https://github.com/bersen66 - каналы и взаимодействие

<br>

# Примеры сборки
## Приложение тестировалось на системах с ОС <b>Ubuntu 22.04</b> (clang 14.0.6) и <b>Windows 10</b> (Visual Studio 2022).

<br>

# Ubuntu 22.04
## Необоходимые пакеты:
```bash
apt-get update && apt-get install -y sudo build-essential git cmake curl unzip tar zip autoconf libtool pkg-config libx11-dev libxrandr-dev libxi-dev libudev-dev libgl1-mesa-dev 
```
## Получение <b>vcpkg</b>:
```bash
cd /opt/
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg/
./bootstrap-vcpkg.sh && ln -s vcpkg /bin/vcpkg
```

## Непосредственно сборка:
```bash
git clone https://github.com/nothingIIersonal/sea_sim.git
cd ./sea_sim/
./build.sh /opt/vcpkg/script/buildsystems/vcpkg.cmake 4
```

## Запуск:
```bash
cd ./.build/
./sea_sim
```

<br>

# Windows 10
### Для сборки понадобится установить <b>MSBuild</b> и сопутствующие утилиты <b>Visual Studio</b>.
### Также понадобится <b>vcpkg</b>, путь к исполняемому файлу которого следует прописать в переменные среды. После чего достаточно будет запустить скрипт <b>build.bat</b> и получить исполняемый файл в папке <b>./.build/Release/</br>