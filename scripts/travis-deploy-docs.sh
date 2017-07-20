#!/bin/bash

COMMIT_USER="Automated Documentation Builder"
COMMIT_EMAIL="travis@travis-ci.org"
CHANGESET=$(git rev-parse --verify HEAD)

apt-get install doxygen graphviz;
rm -rf ./docs/out
mkdir -p ./docs/out
git clone -b gh-pages git@github.com:liavt/MACE.git --single-branch ./docs/out
cd docs/out
git rm -rf .
cd -

doxygen ../Doxyfile
git add -A
git config user.name "${COMMIT_USER}"
git config user.email "${COMMIT_EMAIL}"
git commit -m "Automatically updated documentation for latest commit. ${CHANGESET}"
git push origin gh-pages
cd -