#!/bin/bash

cmake -B build-release -DCMAKE_BUILD_TYPE=Release
cmake --build build-release

tar -czvf build-release/gamepad_tester_linux_x86_64.tar.gz -C build-release gamepad_tester

echo "Done! Archive created in build-release/ folder."
