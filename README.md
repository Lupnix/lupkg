# Lupkg
This is still a work in progress, but the goal is to have a simple packaging
tool to group deps together in a single executable.

## Required Commands:
* mount
* umount
* mksquashfs

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
