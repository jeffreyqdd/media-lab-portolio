import os
import sys
from typing import Callable, Any


def safe_env_get(env_var: str) -> str:
    """ Safe access for an environment variable
    ## Usage:
        safe_env_get("PATH")

    If the variables does not exist, this function terminates the current process. 
    """
    if env_var in os.environ:
        return os.environ.get(env_var)
    else:
        print(f"Environment variable {env_var} not Found. Please refer to README for correct setup.")
        sys.exit(1)


def safe_read_file(filename: str, fun: Callable) -> Any:
    """ opens filename within a context manager, handling for
    FileNotError, and executing return fun(filename)
    """
    try:
        with open(filename, 'r') as file:
            return fun(file)
    except FileNotFoundError as e:
        print(f"Error:\n\t{e}")
        print(f"\t1. Check if file exists\n\t2. Check README for documentation on this file")
        sys.exit(1)


def guard_directory(dir: str, override=False):
    exists = os.path.exists
    listdir = os.listdir
    make = os.makedirs
    if exists(dir) and listdir(dir):
        print(f'{dir} is non empty. Skipping this step.')
        _ = input("[ENTER] to acknowledge and proceed OR [CTRL-C] to stop this program.")
        return False or override
    if not exists(dir):
        os.makedirs(dir)
    return True
