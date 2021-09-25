# tcp_over_websocket: bridge TCP services over WebSockets

- on a server or a proxy, tcp2web serves a TCP service on a WebSocket
- on a client, web2tcp connects to the websocket and makes the remote service accessible on a local socket

This "naive experiment" may get you around proxies (restricting internet to web only, even blocking tunneling). Be careful not to expose yourself to difficult situations and rather explain your needs to responsible teams: you may dangerously break your network security! I am not responsible for anything you do with this tiny piece of code.

Licenced under the [WTFPL](http://www.wtfpl.net/txt/copying).

## Installing

On the server:

```
mkdir tcp2web/build
cd tcp2web/build
cmake -GNinja ..
ninja
```

On the client:

```
mkdir web2tcp/build
cd web2tcp/build
cmake -GNinja ..
ninja
```

## Running

On the server:

```
tcp2web -u [tcp_service_host:port] -w [websocket_serving_port]
```
example:
```
tcp2web -u localhost:22 -w 8080
```

On the client:

```
web2tcp -u [ws://websocket_service_host:port] -p [tcp_serving_port] -P [http_proxy_host:port]
```
example
```
web2tcp -u ws://tcp2web.example.com:8080 -p 8022 -P $http_proxy
```
then
```
ssh -o Port=8022 remote_user@localhost
```
