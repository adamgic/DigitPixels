set -euo pipefail

cmake -S . -B build/debug -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
  -DBUILD_TESTING=ON \
  -DOpenCV_DIR="$(brew --prefix opencv@4)/lib/cmake/opencv4"
cmake --build build/debug --parallel 2
#ctest --test-dir build/debug -N
ctest --test-dir build/debug --output-on-failure -V
