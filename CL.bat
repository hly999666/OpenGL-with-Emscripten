emcc main.cpp -o ./build.emscripten/index.html -s USE_WEBGL2=2 -s USE_GLFW=3 -s WASM=1 -std=c++1z --shell-file build.emscripten/shell_minimal.html

emcmake cmake  ..
 
 mingw32-make