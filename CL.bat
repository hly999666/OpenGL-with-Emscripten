emcmake cmake  ..  -DLINK_FLAGS  "-O2 -s  USE_WEBGL2=2 -s USE_GLFW=3   -s WASM=1 -std=c++11" -DBUILDWASM=1

emcc main.cpp -o ./build.emscripten/index.html -s USE_WEBGL2=2 -s USE_GLFW=3 -s WASM=1 -std=c++1z --shell-file build.emscripten/shell_minimal.html

 
 emcmake cmake  ..  -DBUILDWASM=1
 mingw32-make

 emcc pong_3_3.cpp -o pong_wasm.js --std=c++11 --bind -s WASM=1 
 //with lidbfs.js filesystem
  emcc pong_3_3.cpp -o pong_wasm.js --std=c++11 --bind -s WASM=1 -lidbfs.js
 emcc factorial.cpp -o factorial.wasm -Oz -s EXPORTED_FUNCTIONS="[_factorial]"
 emcc string_example.cpp -o string_example.html  -s EXTRA_EXPORTED_RUNTIME_METHODS="['getValue']"
 //with embedden package 
  emcc pong_3_3.cpp -o pong_wasm.js --std=c++17 --bind -s WASM=1 -s FETCH=1 --embed-file asset_dir 
  //with with  preload-file  package
  --preload-file  res 
  //using boost emscripten port
  USE_BOOST_HEADERS=1 -lboost_headers
 imgui[opengl3-glew-binding,glfw-binding]:x64-windows

CMAKE DCMAKE_TOOLCHAIN_FILE=P:/vcpkg/scripts/buildsystems/vcpkg.cmake  .. 
emcc ../pong.cpp -o pong_wasm.js --emrun --std=c++17 --bind -lidbfs -s   -lboost_headers -s USE_PTHREADS=1