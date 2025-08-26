# webserv
A group project about writing our own HTTP/1.1 server.

❓If you stumble upon weird behaviour or bugs, don't be a stranger. 🦫

💡Inspired by Nginx and [RFC 2616](https://www.rfc-editor.org/rfc/rfc2616 )💡
--------------------------
This server:
--------------------------

   ➔ Supports multiple server blocks (multiple hosts with individual configurations)
   
   ➔ Supports methods: **``GET``, ``POST``, ``DELETE``** 
   
   ➔ Handles **HTTP status codes** and offers **default error pages**
   
   ➔ Serves static files and directories
   
   ➔ Enables directory listing/auto indexing (modifiable in configuration file)

   ➔ Supports **HTTP redirections** (for example: return 301)

   ➔ Compatible with browsers (tested with Chrome and Firefox), telnet, curl and netcat

   ➔ Is stress-tested: remains responsive under load.

   ➔ Supports **CGI scripts in Python**

   ➔ File upload support:
   
  - multipart/form-data parsing for file uploads
  - configurable upload directory

   ➔ Request body handling: 
   
  - chunked transfer-encoding for POST bodies
  - proper content/length handling (defined by configuration file)

--------------------------
To run:
--------------------------
``make `` to build the executable _webserv_

**Option 1:**

``./webserv`` to run with default configuration provided by the creators (similar to ``./webserv configuration/webserv.conf``)

**Option 2:**

- create your own "custom" configuration file with matching layout rules to the default one's.
- run ``./webserv configuration/custom.conf`` 
  -  No nested server or location (route) blocks
  -  One-liner directives need to end with a singular semicolon (;)
  -  Block declarations (like server) must be followed by space and opening bracket, like: ``server {``
  -  Remember: multiple servers are supported, as long as they aren't nested in configuration file (syntax error)
  -  Trying to run with faulty configuration file should output descriptive error messages
  -  **Your configuration file should be located inside directory configuration/ and end with extension .conf**
 
Example of a minimalist configuration file's content:

    server {
      listen 8080;
      server_name mysite.local;

      error_page 404 /errors/404.html;

      client_max_body_size 10M;

      location / {
        root ./www;
        index index.html;
      }

      location /upload {
        root ./uploads;
        methods POST;
      }

    location /cgi-bin {
        cgi .py /usr/bin/python3;
      }
    }

