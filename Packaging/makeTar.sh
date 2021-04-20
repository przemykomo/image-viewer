#!/bin/sh
cmake -S .. -B ../Release/
cmake --build ../Release/
tar --transform="s,.*/,," -czvf ./piv.tar.gz ../Release/piv ../piv.desktop
