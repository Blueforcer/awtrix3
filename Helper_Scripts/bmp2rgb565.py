# Converts a bmp file to a RGB565 array

import argparse
from PIL import Image

def rgb_to_rgb565(r, g, b):
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)

def convert_bmp_to_rgb565_array(file_path):
    # Open image
    image = Image.open(file_path)
    
    # Retrieve pixel data
    pixels = list(image.convert("RGB").getdata())

    # Initialize RGB565 array
    rgb565_array = []

    # Iterate through each pixel and convert to RGB565
    for r, g, b in pixels:
        rgb565_value = rgb_to_rgb565(r, g, b)
        rgb565_array.append(str(rgb565_value))

    # Join the array elements into a string with square brackets
    return "[" + ", ".join(rgb565_array) + "]"

# Create argument parser
parser = argparse.ArgumentParser(description="Convert a BMP file to an RGB565 array string.")
parser.add_argument("file_path", help="Path to the BMP file to be converted.")

# Parse arguments
args = parser.parse_args()

# Convert the BMP file to an RGB565 array string
rgb565_array_string = convert_bmp_to_rgb565_array(args.file_path)

# Print the converted string
print(rgb565_array_string)
