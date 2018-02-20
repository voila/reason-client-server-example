let ste = ReasonReact.stringToElement;

let evtVal = evt => ReactDOMRe.domElementToObj(ReactEventRe.Form.target(evt))##value;

/* Test whether GeoLocation API is supported */
[%%bs.raw
  {|
     if (!navigator.geolocation)
         alert("Geolocation is not supported by this browser.");
   |}
];

[@bs.val]
external getLocation : (~onSuccess: 'a => unit, ~onError: 'b => unit) => unit =
  "navigator.geolocation.getCurrentPosition";

[@bs.val] external alert : string => unit = "alert";

type position = (float, float);

type location =
  | Location(position)
  | LocError(string)
  | LocWaiting;

let position = ref(LocWaiting);

let onSuccess = p =>
  position := Location((p##coords##latitude, p##coords##longitude));

let onError = e => {
  let msg =
    switch e##code {
    | 1 => "Permission denied"
    | 2 => "Position unavailable"
    | 3 => "Timeout"
    | _ => "Unknown error"
    };
  position := LocError(msg);
};

type state = {
  lat: string,
  lon: string,
  data: option(Client.datapoints)
};

let show_temp = mt =>
  switch mt {
  | None => ""
  | Some(t) => Printf.sprintf("%.2fC", t)
  };

type action =
  | UpdateLat(string)
  | UpdateLon(string)
  | DataLoaded(Client.datapoints)
  | PosLoaded(position)
  | GetData;

let component = ReasonReact.reducerComponent("Page");

let make = _children => {
  ...component,
  initialState: () => {lat: "49.5094827", lon: "5.1377252", data: None},
  reducer: (action, state) =>
    switch action {
    | UpdateLat(s) => ReasonReact.Update({...state, lat: s})
    | UpdateLon(s) => ReasonReact.Update({...state, lon: s})
    | DataLoaded(t) => ReasonReact.Update({...state, data: Some(t)})
    | PosLoaded((l, l')) =>
      let lat = string_of_float(l);
      let lon = string_of_float(l');
      ReasonReact.Update({...state, lat, lon});
    | GetData =>
      ReasonReact.SideEffects(
        (
          self =>
            Client.fetchTemperature(state.lat, state.lon)
            |> Js.Promise.then_((resp: Client.datapoints) => {
                 self.send(DataLoaded(resp));
                 Js.Promise.resolve();
               })
            |> ignore
        )
      )
    },
  didMount: self => {
    getLocation(~onSuccess, ~onError);
    let rec checkPos = () => {
      Js.log("check Pos");
      Js.Global.setTimeout(
        () =>
          switch position^ {
          | Location((lat, lon)) => self.send(PosLoaded((lat, lon)))
          | LocWaiting =>
            let _ = checkPos();
            ();
          | LocError(msg) => alert("GeoLocation: " ++ msg)
          },
        1000
      );
    };
    let _ = checkPos();
    ReasonReact.NoUpdate;
  },
  render: ({state, send}) =>
    <div>
      <p>
        <label name="Latitude" htmlFor="lat"> (ste("Latitude")) </label>
        <br />
        <input
          onChange=(evt => send(UpdateLat(evtVal(evt))))
          value=state.lat
        />
      </p>
      <p>
        <label name="Longitude" htmlFor="lat"> (ste("Longitude")) </label>
        <br />
        <input
          onChange=(evt => send(UpdateLon(evtVal(evt))))
          value=state.lon
        />
      </p>
      <button onClick=(evt => send(GetData))> (ste("Get Forecast!")) </button>
      <hr />
      <ul>
        (
          switch state.data {
          | Some(pts) =>
            ReasonReact.arrayToElement(
              Array.of_list(List.map(pt => <li> <Point pt /> </li>, pts))
            )
          | None => ReasonReact.nullElement
          }
        )
      </ul>
    </div>
  /* <div> (ste(show_temp(state.temp))) </div> */
};