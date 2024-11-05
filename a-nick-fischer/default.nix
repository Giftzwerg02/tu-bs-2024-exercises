{ pkgs ? import <nixpkgs> {} }:
pkgs.mkShell {
  packages = with pkgs; [ gnumake gcc linux-manual man-pages man-pages-posix tldr ];

  inputsFrom = [ ];

  shellHook = ''
    export DEBUG=1
  '';
}