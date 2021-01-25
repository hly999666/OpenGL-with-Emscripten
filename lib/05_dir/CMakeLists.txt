add_library(Operator_OverLoading src_dir/Optr_ol.cpp)
 
#close compiler opoptimization  
 
 
set(CMAKE_CXX_FLAGS "-std=c++11 -O0 -fno-elide-constructors ${CMAKE_CXX_FLAGS}")

target_include_directories(Operator_OverLoading PUBLIC include_dir)