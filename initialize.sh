#!/bin/bash
SOURCE="${BASH_SOURCE[0]}"
while [ -h "$SOURCE" ]; do # resolve $SOURCE until the file is no longer a symlink
  DIR="$( cd -P "$( dirname "$SOURCE" )" >/dev/null 2>&1 && pwd )"
  SOURCE="$(readlink "$SOURCE")"
  [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE" # if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
done
DIR="$( cd -P "$( dirname "$SOURCE" )" >/dev/null 2>&1 && pwd )"

cd $DIR
git submodule init
git submodule update

cd $DIR/extlibs/rapidyaml
git submodule init
git submodule update

cd $DIR/extlibs/rapidyaml/ext/c4core
git submodule init
git submodule update

cd $DIR/extlibs/rapidyaml/ext/cryptopp
CXXFLAGS="-DNDEBUG -g2 -O3 -std=c++11" mingw32-make

cd $DIR