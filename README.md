# ReasonML server and client example

## Client

### to build

    $ cd client
    $ npm install
    $ npm run build
    $ npm run webpack

## Server 

### prerequisites

* you need to put your Darksky (https://darksky.net/) API key in the `DARKSKY_API_KEY` environment variable
* you need the OCaml 4.02.3+buckle-master compiler
* libraries you need to install: cohttp, ezjsonm and re 

```
$ opam switch 4.02.3+buckle-master
$ opam install cohttp.0.22.0 ezjsonm re
```

### to build and start

    $ cd server
    $ make build
    $ make start

open http://localhost:8000 in a browser