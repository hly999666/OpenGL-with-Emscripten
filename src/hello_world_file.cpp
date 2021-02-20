// Copyright 2012 The Emscripten Authors.  All rights reserved.
// Emscripten is available under two separate licenses, the MIT license and the
// University of Illinois/NCSA Open Source License.  Both these licenses can be
// found in the LICENSE file.

#include <stdio.h>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <fstream>
int main() {
    printf("On hello_world_file_main\n");
	std::ifstream fs; fs.open("res/preload-file/shader/basic_c6.glsl");
	std::string now_line;
	char now_t = 'n';
	
	while (std::getline(fs, now_line)) {
		std::cout << now_line << std::endl;
	}
    printf("On hello_world_file_end\n");
    return 0;
}
