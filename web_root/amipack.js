/**
 * AMI packet class
 */
var AMIPack = (function(){
  var type = 0;
  var utils;
  var visual;
  var data;
  var server_id;
  var server_name;
  var ami_is_ssl;
  var ami_type = [ 'UNKNOWN', 'PROMPT', 'ACTION', 'EVENT', 'RESPONSE' ];
  var state_color = {Ring: "#ffcc00", Up: "#00cc00", Down: "#cc0000"};
  var tableid = '#activecalls';

  function AMIPack(event, utils_class, visual_class){
    var json = JSON.parse(event);
    type = json.type;
    data = json.data;
    server_id = json.server_id;
    server_name = json.server_name;
    ami_is_ssl = json.ssl;
    utils = utils_class;
    visual = visual_class;
  }

  /**
   * Private
   */
  function colorPick(state) {
    var color = state_color[state];
    return color ? color : '#000000';
  }

  function getTotalCalls(){
    return utils.getTotalCalls();
  }

  function setTotalCalls(val){ utils.setTotalCalls(val); }

  /**
   * Public
   */
  AMIPack.prototype.typeName = function(){
    return ami_type[type] ? ami_type[type] : 'UNKNOWN';
  }

  AMIPack.prototype.header = function(header_name){
    return data[header_name];
  }

  AMIPack.prototype.elemId = function(){
    return server_id + '-' + this.header('Channel').
                replace(/[^-_a-zA-Z0-9]/g,'-');
  }

  AMIPack.prototype.callsnum_increase = function() { this.callsnum_update(1); }

  AMIPack.prototype.callsnum_decrease = function() { this.callsnum_update(-1); }

  AMIPack.prototype.callsnum_update = function(i) {
    var calls = $("#amiserver-" + server_id).find('div.calls-number').first();
    var val   = parseInt(calls.text()) + i;
    var total = visual.callsUpdate(val, server_id);
    calls.text(  val < 0 ? 0 : val );
    setTotalCalls(total);
    visual.updateCallsGauge("#calls-gauge-s" + server_id, total, val);
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
      "reloaddate": resp.CoreReloadDate,
      "reloadtime": resp.CoreReloadTime,
      "serv_name": server_name,
      "lock": ami_is_ssl ? 'fa-lock' : '',
    };
    $('#ami-servers-list').append($(utils.tmplAMIServ()(rowdata)));
    visual.addAMIServer(server_name, server_id);
    $("#amiserver-" + server_id).find('.amiserv-details').popover({container: 'body'});
    $("#amiserver-" + server_id).find('[data-toggle="tooltip"]')
        .tooltip({placement: 'top'});
    visual.setupCallsGauge('calls-gauge-s' + server_id);
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
    var src = utils.phoneNum(this.header('CallerIDNum'));
    var dst = utils.phoneNum(this.header('Exten'));

    var rowdata = { "id": this.elemId(),
                    "start_time": utils.dateFormat(new Date()),
                    "duration": utils.durFormat(0),
                    "country_src": src.country,
                    "country_dst": dst.country,
                    "source": src.num,
                    "dest": dst.num,
                    "server_name": server_name,
                  };
    if(dst.country) visual.updateMap(dst.country);

    $(tableid + " tbody").append(utils.tmplActivecallsRow()(rowdata));

    this.callsnum_increase();
  }

  AMIPack.prototype.setAudioQoS = function() {
    var qos = new Object();
    var srv = $("#amiserver-" + server_id);
    this.header('Value').split(';').forEach(function(el){
      var key = el.split('=')[0];
      var val = el.split('=')[1];
      var el  = srv.find('div.' + key + ' .value').first();
      var cur = el.text();
      if(['lp', 'rlp'].includes(key)){
        // integer values
        cur = parseInt(cur);
        val = parseInt(val);
        val = cur == 0 ? val : Math.round((cur + val) / 2);
      } else {
        // float values
        cur = parseFloat(cur);
        val = parseFloat(val);
        val = cur == 0 ? val : (cur + val) / 2;
        // normalize to 7 chars (including dot)
        val = val.toFixed( val > 1 ? Math.abs(6 - Math.ceil(Math.log10(val))) : 6);
      }
      srv.find('div.' + key + ' .value').text(val);
    });
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
        if($('#' + this.elemId()).length == 0){
          this.newChannel();
        }
        this.newState();
        break;
      case 'SoftHangupRequest':
      case 'HangupRequest':
        this.newState('Down');
        break;
      case 'Hangup':
        this.hangup();
        break;
      case 'CoreShowChannelsComplete':
        break;
      case 'VarSet':
        if(this.header('Variable') == 'RTPAUDIOQOS'){
          this.setAudioQoS();
        }
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

