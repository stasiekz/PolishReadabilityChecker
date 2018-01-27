"""
Very simple HTTP server serving as a wrapper for WCRFT2 CLI.
Handles multipart POST request with premorph file under "file" name returning tagged file in ccl format.
Usage::
    ./wcrft2_server.py [<port>]

    curl --form file=@path/to/file http://localhost:<port> > output.ccl
"""
import sys
import cgi
from subprocess import Popen, PIPE

from http.server import BaseHTTPRequestHandler, HTTPServer


class WCRFT2HTTPHandler(BaseHTTPRequestHandler):
    def _set_headers(self):
        self.send_response(200)
        self.send_header('Content-type', 'text/html; charset=utf-8')
        self.end_headers()

    def do_POST(self):
        self._set_headers()
        form = cgi.FieldStorage(
            fp=self.rfile,
            headers=self.headers,
            environ={'REQUEST_METHOD': 'POST'}
        )
        cmd = ['wcrft-app', 'nkjp_e2', '-i', 'premorph', '-o', 'ccl', '-']
        p = Popen(cmd, stdout=PIPE, stdin=PIPE, stderr=PIPE)
        stdin_data = form.getvalue("file")
        stdout_data = p.communicate(input=stdin_data)[0]
        self.wfile.write(stdout_data.strip())


def run(server_class=HTTPServer, handler_class=WCRFT2HTTPHandler, port=8088):
    server_address = ('', port)
    httpd = server_class(server_address, handler_class)
    print('Server running at localhost:{} ...'.format(port))
    httpd.serve_forever()

if __name__ == "__main__":
    port = int(sys.argv[1]) if len(sys.argv) >= 2 else 8088
    run(port=port)
