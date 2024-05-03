# Deprecated, as the Makefile now runs a script that does all the hard work, while keeping things up to date.
#EMSDK_PATH :=
#EMSCRIPTEN_PATH :=
#CLANG_PATH :=
#PYTHON_PATH :=
#NODE_PATH :=

EMSDK_ENV :=

ifeq ($(OS),Windows_NT)
	EMSDK_ENV += ../emsdk_win/emsdk_env.sh
	#EMSDK_PATH += ..\\emsdk_win
	#EMSCRIPTEN_PATH += $(EMSDK_PATH)\\upstream\\emscripten
	#CLANG_PATH += $(EMSDK_PATH)\\upstream\\emscripten
	#PYTHON_PATH += $(EMSDK_PATH)\\python\\3.9.2-1_64bit
	#NODE_PATH += $(EMSDK_PATH)\\node\\14.18.2_64bit\\bin	
else
	EMSDK_ENV += ../emsdk/emsdk_env.sh
	#EMSDK_PATH += ../emsdk
	#EMSCRIPTEN_PATH += $(EMSDK_PATH)/upstream/emscripten
	#CLANG_PATH += $(EMSDK_PATH)/upstream/bin
	#PYTHON_PATH += /usr/lib/
	#NODE_PATH += $(EMSDK_PATH)/node/12.9.1_64bit/bin
endif

#export PATH := $(EMSDK_PATH):$(EMSCRIPTEN_PATH):$(CLANG_PATH):$(NODE_PATH):$(PYTHON_PATH):$(PATH)

# Defining the shell and adding necessary directories to PATH
SHELL := /bin/bash
ENV_CMD := EMSDK_QUIET=1 source $(EMSDK_ENV)


# Defining compilers
CC_WEB=em++
CC=g++


# LINKER DIRECTORIES

# Linker directories for Linux
INCLUDE_LINUX=-I../usr/include
LIBS_LINUX=-L../usr/lib

# Linker directories for Windows (32 bit)
INCLUDE_WIN32=-I../usr/include/win32
LIBS_WIN32=-L../usr/lib/win32

# Linker directories for Windows (64 bit)
INCLUDE_WIN64=-I../usr/include/win64
LIBS_WIN64=-L../usr/lib/win64

# Linker directories for Web
INCLUDE_WEB=-I../usr/include/RLweb
LIBS_WEB=-L../usr/lib/RLweb


# COMPILER FLAGS

# Compiler flags for Desktop (Should make separate ones for Linux and Windows at some point, for compatibility sake)
CFLAGS_DESKTOP=-DPLATFORM_DESKTOP -DUSE_EXTERNAL_GLFW=0 -Wall -Wnarrowing -Wno-missing-braces -s -O1

# Minimal web pages for emscripten
WEB_PAGE=--shell-file ../shell.html
WEB_PAGE_NETWORK=--shell-file ../shell_network.html

# Compiler flags for Web (Common)
CFLAGS_WEB=-D__EMSCRIPTEN__ -O2 -s USE_GLFW=3 -Wall -Wnarrowing -Wno-missing-braces --preload-file resources -s "EXPORTED_FUNCTIONS=['_main', '_malloc']"

# Deprecated
#CFLAGS_WEB_AUDIO= -s USE_SDL=2 -s ERROR_ON_UNDEFINED_SYMBOLS=0 -s ALLOW_MEMORY_GROWTH=1 

# Compiler flags for Web (Client)
CFLAGS_WEB_NET_CLIENT=--js-library "../usr/include/RLweb/webrtc/js/api.js" -s ASYNCIFY -s ASYNCIFY_IMPORTS="[\"__js_game_client_start\", \"__js_game_client_close\", \"__js_game_server_start\"]" -s ALLOW_MEMORY_GROWTH=1 -s EXIT_RUNTIME=1

# Additional compiler flags for Web (Server) (Needs POSIX threads library to work)
CFLAGS_WEB_NET_SERVER=-s USE_PTHREADS=1 -s PTHREAD_POOL_SIZE=4 -s WASM=1


# Libraries to link against
LINKS_LINUX=-lraylib -lglfw -lGL -lopenal -lm -lpthread -ldl
LINKS_WIN32=-lraylib -lglfw3 -lgdi32 -lwinmm -lopenal -lm -pthread -Wl,--subsystem,windows -static
LINKS_WIN64=-lraylib -lglfw3 -lgdi32 -lwinmm -lopenal -lm -pthread -Wl,--subsystem,windows -static
LINKS_WEB=-lraylib

# Deprecated
#LINKS_WEB_AUDIO= -lFLAC -lopusfile -lopus -lvorbisfile -lvorbis -logg -lmad -lmodplug -lEDMIDI -lADLMIDI -lOPNMIDI -lxmp -lgme -lmpg123 -lfluidlite -ltimidity_sdl2 -lSDL2_mixer_ext


# SOURCE FILES

SRC=main.cpp
# Needs some work, still early phase, will likely be updated with a wrwaylib network implementation
SRC_WEB_CLIENT=client.cpp shared.cpp
SRC_WEB_SERVER=server.cpp shared.cpp

OUTPUT_LINUX=build/linux/main.out
OUTPUT_WIN32=build/win32/main.exe
OUTPUT_WIN64=build/win64/main.exe
OUTPUT_WEB=build/web/main.html
OUTPUT_WEB_CLIENT=build/web_network/main.html
OUTPUT_WEB_SERVER=build/web_network/server.js

# NBNet bundle (Required by the Emscripten shell for networking!)
NBNET_BUNDLE=../usr/include/RLweb/nbnet/nbnet_bundle.js

# Deprecated
#ifdef AUDIO
#	LINKS_WEB += ${LINKS_WEB_AUDIO}
#	CFLAGS_WEB += ${CFLAGS_WEB_AUDIO}
#endif

web_client:
	mkdir -p build/web_network
	${CC_WEB} ${SRC_WEB_CLIENT} -o ${OUTPUT_WEB_CLIENT} ${INCLUDE_WEB} ${LIBS_WEB} ${LINKS_WEB} ${CFLAGS_WEB} ${CFLAGS_WEB_NET_CLIENT} ${WEB_PAGE_NETWORK}
	cp ${NBNET_BUNDLE} build/web_network/nbnet_bundle.js
web_server:
	mkdir -p build/web_network
	${CC_WEB} ${SRC_WEB_SERVER} -o ${OUTPUT_WEB_SERVER} ${INCLUDE_WEB} ${LIBS_WEB} ${LINKS_WEB} ${CFLAGS_WEB} ${CFLAGS_WEB_NET_CLIENT} ${CFLAGS_WEB_NET_SERVER} 
	cp ${NBNET_BUNDLE} build/web_network/nbnet_bundle.js
web:
	mkdir -p build/web
	${ENV_CMD} && ${CC_WEB} ${SRC} -o ${OUTPUT_WEB} ${INCLUDE_WEB} ${LIBS_WEB} ${LINKS_WEB} ${CFLAGS_WEB} ${WEB_PAGE}
	#cp ${WEB_PAGE} build/web/main.html
	../emsdk/upstream/emscripten/emrun build/web/main.html
linux:
	mkdir -p build/linux
	${CC} ${SRC} -o ${OUTPUT_LINUX} ${INCLUDE_LINUX} ${LIBS_LINUX} ${LINKS_LINUX} ${CFLAGS_DESKTOP}  
win64:
	mkdir -p build/win64
	${CC} ${SRC} -o ${OUTPUT_WIN64} ${INCLUDE_WIN64} ${LIBS_WIN64} ${LINKS_WIN64} ${CFLAGS_DESKTOP}  
win32:
	mkdir -p build/win32
	${CC} ${SRC} -o ${OUTPUT_WIN32} ${INCLUDE_WIN32} ${LIBS_WIN32} ${LINKS_WIN32} ${CFLAGS_DESKTOP}  
