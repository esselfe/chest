/* Like chest.py, this code was generated with ChatGPT.
I simply fed it the chest.py script and asked to implement
it in go. Lately I've been learning a little go and found
this source inspiring.
*/
package main

import (
	"crypto/sha512"
	"fmt"
	"io"
	"os"
	"strings"
)

func hashPassword(password string) []byte {
	hash := sha512.Sum512([]byte(password))
	return hash[:]
}

func processFile(filePath string, password string, mode string) error {
	hashBytes := hashPassword(password)
	hashLen := len(hashBytes)
	hashIndex := 0

	var outputFilePath string
	if mode == "encrypt" {
		outputFilePath = filePath + ".chest"
	} else if mode == "decrypt" {
		if strings.HasSuffix(filePath, ".chest") {
			outputFilePath = strings.TrimSuffix(filePath, ".chest")
		} else {
			return fmt.Errorf("file does not have a .chest extension for decryption")
		}
	} else {
		return fmt.Errorf("invalid mode: %s", mode)
	}

	inputFile, err := os.Open(filePath)
	if err != nil {
		return fmt.Errorf("could not open input file: %v", err)
	}
	defer inputFile.Close()

	outputFile, err := os.Create(outputFilePath)
	if err != nil {
		return fmt.Errorf("could not create output file: %v", err)
	}
	defer outputFile.Close()

	buffer := make([]byte, 1)
	for {
		_, err := inputFile.Read(buffer)
		if err != nil {
			if err == io.EOF {
				break
			}
			return fmt.Errorf("error reading file: %v", err)
		}

		byteValue := buffer[0]
		hashByteValue := hashBytes[hashIndex]

		var outputByteValue byte
		if mode == "encrypt" {
			outputByteValue = byte((int(byteValue) - int(hashByteValue)) % 256)
		} else if mode == "decrypt" {
			outputByteValue = byte((int(byteValue) + int(hashByteValue)) % 256)
		}

		_, err = outputFile.Write([]byte{outputByteValue})
		if err != nil {
			return fmt.Errorf("error writing to output file: %v", err)
		}

		// Increment and wrap the hash index
		hashIndex = (hashIndex + 1) % hashLen
	}

	return nil
}

func main() {
	if len(os.Args) < 2 {
		fmt.Println("Usage: gochest <file>")
		return
	}

	filePath := os.Args[1]
	var password string
	fmt.Print("Enter the password: ")
	fmt.Scanln(&password)

	// Determine mode based on file extension
	mode := "encrypt"
	if strings.HasSuffix(filePath, ".chest") {
		mode = "decrypt"
	}

	err := processFile(filePath, password, mode)
	if err != nil {
		fmt.Println("Error:", err)
	} else {
		fmt.Printf("File successfully processed in %s mode.\n", mode)
	}
}

