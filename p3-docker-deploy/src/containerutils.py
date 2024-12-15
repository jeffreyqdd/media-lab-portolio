"""
Utilities for getting information about student containers
"""

import os
import sys
from dataclasses import dataclass

import yaml
import csv

from osutils import safe_env_get, safe_read_file

ROSTER_FILE = safe_env_get("CS3410_ROSTER")


@dataclass
class ContainerInfo:
    """parse container info into this class"""
    port: int
    configure_pwd: str
    lab08_pwd: str
    proj3_pwd: str
    q3_result: str


def get_container_info(passkey_dir: str, compose_dir: str, net_id: str):
    """
    Return student container information as a dict
    """

    def return_pwds(f: str):
        ret = {}
        data = csv.DictReader(f)
        for row in data:
            if row['user'] == 'configure':
                ret['configure'] = row['passwd'].strip()
            elif row['user'] == 'lab08':
                ret['lab08_pwd'] = row['passwd'].strip()
            elif row['user'] == 'proj3':
                ret['proj3_pwd'] = row['passwd'].strip()
            elif row['user'] == 'gammaobstacle':
                ret['q3_result'] = row['passwd'].strip()

        return ret

    def return_ports(f: str):
        return yaml.safe_load(f)['services']['server']['ports'][0].split(':')[0]

    try:
        open(f'{compose_dir}/{net_id}.yml', 'r').close()
        open(f'{passkey_dir}/{net_id}.txt', 'r').close()
    except FileNotFoundError as e:
        print(f'Skipping {net_id}: File not found')
        print(e)
        return None

    passwords = safe_read_file(f'{passkey_dir}/{net_id}.txt', return_pwds)
    ports = safe_read_file(f'{compose_dir}/{net_id}.yml', return_ports)
    container_info = ContainerInfo(ports,
                                   passwords['configure'],
                                   passwords['lab08_pwd'],
                                   passwords['proj3_pwd'],
                                   passwords['q3_result'])

    return container_info


def get_net_ids():
    """
    Returns a list of net_ids from the roster file as a list of strings
    """

    def return_net_ids(f: str):
        data = csv.DictReader(f)
        net_ids = []
        for row in data:
            net_ids.append(row['SIS Login ID'])
        return net_ids

    return safe_read_file(f'{ROSTER_FILE}', return_net_ids)


def verify_container_files(passkey_dir: str, compose_dir: str):
    """
    Verify compose and passkey folders have matching pairs
    """

    compose_files = [x[:-4] for x in os.listdir(compose_dir) if x.endswith('.yml')]
    passkeys_files = [x[:-4] for x in os.listdir(passkey_dir) if x.endswith('.txt')]

    # both lists should have same items
    try:
        assert set(compose_files) == set(passkeys_files), 'compose and passkeys folders do not have matching net_ids'
    except AssertionError as e:
        oops1 = set(compose_files) - set(passkeys_files)
        oops2 = set(passkeys_files) - set(compose_files)
        print(e,
              f'\nnon-matching passkey files to these compose files: {oops1}\nnon-matching compose files to these '
              f'passkey files: {oops2}')
        sys.exit(1)
