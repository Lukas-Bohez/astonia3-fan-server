#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
cd "$ROOT_DIR"

BUILD_DIR=${1:-build}
RELEASE_NAME=${2:-astonia3-fan-server}
TS=$(date +%Y%m%d%H%M%S)
RELEASE_DIR="release/${RELEASE_NAME}-${TS}"

mkdir -p "$BUILD_DIR"
mkdir -p "$(dirname "$RELEASE_DIR")"

# configure and build
cmake -S . -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release -DUSE_ARGON2=ON
cmake --build "$BUILD_DIR" --parallel

# prepare release bundle
mkdir -p "$RELEASE_DIR"
cp -v "$BUILD_DIR/bin/astonia3-server" "$RELEASE_DIR/" 2>/dev/null || true
cp -v "$BUILD_DIR/bin/chatserver" "$RELEASE_DIR/" 2>/dev/null || true
cp -v "$BUILD_DIR/bin/create_account" "$RELEASE_DIR/" 2>/dev/null || true
cp -v "$BUILD_DIR/bin/create_character" "$RELEASE_DIR/" 2>/dev/null || true

mkdir -p "$RELEASE_DIR/config"
cp -v config/server.conf.example "$RELEASE_DIR/config/server.conf.example"

# copy runtime files you may need (zone files etc are huge; user can mount from source)
cp -v README.md "$RELEASE_DIR/"
cp -v todo.md "$RELEASE_DIR/"

# bundle into archive
ARCHIVE="release/${RELEASE_NAME}-${TS}.tar.gz"
tar -czf "$ARCHIVE" -C "release" "${RELEASE_NAME}-${TS}"

cat <<EOF
Release package created: $ARCHIVE
Extract and run with:
  tar -xzf "$ARCHIVE" -C /path/to/deploy
  ./astonia3-server --config config/server.conf
EOF
