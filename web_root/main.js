
function durFormat(val) {
  val = parseInt(val);
  var h = Math.floor(val / 3600);
  var m = Math.floor(val / 60) % 60;
  var s = val % 60;
  var out = '';
  out += h < 10 ? '0' + h : h;
  out += ':';
  out += m < 10 ? '0' + m : m;
  out += ':';
  out += s < 10 ? '0' + s : s;
  return out;
}

function phoneNumFormat(val) {
  var num = libphonenumber.parse('+' + val);
  if(num.phone) {
    return '<span class="flag-icon flag-icon-' +
      num.country.toLowerCase() + '"></span>' +
      libphonenumber.format(num,'International');
  } else {
    return val;
  }
}

function updateDuration() {
  $("#activecalls tbody td.duration")
    .each(function(){
      var len = parseInt($(this).attr('value'));
      if($(this).attr('status') == 'up') {
        len++;
        $(this).attr('value', len);
        $(this).text(durFormat(len));
      }
    });
}

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
      setTimeout(function() { connect(); }, 3000)
    };

    sock.onerror = function(err) {
      console.error('Socket encountered error: ', err.message, 'Closing socket')
      sock.close()
    };
  }

  connect();
  setInterval(function() { updateDuration(); }, 1000);

});

