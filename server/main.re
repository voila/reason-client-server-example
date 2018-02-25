open Lwt;

open Cohttp_lwt_unix;

/* Helpers */
let celsius_of_fahrenheit = t => (t -. 32.0) /. 1.8;

let callApi = uri_str =>
  Client.get(Uri.of_string(uri_str))
  >>= (((_, body)) => Cohttp_lwt_body.to_string(body));

/* extract data from JSON returned by weather API */
let darksky = (key: string, lat: string, lon: string) : Lwt.t(Datapoint.t) => {
  let uri =
    "https://api.darksky.net/forecast/"
    ++ key
    ++ "/"
    ++ lat
    ++ ","
    ++ lon
    ++ "?units=si";
  callApi(uri) >|= Datapoint.of_json;
};

/* call API, return JSON object {"time": <float>, ... } */
let weather = (key: string, lat: string, lon: string) : Lwt.t(string) =>
  darksky(key, lat, lon) >|= Datapoint.to_json;

/* HTTP server */
let make_server = (~port: int, ~key: string) : Lwt.t(unit) => {
  let callback = (conn_id, req, body) => {
    let uri = Request.uri(req);
    Lwt_io.printf("%s\n", Uri.path(uri))
    >>= (
      (_) =>
        switch Re_str.(split_delim(regexp_string("/"), Uri.path(uri))) {
        | ["", "index.js", ...rest] =>
          Server.respond_file(~fname="index.js", ())
        | ["", "index", ...rest] =>
          Server.respond_file(~fname="index.html", ())
        | ["", "weather", lat, lon, ...rest] =>
          weather(key, lat, lon)
          >>= (
            data => {
              let headers =
                Cohttp.Header.(
                  init_with("content-type", "application/json; charset=utf-8")
                );
              Server.respond_string(~headers, ~status=`OK, ~body=data, ());
            }
          )
        | _ =>
          Server.respond_string(
            ~status=`Not_found,
            ~body="Route not found",
            ()
          )
        }
    );
  };
  Lwt_io.printf("HTTP server started on port %s\n", string_of_int(port))
  >>= (
    (_) => Server.create(~mode=`TCP(`Port(port)), Server.make(~callback, ()))
  );
};

try {
  let key = Sys.getenv("DARKSKY_API_KEY");
  Lwt_main.run(make_server(~port=8000, ~key));
} {
| Not_found => print_endline("$DARKSKY_API_KEY is missing")
};