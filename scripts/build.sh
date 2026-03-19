#!/usr/bin/env bash
set -euo pipefail

# Script for local build in workspace
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build . --parallel

# Print binary location
echo "Build complete. Binaries in: $(pwd)/bin"
