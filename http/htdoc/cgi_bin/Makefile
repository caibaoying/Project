FIND_PATH=-I/home/caibaoying/practice/http/sql_connect -I/home/caibaoying/practice/http/sql_connect/lib/include -L/home/caibaoying/practice/http/sql_connect/lib/lib -lmysqlclient

.PHONY:all
all:debug_cgi math_cgi show_student_cgi register_cgi
debug_cgi:debug_cgi.c
	gcc -o $@ $^
math_cgi:math_cgi.c
	gcc -o $@ $^
show_student_cgi:show_student_cgi.cpp /home/caibaoying/practice/http/sql_connect/sql_connect.cpp
	g++ -o $@ $^ $(FIND_PATH) -g
register_cgi:register_cgi.cpp /home/caibaoying/practice/http/sql_connect/sql_connect.cpp
	g++ -o $@ $^ $(FIND_PATH)

.PHONY:clean
clean:
	rm -rf debug_cgi math_cgi show_student_cgi register_cgi
