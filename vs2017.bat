if not exist "project" mkdir "project"
cd project
cmake .. -DWITH_GOT2D_TESTS=ON -G "Visual Studio 15"
pause