type point = {
  time: float,
  icon: string,
  summary: string,
  precInt: float,
  precProb: float,
  temp: float,
  wind: float
};

type points = list(point);

let decodeDataPoint: Json.Decode.decoder(point) =
  json =>
    Json.Decode.{
      time: field("time", Json.Decode.float, json),
      icon: field("icon", Json.Decode.string, json),
      summary: field("summary", Json.Decode.string, json),
      precInt: field("precipIntensity", Json.Decode.float, json),
      precProb: field("precipProbability", Json.Decode.float, json),
      temp: field("temperature", Json.Decode.float, json),
      wind: field("windSpeed", Json.Decode.float, json)
    };

let parseJson = (json: Js.Json.t) : points =>
  json |> Json.Decode.list(decodeDataPoint);

/* temp: Json.Decode.field("temp", Json.Decode.float, json) */
let fetchTemperature = (lat: string, lon: string) : Js_promise.t(points) => {
  let apiUrl = {j|/weather/$lat/$lon?|j};
  Js.Promise.(
    Bs_fetch.fetch(apiUrl)
    |> then_(Bs_fetch.Response.text)
    |> then_(jsonText => resolve(parseJson(Js.Json.parseExn(jsonText))))
  );
};