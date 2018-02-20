/* Test whether GeoLocation API is supported */
[%%bs.raw
  {|
     if (!navigator.geolocation)
         alert("Geolocation is not supported by this browser.");
   |}
];

[@bs.val]
external getCurrentPosition :
  (~onSuccess: 'a => unit, ~onError: 'b => unit) => unit =
  "navigator.geolocation.getCurrentPosition";

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

let getLocation = () => getCurrentPosition(~onSuccess, ~onError);