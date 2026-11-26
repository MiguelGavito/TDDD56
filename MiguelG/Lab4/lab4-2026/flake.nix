
{
  description = "CUDA development environment";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

  outputs = { self, nixpkgs }:
    let
      system = "x86_64-linux";

      pkgs = import nixpkgs {
        inherit system;
        config.allowUnfree = true;
        config.cudaSupport = true;
      };

      cuda = pkgs.cudaPackages_13_0;
    in {
      devShells.${system}.default = pkgs.mkShell {
        packages = [
          cuda.cuda_nvcc
          cuda.cuda_gdb
          cuda.cuda_cudart
          pkgs.gcc
          pkgs.gnumake
        ];
        shellHook = ''
          export LD_LIBRARY_PATH="/run/opengl-driver/lib:$LD_LIBRARY_PATH"
        '';
      };
    };
}
