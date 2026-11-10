{pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  packages = with pkgs; [
    gcc
    gnumake
    pkg-config
    mesa
    libGLU
    freeglut
    zip
    R
    imagemagick
  ];
}
