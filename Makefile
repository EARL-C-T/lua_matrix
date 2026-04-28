CC= gcc
INC= -I/usr/include/lua5.5/ -I/home/et/Desktop/lua_matrix/inc
CFLAGS = -Wall -O2 -fPIC -shared
BUILD= bld/
SRC= src/


all: $(BUILD)lua_matrix.so $(BUILD)add_test.lua

$(BUILD)add_test.lua:$(SRC)add_test.lua
	cp $(SRC)add_test.lua $(BUILD)
	chmod +x $(BUILD)add_test.lua
$(BUILD)lua_matrix.so:$(SRC)lua_matrix.c $(BUILD)
	$(CC) $(CFLAGS) $(INC) -o $@ $<
$(BUILD):
	mkdir $(BUILD)

clean: $(BUILD)
	rm -R $(BUILD)
