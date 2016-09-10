# delete the default suffixes (disable implicit rules)
.SUFFIXES:
# phony targets
.PHONY: all clean

# directories
BASE_DIR	:= ..
SRC_DIR		:= $(BASE_DIR)/Sources
KORE_DIR	:= $(BASE_DIR)/Kore
BUILD_DIR	:= $(BASE_DIR)/build

# sources and associated includes
SOURCES 	:= $(shell find $(SRC_DIR) -type f -name '*.cpp')
INCLUDES	:= -I$(KORE_DIR)/Sources -I$(KORE_DIR)/Backends/Linux/Sources -I$(KORE_DIR)/Backends/OpenGL2/Sources
SOURCES		+= $(shell find $(KORE_DIR)/Sources -type f -name '*.cpp')
SOURCES		+= $(shell find $(KORE_DIR)/Backends/Linux/Sources -type f -name '*.cpp')
SOURCES		+= $(shell find $(KORE_DIR)/Backends/OpenGL2/Sources -type f -name '*.cpp')

# build files
OBJECTS		:= $(patsubst $(BASE_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SOURCES))
DEPENDS		:= $(patsubst $(BASE_DIR)/%.cpp, $(BUILD_DIR)/%.d, $(SOURCES))
BINARY		:= run

# shaders
PRE_SHADERS	:= $(shell find $(SRC_DIR) -type f -name '*.glsl')
SHADERS		:= $(patsubst $(SRC_DIR)/%.glsl, %, $(PRE_SHADERS))

# compiler and linker configuration
KRAFIX		:= $(KORE_DIR)/Tools/krafix/krafix-linux$(shell getconf LONG_BIT)
CC			:= clang++
LIBS		:= -pthread -lGL -lX11 -lasound -ldl
CFLAGS		:= -Wall -DSYS_LINUX -DOPENGL -DSYS_UNIXOID -std=c++11 -MMD -MP

# build all
all: $(OBJECTS) $(SHADERS)
	$(CC) $(LIBS) $(OBJECTS) -o $(BINARY)

# generate fragment shaders and apply a fix
%.frag: $(SRC_DIR)/%.frag.glsl
	$(KRAFIX) glsl $< $@ $(BUILD_DIR) linux
	sed -i -e 's/version 100/version 110/g' $@

# generate vertex shaders and apply a fix
%.vert: $(SRC_DIR)/%.vert.glsl
	$(KRAFIX) glsl $< $@ $(BUILD_DIR) linux
	sed -i -e 's/version 100/version 110/g' $@

# include dependencies to detect header file changes
-include $(DEPENDS)

# create object files along with dependency files in BUILDIR
$(BUILD_DIR)/%.o: $(BASE_DIR)/%.cpp
	mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@

# remove BUILDIR, generated shader files and the executable
clean:
	@rm -rf $(BUILD_DIR)
	@rm -rf $(SHADERS)
	@rm -rf $(BINARY)
