/*
 * sys_update
 * a tool to update the system in any system
 * texturawasd, june 2026
 * version 0.1: supports linux and FreeBSD
 * technically others too, but only tested on:
 * Arch Linux
 * Ubuntu
 * FreeBSD
*/

#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include "sys_update.c"
#include "../common_utils/args.h"
#include "../common_utils/parsing_utils.h"

int main(int argc, char **argv) {

    if (getuid() == 0 && !arg_is_present("i-know-what-i-am-doing", argc, argv)) {
        fprintf(stderr, "Running %s as root is not recommended. Pass --i-know-what-i-am-doing if you really do.\n", argv[0]);
        return -1;
    }

    if (arg_is_present("update", argc, argv)) {
        sys_update();
        return 0;
    } else if (arg_is_present("install", argc, argv)) {
        fprintf(stderr, "Installing packages is not yet implemented, exiting.\n");
        return -1;
    } else if (arg_is_present("remove", argc, argv)) {
        fprintf(stderr, "Removing packages is not yet implemented, exiting.\n");
        return -1;
    }

    /* no args, update the system */
    puts("confirm to update the system? (y/n)");
    char response[420];
    scanf("%s", response);
    if (parse_bool(response)) {
        sys_update();
    } else {
        puts("aborting.");
    }
    return 0;

}

