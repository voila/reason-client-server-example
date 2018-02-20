/*
 $ make build
 $ make start
 $ curl http://0.0.0.0:8000/weather/42.3601/-71.0589
 {"temp":8.305555555555557}
 */
open Lwt;

open Cohttp_lwt_unix;

open Ezjsonm;

/* Helpers */
let celsius_of_fahrenheit = t => (t -. 32.0) /. 1.8;

let call_api = uri_str =>
  Client.get(Uri.of_string(uri_str))
  >>= (((_, body)) => Cohttp_lwt_body.to_string(body));

type datapoint = {
  time: float,
  icon: string,
  summary: string,
  precInt: float,
  precProb: float,
  temp: float,
  wind: float
};

let dataPoint = (v: Ezjsonm.value) : datapoint => {
  let d = v |> get_dict;
  let time = d |> List.assoc("time") |> get_float;
  let icon = d |> List.assoc("icon") |> get_string;
  let summary = d |> List.assoc("summary") |> get_string;
  let precInt = d |> List.assoc("precipIntensity") |> get_float;
  let precProb = d |> List.assoc("precipProbability") |> get_float;
  let temp = d |> List.assoc("temperature") |> get_float;
  let wind = d |> List.assoc("windSpeed") |> get_float;
  {time, icon, summary, precInt, precProb, temp, wind};
};

type datapoints = list(datapoint);

let dataPoints = (json: string) : datapoints =>
  get_dict(from_string(json))
  |> List.assoc("hourly")
  |> get_dict
  |> List.assoc("data")
  |> get_list(dataPoint);

exception MissingApiKey;

/* extract temperature from JSON returned by weather API */
let darksky = (~lat: string, ~lon: string) : Lwt.t(datapoints) => {
  let key =
    try (Sys.getenv("DARKSKY_API_KEY")) {
    | Not_found => raise(MissingApiKey)
    };
  let uri =
    "https://api.darksky.net/forecast/"
    ++ key
    ++ "/"
    ++ lat
    ++ ","
    ++ lon
    ++ "?units=si";
  call_api(uri) >|= dataPoints;
};

let json_of_datapoint = ({time, icon, summary, precInt, precProb, temp, wind}) =>
  `O([
    ("time", `Float(time)),
    ("summary", `String(summary)),
    ("icon", `String(icon)),
    ("precipIntensity", `Float(precInt)),
    ("precipProbability", `Float(precProb)),
    ("temperature", `Float(temp)),
    ("windSpeed", `Float(wind))
  ]);

let string_of_datapoints = (pts: datapoints) =>
  to_string(`A(List.map(json_of_datapoint, pts)));

/* call API, return JSON object {"temp": <float> } */
let temperature = (~lat: string, ~lon: string) : Lwt.t(string) =>
  darksky(~lat, ~lon) >|= string_of_datapoints;

/* to_string(`O([("temp", `Float(temp))]))); */
/* web  server */
let make_server = (~port: int) : Lwt.t(unit) => {
  let callback = (conn_id, req, body) => {
    let uri = Request.uri(req);
    Lwt_io.printf("%s\n", Uri.path(uri))
    >>= (
      (_) =>
        switch Re_str.(split_delim(regexp_string("/"), Uri.path(uri))) {
        | ["", "index.js", ...rest] =>
          Server.respond_file(~fname="index.js", ())
        | ["", "index", ...rest] =>
          /* Access-Control-Allow-Origin: * */
          Server.respond_file(~fname="index.html", ())
        | ["", "weather", lat, lon, ...rest] =>
          temperature(~lat, ~lon)
          >>= (
            temp => {
              let headers =
                Cohttp.Header.(
                  init_with("content-type", "application/json; charset=utf-8")
                );
              Lwt_io.printf("%s\n", temp)
              >>= (
                () =>
                  Server.respond_string(~headers, ~status=`OK, ~body=temp, ())
              );
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

let _ = Lwt_main.run(make_server(~port=8000));