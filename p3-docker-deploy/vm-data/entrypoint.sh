#!/bin/bash

chmod 777 /home/cs3410
chmod -R 777 /home/cs3410/bin
chmod 0777 /home/cs3410/configure
chmod 0777 /home/cs3410/configure/offset
chmod 0777 /home/cs3410/configure/netid
chmod 755 /etc/profile.d


chmod 777 /bind-mount/lab08_egg
chmod 777 /bind-mount/proj3_egg
chmod 777 /bind-mount/springLeak

# disable kernel random VCA
echo 0 | sudo tee /proc/sys/kernel/randomize_va_space

/usr/sbin/sshd -D&
/home/gammaobstacle/spawner.sh&

# kill vscode server installs
while true; do
    if [[ -d "/home/lab08/.vscode-server" ]]; then
        rm -rf /home/lab08/.vscode-server
    fi

    if [[ -d "/home/proj3/.vscode-server" ]]; then
        rm -rf /home/proj3/.vscode-server
    fi

    sleep 5
done

