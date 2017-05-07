/**
 * AMI packet class
 */
var AMIPack = (function(){
  var type = 0;
  var data;
  var container;
  var ami_type = [ 'UNKNOWN', 'PROMPT', 'ACTION', 'EVENT', 'RESPONSE' ];

  function AMIPack(event, el){
    var json = JSON.parse(event);
    type = json.type;
    data = json.data;
    container = el;
  }

  AMIPack.prototype.typeName = function(){
    return ami_type[type] ? ami_type[type] : 'UNKNOWN';
  }

  AMIPack.prototype.header = function(header_name){
    return data[header_name];
  }

  AMIPack.prototype.getEventId = function(){
    return this.header('Channel').
                replace(/[^-_a-zA-Z0-9]/g,'-');
  }

  AMIPack.prototype.stateText = function(state){
    var text = '[' + state + ': ' + this.header('Channel') + '] ';
    return text;
  }

  AMIPack.prototype.callText = function(){
    var text = '';
    text += ' Call ' + this.header('CallerIDNum') + ' -> ';
    text += this.header('Exten');

    return text;
  }

  AMIPack.prototype.newState = function() {
    var el_id = this.getEventId();
    var text = this.stateText(this.header('ChannelStateDesc'));
    text += this.callText();

    $('#' + el_id).first('div')
        .text(text);
  }

  AMIPack.prototype.hangupRequest = function() {
    var el_id = this.getEventId();
    var text = this.stateText('Hangup');
    text += this.callText();

    console.info("Hanup channel " + this.getEventId());

    $('#' + el_id).first('div')
        .text(text);
  }
  AMIPack.prototype.hangup = function() {
    var el_id = this.getEventId();
    $('#' + el_id).remove();
  }

  AMIPack.prototype.newChannel = function() {
    var el_id = this.getEventId();
    var call_div = $('<div/>',{ id: el_id, class: 'calldesc' });
    var text = this.stateText('Start');
    text += this.callText();

    call_div.append($('<div/>',{ text: text }));

    $("#calls_list").append(call_div);
    $("#pnum").text(parseInt($("#pnum").text()) + 1);
  }

  AMIPack.prototype.eventProc = function() {
    var event = this.header('Event');
    switch(event){
      case 'Newchannel':
        this.newChannel();
        break;
      case 'Newstate':
        this.newState();
        break;
      case 'SoftHangupRequest':
      case 'HangupRequest':
        this.hangupRequest();
        break;
      case 'Hangup':
        this.hangup();
        break;
      default:
        //console.log('Skip event "' + event + '"');
        break;
    }
  }

  AMIPack.prototype.debug = function(el){
    switch(type) {
      case 1: // PROMPT
        break;
      case 2: // ACTION
        break;
      case 3: // EVENT
        this.eventProc();
        break;
      case 4: // RESPONSE
        break;
      default:
        // Unknown type
        console.log('Unknown type: ' + type);
        break;
    }
  }

  return AMIPack;
})();

$(document).ready(function() {

  function connect(){
    var sock = new WebSocket('ws://' + location.host);

    sock.onopen = function(){
      console.info("Onopen event.");
    };

    sock.onmessage = function(ev){
      if(ev.type == 'message'){
        var pack = new AMIPack(ev.data);
        pack.debug($("#console"));
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

