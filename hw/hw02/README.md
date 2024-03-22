# Epol server

To build the project, install dependencies or use nix shell:

```sh
nix-shell
```

Then build the project with meson:

```sh
meson setup builddir
meson compile -C builddir

meson setup builddir && meson compile -C builddir
```

And finally test the project:

```sh
meson test -C builddir
```

To build archive for uploading to brute

```sh
zip kucerp28-epol.zip incl/* src/* meson.build shell.nix
```
