#!/bin/bash

set -eu
cd "$(dirname "$0")"

# --- Argument Unpacking ---
for arg in "$@"; do declare $arg='1'; done


# --- Address Sanitizer ---
cc_sanitize=""
if [[ "${asan:-0}" == "1" ]]; then
	echo "[asan enabled]"
	cc_sanitize="-fsanitize=address"
fi

# --- Comipile/Link Line Defs ---
cc_cflags_gcc=""
cc_cflags_clang=${cc_sanitize}"-fdiagnostics-absolute-paths -Wno-for-loop-analysis -Wno-incompatible-pointer-types-discards-qualifiers -Wno-initializer-overrides -Wno-compare-distinct-pointer-types -Wno-unknown-warning-option -msse2"
cc_common="-mcx16 -I../src/ -D_GNU_SOURCE -g -Wall -Wno-missing-braces -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-value -Wno-unused-function -D_USE_MATH_DEFINES"
cc_debug="-g -O0 -DBUILD_DEBUG=1 ${cc_common}"
cc_release="-g -O2 -DBUILD_DEBUG=1 ${cc_common}"
cc_link="-lpthread -lm -lrt -ldl"
cc_link_dll="-fPIC"


# --- Temporary Libs --- 
cc_raylib="-lraylib"
cc_sdl="-lSDL3"
cc_glfw="-lglfw"
cc_glew="-lGLEW"

# --- External libraries ---
if [[ -x "$(command -v pkg-config)" ]]; then
	cc_font_provider="$(pkg-config --cflags --libs freetype2)"
	cc_os_gfx="$(pkg-config --cflags --libs x11 xext)"
	cc_render="$(pkg-config --cflags --libs gl egl)"
else
	cc_font_provider="-I/usr/include/freetype2 -lfreetype"
	cc_os_gfx="-lX11 -lXext -lXfixes -lXrandr"
	cc_render="-lGL -lEGL"
fi


# --- Choose Compile/Link Lines ---

# Select Compiler (Clang Default)
if	 [[ "${gcc:-0}" == "1" ]]; then compiler="${CC:-gcc} $cc_cflags_gcc"; ar="${AR:-ar}";		echo "[gcc compile]";
elif [[ "${clang:-1}" == "1" ]]; then compiler="${CC:-clang} $cc_cflags_clang"; ar="${AR:-llvl-ar}"; echo "[clang_compile]";
fi

# Select Release or Debug (Debug Default)
if	 [[ "${release:-0}" == "1" ]]; then echo "[release mode]"; compile="$compiler $cc_release";
elif [[ "${debug:-1}" == "1" ]]; then echo "[debug mode]"; compile="$compiler $cc_debug";
fi

mkdir -p build

# --- Build & Run Metaprogram ---
# TODO(seppo): Get this working

# cd build
# if [[ ! -f metagen ]]; then meta='1'; fi
# if [[ "${meta:-0}" == "1" ]]; then
#     echo "[building metagen]"
#     $compiler $cc_debug ../src/metagen/metagen_main.c $cc_link -o metagen
# fi
# if [[ ! -v no_meta ]]; then
#     ./metagen
# fi
# cd ..



# --- Targets ---

# Usage: ./build.sh {target} {mode} {optional_params}

# Build Targets 
cd build

# Metaprogram

if [[ "${metagen:-0}" == "1" ]]; then didbuild=1 && $compiler $cc_debug ../src/metagen/metagen_main.c $cc_link -o my_metagen; fi

# Engine Assignments
if [[ "${week1:-0}"				== "1" ]]; then didbuild=1 && $compile ../src/engine_course/week1/week1_main.c $cc_link $cc_glfw $cc_glew $cc_render -o week1; fi

if [[ "${dmb:-0}"					== "1" ]]; then didbuild=1 && $compile ../src/dmb/dmb_main.c $cc_link -o dmb; fi
if [[ "${mudapp:-0}"			== "1" ]]; then didbuild=1 && $compile ../src/mud/mud_main.c $cc_link $cc_render $cc_raylib $cc_font_provider -o mudapp; fi
if [[ "${engine:-0}"			== "1" ]]; then didbuild=1 && $compile ../src/main.c $cc_link $cc_os_gfx $cc_render $cc_font_provider -o engine; fi
if [[ "${base_test:-0}"		== "1" ]]; then didbuild=1 && $compile ../src/tests/base_test.c $cc_link $cc_font_provider -o base_layer; fi
if [[ "${x11:-0}" 				== "1" ]]; then didbuild=1 && $compile ../src/x11/x11_main.c $cc_link $cc_os_gfx $cc_font_provider -o x11; fi
if [[ "${sdl:-0}" 				== "1" ]]; then didbuild=1 && $compile ../src/gfx/SDL/sdl_main.c $cc_link $cc_os_gfx $cc_font_provider -o sdl; fi
cd ..

# --- Warning on no builds ---
if [[ "${didbuild:-0}" == "0" ]]
then
	echo "[WARNING] no build target specified"
	exit 1
fi

# --- Info on no build mode ---
if [[ ! -v debug && ! -v release ]]
then
	echo "[INFO] Debug build complete. no build type specified defaulted to debug build"
fi
