#!/bin/zsh
# Check for dead emud services and resurrect them.

PATTERN="s/[^']*'\([^']*\)'[^']*/\1/"
/sbin/service emud status | while read LINE; do
    SERVICE=$(echo $LINE | sed -e $PATTERN)
    [[ $LINE == *FAILED* ]] && /sbin/service emud start $SERVICE
done
