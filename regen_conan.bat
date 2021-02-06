IF NOT EXIST .\build mkdir build 
cd build & conan install .. --profile ../vs2019 & mkdir bin & cmake .. & cd ..
