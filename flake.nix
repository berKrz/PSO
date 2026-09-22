{
  description = "Binary Genetic Algorithm in C";

  inputs.nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";

  outputs = { nixpkgs, ... }:
    let
      pkgs = nixpkgs.legacyPackages.x86_64-linux;
    in {
      devShells.x86_64-linux.default = pkgs.mkShell {
        packages = with pkgs; [
          gcc            # compiler + runtime
          clang-tools    # clangd (LSP) + clang-format (formatter)
          cmake          # build system — omit if not using it
          ninja          # fast build backend for cmake
          gdb            # debugger
        ];
      };

      packages.x86_64-linux.default = pkgs.stdenv.mkDerivation {
        pname = "pso";
        version = "0.1.0";
        src = ./.;
        buildPhase = "make";
        installPhase = ''
          mkdir -p $out/bin
          cp pso $out/bin/
        '';
      };
    };
}
