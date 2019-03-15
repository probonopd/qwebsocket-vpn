# qwebsocket-tcpproxy
A program to map remote tcp port to local tcp port, local ad remote using websocket

Assume you can only access remote host using http protocol, such as a public docker container. And now you want to access any other tcp services. This tool can do this.
For example, ssh service.
Run in remote: ./websocket-proxy -m server -P 22 -p 80
Run in local: ./websocket-proxy -m client -u ws://remote_ip:80 -p 9999
ssh to local port 9999, you can access to remote host now.
