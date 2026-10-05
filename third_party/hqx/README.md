# hqx

hqx ("hq" stands for "high quality" and "x" stands for magnification) is one of the pixel art
scaling algorithms developed by Maxim Stepin, used in emulators such as Nestopia, bsnes, ZSNES,
Snes9x, FCE Ultra and many more. There are 3 hqx filters: hq2x, hq3x and hq4x which magnify by
factor of 2, 3 and 4 respectively. Solarus uses hqx for software-based pixel scaling.

The implementation used in Solarus can be found in
[this repository](https://github.com/grom358/hqx) in the `master` branch.

Local patches (included) must be applied whenever the source code is updated from upstream:
```
cat patches/*.patch | patch -p1
```
