EXECUTABLE_NAME=DigitRecognizer

INCLUDE_DIR=src/include
SOURCE_DIR=src

OBJ_DIR=src/obj

INCLUDE_FILES=$(wildcard $(INCLUDE_DIR)/*.hpp)
SOURCE_FILES=$(wildcard $(SOURCE_DIR)/*.cpp)

OBJ_FILES=$(patsubst $(SOURCE_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SOURCE_FILES))

CFLAGS=$(shell pkg-config --cflags opencv) -I $(INCLUDE_DIR)
LIBS=$(shell pkg-config --libs opencv)


all: directories $(EXECUTABLE_NAME)

$(EXECUTABLE_NAME): $(OBJ_FILES)
	g++ -o $@ $^ $(CFLAGS) $(LIBS)

$(OBJ_DIR)/%.o: $(SOURCE_DIR)/%.cpp $(INCLUDE_FILES)
	g++ -c -o $@ $< $(CFLAGS)


directories: $(OBJ_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -fr $(OBJ_DIR) $(EXECUTABLE_NAME)

.PHONY: all clean directories

