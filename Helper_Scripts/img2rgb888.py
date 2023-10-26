# Converts an image to a RGB888 array

import argparse
from PIL import Image

def rgb_to_rgb888(r, g, b):
    return (r << 16) | (g << 8) | (b)

def convert_bmp_to_rgb888_array(file_path):
    # Open image
    image = Image.open(file_path)
    
    # Retrieve pixel data
    pixels = list(image.convert("RGB").getdata())

    # Initialize RGB888 array
    rgb888_array = []

    # Iterate through each pixel and convert to RGB888
    for r, g, b in pixels:
        rgb888_value = rgb_to_rgb888(r, g, b)
        rgb888_array.append(str(rgb888_value))

    # Join the array elements into a string with square brackets
    return "[" + ", ".join(rgb888_array) + "]"

# Create argument parser
parser = argparse.ArgumentParser(description="Convert an image to an RGB888 array string.")
parser.add_argument("file_path", help="Path to the BMP file to be converted.")

# Parse arguments
args = parser.parse_args()

# Convert the BMP file to an RGB888 array string
rgb888_array_string = convert_bmp_to_rgb888_array(args.file_path)

# Print the converted string
print(rgb888_array_string)
