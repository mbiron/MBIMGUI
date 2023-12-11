@echo off

set __BUILD_DIR__=./build
rem set __CONFIG__=Release
set __CONFIG__=Debug

IF NOT EXIST %__BUILD_DIR__% (
	echo Configure...
	mkdir "%__BUILD_DIR__%"
	cmake -S./ -B%__BUILD_DIR__%
	)

echo ************************
echo * Build in %__CONFIG__%...
echo ************************

cmake --build %__BUILD_DIR__% --config %__CONFIG__% --target install -j 14

echo Done !
pause