#!/usr/bin/env python3

import os
import time
import argparse
import subprocess

import numpy as np

from pathlib import Path

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('dir', nargs='?', type=str, help='directory of .dat files')
    parser.add_argument('exec', nargs='?', type=str, help='path to executable ptree')

    parser.add_argument('--build-debug', action='store_true')
    parser.add_argument('--build-release', action='store_true')
    parser.add_argument('--build-reldebug', action='store_true')
    parser.add_argument('--all', action='store_true')
    parser.add_argument('--test', action='store_true')
    parser.add_argument('--args', nargs='+', help='space deliminated arguments to pass to executable')

    args = parser.parse_args()

    if args.build_debug or args.all or args.test:
        ret = subprocess.call(['bash', 'build.sh', 'Debug'])
        if ret != 0:
            exit(ret)

    if args.build_release or args.all:
        ret = subprocess.call(['bash', 'build.sh', 'Release'])
        if ret != 0:
            exit(ret)

    if args.build_reldebug or args.all:
        ret = subprocess.call(['bash', 'build.sh', 'RelWithDebInfo'])
        if ret != 0:
            exit(ret)
    
    if args.test:
        subprocess.call(['make', '-C', 'build-Debug', 'test'])
        exit(0)


    if args.dir is None or args.exec is None:
        exit(0)

    species_dir = Path(args.dir).absolute()
    exec_path = Path(args.exec).absolute()

    assert species_dir.exists() and species_dir.is_dir(), "species dir does not exist"
    assert exec_path.exists() and exec_path.is_file(), "executable does not exist"


    NUM_IT = 300
    measured_times = np.zeros((NUM_IT, ))

    with open(os.devnull, 'w') as devnull:
        for it in range(NUM_IT):
            start = time.monotonic()
            subprocess.call([str(exec_path)] + args.args, cwd=str(species_dir), stdout=devnull, stderr=devnull)
            end = time.monotonic()
            measured_times[it] = end - start

    print(f"fastest time: {np.min(measured_times):6.4f}")
    print(f"slowest time: {np.max(measured_times):6.4f}")
    print(f"average time: {np.mean(measured_times):6.4f}")
    print(f"p95:          {np.percentile(measured_times, 95):6.4f}")
    print(f"p99:          {np.percentile(measured_times, 99):6.4f}")
    print(f"mean:         {np.mean(measured_times):6.4f}")
