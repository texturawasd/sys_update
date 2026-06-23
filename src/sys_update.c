#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../common_utils/have.h" // command_exists()
#include "../include/pma.h"

int sys_update() {

    const char *system_package_manager = determine_package_manager();

    #if defined (__linux__)
    puts("-> Updating the system...");

    /* first check for universal pkgmgrs, snap and flatpak */
    if (command_exists("snap")) {
        puts("-> Updating snap...");
        const char *update_command_for_snap = build_final_update_command("snap");
        system(update_command_for_snap); puts("-> updated snap.");
    }
    if (command_exists("flatpak")) {
        puts("-> Updating flatpak...");
        const char *update_command_for_flatpak = build_final_update_command("flatpak");
        system(update_command_for_flatpak); puts("-> updated flatpak.");
    }

    /* since Arch is the arch-distro, first check if we're in it or a derivative
     * in order to update with the AUR helper command (simpler) */
    if (aur()) {
        const char *AUR = get_aur_helper();
        printf("-> Detected AUR helper: %s, updating the system\n", AUR);
        fflush(stdout);
        system(AUR);
        exit(EXIT_SUCCESS);
    }
    /* no aur helper, proceed normally. */
    printf("-> Detected system package manager: %s\n", system_package_manager);
    const char *final_update_command = build_final_update_command(system_package_manager);
    system(final_update_command);
    return 0;
#else
    puts("-> Updating the system...");
    printf("-> Detected system package manager: %s\n", system_package_manager);
    const char *final_update_command = build_final_update_command(system_package_manager);
    system(final_update_command);

    return 0;
#endif
}