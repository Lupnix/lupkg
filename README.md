# Lupkg
This is still a work in progress, but the goal is to have a simple packaging
tool to group deps together in a single executable.

## Required Commands For Build:
* mksquashfs

## Required Commands For Runtime:
* [squashfuse](https://github.com/vasi/squashfuse/)
* fusermount

(NOTE: squashfuse and/or fusermount might go away when I get a chance to code the
functionality myself)

## Installing
```
make
sudo make install
```

## Usage
```
mkdir myproject
cd ./myproject
lupkg init
lupkg build
sudo ./build/app.lupkg

# edit ./app/run, add files, etc...

lupkg build
sudo ./build/app.lupkg
```

## TODO:
* usage, comand line options, and better error checking
* use fuse for squashfs
* a lot more...

## Authors
* Braxton Plaxco
