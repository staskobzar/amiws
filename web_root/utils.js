/**
 * Project utilities class
 */
var Utils = (function(){

  function Utils(){}

  Utils.prototype.reset = function(){
    this.setTotalCalls(0);
  }

  Utils.prototype.tmplActivecallsRow = function(){
    return Handlebars.compile($("#activecall-row-tmpl").html());
  }

  Utils.prototype.tmplAMIServ = function(){
    return Handlebars.compile($("#ami-server-tmpl").html());
  }

  Utils.prototype.durFormat = function(val) {
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

  Utils.prototype.dateFormat = function(d) {
    var yyyy = d.getFullYear();
    var mm   = ('0' + (d.getMonth() + 1)).slice(-2);
    var dd   = ('0' + d.getDate()).slice(-2);

    var HH   = ('0' + d.getHours()).slice(-2);
    var MM   = ('0' + d.getMinutes()).slice(-2);
    var SS   = ('0' + d.getSeconds()).slice(-2);

    return yyyy + '-' + mm + '-' + dd + ' ' +
           HH + ':' + MM + ':' + SS;
  }

  Utils.prototype.timeToInt = function(val) {
    var t = val.split(':');
    return (t[0] ? parseInt(t[0]) * 3600 : 0 ) +
           (t[1] ? parseInt(t[1]) * 60   : 0 ) +
           (t[2] ? parseInt(t[2])        : 0 );
  }

  Utils.prototype.phoneNum = function(val) {
    var num = libphonenumber.parse('+' + val);
    if(num.phone) {
      return {"country": num.country.toLowerCase(),
              "num": libphonenumber.format(num,'International')
             };
    } else {
      return {"country": null, "num": val};
    }
  }

  Utils.prototype.updateDuration = function() {
    var utils = this;
    $("#activecalls tbody td.duration")
      .each(function(){
        var len = parseInt($(this).attr('value'));
        if($(this).attr('status') == 'up') {
          len++;
          $(this).attr('value', len);
          $(this).text(utils.durFormat(len));
        }
      });
  }

  Utils.prototype.setTotalCalls = function(val) {
    $("#ami-servers-list").attr('data-total-calls', val);
  }

  Utils.prototype.getTotalCalls = function() {
    var val = $("#ami-servers-list").attr('data-total-calls');
    return parseInt(val);
  }

  return Utils;
})();

