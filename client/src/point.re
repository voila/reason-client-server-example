let ste = ReasonReact.stringToElement;

exception NotWeekday(int);

let pic_of_icon = i =>
  switch i {
  | "rain" => {js|🌧|js}
  | "snow" => {js|❄️ ️|js}
  | "sleet" => {js|🌧|js}
  | "wind" => {js|💨 |js}
  | "fog" => {js|☁️|js}
  | "cloudy" => {js|☁️|js}
  | "partly-cloudy-day" => {js|⛅|js}
  | "partly-cloudy-night" => {js|⛅|js}
  | "clear-night" => {js|🌛|js}
  | "clear-day" => {js|☀️|js}
  | _ => {js|🤔|js}
  };

let getDay = (d: Js.Date.t) => {
  let n = Js.Date.getDay(d) |> int_of_float;
  switch n {
  | 0 => "Sun"
  | 1 => "Mon"
  | 2 => "Tue"
  | 3 => "Wed"
  | 4 => "Thu"
  | 5 => "Fri"
  | 6 => "Sat"
  | _ => raise(NotWeekday(n))
  };
};

let string_of_date = (d: Js.Date.t) => {
  let day = getDay(d);
  let date = Js.Date.getDate(d) |> int_of_float;
  let hours = Js.Date.getHours(d) |> int_of_float;
  {j|$day $date - $(hours):00 |j};
};

let string_of_prec = (int, prob) => {
  let prob = prob *. 100.;
  {js|💧|js} ++ Printf.sprintf(" %0.1fmm/h (%0.2f%%)", int, prob);
};

let string_of_temp = t => {js|🌡|js} ++ {j| $(t)C|j};

/* let deg = 0x00B0; */
let string_of_wind = w => {js|💨|js} ++ {j| $(w) m/sec|j};

let component = ReasonReact.statelessComponent("Point");

/* let calculateStyle = (pressure: float) : ReactDOMRe.style =>
   ReactDOMRe.Style.make(~backgroundColor=calculateDiff(pressure), ()); */
let make = (~pt: Api.point, _children) => {
  ...component,
  render: _self => {
    let date = Js.Date.(pt.time *. 1000. |> fromFloat);
    <div>
      <div className="date"> (ste(date |> string_of_date)) </div>
      <div className="icon"> (ste(pt.icon |> pic_of_icon)) </div>
      <div className="summary"> (ste(pt.summary)) </div>
      <div className="temp"> (ste(string_of_temp(pt.temp))) </div>
      <div className="rain">
        (ste(string_of_prec(pt.precInt, pt.precProb)))
      </div>
      <div className="wind"> (ste(string_of_wind(pt.wind))) </div>
    </div>;
  }
};