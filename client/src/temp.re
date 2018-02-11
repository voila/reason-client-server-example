let ste = ReasonReact.stringToElement;

let evtVal = evt => ReactDOMRe.domElementToObj(ReactEventRe.Form.target(evt))##value;

type state = {
  lat: string,
  lon: string,
  temp: option(float)
};

let show_temp = mt =>
  switch mt {
  | None => ""
  | Some(t) => Printf.sprintf("%.2fC", t)
  };

type action =
  | UpdateLat(string)
  | UpdateLon(string)
  | TempLoaded(float)
  | GetTemp;

let component = ReasonReact.reducerComponent("Page");

let make = _children => {
  ...component,
  initialState: () => {lat: "49.5094827", lon: "5.1377252", temp: None},
  reducer: (action, state) =>
    switch action {
    | UpdateLat(s) => ReasonReact.Update({...state, lat: s})
    | UpdateLon(s) => ReasonReact.Update({...state, lon: s})
    | TempLoaded(t) => ReasonReact.Update({...state, temp: Some(t)})
    | GetTemp =>
      ReasonReact.SideEffects(
        (
          self =>
            Client.fetchTemperature(state.lat, state.lon)
            |> Js.Promise.then_((resp: Client.response) => {
                 self.send(TempLoaded(resp.temp));
                 Js.Promise.resolve();
               })
            |> ignore
        )
      )
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
      <button onClick=(evt => send(GetTemp))>
        (ste("Load Temperature"))
      </button>
      <hr />
      <div> (ste(show_temp(state.temp))) </div>
    </div>
};