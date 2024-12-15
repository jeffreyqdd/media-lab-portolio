# Deploy Docker
This is the Fa2023 rework of the P3-BufferOverflow project. The purpose of this project is to give students `scp` capabilities so they can download their solutions onto their local system and submit it. This rework uses docker to create a custom environment for every student. 

> IMPORTANT NOTE: I took the time to create this detailed writeup after designing this system. *It benefits you enormously to read this writeup carefully*. It should provide you with a good understanding of this system, enough to deploy and maintain it.

## Acknowledgments
This project was a massive undertaking and could not be possible without the help of the Chris Fouracre from IT. Super responsive and knowledgeable. He was an amazing mentor through this process. We truly appreciate it.

# Brief Architecture
## Terminology
This documentation references the following terms:
1. **Docker image**: A snapshot of a VM
2. **Docker container**: An instance of a docker image. Think of the docker image as a class and the docker container as an instance of that class. 
3. **Host/Server**: The device that hosts *this* software stack and provides SSH access.

This project should be cloned onto a server that CS3410 students can access via SSH.  **The students should not download or interact with this code in any manner**. 
> IMPORTANT NOTE: This software stack should be cloned into a home directory that students cannot get access to. I suggest cloning it under the root user in /root/.

> IMPORTANT NOTE: This project requires the binary to be compiled using `execstack`, which was broken by an update to the linux kernel between versions `5.4-5.8`. The Linux server should run ubuntu 20.04 with the 5.4 kernel. 
>
> The host kernel is important because docker uses the host kernel's functionality.


## Example student workflow
> I have no originality, I get my names from [usaco.org](https://www.usaco.org)

This will be a the typical use case. Consider a singular student, Bessie The Cow, (the cow is her last name). Bessie requires access to the following users:
1. lab08
2. alphatrouble
3. configure (tentative, configuration is now done automatically)
4. proj3
5. betastruggle
6. gammaobstacle

Each user will have their own password for example (lab08's password is password). Bessie's development environment is on port 2222. Thus, Bessie can ssh into Lab08 with the following command `ssh lab08@3410containers.cs.cornell.edu -p 2222`.

After Bessie successfully executes the exploit, she obtain alphatrouble's password which is doubletrouble. She can log into alphatrouble's account using the following command `ssh alphatrouble@3410containers.cs.cornell.edu -p 2222`.

## Students in 3410
Assume there are 400 students in CS3410. We will need to build and deploy 400 docker containers on a server accessible using ssh. Each container can be accessed through `ssh lab08@cs3410containers.cs.cornell.edu -p <port>`. We will open ports 2222 through 2621 so that each student gets their own port.

Say Farmer John's port is 2223, he can access the lab08 user of his container using the command `ssh lab08@cs3410containers.cs.cornell.edu -p 2223`. If Bessie is assigned port 2222, what's stopping Farmer John from using `-p 2222` to access Bessie's work (if both lab08 passwords are 'password')?

To avoid the above scenario, we need to employ the following:
1. Still have 400 docker containers, each running on their own port
2. Have each user have a different password.

## Architectural Overview
So we've identified 4 areas that need to be covered.
1. 400 unique docker images
2. 400 unique passwords
3. 400 unique bindmounts
4. 400 unique container launch configurations

#### I. Docker Containers:
To build a docker image, you need a dockerfile. This process is deterministic, meaning the same dockerfile will always produce an identical image. The question here is how can we customize each image to each student? 
1. We factor out shared libraries and setup to a base dockerfile called `Dockerfile.base`
2. We take advantage of [multi-stage builds](https://docs.docker.com/build/building/multi-stage/) and generate 400 dockerfiles with the student-level customization. We name each customized dockerfile `Dockerfile.<netid>`

#### II. Passwords
The passkeys will be generated under a folder in this stack. Because this project is stored under the root user, students will **never** have access to these credentials. However, the staff can go in and refer to them for troubleshooting purposes
1. When students SSH, they go directly into the docker container
2. Staff can SSH into the root user and perform management work.

#### III. Bindmounts
Docker containers are ephemeral, meaning that when the server goes down, or if the docker container is stopped for whatever reason, all the data inside the container is lost. This project uses **bindmounts** to resolve that issue. We have a folder called `/bindmount/`in the docker container that points to `/path/to/bindmount/directory/netid` on the server. Changes to files within the `/bindmount/` directory will be reflected on the file system of the server, which is persistent and can be backed up. 

There are 3 files that will be bindmounted
1. egg (for lab08)
2. egg (for proj3/beta)
2. springLeak (for proj3/gamma)

#### IV. Launch Configurations
This project uses [docker compose](https://docs.docker.com/compose/) files to store launch configurations for the container. This ensures that each student will have their own port, and that the correct bindmounts are attached during startup.  

# Installation and Setup 
> The command `sudo su` will log into the root user. Also ensure the root user is added to the docker group via `usermod -aG docker $USER`. After the group permissions are added, log out and back in to revaluate group permissions.

### Step 0: Installing the stack
You should clone this repository to the `$HOME` directory of your current user. The directory structure should look like `/home/<user>/p3-deploy-docker`. The recommended clone location is `/root/p3-deploy-docker`.

### Step 1: Creating a Python Environment
The stack uses python to generate passkeys, dockerfile, bindmounts, and compose files. It should work with any python version `3.8` and higher. In the project root, run the following commands:
1. `python3 -m venv .env`
2. `source .env/bin/activate` 
3. `pip3 install -r requirements.txt`

### Step 1.5: Importing Configuration Files
The only configuration file that should be imported is the `.csv` file that contains all the students enrolled in 3410. The recommended location is under the `assets/` folder. Below is a an example csv file.
```
Student,ID,SIS User ID,SIS Login ID,Section
"Bessie, Bessie",11111,1111111,bb1,DIS and LEC
"Elsie, Elsie",11112,1111112,ee1,DIS and LEC
"Buttercup, Buffercup",11113,1111113,dd1,DIS and LEC
"John, Farmer",11114,1111114,fj54,DIS and LEC
```

### Step 2: Configuring Environment Variables
This stack uses environment variables for flexible configuration. Customize these variables in `environment.sh`. 

```shell
#!/bin/bash

# CHANGE THIS TO THE DIRECTORY YOU CLONE IT TO
export CS3410_ROOT=$HOME/p3-docker-deploy

# path to the directory holding the autogenerated passkeys 
export CS3410_PASSKEYS="$CS3410_ROOT/assets/passkeys"

# path to the directory holding autogenerated dockerfiles
export CS3410_DOCKERFILES="$CS3410_ROOT/assets/dockerfiles"

# path to the directory holding all the bindmounts
export CS3410_BINDMOUNTS="$CS3410_ROOT/assets/bindmounts"

# path to the directory holding all the autogenerated compose files
export CS3410_COMPOSE="$CS3410_ROOT/assets/compose"

# path to csv file containing student data. Should have elements
# Student, ID, SIS User ID, SIS Login ID, Section
export CS3410_ROSTER="$CS3410_ROOT/assets/roster.csv"

# path dictionary (text file with 1 word on each line) used to generate
export CS3410_PASSKEY_STUBS="$CS3410_ROOT/assets/passkey-stubs.txt"

# ensures global access of maintenance scripts (don't need to change)
export PATH=$CS3410_ROOT/bin:$PATH

# setting python search path (don't need to change)
export PYTHONPATH=$CS3410_ROOT
```
This is project is supposed to be deployed on a linux server. You should run:
- `echo "source /<path>/<to>/environment.sh" >> ~/.bashrc` 

However, if you are developing on a Mac and have no access to a linux system:
- `echo "source /<path>/<to>/environment.sh" >> ~/.zshrc` 

The above guarantees that any new shell instance will have the correct environment configuration.

Note that we have roster.csv file to create docker containers for each student. Feel free to add dummy students to test the system.

Now, close out of your terminal window and open a new one. If the setup was correct, then entering the command

```
env | grep CS3410
```

Should output something similar to this:

```
CS3410_DOCKERFILES=/root/p3-docker-deploy/assets/dockerfiles
CS3410_PASSKEY_STUBS=/root/p3-docker-deploy/assets/passkey-stubs.txt
CS3410_BINDMOUNTS=/root/p3-docker-deploy/assets/bindmounts
CS3410_ROOT=/root/p3-docker-deploy
CS3410_COMPOSE=/root/p3-docker-deploy/assets/compose
CS3410_PASSKEYS=/root/p3-docker-deploy/assets/passkeys
CS3410_LOG=/root/p3-docker-deploy/logs
CS3410_ROSTER=/root/p3-docker-deploy/assets/roster.csv
```

### Step 3: Generating Stubs
This step is very easy. Assuming you did the above two steps correctly, all you 
need to run is `./src/autogen-docker.py`.

If any error messages arise, go through the following debugging steps:
1. Are the environment variables in `environment.sh` in your current shell environment?
   - If python cannot import `src.docker` or `src.utils`, check if PYTHONPATH is being set correctly.
2. Ensure the environment variables are correct (e.g. no typos in the path)
3. Ensure the files the environment variables point exist. 
4. Inspect error and inspect the code.

If no errors occur, you should see 4 new populated folders with the following environment names:
1. `$CS3410_COMPOSE`
2. `$CS3410_PASSKEYS`
3. `$CS3410_BINDMOUNTS`
4. `$CS3410_DOCKERFILES`


### Step 4: Building and Deploying
Now it's time to introduce you to your new best friend: `trogdor` Trogdor is the docker service management system will make maintaining the infrastructure easier. Parts of trogdor were taken from the CUAUV software stack (shameless plug). Trogdor lives in `project_root/bin`. Say hi to him or else he'll burn your docker containers. 
[Origins of Trogdor](https://youtu.be/90X5NJleYJQ).

> I recommend adding `alias t=trogdor` to the .bashrc or .zshrc. The documentation assumes that you do that.

`t` by itself will bring up the help menu
```
root@3410containers:~/p3-docker-deploy# t

Usage: trogdor [COMMAND] ...
    build-base                  build base image
    build-student <?netid...>   build image for all specified students. default=all
    status <?netid...>          display container status for all specified students. default=summary
    start <?netid...>           start container for all specified students. default=all
    stop <?netid...>            stop container for all specified students. default=all
    restart <?netid...>         restart container for all specified students. default=all
    info <netid...>             display sensitive container information for all specified students
    grade <netid...>            grade container for all specified students
    help                        display this help message and exit
```

Run `t build-base` to build the base image. Run `t build-student` to build all student images (this will take a while, so sit back and relax).

Finally, before you can deploy, you need to expose ports on the server. (The following is a snippet from one of our emails with Chris:)
```
To do that, I modify this line:
-A INPUT -p tcp --dport 2222:2622 -j ACCEPT

In this file:
 /etc/iptables/rules.v4

Then run:
sudo iptables-restore /etc/iptables/rules.v4

Then restart the docker service so that it restores its entries in iptables:
sudo systemctl restart docker.service
```

There's an well-known issue where docker runs out of ports or IP addresses. THis prevents the stack from launching all 400. We need to modify `/etc/docker/daemon.json`. If the file doesn't exist, create it. Then add the following entry:
```json
{
    "bip": "10.254.1.1/24",
    "default-address-pools" : [{"base": "10.254.0.0/16", "size": 28}]
}
```
After adding the line, run `systemctl restart docker`

### Step 5: Maintenance 
Other commands should be used for maintenance as needed.

Different Scenario Use Cases:

1. Students with netids jj578 and jq54 broke the configuration, so their containers need to be restarted:  <br />
`t restart jj578 jq54`

2. Students jj578 and jq54 need their containers rebuilt:  <br />
`t build-student jj578 jq54`  <br />
`t restart jj578 jq54`

3. We (Course Staff) roll out an docker update and ALL containers need to be rebuild and restarted:  <br />
`t build-student`  <br />
`t restart`

4. We (Course Staff) need to run the autograder on students jj578 and jq54:  <br />
`t grade jj578 jq54`

5. We (Course Staff) want to see student credentials and launch configurations <br />
`t info jq54`

### Step 6: Giving Students Access
**Issue**: Need to give hundreds of students their ports and passwords

**Solution**: Use CMS assignments in unconventional ways
> Create CMS Assignment whose sole purpose is to give ports and passwords out to students

1. Create assignment with 0 score and 0 weight (should not count towards final score) and nothing else
2. Go to "Grading" tab for that assignment
3. Download template containing individual scores (at very bottom of page)
4. Rename that file to be `template.csv`
5. Place template in `CS3410_ROOT` directory
6. Run `./src/to-csv.py` to generate csv file `assignemnt.csv`
7. Go to "Grading" tab for the assignment
8. Upload `assignemnt.csv` to "grade" assignment

This will give students access to their own ports and passwords through CMS 

# Grading
## Setup
First make sure that Python and your environment variables are set up as describe in steps 0-2 above.

## Usage
### Parameters
`autograde.py` is your tool for grading submissions. For usage see ./src/autograde.py -h
For convience, here it is:
```
usage: autograde [-h] -d DIR [-p NETID_POOL] [-b]

given submission, grades the autograded portion

optional arguments:
  -h, --help            show this help message and exit
  -d DIR, --dir DIR     DIR specifies the directory of submissions to grade
  -v, --verbose         when verbose is set, extra error information is printed
  -p NETID_POOL, --netid_pool NETID_POOL
                        specify a one line csv file, NETID_POOL, of netids; only netids in this file
                        will be graded. default=netids in roster.csv (note roster.csv is NOT in the
                        format of a netid_pool csv, it is special cased)
  -b, --build           build new docker containers, if no docker files exist this should be run at
                        least once after taking down the project to stop people from hardcoding
                        answers
enjoy the grading script <3
``` 

Hopefully `--build`, `--verbose`, and `--help` are clear enough. One thing to note is rebuilding containers help if there is an alert saying "Authentication Error". 

`--netid_pool` is useful when regrading a single submission or for handling regrade requests without having to regrade every student. An example csv file follows:
```
tt1,tt2,tt3,jq54,jk2582
```
In the above, only submissions for `tt1`, `tt2`, etc. will be graded. If their submission folder don't exist, that is okay and it is simply the case, the extra netid(s) does nothing.

### Submission Format
Now for the most complicated part. 
`DIR` contains student submissions and should be in the following structure:
```
DIR/
|-- jq54/
|  |-- q1_egg
|  |-- q1_explanation.pdf
|  |-- q2_egg
|  |-- q2_explanation.pdf
|  |-- springLeak
|  |-- q3_explanation.pdf
|-- jj578/
|  |-- q1_egg
|  |-- q1_explanation.pdf
|  |-- q2_egg
|  |-- q2_explanation.pdf
|  |-- springLeak
|  |-- q3_explanation.pdf
.
.
.
```

Here `DIR` is directory containing subdirectories `jq54` and `jj578`, netids of students in the course. 

Each subdirectory contains the files the student submitted (without the windows endline shenanigans). They should exactly be named as shown. cmsx might give your eggs extensions you don't want so here is a script which could help out a bit (it does require dos2unix to do the obvious thing). I use the name `Submissions` instead of `DIR` below as that is cmsx's convention so it should make the copy paste easier:
```
find Submissions/ -name '*.tar.gz' | while read NAME ; do tar -xvf "$NAME" -C $(dirname $NAME); done
```
This command will change as the submission format changes, but of course you knew that.

The script will then check that no docker container is currently running and if so it will do the following:
1. (if -b is set) Generate *new* passwords (and other docker container information) in `assets/tmp/` **overwriting previously generated information if it exists**.
2. Start all docker containers with new information.
3. Run the test each students submission on their docker container.
4. Collect the outputs into back into `src`
5. Stop all docker containers.

After running `DIR` will look like the following:
```
DIR/
|-- jq54/
|  |-- grading_comment.txt
|  |-- lab08_egg
|  |-- q1_explanation.pdf
|  |-- proj3_egg
|  |-- q2_explanation.pdf
|  |-- springLeak
|  |-- q3_explanation.pdf
|-- jj578/
|  |-- grading_comment.txt
|  |-- lab08_egg
|  |-- q1_explanation.pdf
|  |-- proj3_egg
|  |-- q2_explanation.pdf
|  |-- springLeak
|  |-- q3_explanation.pdf
.
.
.
```
`grading_comment.txt` contains the grades for the autograded section of p3, formated to be easily updated with manual grades and copy pasted directly into the cmsx's comment section. 

## Details for Maintainers 
### WARNING
**`autograde.py` CALLS `trogdor start`, `trogdor stop`, `trogdor build-student`, and `trogdor build-base` AND USES THE RETURN VALUE OF `trogdor start`. CHANGES TO `trogdor` MAY BREAK THIS SCRIPT**

Hopefully the script is neat and simple enough that these breaks should not be too hard to fix.

### Settings
Settings are specified as global variables in the beginning of the autograder script. Here is a list of them:
- the colors are just colors for output
- `ROOT_DIR`: the root directory for the p3 project, should match the environment variable
- `Q1_RESULT, Q2_RESULT`: strings which should prefix the output of a correct solution to Q1 or Q2
- `HOST`: the host the computer will try to connect to when sshing into containers, you almost certainly want this to loopback to the server the computer is on as that is the computer we are starting and stopping containers on.
- `Q[1|2]_EGG_FILENAME, Q3_SPRINGLEAK_FILENAME`: the filenames of the respective submission files
- `P[1|2|3]_WORTH`: the points each part is worth
- `TOTAL_AUTOGRADER_POINTS`: the total points possibly awarded by the autograder, should probably be the sum of the points from each individual part
- the enum is used for giving partial credit for question 3 if they messed up isolating their solution, the float specifying how much a partially correct solution is worth
- `GRADING_COMMENT_FILENAME`: the filename of the output of the autograder to be placed in each students submission directory

