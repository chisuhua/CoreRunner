#!/bin/bash
# -*- coding: utf-8 -*-
cd ../../

./design/build_libgem5.sh

cd ./design/cosim
scons
