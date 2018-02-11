/* Response: {"lat":"42.3601","lon":"-71.0589","temp":1.344444444444445,"took":"1756.26 ms"} */
type response = {temp: float};

let parseResponseJson = (json: Js.Json.t) : response => {
  temp: Json.Decode.field("temp", Json.Decode.float, json)
};

let fetchTemperature = (lat: string, lon: string) : Js_promise.t(response) => {
  let apiUrl = {j|/weather/$lat/$lon|j};
  Js.Promise.(
    Bs_fetch.fetch(apiUrl)
    |> then_(Bs_fetch.Response.text)
    |> then_(jsonText =>
         resolve(parseResponseJson(Js.Json.parseExn(jsonText)))
       )
  );
};