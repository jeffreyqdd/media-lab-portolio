"""
Takes a template CMS grading file and creates a filled CMS grading file
"""
import csv

from src.containerutils import verify_container_files, get_container_info
from src.osutils import safe_env_get

PASSKEY_DIR = safe_env_get("CS3410_PASSKEYS")
COMPOSE_DIR = safe_env_get("CS3410_COMPOSE")


def modify_row(row: dict):
    net_id = row['NetID']
    c_info = get_container_info(PASSKEY_DIR, COMPOSE_DIR, net_id)
    comment = f'ports: {c_info.port}, lab08 password: {c_info.lab08_pwd}, proj3 password: {c_info.proj3_pwd}'
    row['Add Comments'] = comment

    return row


def create_cvs_file():
    # verify files
    verify_container_files(PASSKEY_DIR, COMPOSE_DIR)

    rows = []
    with open('template.csv', 'r') as file:
        template = csv.DictReader(file)
        header = template.fieldnames
        for row in template:
            new_row = modify_row(row)
            if new_row is not None:
                rows.append(new_row)

    with open('assignment.csv', 'w') as file:
        writer = csv.DictWriter(file, fieldnames=header)
        writer.writeheader()
        for row in rows:
            writer.writerow(row)


if __name__ == "__main__":
    create_cvs_file()
