/**
 * AMI packet class
 */
var AMIPack = (function(){
  var type = 0;
  var data;
  var server_id;
  var server_name;
  var container;
  var ami_type = [ 'UNKNOWN', 'PROMPT', 'ACTION', 'EVENT', 'RESPONSE' ];
  var state_color = {Ring: "#ffcc00", Up: "#00cc00", Down: "#cccccc"};
  var tableid = '#activecalls';

  function AMIPack(event, el){
    var json = JSON.parse(event);
    type = json.type;
    data = json.data;
    server_id = json.server_id;
    server_name = json.server_name;
    container = el;
  }

  var colorPick = function(state) {
    var color = state_color[state];
    return color ? color : '#000000';
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

  AMIPack.prototype.callsnum_increase = function() { this.callsnum_update(1); }

  AMIPack.prototype.callsnum_decrease = function() { this.callsnum_update(-1); }

  AMIPack.prototype.callsnum_update = function(i) {
    var calls = $("#amiserver-" + server_id).find('div.calls-number').first();
    var val   = parseInt(calls.text()) + i;
    calls.text(  val < 0 ? 0 : val );
  }

  AMIPack.prototype.hangup = function() {
    this.callsnum_decrease();
    this.newState('Down');
    $('#' + this.elemId()).fadeOut(1000,
        function(){$(this).remove();});
  }

  AMIPack.prototype.setAMIServers = function(resp) {
    if($("#amiserver-" + server_id).length) return;
    var rowdata = {
      "id": server_id,
      "callnum": 0, //resp.CoreCurrentCalls,
      "startdate": resp.CoreStartupDate,
      "starttime": resp.CoreStartupTime,
      "serv_name": server_name
    };
    $('#ami-servers-list').append(serv_tmpl(rowdata));
  }

  AMIPack.prototype.newState = function(state) {
    state = state ? state : this.header('ChannelStateDesc');
    var tr = $('#' + this.elemId());
    tr.children('.status')
      .css('color', colorPick(state));
    if(state == 'Up') {
      tr.children('.duration')
        .attr('status', 'up');
    }
  }

  AMIPack.prototype.newChannel = function() {
    var src = phoneNum(this.header('CallerIDNum'));
    var dst = phoneNum(this.header('Exten'));

    var rowdata = { "id": this.elemId(),
                    "start_time": dateFormat(new Date()),
                    "duration": durFormat(0),
                    "country_src": src.country,
                    "country_dst": dst.country,
                    "source": src.num,
                    "dest": dst.num,
                    "server_name": server_name,
                  };

    $(tableid + " tbody").append(row_tmpl(rowdata));

    this.callsnum_increase();
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
      case 'CoreShowChannel':
        if($('#' + this.elemId()).length){
          this.newState();
        } else {
          this.newChannel();
        }
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
        if(data.CoreStartupDate) this.setAMIServers(data);
        break;
      default:
        // Unknown type
        console.log('Unknown type: ' + type);
        break;
    }
  }

  return AMIPack;
})();

