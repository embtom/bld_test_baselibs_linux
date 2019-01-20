#/bin/bash

# Kill gdbserver if it's running
REMOTE=192.168.1.40
USER=root

ssh ${USER}@${REMOTE} killall gdbserver &> /dev/null

# Compile myprogram and launch gdbserver, listening on port 9091
scp ./build-RPI/test/test_runner ${USER}@${REMOTE}:/tmp
ssh ${USER}@${REMOTE} "gdbserver :9091 /tmp/test_runner"