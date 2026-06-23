// UNFINISHED

#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include "../common_utils/simple_strings.h"

#include "../include/pma.h"

const char *build_final_install_command(const char *system_package_manager, const char *package_name);

str bare_install_command(const char *system_package_manager);

int package_install(const char *package_name) {
    const char *system_package_manager = determine_package_manager();


    return 0;

}

static const char *unelevated_install_command(const char *system_package_manager, const char *package_name)
{
    str unelevated_install_command;
    if (strcmp(determine_package_manager(), "pacman") == 0) {
        unelevated_install_command = str_append(
            bare_install_command("pacman"), str_create("pacman"));
    }
}

str bare_install_command(const char *system_package_manager)
{

}

const char *build_final_install_command(const char *system_package_manager, const char *package_name)
{
    const char *final_install_command = NULL;
    if (getuid() == 0) {
        final_install_command = unelevated_install_command(system_package_manager, package_name);
    } else {
        final_install_command = elevate_command(unelevated_install_command(system_package_manager, package_name));
    }
    return final_install_command;
}