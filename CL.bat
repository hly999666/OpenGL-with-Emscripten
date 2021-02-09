emcmake cmake  ..  -DLINK_FLAGS  "-O2 -s  USE_WEBGL2=2 -s USE_GLFW=3   -s WASM=1 -std=c++11"

emcc main.cpp -o ./build.emscripten/index.html -s USE_WEBGL2=2 -s USE_GLFW=3 -s WASM=1 -std=c++1z --shell-file build.emscripten/shell_minimal.html

 
 
 mingw32-make

 emcc pong.cpp -o pong_wasm.js --std=c++11 --bind -s WASM=1 