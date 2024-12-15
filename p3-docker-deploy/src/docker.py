def generate_dockerfile(passkey_dir, student):
    dockerfile=f"""
FROM cs3410-base 

# copy setup scripts
COPY vm-data/setup-scripts/ /tmp/scripts/

# setup all the users
# I am so sorry for the command line jank.
# what the long awk thing does is that it cleans whitespace, and gets the second column (password)
# sed -n kp gets the kth line of the output, so the kth password.
COPY {passkey_dir} /tmp/scripts/passkey.txt 

RUN /tmp/scripts/createuser.sh cs3410 $(awk -F, '/,/{{gsub(/ /, "", $0); print}}' /tmp/scripts/passkey.txt | awk -F ',' '{{print $2}}' | sed -n 2p) && \ 
    usermod -aG sudo cs3410 && \ 
    mkdir -p /home/cs3410/configure && \ 
    mkdir -p /home/cs3410/bin && \ 
    /tmp/scripts/createuser.sh configure $(awk -F, '/,/{{gsub(/ /, "", $0); print}}' /tmp/scripts/passkey.txt | awk -F ',' '{{print $2}}' | sed -n 3p) && \ 
    usermod -d /home/configure configure && \ 
    chown -R configure:configure /home/cs3410/configure /home/configure && \ 
    /tmp/scripts/createuser.sh lab08 $(awk -F, '/,/{{gsub(/ /, "", $0); print}}' /tmp/scripts/passkey.txt | awk -F ',' '{{print $2}}' | sed -n 4p) && \ 
    usermod -d /home/lab08 lab08 && \ 
    /tmp/scripts/createuser.sh proj3 $(awk -F, '/,/{{gsub(/ /, "", $0); print}}' /tmp/scripts/passkey.txt | awk -F ',' '{{print $2}}' | sed -n 5p) && \ 
    usermod -d /home/proj3 proj3 && \ 
    /tmp/scripts/createuser.sh alphatrouble $(awk -F, '/,/{{gsub(/ /, "", $0); print}}' /tmp/scripts/passkey.txt | awk -F ',' '{{print $2}}' | sed -n 6p) && \ 
    /tmp/scripts/createuser.sh betastruggle $(awk -F, '/,/{{gsub(/ /, "", $0); print}}' /tmp/scripts/passkey.txt | awk -F ',' '{{print $2}}' | sed -n 7p) && \ 
    /tmp/scripts/createuser.sh gammaobstacle $(awk -F, '/,/{{gsub(/ /, "", $0); print}}' /tmp/scripts/passkey.txt | awk -F ',' '{{print $2}}' | sed -n 8p)

# Add and set up the data files for Lab08
# we symlink the file to the bind-mounted version for persistence
RUN mkdir /bind-mount/ && chmod 777 /bind-mount/

WORKDIR /home/lab08
COPY vm-data/lab08 /home/lab08
RUN mv /home/lab08/egg /bind-mount/lab08_egg && \ 
        ln -s /bind-mount/lab08_egg /home/lab08/egg && \ 
        rm /bind-mount/lab08_egg && \ 
    chown alphatrouble:alphatrouble simplebuffer.c && \ 
    gcc -z execstack -fno-stack-protector -g -o simple simplebuffer.c && \ 
    chown alphatrouble:alphatrouble simple && chmod u+s simple && chmod a+x simple && \ 
    chown lab08:lab08 README simplebuffer.c && \ 
    echo $(awk -F, '/,/{{gsub(/ /, "", $0); print}}' /tmp/scripts/passkey.txt | awk -F ',' '{{print $2}}' | sed -n 6p) >> /home/alphatrouble/secret_pwd

# Add and set the data files of Proj3
# we symlink the file to the bind-mounted version for persistence
# we first set up the beta/ directory
WORKDIR /home/proj3/beta
COPY vm-data/proj3/ /home/proj3/
RUN mv /home/proj3/beta/egg /bind-mount/proj3_egg && \ 
    ln -s /bind-mount/proj3_egg /home/proj3/beta/egg && \ 
    rm /bind-mount/proj3_egg && \ 
    chown proj3:proj3 /home/proj3/beta && \ 
    chown betastruggle:betastruggle sizeconfusion.c && \ 
    gcc -z execstack -fno-stack-protector -g -o size sizeconfusion.c && \ 
    chown betastruggle:betastruggle size && chmod u+s size && chmod a+x size && \ 
    chown proj3:proj3 README sizeconfusion.c && \ 
    echo $(awk -F, '/,/{{gsub(/ /, "", $0); print}}' /tmp/scripts/passkey.txt | awk -F ',' '{{print $2}}' | sed -n 7p) >> /home/betastruggle/secret_pwd

# we now set up the proj/gamma directory
WORKDIR /home/proj3/gamma
RUN mv /home/proj3/gamma/springLeak /bind-mount/springLeak && \ 
    ln -s /bind-mount/springLeak /home/proj3/gamma/springLeak && \ 
    rm /bind-mount/springLeak && \ 
    chown -R proj3:proj3 /home/proj3/gamma

# setup /home/gamma
WORKDIR /home/gammaobstacle
COPY vm-data/spawner.sh /home/gammaobstacle/spawner.sh
RUN cp /home/proj3/gamma/server.c /home/gammaobstacle/server.c && \ 
    cp /home/proj3/gamma/server.h /home/gammaobstacle/server.h && \ 
    gcc -z execstack -fno-stack-protector -o main server.c && \ 
    chown gammaobstacle:gammaobstacle server.c server.h main spawner.sh && \ 
    echo $(awk -F, '/,/{{gsub(/ /, "", $0); print}}' /tmp/scripts/passkey.txt | awk -F ',' '{{print $2}}' | sed -n 8p) >> /home/gammaobstacle/secret_pwd

# Configure everything (offset)
RUN /tmp/scripts/configure.sh {student.netid} \ 
    echo "export PATH=$PATH:/home/cs3410/bin" >> /etc/profile.d/conf_env.sh && \ 
    chmod 0777 /etc/profile.d/conf_env.sh && \ 
    echo "export OFFSET_CMD=\"/home/cs3410/configure/offset\"" >> /etc/profile.d/conf_env.sh && \ 
    echo "export NETID_CMD=\"/home/cs3410/configure/netid\"" >> /etc/profile.d/conf_env.sh && \ 
    cp /tmp/scripts/invoke /home/cs3410/bin/invoke && \ 
    echo "export PATH=$PATH:/home/cs3410/bin" >> /home/configure/.bashrc && \ 
    echo "export PATH=$PATH:/home/cs3410/bin" >> /home/cs3410/.bashrc && \ 
    echo "export PATH=$PATH:/home/cs3410/bin" >> /home/lab08/.bashrc && \ 
    echo "export PATH=$PATH:/home/cs3410/bin" >> /home/proj3/.bashrc && \ 
    echo "export PATH=$PATH:/home/cs3410/bin" >> /home/alphatrouble/.bashrc && \ 
    echo "export PATH=$PATH:/home/cs3410/bin" >> /home/betastruggle/.bashrc && \ 
    echo "export PATH=$PATH:/home/cs3410/bin" >> /home/gammaobstacle/.bashrc

# remove passkey file
RUN rm /tmp/scripts/passkey.txt

# setup ssh
RUN sed -i 's/#PasswordAuthentication yes/PasswordAuthentication yes/' /etc/ssh/sshd_config
RUN mkdir /run/sshd
RUN chmod 0755 /run/sshd && chown root:root /run/sshd

COPY vm-data/entrypoint.sh /launch/entrypoint.sh
ENTRYPOINT ["/launch/entrypoint.sh"]
"""
    return dockerfile

def generate_compose(netid:str, mount_dir : str, port_number:int):
    compose_file=f"""
version: "3"
services:
  server:
    image: cs3410-{netid}
    ports:
      - "{port_number}:22"
    privileged: true
    volumes:
      - {mount_dir}:/bind-mount/
    container_name: cs3410-{netid}
"""
    return compose_file
