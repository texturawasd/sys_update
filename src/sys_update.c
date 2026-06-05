/*
 * sys_update
 * a tool to update the system in any system
 * texturawasd, june 2026
 * version 0.1: supports linux and FreeBSD
*/

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "../common_utils/have.c" // command_exists()
#include "../common_utils/elevate.c" // elevate()

/* determine the package manager(s) available. */
const char *determine_package_manager() {

    char *pkgmgr = NULL;

#if defined(__FreeBSD__)
    /* FreeBSD */
    pkgmgr = "pkg";

#elif defined(__OpenBSD__)
    /* OpenBSD */
    pkgmgr = "pkg_add";
#elif defined(__APPLE__)
    /* macOS */
    pkgmgr = "softwareupdate";
#elif defined(__linux__)
    /* linux distros */
	if (command_exists("pacman")) {
        pkgmgr = "pacman";
    } else if (command_exists("apt-get")) {
        pkgmgr = "apt";
    } else if (command_exists("dnf")) {
        pkgmgr = "dnf";
    } else if (command_exists("yum")) {
        pkgmgr = "dnf"; // yum is link to dnf
    } else if (command_exists("zypper")) {
        pkgmgr = "zypper";
    } else if (command_exists("apk")) {
        pkgmgr = "apk";
    } else if (command_exists("xbps-install")) {
        pkgmgr = "xbps";
    } else if (command_exists("emaint") || command_exists("emerge")) {
        pkgmgr = "emerge";
    } else if (command_exists("eopkg") == 1) {
        pkgmgr = "eopkg";
    } else if (command_exists("nix-channel") || command_exists("nixos-rebuild")) {
        pkgmgr = "nix";
    } else if (command_exists("guix")) {
        pkgmgr = "guix";
    } else if (command_exists("urpmi")) {
        pkgmgr = "urpmi";
    } else if (command_exists("swupd")) {
        pkgmgr = "swupd";
    } else {
        pkgmgr = NULL;
    }
#endif
    return pkgmgr;
}

// Returns the (not yet elevated) update command for the given pkgmgr (you may need to insert sudo)
const char *get_unelevated_update_command(const char *pkgmgr) {

    const char *unelevated_update_command = NULL;

    // first handle flatpak and snap
    if (strcmp(pkgmgr, "flatpak") == 0) {
        unelevated_update_command = "flatpak update";
    } else if (strcmp(pkgmgr, "snap") == 0) {
        unelevated_update_command = "snap refresh";
    }

    /* linux system pkgmgrs */
#if defined(__linux__)
    if (strcmp(pkgmgr, "pacman") == 0) {
        unelevated_update_command = "pacman -Syu";
    } else if (strcmp(pkgmgr, "apt") == 0) {
        unelevated_update_command = "apt-get update && apt-get dist-upgrade -y";
    } else if (strcmp(pkgmgr, "dnf") == 0) {
        unelevated_update_command = "dnf upgrade -y";
    } else if (strcmp(pkgmgr, "zypper") == 0) {
        unelevated_update_command = "zypper ref && zypper dup -y";
    } else if (strcmp(pkgmgr, "apk") == 0) {
        unelevated_update_command =  "apk update && apk upgrade";
    } else if (strcmp(pkgmgr, "xbps") == 0) {
        unelevated_update_command = "xbps-install -Syu";
    } else if (strcmp(pkgmgr, "emerge") == 0) {
        unelevated_update_command = "emaint sync -a && emerge --update --deep --with-bdeps=y @world";
    } else if (strcmp(pkgmgr, "eopkg") == 0) {
        unelevated_update_command = "eopkg up -y";
    } else if (strcmp(pkgmgr, "nix") == 0) {
        unelevated_update_command = "nix-channel --update && nixos-rebuild";
    } else if (strcmp(pkgmgr, "guix") == 0) {
        unelevated_update_command = "guix pull && guix system reconfigure";
    } else if (strcmp(pkgmgr, "urpmi") == 0) {
        unelevated_update_command = "urpmi.update -a && urpmi --auto-select --auto";
    } else if (strcmp(pkgmgr, "swupd") == 0) {
        unelevated_update_command = "swupd update";
    }
#elif defined(__FreeBSD__)
    unelevated_update_command = "freebsd-update fetch install && pkg update && pkg upgrade -y";
#elif defined(__OpenBSD__)
    unelevated_update_command = "syspatch && pkg_add -u";
#elif defined(__APPLE__)
    unelevated_update_command = "softwareupdate --install --all";
#endif
    return unelevated_update_command;
}

// function to build a update command accordingly with the elevator for the given pkgmgr
const char *build_final_update_command(const char *pkgmgr)
{
    return elevate_command(get_unelevated_update_command(pkgmgr));
}

int main(void) {

    const char *system_package_manager = determine_package_manager();

    puts("Updating the system...");
    printf("detected system package manager: %s\n", system_package_manager);
    const char *final_update_command = build_final_update_command(system_package_manager);
    system(final_update_command);

    /* also update universal pkgmgrs: snap, flatpak */

    if (command_exists("snap")) {
        const char *update_command_for_snap = build_final_update_command("snap");
        system(update_command_for_snap);
    }
    if (command_exists("flatpak")) {
        const char *update_command_for_flatpak = build_final_update_command("flatpak");
        system(update_command_for_flatpak);
    }

    return 0;
}

