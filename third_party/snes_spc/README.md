# SNES-SPC

Super Nintendo (SNES) SPC-700 APU emulator and S-DSP emulators. Includes full APU emulator with
save state support and SPC music file dumping. Solarus uses SNES-SPC to support playing `.spc`
files in the Music API.

The implementation used in Solarus can be found in
[this repository](https://github.com/jprjr/snes_spc) in the `master` branch.

Local patches (included) must be applied whenever the source code is updated from upstream:
```
cat patches/*.patch | patch -p1
```
