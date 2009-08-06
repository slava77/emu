#!/bin/bash
# Create schar devices for Gbit drivers
# Usage: create_char_devices.sh 2 3 ...

for N in $@
  do
  [[ -c /dev/schar${N} ]] || ( mknod /dev/schar${N} c 23${N} 0 && chmod 777 /dev/schar${N} )
done
