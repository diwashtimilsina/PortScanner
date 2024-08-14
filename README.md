
# Port Scanner

**Version:** 1.0.0  
**Author:** [Diwash Timilsina]  
**Date:** August 2024

## Overview

The Port Scanner project is a simple tool designed to scan open ports on a specified URL. This project includes both a backend component written in C++ and a frontend user interface built using HTML, CSS, and JavaScript.

## Features

- **Port Scanning:** Scan specified URLs for open ports.
- **User-Friendly Interface:** A simple and intuitive web-based interface to initiate port scans.
- **Real-Time Results:** Displays port scan results in real-time.

## Project Structure

- **`main.cpp`:** The C++ backend program that handles the port scanning logic.
- **`index.html`:** The frontend interface for users to enter a URL and view port scan results.

## Installation

### Prerequisites

- A C++ compiler (e.g., GCC)
- A web server (e.g., Apache, Nginx, or a simple Python HTTP server)
- Basic knowledge of how to run C++ programs and serve HTML files

### Steps

1. **Clone the repository:**

   ```bash
   git clone https://github.com/yourusername/port-scanner.git
   cd port-scanner
   ```

2. **Compile the C++ program:**

   ```bash
   g++ -o portscanner main.cpp
   ```

3. **Run the web server:**

   - For a simple Python HTTP server:
     ```bash
     python3 -m http.server
     ```

4. **Access the application:**

   Open your web browser and navigate to `http://localhost:8000/index.html`.

## Usage

1. Open the application in your web browser.
2. Enter the URL you want to scan in the input field.
3. Click the "Scan Ports" button to initiate the scan.
4. The results will be displayed in the "Scan Results" section.

## Contributing

Contributions are welcome! Please follow these steps:

1. Fork the repository.
2. Create a new branch (`git checkout -b feature-branch`).
3. Make your changes.
4. Commit your changes (`git commit -m 'Add some feature'`).
5. Push to the branch (`git push origin feature-branch`).
6. Open a pull request.

## License

This project is licensed under the MIT License. See the `LICENSE` file for more details.

## Contact

For any questions or issues, please contact [Your Name] at [diwashtimilsina7@gmail.com].
