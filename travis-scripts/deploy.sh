#!/bin/bash
apt-get install doxygen;
cd ..;
doxygen Doxyfile;
git add -A
git commit -m "Automatically updated documentation for latest commit."
git push origin gh-pages;