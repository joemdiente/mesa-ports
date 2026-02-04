#!/bin/sh

#clear keys
if [ "$1" = "clear-keys" ];
    then
        echo "Clearing keys..."
        ssh-keygen -f '/home/test/.ssh/known_hosts' -R '192.168.137.221'
        echo "========================================================================================"
        echo "Make sure to answer \"yes\" when asked to continue connecting..."
        echo "If target is asking for password, make sure host .pub key is in target's authorized_keys"
        echo "========================================================================================"
        echo "\r\n"
fi

#transfer files to DUT
echo "Transfer files to DUT....\r\n"
scp ./malibu10_sanity_check_app root@192.168.137.221:~/
echo "\r\nDone."