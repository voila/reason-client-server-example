let ste = ReasonReact.stringToElement;

let evtVal = evt => ReactDOMRe.domElementToObj(ReactEventRe.Form.target(evt))##value;

[@bs.val] external alert : string => unit = "alert";

type state = {data: option(Api.points)};

type action =
  | DataLoaded(Api.points)
  | PosLoaded(float, float)
  | PosError(string);

let component = ReasonReact.reducerComponent("Page");

let make = _children => {
  ...component,
  initialState: () => {data: None},
  reducer: (action, _state) =>
    switch action {
    | DataLoaded(pts) => ReasonReact.Update({data: Some(pts)})
    | PosError(msg) =>
      alert("GeoLocation: " ++ msg);
      ReasonReact.NoUpdate;
    | PosLoaded(lat, lon) =>
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
    Geo.getLocation(
      ~sendPos=(lat, lon) => self.send(PosLoaded(lat, lon)),
      ~sendErr=msg => self.send(PosError(msg))
    );
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