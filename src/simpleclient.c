#include "wrappers/wrapper.h"

#include <stdio.h>
#include <getopt.h>

#define SOCKOPTS "a:p:h"

void print_help() {
    printf("usage options:\n"
            "\t -[-a]ddress <url | IP addreses> - the address to connect\n"
            "\t -[-p]ort <1-65535> - the port to listen to\n"
            "\t -[-g]arbage - ignore stdin and spew garbage as fast as the server will accept it. defaults to off\n"
            "\t -[-h]elp - this message\n"
          );
}

int main(int argc, char ** argv) {
    char * listening_port = 0;
    char * address = 0;
    int garbage_mode = 0;
    {
        int c;
        for (;;) {
            int option_index = 0;
            static struct option long_options[] = {
                {"port",    required_argument, 0, 'p'},
                {"address", required_argument, 0, 'a'},
                {"garbage", no_argument,       0, 'g'},
                {"help",    no_argument,       0, 'h'},
                {0,         0,                 0, 0}
            };
            c = getopt_long(argc, argv, SOCKOPTS, long_options, &option_index);
            if (c == -1) {
                break;
            }

            switch (c) {
                case 'a':
                    address = optarg;
                    break;
                case 'p':
                    listening_port = optarg;
                    break;
                case 'g':
                    garbage_mode = 1;
                    break;
                case 'h':
                case '?':
                default:
                    print_help();
                    return 0;
            }
        }
        if (!listening_port || !address) {
            print_help();
            return 1;
        }
    }
}
