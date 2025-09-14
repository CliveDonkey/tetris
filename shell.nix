{ pkgs ? import <nixpkgs> {} }:
pkgs.mkShell {
  packages = [
    pkgs.mesa
    pkgs.glfw
  ];
  inputsFrom = [ 
    pkgs.mesa
    pkgs.glfw
  ];
}
