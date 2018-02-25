open Ezjsonm;

type point = {
  time: float,
  icon: string,
  summary: string,
  precInt: float,
  precProb: float,
  temp: float,
  wind: float
};

type t = list(point);

let point_of_json = (v: Ezjsonm.value) : point => {
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

let of_json = (json: string) : t =>
  get_dict(from_string(json))
  |> List.assoc("hourly")
  |> get_dict
  |> List.assoc("data")
  |> get_list(point_of_json);

let point_to_json = ({time, icon, summary, precInt, precProb, temp, wind}) =>
  `O([
    ("time", `Float(time)),
    ("summary", `String(summary)),
    ("icon", `String(icon)),
    ("precipIntensity", `Float(precInt)),
    ("precipProbability", `Float(precProb)),
    ("temperature", `Float(temp)),
    ("windSpeed", `Float(wind))
  ]);

let to_json = (pts: t) => to_string(`A(List.map(point_to_json, pts)));