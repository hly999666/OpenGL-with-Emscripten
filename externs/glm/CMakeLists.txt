add_library(glm STATIC 
    src/glm_test.cpp
)

target_include_directories(glm
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/glm
)

add_library(lib_glm  ALIAS  glm)
