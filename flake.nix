{
  description = "Nix flake for building zone2jsonl";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";
    simdzone-src = {
      url = "github:NLnetLabs/simdzone";
      flake = false;
    };
  };

  outputs =
    {
      self,
      nixpkgs,
      flake-utils,
      simdzone-src,
      ...
    }:
    {
      overlays.default = final: prev: {
        simdzone = final.stdenv.mkDerivation rec {
          pname = "simdzone";
          version = "git-${src.rev or "unknown"}";
          src = simdzone-src;
          nativeBuildInputs = with final; [
            cmake
            pkg-config
          ];
          postInstall = ''
            mkdir $out/bin
            cp zone-bench $out/bin
          '';
          meta = with final.lib; {
            description = "Fast and standards compliant DNS zone parser";
            homepage = "https://github.com/NLnetLabs/simdzone";
            license = licenses.bsd3;
            platforms = platforms.unix;
          };
        };
        zone2jsonl = final.stdenv.mkDerivation rec {
          pname = "zone2jsonl";
          version = "1.0.0";
          src = final.lib.cleanSource self;
          nativeBuildInputs = with final; [
            cmake
            pkg-config
          ];
          buildInputs = with final; [
            simdzone
            jansson
            ldns
            openssl
          ];
          meta = with final.lib; {
            description = "Fast zone file parser that outputs JSONL";
            homepage = "https://github.com/schlarpc/zone2jsonl";
            license = licenses.mit;
            platforms = platforms.unix;
          };
        };
      };
    }
    // (flake-utils.lib.eachDefaultSystem (
      system:
      let
        pkgs = import nixpkgs {
          inherit system;
          overlays = [ self.overlays.default ];
        };
      in
      {
        packages = rec {
          simdzone = pkgs.simdzone;
          zone2jsonl = pkgs.zone2jsonl;
          default = zone2jsonl;
        };
        apps.default = flake-utils.lib.mkApp { drv = self.packages.${system}.default; };
      }
    ));
}
