/* Like chest.py, this code was generated with ChatGPT.
I simply fed it the chest.py script and asked to implement
it in go. Lately I've been learning a little go and found
this source inspiring.
*/
package main

import (
    "crypto/sha512"
    "golang.org/x/crypto/sha3"
    "flag"
    "fmt"
    "io"
    "os"
    "strings"
    "bufio"
)

var chestExtension string
var useShake256 bool
var shake256Base int
var shake256Factor int

func hashSHA512Password(password string) []byte {
    hash := sha512.Sum512([]byte(password))
    return hash[:]
}

func hashSHAKE256Password(password string, hashLen int) []byte {
    hash := sha3.NewShake256()
    hash.Write([]byte(password))
    
    output := make([]byte, hashLen)
    hash.Read(output)
    
    return output[:]
}

func processFile(filePath string, password string, mode string) error {
    var hashBytes []byte
    if useShake256 == true {
        hashBytes = hashSHAKE256Password(password, shake256Base * shake256Factor)
    } else {
        hashBytes = hashSHA512Password(password)
    }
    hashLen := len(hashBytes)
    hashIndex := 0

    var outputFilePath string
    if mode == "encrypt" {
        outputFilePath = filePath + chestExtension
    } else if mode == "decrypt" {
        if strings.HasSuffix(filePath, chestExtension) {
            outputFilePath = strings.TrimSuffix(filePath, chestExtension)
        } else {
            return fmt.Errorf(
                "file does not have a %v extension for decryption",
                chestExtension)
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
    passwordFile := flag.String("p", "", "Password file")
    extension := flag.String("e", ".chest", "File extension for encrypted files")
    useShake256Flag := flag.Bool("s", false, "Use shake256 instead of sha512")
    shake256BaseFlag := flag.Int("b", 1024, "Base size of shake256 hash")
    shake256FactorFlag := flag.Int("f", 1024, "Base size multiplication factor")
    flag.Parse()
    
    if *shake256BaseFlag <= 0 {
        *shake256BaseFlag = 1024
    }
    if *shake256FactorFlag <= 0 {
        *shake256FactorFlag = 1024
    }
    
    useShake256 = *useShake256Flag
    shake256Base = *shake256BaseFlag
    shake256Factor = *shake256FactorFlag

    if flag.NArg() < 1 {
        fmt.Println("Usage: gochest -e extension -p password_file filename")
        fmt.Println("               -s -b size -f factor")
        os.Exit(1)
    }
    
    chestExtension = *extension

    filePath := flag.Arg(0)

    // Read password from file or prompt the user
    var password string
    if *passwordFile != "" {
        file, err := os.Open(*passwordFile)
        if err != nil {
            fmt.Printf("Error: could not open password file %s\n", *passwordFile)
            os.Exit(1)
        }
        defer file.Close()

        passwordBytes, err := io.ReadAll(file)
	if err != nil {
	    fmt.Printf("Error reading password file: %v\n", err)
	    os.Exit(1)
	}
	if len(passwordBytes) == 0 {
	    fmt.Println("Error: password file is empty")
	    os.Exit(1)
	}
	password = string(passwordBytes)
    } else {
        fmt.Print("Enter password: ")
        scanner := bufio.NewScanner(os.Stdin)
        scanner.Scan()
        password = scanner.Text()
    }

    // Determine whether to encrypt or decrypt
    mode := "encrypt"
    if strings.HasSuffix(filePath, chestExtension) {
        mode = "decrypt"
    }

    // Process the file
    err := processFile(filePath, password, mode)
    if err != nil {
        fmt.Printf("Error: %v\n", err)
        os.Exit(1)
    }

    fmt.Printf("%s %s\n", strings.Title(mode), filePath)
}

