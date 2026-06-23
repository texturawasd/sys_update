#include <stdbool.h>

#include "../include/pma.h"

const char *build_final_install_command(const char *system_package_manager, const char *package_name);

int package_install(const char *package_name) {
    const char *system_package_manager = determine_package_manager();

    const char *final_install_command = build_final_install_command(system_package_manager, package_name);
    system(final_install_command);
    return 0;

}

const char *build_final_install_command(const char *system_package_manager, const char *package_name)
{

}