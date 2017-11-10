#ifndef WRAPPER_H
#define WRAPPER_H

//we need a struct because non blocking reads and writes with
//splice may not actually empty the pipes if it exits due to E_AGAIN
typedef struct {
    int fd;
    int pipefd[2];
} epoll_data;

int make_bound(const char * port);
int make_non_blocking(int sfd);
//echo
int echo(epoll_data * epd);
int echo_harder(epoll_data * epd);
//close all the members
void epoll_data_close(epoll_data * epd);
//initialize the pip and set the fd
int epoll_data_init(epoll_data * epd, int fd);

#endif //WRAPPER_H

