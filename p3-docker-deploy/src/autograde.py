#!/usr/bin/env python3
import math, os, sys, threading, shutil, traceback, csv, time
import argparse

from cryptography.utils import Enum

from src import autogendocker
from src.containerutils import get_container_info, get_net_ids
from src.osutils import safe_env_get, safe_read_file

import subprocess
import paramiko
import time
from scp import SCPClient

# constants for colored output
GRAY="\033[0;30m"
CYAN="\033[0;36m"
RED="\033[1;31m"
BLUE="\033[0;34m"
YELLOW="\033[0;33m"
GREEN="\033[1;32m"
ENDCOLOR="\033[0m"

# Settings for the program
# this includes things like point worths for questions or 
# expected results and whatnot

ROOT_DIR = safe_env_get("CS3410_ROOT")

Q1_RESULT = ["whoami", "alphatrouble"]
Q2_RESULT = ["whoami", "betastruggle"]
Q3_YOU_GUESSED_THE_SECRET_MESSAGE = "You guessed the secret! Good job :)"

HOST = "127.0.0.1"

Q1_EGG_FILENAME = "lab08_egg"
Q2_EGG_FILENAME = "proj3_egg"
Q3_SPRINGLEAK_FILENAME = "springLeak"

P1_WORTH = 15
P2_WORTH = 25
P3_WORTH = 10
TOTAL_AUTOGRADER_POINTS = 50


# partial credits to give for q3
class P3Grade(Enum):
    PASS = 1
    PARTIAL = 0.9
    FAIL = 0


GRADING_COMMENT_FILENAME = "grading_comment.txt"

def comment_string(passed_q1, passed_q2, p3_grade: P3Grade, q1_output, q2_output, q3_output):
    """
    given results on q1, q2, and q3, returns a comment rubric to be used for grading
    """
    msg1 = "succeeded" if passed_q1 else "failed"
    msg2 = "succeeded" if passed_q2 else "failed"
    msg3 = "succeeded" if p3_grade == P3Grade.PASS \
        else "partially succeeded (error in output format/didn't clean secret)" if p3_grade == P3Grade.PARTIAL \
        else "failed"
    p1_points = P1_WORTH if passed_q1 else 0
    p2_points = P2_WORTH if passed_q2 else 0
    p3_points = math.ceil(P3_WORTH * p3_grade.value)
    total_points = p1_points + p2_points + p3_points

    return f"""\
Exploit Correctness ({total_points}/{TOTAL_AUTOGRADER_POINTS})
-------------------------------------------------

alphatrouble ({p1_points}/{P1_WORTH} pts)
    exploit {msg1}

betastruggle ({p2_points}/{P2_WORTH} pts)
    exploit {msg2}

gammaobstacle ({p3_points}/{P3_WORTH} pts)
    exploit {msg3}

Output From Running Scripts
-------------------------------------------------

alphatrouble output
-------------------
{q1_output}
-------------------

betastruggle output
-------------------
{q2_output}
-------------------

gammaobstacle output
-------------------
{q3_output}
-------------------
"""

def grade(host, net_id, port, lab08PW, proj3PW, q3_result, dir_to_grade, verbose):
    def test_q1(egg_path):
        """
        returns (True, output) if submission is correct, else (False, output) from the script
        """
        # assert that the file exists
        try:
            open(egg_path)
        except:
            print(f"\033[0;33mAlert:\033[0msubmission for {net_id} q1 not found")
            return False, "script not found"
        ssh = paramiko.SSHClient()
        ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
        ssh.connect(host, port=port, password=lab08PW, username="lab08")
        scp = SCPClient(ssh.get_transport())
        scp.put(egg_path, "egg")
        shell = ssh.invoke_shell()
        time.sleep(1)
        shell.send("cd /home/lab08/\n")
        shell.recv(1000)
        shell.send("(./egg && cat) | invoke simple\n")
        time.sleep(1)
        shell.recv(1000)
        shell.send("whoami\n")
        time.sleep(1)
        output = shell.recv(1000).decode()
        output = output.replace("\033[?2004l", "")
        output = output.replace("\033[?2004h", "")
        result = output.splitlines()
        shell.close()
        ssh.close()

        # check that result is correct
        return result == Q1_RESULT, output

    def test_q2(egg_path):
        """
        returns (True, output) if submission is correct, else (False, output) from the script
        """
        try:
            open(egg_path)
        except:
            print(f"\033[0;33mAlert:\033[0msubmission for {net_id} q2 not found")
            return False, "script not found"
        ssh = paramiko.SSHClient()
        ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
        ssh.connect(host, port=port, password=proj3PW, username="proj3")
        scp = SCPClient(ssh.get_transport())
        scp.put(egg_path, "/home/proj3/beta/egg")
        shell = ssh.invoke_shell()
        time.sleep(1)
        shell.send("cd /home/proj3/beta/\n")
        shell.recv(1000)
        shell.send("(./egg && cat) | invoke size\n")
        time.sleep(1)
        shell.recv(1000)
        shell.send("whoami\n")
        time.sleep(1)
        output = shell.recv(1000).decode()
        output = output.replace("\033[?2004l", "")
        output = output.replace("\033[?2004h", "")
        result = output.splitlines()
        shell.close()
        ssh.close()

        # check that result is correct
        return result == Q2_RESULT, output

    def test_q3(script_path):
        """
        returns a tuple (p3grade, output) where p3grade is the student's grade
        as a P3Grade and output is the output string which the server returns
        when the student's script is graded
        """
        try:
            open(script_path)
        except:
            print(f"\033[0;33mAlert:\033[0msubmission for {net_id} q3 not found")
            return P3Grade.FAIL, "script not found"
        ssh = paramiko.SSHClient()
        ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
        ssh.connect(host, port=port, password=proj3PW, username="proj3")
        scp = SCPClient(ssh.get_transport())
        scp.put(script_path, "/home/proj3/gamma/springLeak")
        shell = ssh.invoke_shell()
        time.sleep(1)
        shell.send("cd /home/proj3/gamma/\n")
        time.sleep(1)
        shell.recv(1000)
        shell.send("./springLeak\n")
        time.sleep(4)
        output = shell.recv(1000).decode()
        output = output.replace("\033[?2004l", "")
        output = output.replace("\033[?2004h", "")
        result = output.splitlines()
        shell.close()
        ssh.close()

        # full credit
        if len(result) >= 2 and Q3_YOU_GUESSED_THE_SECRET_MESSAGE == result[2]:
            return P3Grade.PASS, output
        else:
            return P3Grade.FAIL, output

    submission_dir = os.path.join(dir_to_grade, net_id)
    try:
        q1_passed, output1 = test_q1(os.path.join(submission_dir, Q1_EGG_FILENAME))
        q2_passed, output2 = test_q2(os.path.join(submission_dir, Q2_EGG_FILENAME))
        q3_grade, output3 = test_q3(os.path.join(submission_dir, Q3_SPRINGLEAK_FILENAME))
    except Exception as e:
        print(f"{YELLOW}Alert:{ENDCOLOR} failed to grade {CYAN}{net_id}{ENDCOLOR}'s submission: " + str(e))
        if verbose:
            traceback.print_tb(e.__traceback__)
        return

    comment = comment_string(q1_passed, q2_passed, q3_grade, output1, output2, output3)
    try:
        with open(os.path.join(submission_dir, GRADING_COMMENT_FILENAME), "w") as fout:
            fout.write(comment)
    except:
        print(
            f"{YELLOW}Alert:{ENDCOLOR} cannot find file {os.path.join(submission_dir, GRADING_COMMENT_FILENAME)}\n" \
            "maybe the roster set in the environment variables doesn't match the submissions?"
        )
def get_info_dict():
    """
    Returns a nested dictionary of ContainerInformation objects
    """

    info_dict = {}
    for net_id in get_net_ids():
        info_dict[net_id] = get_container_info(
            safe_env_get(f"CS3410_PASSKEYS"), safe_env_get(f"CS3410_COMPOSE"), net_id
        )
    return info_dict

def build_containers():
    PROCESS_TIMEOUT = 3600

    result_base = subprocess.run(
        "trogdor build-base", shell=True, text=True, timeout=PROCESS_TIMEOUT
    )
    if result_base.returncode != 0:
        print(f"trogdor build-base failed: {result_base.stderr}")
        stop_containers()
        sys.exit(1)

    result_build = subprocess.run(
        "trogdor build-student", shell=True, text=True, timeout=PROCESS_TIMEOUT
    )
    if result_build.returncode != 0:
        print(f"trogdor build-student failed: {result_build.stderr}")
        stop_containers()
        sys.exit(1)


def start_containers(net_ids):
    PROCESS_TIMEOUT = 3600

    for net_id in net_ids:
        result_start = subprocess.run(
            "trogdor start " + net_id, shell=True, text=True, timeout=PROCESS_TIMEOUT
        )
        if result_start.returncode != 0:
            print(f"trogdor start failed: {result_start.stderr}")
            sys.exit(1)
    time.sleep(0.2)


def stop_containers():
    PROCESS_TIMEOUT = 3600
    result_stop = subprocess.run(
        "yes | trogdor stop", shell=True, text=True, timeout=PROCESS_TIMEOUT
    )
    if result_stop.returncode != 0:
        print(f"trogdor stop failed: {result_stop.stderr}")
        sys.exit(1)


def grade_all(rebuild_containers, dir_to_grade, net_id_pool, verbose):
    """
    Grades all containers in parallel
    """
    print(f"{CYAN}autograder:{ENDCOLOR}Setting up grading environment...")
    # save curr env variables
    curr_cs3410_passkeys = safe_env_get("CS3410_PASSKEYS")
    curr_cs3410_dockerfiles = safe_env_get("CS3410_DOCKERFILES")
    curr_cs3410_bindmounts = safe_env_get("CS3410_BINDMOUNTS")
    curr_cs3410_compose = safe_env_get("CS3410_COMPOSE")

    # set env variables to tmp
    os.environ["CS3410_PASSKEYS"] = f"{ROOT_DIR}/assets/tmp/passkeys"
    os.environ["CS3410_DOCKERFILES"] = f"{ROOT_DIR}/assets/tmp/dockerfiles"
    os.environ["CS3410_BINDMOUNTS"] = f"{ROOT_DIR}/assets/tmp/bindmounts"
    os.environ["CS3410_COMPOSE"] = f"{ROOT_DIR}/assets/tmp/compose"

    setup_successful = True

    if rebuild_containers:
        # remove old generated files
        shutil.rmtree(os.path.join(ROOT_DIR, "assets/tmp"), ignore_errors=True)

        print(f"{CYAN}autograder:{ENDCOLOR}Generating new passwords...")
        # generate tmp docker container passwords
        autogendocker.generate()
        print(f"{CYAN}autograder:{ENDCOLOR}Building all containers (this may take a while)...")
        try:
            build_containers()
        except subprocess.TimeoutExpired as e:
            print(f"{RED}error:{ENDCOLOR} {str(e)}")
            setup_successful = False

    print(f"{CYAN}autograder:{ENDCOLOR}Starting all containers...")
    info_dict = get_info_dict()
    if net_id_pool:
        info_dict = {key: info_dict[key] for key in net_id_pool}
    net_ids = info_dict.keys()
    try:
        start_containers(net_ids)
    except subprocess.TimeoutExpired as e:
        print(f"{RED}error:{ENDCOLOR} {str(e)}")
        setup_successful = False
    time.sleep(5)
    # run barry's script for every student in parallel
    if setup_successful:
        print(f"{CYAN}autograder:{ENDCOLOR}Grading files (this may take a while)...")
        threads = []
        for net_id in info_dict:
            curr_info = info_dict[net_id]
            t = threading.Thread(
                target=grade,
                args=(
                    HOST,
                    net_id,
                    curr_info.port,
                    curr_info.lab08_pwd,
                    curr_info.proj3_pwd,
                    curr_info.q3_result,
                    dir_to_grade,
                    verbose
                ),
            )
            t.start()
            threads.append(t)
        for t in threads:
            t.join()

    print(f"{CYAN}autograder:{ENDCOLOR}Stopping all containers...")
    stop_containers()

    print(f"{CYAN}autograder:{ENDCOLOR}Restoring Environment...")
    # restore env variables
    os.environ["CS3410_PASSKEYS"] = curr_cs3410_passkeys
    os.environ["CS3410_DOCKERFILES"] = curr_cs3410_dockerfiles
    os.environ["CS3410_BINDMOUNTS"] = curr_cs3410_bindmounts
    os.environ["CS3410_COMPOSE"] = curr_cs3410_compose

    if setup_successful:
        print(f"{GREEN}Finished:{ENDCOLOR} Automatic grading terminated")
        print(
            f"{YELLOW}Important{ENDCOLOR}: An error in when grading an individual file may still have occured\n"
            "check grading_comments for existence to make sure stuff looks sane"
        )
    else:
        print(f"{RED}Failure:{ENDCOLOR} an error occured, no files graded")


# run only if main
if __name__ == "__main__":
    # Argument parsing
    parser = argparse.ArgumentParser(
        prog='autograde',
        description="given submission, grades the autograded portion",
        epilog=f'enjoy the grading script {RED}<3{ENDCOLOR}'
    )

    parser.add_argument('-d', '--dir', help="DIR specifies the directory of submissions to grade", required=True)
    parser.add_argument('-v', '--verbose', help="when verbose is set, extra error information is printed", action='store_true')
    parser.add_argument('-p', '--netid_pool',
    help='''
    specify a one line csv file, NETID_POOL, of netids;
        only netids in this file will be graded. default=netids in roster.csv 
        (note roster.csv is NOT in the format of a netid_pool csv, it is special cased) ''')
    parser.add_argument('-b', '--build', action="store_true", 
    help='''
    build new docker containers, if no docker files exist
         this should be run at least once after taking down the project 
         to stop people from hardcoding answers''')

    args = parser.parse_args()
    def return_net_ids(f: str):
        data = csv.reader(f)
        return next(data)

    # this is None if -p is not secified, else the array
    if args.netid_pool:
        net_id_pool = safe_read_file(args.netid_pool, return_net_ids)
    else:
        net_id_pool = None

    grade_all(args.build, args.dir, net_id_pool, args.verbose)
