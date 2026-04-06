# Cloud Disk Server Project

This is a simple implementation of a cloud disk server, providing basic functionalities such as file storage and user management.

## Features

- User registration and login
- File upload and download
- File querying
- User information management
- Static resource serving
- SHA1 hash generation
- Token generation

## Directory Structure

- `CloudDiskServer.cc` - Main server implementation
- `Hash.cc` - Hash generation utility
- `Token.cc` - Token generation utility
- `main.cc` - Program entry point
- `static/` - Static resource files (HTML, CSS, JS)
- `tbl_sql.sql` - Database table schema

## Compilation and Execution

1. Ensure required dependency libraries are installed.
2. Compile the project using the Makefile.
3. Run the server: `./cloudDisk_server`
4. The server listens on port 8080 by default.

## Usage Examples

1. User registration: Access the `/register` endpoint.
2. User login: Access the `/login` endpoint.
3. File upload: Upload files via the `/upload` endpoint.
4. File download: Download files via the `/download` endpoint.

## Testing

- `testHash.cc` - Tests hash generation functionality
- `testToken.cc` - Tests token generation functionality

## Technology Stack

- C++11
- HTTP server
- MySQL database
- Bootstrap frontend framework

## Notes

- The `file.conf` configuration file must be set up.
- A database must be created and the `tbl_sql.sql` schema imported.
- The project uses several third-party JavaScript libraries (jQuery, Bootstrap, etc.).