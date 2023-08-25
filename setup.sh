#!/bin/sh

echo 'Installing dependencies...'
HOMEBREW_NO_AUTO_UPDATE=1 brew install -v \
	sdl2 \
	sdl2_image \
	sdl2_ttf \
	sdl2_mixer --with-mpg123 \
	sfml \
	glew \
	glm

echo 'Building Nibbler...'
make
