CC= gcc
INC= -I/usr/include/lua5.5/
CFLAGS= -Wall -O3 -fPIC -shared
DBGFLAGS = -Wall -g -O0 -fPIC -shared
BUILD= bld/
SRC= src/
DBG= dbg/

all: $(BUILD)lua_matrix.so $(BUILD)add_test.lua

$(BUILD)add_test.lua:$(SRC)add_test.lua
	cp $(SRC)add_test.lua $(BUILD)
	chmod +x $(BUILD)add_test.lua
$(BUILD)lua_matrix.so:$(SRC)lua_matrix.c $(BUILD)
	$(CC) $(CFLAGS) $(INC) -o $@ $<
$(BUILD):
	mkdir $(BUILD)

clean: $(BUILD) $(DBG)
	rm -R $(BUILD)
	rm -R $(DBG)

debug:$(DBG)lua_matrix.so

$(DBG)lua_matrix.so:$(SRC)lua_matrix.c $(DBG)
	$(CC) $(DBGFLAGS) $(INC) -o $@ $<
$(DBG):
	mkdir $(DBG)
