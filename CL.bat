emcmake cmake  ..  -DLINK_FLAGS  "-O2 -s  USE_WEBGL2=2 -s USE_GLFW=3   -s WASM=1 -std=c++11" -DBUILDWASM=1

emcc main.cpp -o ./build.emscripten/index.html -s USE_WEBGL2=2 -s USE_GLFW=3 -s WASM=1 -std=c++1z --shell-file build.emscripten/shell_minimal.html

 
 emcmake cmake  ..  -DBUILDWASM=1
 mingw32-make

 emcc pong_3_3.cpp -o pong_wasm.js --std=c++11 --bind -s WASM=1 
 emcc factorial.cpp -o factorial.wasm -Oz -s EXPORTED_FUNCTIONS="[_factorial]"
 emcc string_example.cpp -o string_example.html  -s EXTRA_EXPORTED_RUNTIME_METHODS="['getValue']"


 imgui[opengl3-glad-binding,glfw-binding]:x64-windows