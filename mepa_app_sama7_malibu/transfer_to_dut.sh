#!/bin/sh

#clear keys
if [ "$1" = "clear_keys" ];
    then
        echo "Clearing keys..."
        ssh-keygen -f '/home/test/.ssh/known_hosts' -R '192.168.137.221'
        echo "Make sure to answer \"yes\"."
fi

#transfer files to DUT
echo "Transfer files to DUT"
scp ./malibu10_sanity_check_app root@192.168.137.221:~/