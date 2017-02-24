# Lupkg
This is still a work in progress, but the goal is to have a simple packaging
tool to group deps together in a single executable.

## Required Commands For Build:
* mksquashfs

## Required Commands For Runtime:
* fusermount

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

# edit ./app/app_run, add files, etc...

lupkg build

# run the package
./build/app.lupkg
```

## TODO:
* usage, comand line options, and better error checking
* use fuse for squashfs
* a lot more...

## Authors
* Braxton Plaxco
