#!/bin/bash

set -e

if [ $# -lt 1 ]; then
    echo "Usage: $0 <binary-path> <output-dir>"
    exit 1
fi
binary_path="$1"
output_dir="$2"

mkdir -p "$output_dir"
cp "$binary_path" "$output_dir/"

ignore_libs=\
"linux-vdso\.so"\
"|libm\.so"\
"|libgcc_s\.so"\
"|libpthread\.so"\
"|libc\.so"\
"|ld-linux[^/]*\.so"\
"|libdl\.so"
declare -A processed_libs

copy_dynamic_dependencies() {
    local target="$1"
    local basename
    local lib_path
    local resolved

    while read -r line; do
        if [[ ! "$line" =~ \=\>[[:space:]]*[^[:space:]]+ ]]; then
            continue
        fi
        [[ "$line" =~ \=\>[[:space:]]*([^[:space:]]+) ]]\
            && lib_path="${BASH_REMATCH[1]}"

        basename="$(basename "$lib_path")"
        if [[
            "$lib_path" =~ $ignore_libs
            || ! -f "$lib_path"
            || -v processed_libs["$basename"]
        ]]; then
            continue
        fi
        processed_libs["$basename"]=1

        if [[ -L "$lib_path" ]]; then
            resolved_path="$(readlink -f "$lib_path")"
            target_basename="$(basename "$resolved_path")"
            cp "$resolved_path" "$output_dir/$target_basename"
            ln -sn "$target_basename" "$output_dir/$basename"
        else
            cp "$lib_path" "$output_dir/"
        fi

        copy_dynamic_dependencies "$lib_path"
    done < <(ldd "$target")
}

copy_dynamic_dependencies "$binary_path"

for lib in "$output_dir"/*; do
    if [ ! -f "$lib" ]; then
        continue
    fi
    patchelf --set-rpath '$ORIGIN' "$lib"
done
