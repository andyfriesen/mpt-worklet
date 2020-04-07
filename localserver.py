import SimpleHTTPServer
import SocketServer

PORT = 80

class Handler(SimpleHTTPServer.SimpleHTTPRequestHandler):
    pass

Handler.extensions_map['.wasm'] = 'application/wasm'
Handler.extensions_map['.js'] = 'application/javascript'

httpd = SocketServer.TCPServer(("", PORT), Handler)

print "serving at port", PORT
httpd.serve_forever()