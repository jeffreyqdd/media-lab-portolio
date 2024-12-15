#!/usr/bin/env python3

import os
import sys
import csv
import yaml
import random
import shutil
from typing import List
from dataclasses import dataclass

from docker import generate_dockerfile, generate_compose
from osutils import safe_env_get, safe_read_file, guard_directory

abspath = os.path.abspath
dirname = os.path.dirname
dirjoin = os.path.join

ROSTER_FILE = safe_env_get("CS3410_ROSTER")
PASSKEY_STUB_FILE = safe_env_get("CS3410_PASSKEY_STUBS")


# read files
@dataclass
class Student:
    """parse student csvs into this class"""
    firstname: str
    lastname: str
    id: int
    sisid: int
    netid: str
    section: str


students: List[Student] = safe_read_file(  # what a cool 1-liner
    ROSTER_FILE,
    lambda x: [
        Student(
            firstname=str(entry[0].split(',')[1]).strip(),
            lastname=str(entry[0].split(',')[0]).strip(),
            id=int(entry[1]),
            sisid=int(entry[2]),
            netid=str(entry[3]),
            section=str(entry[4])
        ) for entry in [item for item in csv.reader(x)][1:]
    ]
)

passkey_stubs = safe_read_file(
    PASSKEY_STUB_FILE,
    lambda x: list(
        filter(
            lambda y: len(y) > 0,  # this to filter out the posssible empty newline
            [line.strip() for line in x]
        )
    )
)


def generate():
    root_dir = safe_env_get("CS3410_ROOT")
    passkey_dir = safe_env_get("CS3410_PASSKEYS")
    dockerfile_dir = safe_env_get("CS3410_DOCKERFILES")
    bindmount_dir = safe_env_get("CS3410_BINDMOUNTS")
    compose_dir = safe_env_get("CS3410_COMPOSE")

    if guard_directory(passkey_dir):
        passkey_indices = set()
        for i in range(len(students) * 6):
            a = b = None
            while (a, b) in passkey_indices or a is None:
                a = random.randint(0, len(passkey_stubs) - 1)
                b = random.randint(0, len(passkey_stubs) - 1)
            passkey_indices.add((a, b))
        passkey_indices = list(passkey_indices)
        tuple_to_string = lambda x: passkey_stubs[x[0]] + "-" + passkey_stubs[x[1]]
        for idx, student in enumerate(students):
            cs3410_key = "buffermonkeygrape"
            configure_key = tuple_to_string(passkey_indices[idx * 6])
            lab08_key = tuple_to_string(passkey_indices[idx * 6 + 1])
            proj3_key = tuple_to_string(passkey_indices[idx * 6 + 2])
            alphatrouble_key = tuple_to_string(passkey_indices[idx * 6 + 3])
            betastruggle_key = tuple_to_string(passkey_indices[idx * 6 + 4])
            gammaobstacle_key = tuple_to_string(passkey_indices[idx * 6 + 5])

            with open(dirjoin(
                    passkey_dir,
                    student.netid + ".txt"), 'w') as student_file:
                lines = [
                    "user,passwd,info\n",
                    f"cs3410,           {cs3410_key}, the main user for configuring the VM and has sudo access - students should NEVER have access to this\n",
                    f"configure,        {configure_key}, the user that students will use to reconfigure the VM to their netid\n"
                    f"lab08,            {lab08_key}, the user for doing lab08 - it only contains the info for doing the first problem\n",
                    f"proj3,            {proj3_key}, the user for doing proj3 - it contains the info for betastruggle and gammaobstacle\n",
                    f"alphatrouble,     {alphatrouble_key}, the user that the first question (lab08) is taking over (i.e. spawns a shell as this user)\n",
                    f"betastruggle,     {betastruggle_key}, the user that the second question (proj3) is taking over\n",
                    f"gammaobstacle,    {gammaobstacle_key}, the user that is running the third question (proj3) which has a secret the students need to figure out\n"
                ]
                student_file.writelines(lines)

    if guard_directory(bindmount_dir):
        for idx, student in enumerate(students):
            student_bm_dir = dirjoin(bindmount_dir, student.netid)
            os.makedirs(student_bm_dir)
            shutil.copy(
                dirjoin(root_dir, "vm-data/lab08/egg"),
                dirjoin(student_bm_dir, "lab08_egg")
            )
            shutil.copy(
                dirjoin(root_dir, "vm-data/proj3/beta/egg"),
                dirjoin(student_bm_dir, "proj3_egg")
            )
            shutil.copy(
                dirjoin(root_dir, "vm-data/proj3/gamma/springLeak"),
                dirjoin(student_bm_dir, "springLeak")
            )

    if guard_directory(dockerfile_dir, override=True):
        for student in students:
            curr_student_passkey_dir = dirjoin(passkey_dir, student.netid + ".txt")
            curr_student_dockerfile_file = dirjoin(dockerfile_dir, f"Dockerfile.{student.netid}")
            relative_dir = curr_student_passkey_dir.replace(f"{root_dir}/", "")
            student_dockerfile_src = generate_dockerfile(relative_dir, student)
            with open(curr_student_dockerfile_file, 'w') as file:
                file.write(student_dockerfile_src)

    if guard_directory(compose_dir):
        for idx, student in enumerate(students):
            student_compose_file = dirjoin(compose_dir, student.netid + ".yml")
            student_compose_data = generate_compose(
                student.netid,
                dirjoin(bindmount_dir, student.netid),
                2222 + idx
            )
            with open(student_compose_file, 'w') as file:
                file.write(student_compose_data)


if __name__ == '__main__':
    generate()
