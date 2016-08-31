WORK_PATH=$(shell pwd)

BIN=sql_connect
CC=g++
SRC=sql_connect.cpp

INCLUDE_PATH=-I$(WORK_PATH)/lib/include
LIB_PATH=-L$(WORK_PATH)/lib/lib

LIB_NAME=-lmysqlclient -D_DEBUG_
$(BIN):$(SRC)
	$(CC) -o $@ $^ $(LIB_NAME) $(INCLUDE_PATH) $(LIB_PATH)

.PHONY:clean
clean:
	rm -f $(BIN)
