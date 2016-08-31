ROOT_PATH=$(shell pwd)
SER_PATH=$(ROOT_PATH)
FLAG=-lpthread -D_DEBUG_

SER=Server
CC=gcc

SER_SRC=$(SER_PATH)/httpd.c
SER_OBJ=$(SER_SRC:.c=.o)

$(SER):$(SER_OBJ)
	$(CC) -o $@ $^ $(FLAG)
%.o:$(SER_PATH)/%.c
	$(CC) -c $< $^ 
.PHONY:clean
clean:
	rm -f $(SER) *.o
	
.PHONY:debug
debug:
	echo $(SER_PATH)     

	
