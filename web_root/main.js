// Main
$(document).ready(function() {
  var utils = new Utils();
  var visual = new Visual();

  function connect(){
    var sock = new WebSocket('ws://' + location.host);

    sock.onopen = function(){
      console.info("On open send status request.");
      $(".amiserver").remove();
      $("#activecalls tbody tr").remove();
      $("#alertwin").modal('hide');
      utils.reset();
      visual.reset();
      sock.send(JSON.stringify({"Action": "CoreStatus"}));
      sock.send(JSON.stringify({"Action": "CoreShowChannels"}));
    };

    sock.onmessage = function(ev){
      if(ev.type == 'message'){
        var pack = new AMIPack(ev.data, utils, visual);
        pack.proc();
      } else {
        console.info("No message type.");
      }
    };

    sock.onclose = function(ev){
      console.info("Connection closed.");
      $("#alertwin").modal({backdrop: 'static', keyboard: false});
      setTimeout(function() { connect(); }, 3000)
    };

    sock.onerror = function(err) {
      console.error('Socket encountered error: ', err.message, 'Closing socket')
      sock.close()
    };
  }

  connect();
  setInterval(function() { utils.updateDuration(); }, 1000);

});

