#!/bin/bash
# -*- coding: utf-8 -*-

find . -type f -exec sed -i -e 's/DPRINTF(/DPRINTF(Ppu/g' '{}' \;'))'
find . -type f -exec sed -i -e 's/include \"debug\//include \"debug\/Ppu/g' '{}' \;
find . -type f -exec sed -i -e "s/DebugFlag('/DebugFlag('Ppu/g" '{}' \;


# for both arch/ppu and ppu
find . -type f -exec sed -i -e 's/include \"cpu/include \"ppu/g' '{}' \;
find . -type f -exec sed -i -e 's/__CPU_/__PPU_/g' '{}' \;

find . -type f -exec sed -i -e 's/Minor/PpuMinor/g' '{}' \;
find . -type f -exec sed -i -e 's/PpuPpu/Ppu/g' '{}' \;
