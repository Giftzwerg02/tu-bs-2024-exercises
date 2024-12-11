{
  description = "Go proj flake";

  inputs.nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
  inputs.flake-utils.url = "github:numtide/flake-utils";

  outputs = {
    self,
    nixpkgs,
    flake-utils,
    ...
  }:
    flake-utils.lib.eachDefaultSystem (
      system: let
        pkgs = import nixpkgs {inherit system;};
      in {
        devShells.default =
          pkgs.mkShell
          {
            buildInputs = with pkgs; [
              gcc
              gnumake
              gdb
              linux-manual
              man-pages
              man-pages-posix
            ];
          };
      }
    );
}
