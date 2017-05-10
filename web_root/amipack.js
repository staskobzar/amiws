/**
 * AMI packet class
 */
var AMIPack = (function(){
  var type = 0;
  var data;
  var container;
  var ami_type = [ 'UNKNOWN', 'PROMPT', 'ACTION', 'EVENT', 'RESPONSE' ];
  var state_color = {Ring: "#ffcc00", Up: "#00cc00", Down: "#cccccc"};
  var tableid = '#activecalls';

  function AMIPack(event, el){
    var json = JSON.parse(event);
    type = json.type;
    data = json.data;
    container = el;
  }

  var colorPick = function(state) {
    var color = state_color[state];
    return color ? color : '#000000';
  }

  var durFormat = function(val) {
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

  AMIPack.prototype.updateDuration = function() {
    $("#activecalls tbody td.duration")
      .each(function(){
              var len = parseInt($(this).attr('value')) + 1;
              $(this).attr('value', len);
              $(this).text(durFormat(len));
      });
  }

  AMIPack.prototype.typeName = function(){
    return ami_type[type] ? ami_type[type] : 'UNKNOWN';
  }

  AMIPack.prototype.header = function(header_name){
    return data[header_name];
  }

  AMIPack.prototype.elemId = function(){
    return '1-' + this.header('Channel').
                replace(/[^-_a-zA-Z0-9]/g,'-');
  }

  AMIPack.prototype.hangup = function() {
    this.newState('Down');
    /*
    $('#' + this.elemId()).fadeOut(1000,
        function(){$(this).remove();});
    */
  }

  AMIPack.prototype.newState = function(state) {
    state = state ? state : this.header('ChannelStateDesc');
    var tr = $('#' + this.elemId());
    tr.children('.status')
      .css('color', colorPick(state));
  }

  AMIPack.prototype.newChannel = function() {
    var el_id = this.elemId();
    var row = $('<tr>', {id: el_id});

    row.append($('<td>',{class: 'status'})
                .append('<i class="fa fa-lg fa-phone-square" aria-hidden="true"></i>') );
    row.append($('<td>', {class: 'start-time'}).text(new Date().toLocaleString()));
    row.append($('<td>', {class: 'duration', value: '0'}).text(durFormat(0)));
    row.append($('<td>', {class: 'source'}).text(this.header('CallerIDNum')));
    row.append($('<td>', {class: 'destination'}).text(this.header('Exten')));

    $(tableid).append(row);
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
        this.newState('Down');
        break;
      case 'Hangup':
        this.hangup();
        break;
      default:
        //console.log('Skip event "' + event + '"');
        break;
    }
  }

  AMIPack.prototype.proc = function(){
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

