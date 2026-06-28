{
  description = "LLVM development environment";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

  outputs =
    inputs:
    let
      systems = [
        "x86_64-linux"
        "aarch64-linux"
        "x86_64-darwin"
        "aarch64-darwin"
      ];
      forAllSystems =
        f: inputs.nixpkgs.lib.genAttrs systems (system: f inputs.nixpkgs.legacyPackages.${system});
    in
    {
      devShells = forAllSystems (pkgs: {
        default = pkgs.mkShell.override { stdenv = pkgs.clangStdenv; } {
          packages = with pkgs; [
            cmake
            ninja
            python3
            git

            lld
            mold
            ccache
            clang-tools

            libxml2
            libffi
            zlib
            ncurses
            zstd
          ];

          # clangd reads compile_commands.json; CMake only writes it when this is set.
          CMAKE_EXPORT_COMPILE_COMMANDS = "1";

          shellHook = ''
            configure-llvm() {
              cmake -G Ninja -S llvm -B build \
                -DCMAKE_BUILD_TYPE=Debug \
                -DLLVM_ENABLE_PROJECTS="clang;lld" \
                -DLLVM_USE_LINKER=mold \
                -DLLVM_USE_SPLIT_DWARF=ON \
                -DLLVM_OPTIMIZED_TABLEGEN=ON \
                -DLLVM_CCACHE_BUILD=ON \
                -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
                "$@"
              ln -sf build/compile_commands.json compile_commands.json
            }

            echo "llvm dev shell — run 'configure-llvm [extra cmake args]' then 'ninja -C build'"
          '';
        };
      });
    };
}
