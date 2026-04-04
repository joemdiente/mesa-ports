#!/bin/sh
DUT_IP=192.168.137.221
SCRIPT_DIR=$(dirname "$0")


if [ "$1" = "help" ];
    then
        echo "Usage: $0 [clear-keys|show-pub|debug]"
        echo "  clear-keys: Clear SSH keys for the DUT IP from known_hosts"
        echo "  show-pub: Show public SSH keys in $SCRIPT_DIR/dut_key.pub for copying to target's authorized_keys"
        echo "  debug: Start a debug session by killing existing gdbserver and starting a new one on the DUT"
        echo "  SCRIPT_DIR: $SCRIPT_DIR"
        exit 1
fi
#clear keys
if [ "$1" = "clear-keys" ];
    then
        echo ""
        echo "Clearing keys..."
        ssh-keygen -f '/home/test/.ssh/known_hosts' -R $DUT_IP
        echo "========================================================================================"
        echo "Make sure to answer \"yes\" when asked to continue connecting..."
        echo "If target is asking for password, make sure test .pub key is in target's authorized_keys"
        echo "========================================================================================"
        echo "\r\n"
fi

if [ "$1" = "show-pub" ];
    then
        echo ""
        echo "Showing test public keys..."
        echo "========================================================================================"
        echo "$SCRIPT_DIR/dut_key.pub"
        cat $SCRIPT_DIR/dut_key.pub
        echo "========================================================================================"
        echo "Copy keys above to target's ~/.ssh/authorized_keys file to enable passwordless ssh and scp"
        echo "\r\n"
        exit 1
fi
#transfer files to DUT
echo "Transfer all ./mepa_app* to $DUT_IP....\r\n"
echo "scp will use key $SCRIPT_DIR/dut_key, make sure it is added to target's authorized_keys\r\n"
scp -i $SCRIPT_DIR/dut_key ./mepa_app* root@$DUT_IP:~/

if [ "$1" = "debug" ];
    then
        echo "Starting debug session...\r\n"
        ssh root@192.168.137.221 'kill -9 $(pidof gdbserver) 2>/dev/null'
        echo "Kill any existing gdbserver instances...\r\n"
        ssh -i $SCRIPT_DIR/dut_key root@192.168.137.221 'gdbserver :9999 ~/mepa_app_main_entry -d /dev/spidev0.1 > /dev/ttyS0'
fi
echo "\r\nDone."