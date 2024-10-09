use sha2::{Sha512, Digest};
use std::fs::File;
use std::io::{self, Read, Write};
use std::path::Path;
use std::env;
use std::process;
use rpassword::read_password;

/// Hashes the password using SHA-512
fn hash_password(password: &str) -> Vec<u8> {
    let mut hasher = Sha512::new();
    hasher.update(password.as_bytes());
    hasher.finalize().to_vec()
}

/// Encrypts or decrypts the file based on the mode
fn process_file(file_path: &str, password: &str, mode: &str, extension: &str) -> io::Result<()> {
    let hash_bytes = hash_password(password);
    let hash_len = hash_bytes.len();
    let mut hash_index = 0;

    // Determine the output file name
    let output_file_path = match mode {
        "encrypt" => format!("{}{}", file_path, extension),
        "decrypt" => {
            if file_path.ends_with(extension) {
                file_path.trim_end_matches(extension).to_string()
            } else {
                eprintln!("Error: File does not have a {} extension for decryption.", extension);
                process::exit(1);
            }
        }
        _ => unreachable!(),
    };

    let mut input_file = File::open(file_path)?;
    let mut output_file = File::create(output_file_path)?;

    let mut buffer = [0; 4096]; // Using a buffer to handle larger files more efficiently
    while let Ok(bytes_read) = input_file.read(&mut buffer) {
        if bytes_read == 0 {
            break; // EOF reached
        }

        for i in 0..bytes_read {
            buffer[i] = match mode {
                "encrypt" => buffer[i].wrapping_sub(hash_bytes[hash_index]),
                "decrypt" => buffer[i].wrapping_add(hash_bytes[hash_index]),
                _ => unreachable!(),
            };
            hash_index = (hash_index + 1) % hash_len;
        }

        output_file.write_all(&buffer[..bytes_read])?;
    }

    println!("{} file written successfully.", if mode == "encrypt" { "Encrypted" } else { "Decrypted" });
    Ok(())
}

/// Main function
fn main() {
    let args: Vec<String> = env::args().collect();

    if args.len() < 2 {
        eprintln!("Usage: {} [-p <password-file>] [-e <extension>] <filename>", args[0]);
        process::exit(1);
    }

    let mut password = String::new();
    let mut extension = String::from(".chest");

    // Parse command-line arguments
    let mut i = 1;
    while i < args.len() - 1 {
        match args[i].as_str() {
            "-p" => {
                if i + 1 < args.len() - 1 {
                    if let Some(password_file) = args.get(i + 1) {
                        password = std::fs::read_to_string(password_file)
                            .expect("Error reading password file")
                            .trim()
                            .to_string();
                    }
                    i += 1;
                } else {
                    eprintln!("Error: Missing password file path after '-p' option.");
                    process::exit(1);
                }
            }
            "-e" => {
                if i + 1 < args.len() - 1 {
                    extension = args[i + 1].clone();
                    i += 1;
                } else {
                    eprintln!("Error: Missing extension after '-e' option.");
                    process::exit(1);
                }
            }
            _ => {}
        }
        i += 1;
    }

    // Check if the last argument is the filename
    let file_path = &args[args.len() - 1];
    if !Path::new(file_path).exists() {
        eprintln!("Error: The specified file '{}' does not exist.", file_path);
        process::exit(1);
    }

    // Prompt for password if not provided by a file
    if password.is_empty() {
        println!("Enter password: ");
        password = read_password().expect("Failed to read password");
    }

    let mode = if file_path.ends_with(&extension) { "decrypt" } else { "encrypt" };

    if let Err(e) = process_file(file_path, &password, mode, &extension) {
        eprintln!("Error processing file: {}", e);
        process::exit(1);
    }
}

