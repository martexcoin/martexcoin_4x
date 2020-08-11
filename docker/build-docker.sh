#!/usr/bin/env bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd $DIR/..

DOCKER_IMAGE=${DOCKER_IMAGE:-martexpay/martexd-develop}
DOCKER_TAG=${DOCKER_TAG:-latest}

BUILD_DIR=${BUILD_DIR:-.}

rm docker/bin/*
mkdir docker/bin
cp $BUILD_DIR/src/martexd docker/bin/
cp $BUILD_DIR/src/martex-cli docker/bin/
cp $BUILD_DIR/src/martex-tx docker/bin/
strip docker/bin/martexd
strip docker/bin/martex-cli
strip docker/bin/martex-tx

docker build --pull -t $DOCKER_IMAGE:$DOCKER_TAG -f docker/Dockerfile docker
