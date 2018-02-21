# ReasonML server and client example

A simple example of a client-server application using DarkSky API (https://darksky.net/dev).

![screenshot](weather.png "screenshot")



## Client

### to build

    $ cd client
    $ npm install
    $ npm run build
    $ npm run webpack

## Server 

### prerequisites

* you need to put your DarkSky API key in the `DARKSKY_API_KEY` environment variable
* you need the OCaml 4.02.3+buckle-master compiler
* libraries you need to install: cohttp, ezjsonm and re 

```
$ opam switch 4.02.3+buckle-master
$ opam install lwt cohttp.0.22.0 ezjsonm re
```

### to build and start

    $ cd server
    $ make build
    $ make start

open http://localhost:8000 in a browser
