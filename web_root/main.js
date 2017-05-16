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

function dateFormat(d) {
  var yyyy = d.getFullYear();
  var mm   = ('0' + (d.getMonth() + 1)).slice(-2);
  var dd   = ('0' + d.getDate()).slice(-2);

  var HH   = ('0' + d.getHours()).slice(-2);
  var MM   = ('0' + d.getMinutes()).slice(-2);
  var SS   = ('0' + d.getSeconds()).slice(-2);

  return yyyy + '-' + mm + '-' + dd + ' ' +
         HH + ':' + MM + ':' + SS;
}

function phoneNum(val) {
  var num = libphonenumber.parse('+' + val);
  if(num.phone) {
    return {"country": num.country.toLowerCase(),
            "num": libphonenumber.format(num,'International')
           };
  } else {
    return {"country": "invalid", "num": val};
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

var row_tmpl = Handlebars.compile($("#activecall-row-tmpl").html());
var serv_tmpl = Handlebars.compile($("#ami-server-tmpl").html());

$(document).ready(function() {

  function connect(){
    var sock = new WebSocket('ws://' + location.host);

    sock.onopen = function(){
      console.info("On open send status request.");
      $(".amiserver").remove();
      $("#activecalls tbody tr").remove();
      $("#alertwin").modal('hide');
      sock.send(JSON.stringify({"Action": "CoreStatus"}));
      sock.send(JSON.stringify({"Action": "CoreShowChannels"}));
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
      $("#alertwin").modal({backdrop: 'static', keyboard: false});
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

