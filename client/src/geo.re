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

let getLocation = (~sendPos, ~sendErr) => {
  let onSuccess = p => sendPos(p##coords##latitude, p##coords##longitude);
  let onError = e => {
    let msg =
      switch e##code {
      | 1 => "Permission denied"
      | 2 => "Position unavailable"
      | 3 => "Timeout"
      | _ => "Unknown error"
      };
    sendErr(msg);
  };
  getCurrentPosition(~onSuccess, ~onError);
};