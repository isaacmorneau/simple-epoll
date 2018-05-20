#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

#include "wrappers/wrapper.h"

#define SOCKOPTS "p:h"

void print_help() {
    printf("usage options:\n"
            "\t [p]ort <1-65535> the port to listen to\n"
            "\t [h]elp this message\n"
          );
}

int main(int argc, char ** argv) {
    char * listening_port = 0;
    //handle the arguments in its own scope
    {
        int c;
        for (;;) {
            int option_index = 0;
            static struct option long_options[] = {
                {"port",    required_argument, 0, 'p'},
                {"help",    no_argument,       0, 'h'},
                {0,         0,                 0, 0}
            };
            c = getopt_long(argc, argv, SOCKOPTS, long_options, &option_index);
            if (c == -1) {
                break;
            }

            switch (c) {
                case 'p':
                    listening_port = optarg;
                    break;
                case 'h':
                case '?':
                default:
                    print_help();
                    return 0;
            }
        }
        if (!listening_port) {
            print_help();
            return 1;
        }
    }

    int sfd;
    int efd;
    int afd;
    struct epoll_event *events;
    //make and bind the socket
    sfd = make_bound_tcp(listening_port);

    //start listening
    set_listening(sfd);

    //register the epoll structure
    efd = make_epoll();

    //wait for the first connection
    afd = accept_blind(sfd);

    //buffer for where events are returned
    events = make_epoll_events();

    //listen to events on the new connection
    add_epoll_fd(efd, afd);

    while (1) {
        int n, i;

        n = wait_epoll(efd, events);
        for (i = 0; i < n; i++) {
            if (EVENT_ERR(events, i) || EVENT_HUP(events, i)) {
                // A socket got closed
                close_echoing(EVENT_PTR(events, i));
                continue;
            } else if(EVENT_IN(events, i)) {
                echoing_read(EVENT_PTR(events, i));
            } else { //EPOLLOUT
                //shoulnt happen but this means we would have blocked
                //we are now notified that we can send the rest of the data
                echoing_flush(EVENT_PTR(events, i));
            }
        }
    }
    return 0;
}
