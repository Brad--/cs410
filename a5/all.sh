#!/bin/bash
rm final.txt
python makefinal.py
./raytracer final.txt final.ppm
