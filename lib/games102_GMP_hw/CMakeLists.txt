add_library(games201_hw   src_dir/games201_hw_test.cpp)

target_include_directories(games201_hw PUBLIC include_dir)
target_include_directories(games201_hw PRIVATE lib_glm)
target_include_directories(games201_hw PRIVATE lib_eigen)