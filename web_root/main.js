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

  AMIPack.prototype.type_name = function(){
    return ami_type[type] ? ami_type[type] : 'UNKNOWN';
  }

  AMIPack.prototype.header = function(header_name){
    return data[header_name];
  }

  AMIPack.prototype.display = function(){
    container.append('[' + this.type_name() + '] ');
    switch(type) {
      case 1: // PROMPT
        break;
      case 2: // ACTION
        break;
      case 3: // EVENT
        container.append(this.header( 'Event' ) + '\n');
        break;
      case 4: // RESPONSE
        container.append(this.header( 'Message' ) + '\n');
        break;
      default:
        // Unknown type
        break;
    }
  }

  return AMIPack;
})();

$(document).ready(function() {
  var container = $("#events");
  container.width('340px').height('80%');
  var sock = new WebSocket('ws://' + location.host);

  sock.onopen = function(){
    console.info("Onopen event.");
  };

  sock.onmessage = function(ev){
    if(ev.type == 'message'){
      var pack = new AMIPack(ev.data, container);
      pack.display();
    } else {
      console.info("No message type.");
    }
  };
});
