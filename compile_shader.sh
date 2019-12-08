#!/bin/zsh

SOURCE_DIR="Shader"
TARGET_DIR="CompiledShader"

mkdir -p ${TARGET_DIR}

for file in "$SOURCE_DIR"/*; do
  FILENAME=${${file##*/}%.*}
  EXTENSION=${file##*.}
  if [[ "$EXTENSION" =~ ^(frag|vert)$ ]]; then
    glslangValidator -V $file -o "${TARGET_DIR}/${FILENAME}_${EXTENSION}.spv"
  fi
done
