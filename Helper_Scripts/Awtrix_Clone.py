#Clones the ICONS and MELODIES folder from one to another AWTRIX Light

import os
import argparse
import requests

def copy_files(source_ip, target_ip, directory):
    print(f'Copying files from directory {directory}')

    # Retrieve file list from source server
    response = requests.get(f'http://{source_ip}/list?dir=/{directory}')
    file_list = response.json()

    # Iterate through the file list
    for file_info in file_list:
        if file_info['type'] == 'file':
            filename = file_info['name']
            source_file_url = f'http://{source_ip}/{directory}/{filename}'
            target_filename = f'/{directory}/{filename}'
            target_file_url = f'http://{target_ip}/edit?filename={target_filename}'

            # Download the file
            response = requests.get(source_file_url)
            if response.status_code == 200:
                # Upload the file to the target server
                files = {'file': (target_filename, response.content)}
                response = requests.post(target_file_url, files=files)
                print(f'{filename}: {response.text}')
            else:
                print(f'Failed to download file {filename}')

# Create argument parser
parser = argparse.ArgumentParser(description="Copy files from one server to another.")
parser.add_argument("source_ip", help="IP address of the source server.")
parser.add_argument("target_ip", help="IP address of the target server.")

# Parse arguments
args = parser.parse_args()

# Copy files from the directories /ICONS and /MELODIES
copy_files(args.source_ip, args.target_ip, 'ICONS')
copy_files(args.source_ip, args.target_ip, 'MELODIES')
