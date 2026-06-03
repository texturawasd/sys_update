/*
 * sys_update
 * a tool to update the system in any system
 * texturawasd, june 2026
 * version 0.1: supports linux and FreeBSD
*/

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "have.c" // command_exists()

/* determine which privilege elevator program, sudo or doas, is available. checks for sudo first, then doas */
const char* determine_elevator() {
	const char *elevator = NULL;
	if (command_exists("sudo")) { elevator = "sudo"; }
	if (command_exists("doas")) { elevator = "doas"; }
    return elevator;
}

/* determine the package manager(s) available. */
const char *determine_package_manager() {

    char *pkgmgr = NULL;

    /* FreeBSD */
#if defined(__FreeBSD__)
    pkgmgr = "pkg";

    /* linux distros */
#elif defined(__linux__)
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

// Returns the update command for the given pkgmgr (you may need to insert sudo)
const char *get_bare_update_command(const char *pkgmgr) {

    const char *bare_update_command = NULL;

    // first handle flatpak and snap
    if (strcmp(pkgmgr, "flatpak") == 0) {
        bare_update_command = "flatpak update";
    } else if (strcmp(pkgmgr, "snap") == 0) {
        bare_update_command = "snap refresh";
    }

    /* linux system pkgmgrs */
#if defined(__linux__)
    if (strcmp(pkgmgr, "pacman") == 0) {
        bare_update_command = "pacman -Syu";
    } else if (strcmp(pkgmgr, "apt") == 0) {
        bare_update_command = "apt-get update && apt-get dist-upgrade -y";
    } else if (strcmp(pkgmgr, "dnf") == 0) {
        bare_update_command = "dnf upgrade -y";
    } else if (strcmp(pkgmgr, "zypper") == 0) {
        bare_update_command = "zypper ref && zypper dup -y";
    } else if (strcmp(pkgmgr, "apk") == 0) {
        bare_update_command = "apk update && apk upgrade";
    } else if (strcmp(pkgmgr, "xbps") == 0) {
        bare_update_command = "xbps-install -Syu";
    } else if (strcmp(pkgmgr, "emerge") == 0) {
        bare_update_command = "emaint sync -a && emerge --update --deep --with-bdeps=y @world";
    } else if (strcmp(pkgmgr, "eopkg") == 0) {
        bare_update_command = "eopkg up -y";
    } else if (strcmp(pkgmgr, "nix") == 0) {
        bare_update_command = "nix-channel --update && nixos-rebuild";
    } else if (strcmp(pkgmgr, "guix") == 0) {
        bare_update_command = "guix pull && guix system reconfigure";
    } else if (strcmp(pkgmgr, "urpmi") == 0) {
        bare_update_command = "urpmi.update -a && urpmi --auto-select --auto";
    } else if (strcmp(pkgmgr, "swupd") == 0) {
        bare_update_command = "swupd update";
    }
#elif defined(__FreeBSD__)
    bare_update_command = "freebsd-update fetch install && pkg update && pkg upgrade -y";
#endif
    return bare_update_command;
}


// function to build a update command accordingly with the elevator for the given pkgmgr
const char *build_final_update_command(const char *pkgmgr)
{
    const char *update_command = get_bare_update_command(pkgmgr);
    const char *elevator = determine_elevator();

    size_t len = snprintf(NULL, 0, "%s %s",
                          elevator, update_command) + 1;

    char *cmd = malloc(len);
    if (!cmd) {
        return NULL;
    }

    snprintf(cmd, len, "%s %s",
             elevator, update_command);

    return cmd;
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

