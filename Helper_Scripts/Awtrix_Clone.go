package main

import (
	"fmt"
	"io/ioutil"
	"net/http"
	"os"
	"bytes"
	"encoding/json"
)

func copyFiles(sourceIP, targetIP, directory string) {
	fmt.Printf("Copying files from directory %s\n", directory)

	// Retrieve file list from source server
	response, err := http.Get(fmt.Sprintf("http://%s/list?dir=/%s", sourceIP, directory))
	if err != nil {
		fmt.Printf("Failed to retrieve file list from source server: %s\n", err.Error())
		return
	}
	defer response.Body.Close()

	// Read the response body
	body, err := ioutil.ReadAll(response.Body)
	if err != nil {
		fmt.Printf("Failed to read response body: %s\n", err.Error())
		return
	}

	// Parse the JSON response
	fileList := make([]map[string]interface{}, 0)
	err = json.Unmarshal(body, &fileList)
	if err != nil {
		fmt.Printf("Failed to parse JSON response: %s\n", err.Error())
		return
	}

	// Iterate through the file list
	for _, fileInfo := range fileList {
		fileType, ok := fileInfo["type"].(string)
		if !ok {
			fmt.Println("Failed to get file type")
			continue
		}

		if fileType == "file" {
			filename, ok := fileInfo["name"].(string)
			if !ok {
				fmt.Println("Failed to get filename")
				continue
			}

			sourceFileURL := fmt.Sprintf("http://%s/%s/%s", sourceIP, directory, filename)
			targetFilename := fmt.Sprintf("/%s/%s", directory, filename)
			targetFileURL := fmt.Sprintf("http://%s/edit?filename=%s", targetIP, targetFilename)

			// Download the file
			response, err := http.Get(sourceFileURL)
			if err != nil {
				fmt.Printf("Failed to download file %s: %s\n", filename, err.Error())
				continue
			}
			defer response.Body.Close()

			if response.StatusCode == http.StatusOK {
				// Read the file content
				fileContent, err := ioutil.ReadAll(response.Body)
				if err != nil {
					fmt.Printf("Failed to read file content: %s\n", err.Error())
					continue
				}

				// Upload the file to the target server
				response, err := http.Post(targetFileURL, "application/octet-stream", bytes.NewBuffer(fileContent))
				if err != nil {
					fmt.Printf("Failed to upload file %s: %s\n", filename, err.Error())
					continue
				}

				fmt.Printf("%s: %s\n", filename, response.Status)
			} else {
				fmt.Printf("Failed to download file %s: %s\n", filename, response.Status)
			}
		}
	}
}

func main() {
	// Parse command-line arguments
	if len(os.Args) != 3 {
		fmt.Println("Usage: awtrix_clone source_ip target_ip")
		return
	}

	sourceIP := os.Args[1]
	targetIP := os.Args[2]

	copyFiles(sourceIP, targetIP, "ICONS")
	copyFiles(sourceIP, targetIP, "MELODIES")
}
