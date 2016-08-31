#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>
#include <fcntl.h>
#include <sys/sendfile.h>

#define _SIZE_ 1024
#define MAIN_PAGE "index.html"
#define HTTP_VERSION "HTTP/1.1"


void usage(const char* proc)
{
    printf("proc:%s[ip][port]\n", proc);
}

static void not_found(int client)
{}

void print_debug(const char* msg)
{
    printf("%s\n", msg);
}

static void bad_request(int client)
{
    print_debug("enter our fault...\n");
    char buf[1024];
    sprintf(buf, "HTTP/1.0 400 BAD REQUEST\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-type: text/html\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<html></br><p>your enter message is a bad request</p></br></html>\r\n");
    send(client, buf, strlen(buf), 0);
}

void print_log(const char* fun, int line, int err_no, const char* err_str)
{
    printf("[%s: %d][%d][%s]\n", fun, line, err_no, err_str);
}

void echo_error_to_client(int client, int error_code)
{
    switch(error_code)
    {
        case 400: // request error
            bad_request(client);
            break;
        case 404: // not found
            not_found(client);
            break;
        case 500: // server error
            //server_error(client);
            break;
        case 503: // server unavailable
            //server_unavailable(client);
            break;
        
        //...............................
        default:
            //default_error(client);
            break;
    }
}

void echo_html(int client, const char* path, unsigned int file_size)
{
    if(!path)
    {
        return;
    }

    int in_fd = open(path, O_RDONLY);
    if(in_fd < 0)
    {
        print_debug("open index.html error");
        return;
    }

    print_debug("open index.html success");
    char echo_line[1024];
    memset(echo_line, '\0', sizeof(echo_line));
    strncpy(echo_line, HTTP_VERSION, strlen(HTTP_VERSION)+1);
    strcat(echo_line, " 200 OK");
    strcat(echo_line, "\r\n\r\n");
    send(client, echo_line, strlen(echo_line), 0);
    print_debug("send echo head success");

    if(sendfile(client, in_fd, NULL, file_size) < 0)
    {
        print_debug("send_file error");
        close(in_fd);
        return;
    }
    print_debug("sendfile success");
    close(in_fd);
}


int startup(const char* _ip, int _port)
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0)
    {
        perror("sock");
        exit(1);
    }

    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in local;
    local.sin_family = AF_INET;
    local.sin_port = htons(_port);
    local.sin_addr.s_addr = inet_addr(_ip);

    if(bind(sock, (struct sockaddr*)&local, sizeof(local)) < 0)
    {
        perror("sock");
        exit(3);
    }

    if(listen(sock, 5) < 0)
    {
        perror("listen");
        exit(4);
    }

    return sock;
}

int get_line(int sock, char buf[], int len)
{
    if(!buf || len <= 0)
    {
        return -1;
    }

    char c = '\0';
    
    int n = 0;
    int i = 0;
    while((i < len-1) && (c != '\n'))
    {
        n = recv(sock, &c, 1, 0);
        if(n > 0)
        {
            if(c == '\r')
            {
                n = recv(sock, &c, 1, MSG_PEEK);
                if(n > 0 && c == '\n')
                {
                    recv(sock, &c, 1, 0);
                }
                else
                {
                    c = '\n';
                }
            }
            buf[i++] = c;
        }
        else
        {
            c = '\n';
        }
    }
    buf[i] = '\0';
    return i;
}

static void echo_errno(int sock)
{}

static clear_header(int sock)
{
    int ret = -1;
    char buf[_SIZE_];
    do{
        ret = get_line(sock, buf, sizeof(buf));
    }while((ret > 0) && (strcmp(buf, "\n") != 0));
}

void exec_cgi(int sock, const char* path, const char* method, const char* query_string)
{
    print_debug("enter cgi\n");
    char buf[_SIZE_];
    int numchars = 0;
    int content_length = -1;
    
    // pipe
    int cgi_input[2] = {0, 0};
    int cgi_output[2] = {0, 0};
    //child proc
    pid_t id;

    print_debug(method);

    if(strcasecmp(method, "GET") == 0) // GET
    {
        clear_header(sock);
    }
    else // POST
    {
        do{
            memset(buf, '\0', sizeof(buf));
            numchars = get_line(sock, buf, sizeof(buf));
            if(strncasecmp(buf, "Content-Length:", strlen("Content-Length:")) == 0)
            {
                content_length = atoi(&buf[16]);
            }
        }while(numchars > 0 && strcmp(buf, "\n") != 0);

        if(content_length == -1)
        {
            return;
        }
    }

    memset(buf, '\0', sizeof(buf));
    strcpy(buf, HTTP_VERSION);
    strcat(buf, " 200 OK\r\n\r\n");
    send(sock, buf, strlen(buf), 0);

    if(pipe(cgi_input) == -1) // pipe error
    {
        return;
    }
    
    if(pipe(cgi_output) == -1) // 
    {
        close(cgi_input[0]);
        close(cgi_input[1]);
        return;
    }

    if((id = fork()) < 0) // fork error
    {
        close(cgi_input[0]);
        close(cgi_input[1]);
        close(cgi_output[0]);
        close(cgi_output[1]);
        return;
    }
    else if( id == 0 ) // child
    {
        char query_env[_SIZE_/10];
        char method_env[_SIZE_];
        char content_len_env[_SIZE_];
        memset(method_env, '\0', sizeof(method_env));
        memset(query_env, '\0', sizeof(query_env));
        memset(content_len_env, '\0', sizeof(content_len_env));

        close(cgi_input[1]);
        close(cgi_output[0]);

        dup2(cgi_input[0], 0);
        dup2(cgi_output[1], 1);

        sprintf(method_env, "REQUEST_METHOD=%s", method);
        //printf("REQUEST_METHOD=%s\n", method_env);
        putenv(method_env);
        if(strcasecmp("GET", method) == 0) // GET
        {
            sprintf(query_env, "QUERY_STRING=%s", query_string);
            //printf("QUERY_STRING=%s\n", query_env);
            putenv(query_env);
        }
        else
        {
            sprintf(content_len_env, "CONTENT_LENGTH=%d", content_length);
            //printf("CONTENT_LENGTH=%s\n", content_len_env);
            putenv(content_len_env);
        }

        execl(path, path, NULL);
        exit(1);
    }
    else   // father
    {
        close(cgi_input[0]);
        close(cgi_output[1]);
        int i = 0;
        char c = '\0';
        if(strcasecmp("POST", method) == 0)
        {
            for(; i < content_length; i++)
            {
                recv(sock, &c, 1, 0);
                write(cgi_input[1], &c, 1);
            }
        }

        while( read(cgi_output[0], &c, 1) > 0)
        {
            send(sock, &c, 1, 0);
        }
        close(cgi_input[1]);
        close(cgi_output[0]);

        waitpid(id, NULL, 0);
    }
}

void* accept_request(void* arg)
{
    
    int sock = (int)arg;
    char buf[_SIZE_];
    char method[_SIZE_];
    char url[_SIZE_];
    char path[_SIZE_];

    memset(buf, '\0', sizeof(buf));
    memset(method, '\0', sizeof(method));
    memset(url, '\0', sizeof(url));
    memset(path, '\0', sizeof(path));
    
    int cgi = 0;
    int ret = -1;
    char* query_string = NULL;

#ifdef _DEBUG_
    do{
        ret = get_line(sock, buf, sizeof(buf));
        printf("%s", buf);
        fflush(stdout);
    } while((ret > 0) && (strcmp(buf, "\n") != 0));
#endif

    ret = get_line(sock, buf, sizeof(buf));
    printf("get_line: %s\n", buf);
    if(ret <= 0)
    {
        echo_errno(sock);
        return (void *)1;
    }

    //GET          /HTTP/1.1
    int i = 0;     // method index
    int j = 0;     // buf index

    while( i < (sizeof(method)-1) && (j < sizeof(buf)) && (!isspace(buf[j])))
    {
        method[i] = buf[j];
        i++, j++;
    }

    method[i] = '\0';

    if((strcasecmp(method, "GET") != 0) && (strcasecmp(method, "POST") != 0))
    {
        echo_errno(sock);
        return (void*)2;
    }

    while( isspace(buf[j]) )
    {
        j++;
    }


    i = 0; // url index
    while( i < (sizeof(url)-1) && (j < sizeof(buf)) && (!isspace(buf[j])))
    {
        url[i] = buf[j];
        i++, j++;
    }

    url[i] = '\0';

    if(strcasecmp(method, "POST") == 0)
    {
        printf("POST 1\n");
        cgi = 1;
    }
    if(strcasecmp(method, "GET") == 0)
    {
        query_string = url;
        while(*query_string != '\0' && *query_string != '?' )
        {
            query_string++;
        }

        if(*query_string == '?')
        {
            cgi = 1;
            *query_string = '\0';
            query_string++;
        }
    }

    // /     /aa/bb/cc
    sprintf(path, "htdoc%s", url);   // htdoc/
    if(path[strlen(path) - 1] == '/')
    {
        strcat(path, "index.html");
    }

    printf("method: %s\n", method);
    printf("path: %s\n", path);
    printf("query_string: %s\n", query_string);
    
    struct stat st;
    if(stat(path, &st) < 0)
    {
        echo_errno(sock);
        printf("stat error\n");
        return (void*)3;
    }
    else
    {
        if(S_ISDIR(st.st_mode))
        {
            strcat(path, "index.html");
        }
        else if( (st.st_mode & S_IXUSR) || (st.st_mode & S_IXGRP) || (st.st_mode & S_IXOTH) )
        {
            cgi = 1;
        }
        else
        {}

        printf("AAAAAAAAAAAAAAAAAA    %d\n", cgi);
        if(cgi)
        {
            printf("AAAAAAAAAAAAAAAAAA\n");
            exec_cgi(sock, path, method, query_string);
            printf("AAAAAAAAAAAAAAAAAA\n");
        }
        else
        {
            clear_header(sock);
            echo_html(sock, path, st.st_size);
        }
    }

    close(sock);
    return (void*)0;
}

int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        usage(argv[0]);
        exit(1);
    }

    int listen_sock = startup(argv[1], atoi(argv[2]));
    printf("listen_sock%d\n", listen_sock);
    struct sockaddr_in peer;
    socklen_t len = sizeof(peer);

    int done = 0;
    while(!done)
    {
        int new_sock = accept(listen_sock, (struct sockaddr*)&peer, &len);

        printf("new_sock:%d\n", new_sock);

        if(new_sock > 0)
        {
            printf("debug:client socket:%s:%d\n", inet_ntoa(peer.sin_addr), ntohs(peer.sin_port));
            
            pthread_t tid;
            pthread_create(&tid, NULL, accept_request, (void*)new_sock);
            pthread_detach(tid);
        }
    }

    return 0;
}
