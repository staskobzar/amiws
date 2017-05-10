
$(document).ready(function() {

  function connect(){
    var sock = new WebSocket('ws://' + location.host);

    sock.onopen = function(){
      console.info("Onopen event.");
    };

    sock.onmessage = function(ev){
      if(ev.type == 'message'){
        var pack = new AMIPack(ev.data);
        pack.proc();
      } else {
        console.info("No message type.");
      }
    };

    sock.onclose = function(ev){
      console.info("Connection closed.");
      setTimeout(function() {
        connect();
      }, 3000)
    };

    sock.onerror = function(err) {
      console.error('Socket encountered error: ', err.message, 'Closing socket')
      sock.close()
    };
  }

  connect();

});

