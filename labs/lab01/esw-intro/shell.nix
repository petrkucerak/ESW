{ pkgs ? import <nixpkgs> {} }:
pkgs.mkShell {
    packages = with pkgs; [
        pkg-config
        meson
        ninja
        json_c
    ];
}
