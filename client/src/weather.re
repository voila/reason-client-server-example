let ste = ReasonReact.stringToElement;

let evtVal = evt => ReactDOMRe.domElementToObj(ReactEventRe.Form.target(evt))##value;

[@bs.val] external alert : string => unit = "alert";

type state = {data: option(Api.points)};

type action =
  | DataLoaded(Api.points)
  | PosLoaded(Geo.position);

let component = ReasonReact.reducerComponent("Page");

let make = _children => {
  ...component,
  initialState: () => {data: None},
  reducer: (action, _state) =>
    switch action {
    | DataLoaded(t) => ReasonReact.Update({data: Some(t)})
    | PosLoaded((lat, lon)) =>
      let lat = string_of_float(lat);
      let lon = string_of_float(lon);
      ReasonReact.SideEffects(
        (
          self =>
            Api.fetchTemperature(lat, lon)
            |> Js.Promise.then_((resp: Api.points) => {
                 self.send(DataLoaded(resp));
                 Js.Promise.resolve();
               })
            |> ignore
        )
      );
    },
  didMount: self => {
    Geo.getLocation();
    let rec checkPos = () =>
      Js.Global.setTimeout(
        () =>
          switch Geo.position^ {
          | Geo.Location((lat, lon)) => self.send(PosLoaded((lat, lon)))
          | Geo.LocWaiting => checkPos() |> ignore
          | Geo.LocError(msg) => alert("GeoLocation: " ++ msg)
          },
        1000
      );
    let _ = checkPos();
    ReasonReact.NoUpdate;
  },
  render: ({state}) =>
    <div>
      <ul>
        (
          switch state.data {
          | Some(pts) =>
            ReasonReact.arrayToElement(
              Array.of_list(List.map(pt => <li> <Point pt /> </li>, pts))
            )
          | None => <div> (ste("Loading data...")) </div>
          }
        )
      </ul>
    </div>
};