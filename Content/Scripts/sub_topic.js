
let ros = new ROSLIB.Ros({
  //   url: "ws://localhost:9090",
  url: "ws://183.105.113.142:9090",
});

ros.on("connection", function () {
  console.log("// Connected to websocket server.");
});

ros.on("error", function (error) {
  console.log("// Error connecting to websocket server: ", error);
});

ros.on("close", function () {
  console.log("// Connection to websocket server closed.");
});

/*
    subscribe tracking object topic
*/
let sub_trkobj = new ROSLIB.Topic({
  ros: ros,
  name: "/Trkobjs",
  messageType: "std_msgs/String",
});

sub_trkobj.subscribe((msg) => {
  document.getElementById("trkobjs").innerHTML = msg.data;

  const trkobjs_json = JSON.parse(TrkMsg.data);
  //   console.log(trkobjs_json);

  // If desired, we can unsubscribe from the topic as well.
  //   sub_trkobj.unsubscribe();
});
