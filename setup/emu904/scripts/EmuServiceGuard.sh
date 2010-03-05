#!/bin/zsh
# Check for dead xdaqd services and resurrect them.

PATTERN="s/[^']*'\([^']*\)'[^']*/\1/"
/sbin/service xdaqd status | while read LINE; do
    SERVICE=$(echo $LINE | sed -e $PATTERN)
    [[ $LINE == *FAILED* ]] && /sbin/service xdaqd start $SERVICE
done
