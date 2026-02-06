#!/bin/sh

DUT_IP=192.168.137.221

#clear keys
if [ "$1" = "clear-keys" ];
    then
        echo "Clearing keys..."
        ssh-keygen -f '/home/test/.ssh/known_hosts' -R \'$DUT_IP\'
        echo "========================================================================================"
        echo "Make sure to answer \"yes\" when asked to continue connecting..."
        echo "If target is asking for password, make sure host .pub key is in target's authorized_keys"
        echo "========================================================================================"
        echo "\r\n"
fi

#transfer files to DUT
echo "Transfer all ./mepa_app* to $DUT_IP....\r\n"
scp ./mepa_app* root@$DUT_IP:~/
echo "\r\nDone."