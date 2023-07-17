package main

import (
	"fmt"
	"image"
	_ "image/png" // Importing for PNG decoding
	"os"
)

func rgbToRGB565(r, g, b uint32) uint16 {
	return uint16(((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3))
}

func convertBmpToRGB565Array(filePath string) ([]uint16, error) {
	// Open the BMP file
	file, err := os.Open(filePath)
	if err != nil {
		return nil, err
	}
	defer file.Close()

	// Decode the BMP image
	imageData, _, err := image.Decode(file)
	if err != nil {
		return nil, err
	}

	// Get the dimensions of the image
	bounds := imageData.Bounds()
	width, height := bounds.Max.X, bounds.Max.Y

	// Initialize the RGB565 array
	rgb565Array := make([]uint16, 0, width*height)

	// Iterate through each pixel and convert to RGB565
	for y := 0; y < height; y++ {
		for x := 0; x < width; x++ {
			r, g, b, _ := imageData.At(x, y).RGBA()
			rgb565Value := rgbToRGB565(r>>8, g>>8, b>>8)
			rgb565Array = append(rgb565Array, rgb565Value)
		}
	}

	return rgb565Array, nil
}

func main() {
	// Retrieve the file path from command-line arguments
	if len(os.Args) < 2 {
		fmt.Println("File path argument is required.")
		return
	}
	filePath := os.Args[1]

	// Convert the BMP file to an RGB565 array
	rgb565Array, err := convertBmpToRGB565Array(filePath)
	if err != nil {
		fmt.Printf("Error: %s\n", err)
		return
	}

	// Print the converted array
	fmt.Printf("%v\n", rgb565Array)
}
