/**
 * Data visualization with charts
 */
var Visual = (function(){

  var mapdata = [
      ['fo', 0], ['us', 0], ['jp', 0], ['in', 0], ['kr', 0], ['fr', 0],
      ['cn', 0], ['sw', 0], ['sh', 0], ['ec', 0], ['au', 0], ['ph', 0],
      ['es', 0], ['bu', 0], ['mv', 0], ['sp', 0], ['ve', 0], ['gb', 0],
      ['gr', 0], ['dk', 0], ['gl', 0], ['pr', 0], ['um', 0], ['vi', 0],
      ['ca', 0], ['tz', 0], ['cv', 0], ['dm', 0], ['sc', 0], ['nz', 0],
      ['ye', 0], ['jm', 0], ['om', 0], ['vc', 0], ['bd', 0], ['sb', 0],
      ['lc', 0], ['no', 0], ['cu', 0], ['kn', 0], ['bh', 0], ['fi', 0],
      ['id', 0], ['mu', 0], ['se', 0], ['ru', 0], ['tt', 0], ['br', 0],
      ['bs', 0], ['pw', 0], ['ir', 0], ['cl', 0], ['gw', 0], ['gd', 0],
      ['ee', 0], ['ag', 0], ['tw', 0], ['fj', 0], ['bb', 0], ['it', 0],
      ['mt', 0], ['pg', 0], ['de', 0], ['vu', 0], ['gq', 0], ['cy', 0],
      ['km', 0], ['va', 0], ['sm', 0], ['az', 0], ['am', 0], ['sd', 0],
      ['ly', 0], ['tj', 0], ['ls', 0], ['uz', 0], ['pt', 0], ['mx', 0],
      ['ma', 0], ['ar', 0], ['sa', 0], ['nl', 0], ['ae', 0], ['ke', 0],
      ['my', 0], ['ht', 0], ['do', 0], ['hr', 0], ['th', 0], ['cd', 0],
      ['kw', 0], ['ie', 0], ['mm', 0], ['ug', 0], ['kz', 0], ['tr', 0],
      ['er', 0], ['tl', 0], ['mr', 0], ['dz', 0], ['pe', 0], ['ao', 0],
      ['mz', 0], ['cr', 0], ['pa', 0], ['sv', 0], ['kh', 0], ['gt', 0],
      ['bz', 0], ['kp', 0], ['gy', 0], ['hn', 0], ['il', 0], ['ni', 0],
      ['mw', 0], ['tm', 0], ['zm', 0], ['nc', 0], ['za', 0], ['lt', 0],
      ['et', 0], ['gh', 0], ['si', 0], ['ba', 0], ['jo', 0], ['sy', 0],
      ['mc', 0], ['al', 0], ['uy', 0], ['cnm', 0], ['mn', 0], ['rw', 0],
      ['sx', 0], ['bo', 0], ['ga', 0], ['cm', 0], ['cg', 0], ['eh', 0],
      ['me', 0], ['rs', 0], ['bj', 0], ['tg', 0], ['af', 0], ['ua', 0],
      ['sk', 0], ['jk', 0], ['pk', 0], ['bg', 0], ['ro', 0], ['qa', 0],
      ['li', 0], ['at', 0], ['sz', 0], ['hu', 0], ['ne', 0], ['lu', 0],
      ['ad', 0], ['ci', 0], ['sl', 0], ['lr', 0], ['bn', 0], ['be', 0],
      ['iq', 0], ['ge', 0], ['gm', 0], ['ch', 0], ['td', 0], ['ng', 0],
      ['kv', 0], ['lb', 0], ['dj', 0], ['bi', 0], ['sr', 0], ['sn', 0],
      ['gn', 0], ['zw', 0], ['pl', 0], ['mk', 0], ['py', 0], ['by', 0],
      ['lv', 0], ['bf', 0], ['ss', 0], ['na', 0], ['la', 0], ['co', 0],
      ['ml', 0], ['cz', 0], ['cf', 0], ['sg', 0], ['vn', 0], ['tn', 0],
      ['mg', 0], ['eg', 0], ['so', 0], ['is', 0], ['lk', 0], ['np', 0],
      ['kg', 0], ['md', 0], ['bt', 0], ['bw', 0]
  ];

  var total_calls;
  var map;
  var calls_history;
  var indexes = new Array();

  function Visual(){
    total_calls = createTotalCalls();
    map = createMapChart();
    calls_history = createCallsHistChart();
    mapdata.forEach(function(el){indexes.push(el['hc-key']);});
  }

  /**
   * Public interfaces
   */
  Visual.prototype.totalCallsChart = function() {
    return total_calls;
  }

  Visual.prototype.addAMIServer = function(name, id) {
    total_calls.series[0].addPoint({name: name, y: 0, id: id});
  }

  Visual.prototype.callsUpdate = function(server_calls, server_id) {
    var calls = updateTotalcallsData(server_calls, server_id);
    updateTitle(calls);
    return calls;
  }

  Visual.prototype.reset = function() {
    updateTitle(0);
    total_calls.series[0].setData([]);
  }

  Visual.prototype.setupCallsGauge = function(elemid) {
    new Highcharts.Chart({
      chart: {
        plotBackgroundColor: '#337ab7',
        renderTo: elemid,
        type: 'pie',
        margin: [0, 0, 0, 0],
        spacingTop: 0,
        spacingBottom: 0,
        spacingLeft: 0,
        spacingRight: 0,
        width: 100,
        height: 50
      },
      credits: { enabled: false },
      legend: { enabled: false },
      tooltip: { enabled: false },
      title: null,
      plotOptions: {
        pie: {
          size: '400%',
          dataLabels: { enabled: false },
          startAngle: -90,
          endAngle: 90,
          center: ['50%', '200%']
        }
      },
      series: [{
        name: 'Calls',
        data: [0,1]
      }]
    });
  }

  Visual.prototype.updateCallsGauge = function(elid, totalcalls, servcalls) {
    var gaugeid = $(elid).data('highchartsChart');
    if(undefined == gaugeid) return;

    Highcharts.charts[gaugeid].series[0].setData([servcalls, totalcalls - servcalls])
  }

  Visual.prototype.updateMap = function(country) {
    var idx = indexes.indexOf(country);
    var val = 0;
    if(idx < 0) return;
    val = map.series[0].points[idx].value + 1;
    map.series[0].points[idx].update({value: val});
  }

  /**
   * Private functions
   */
  function updateTitle(calls){
    var title = '<span style="font-size:32px;font-weight:bold">';
    title += calls + '</span><br/>total calls';
    total_calls.setTitle({ text: title });
  }

  function updateTotalcallsData(server_calls, server_id){
    var data = new Array(), total = 0, calls = 0;
    total_calls.series[0].data.forEach(function(item, i){
      calls = (item.id == server_id ? server_calls : item.y);
      data.push(calls);
      total += calls;
    });
    total_calls.series[0].setData(data,true, true, true);
    return total;
  }

  function createTotalCalls() {
    return new Highcharts.Chart({
      chart: {
        renderTo: 'callsdistr',
        type: 'pie'
      },
      title: {
        text: '',
        align: 'center',
        verticalAlign: 'middle',
        style: { "color": "#333333", "fontSize": "12px" },
        y: 5
      },
      credits: { enabled: false },
      plotOptions: {
        pie: {
          dataLabels: { enabled: false, },
        }
      },
      series: [{
        name: 'Calls',
        innerSize: '60%'
      }]
    });
  }

  function createCallsHistChart(){
    return new Highcharts.Chart({
      chart: {
        zoomType: 'x',
        events: {
          load: function () {
            var series = this.series[0];
            setInterval(function () {
              var x = (new Date()).getTime(),
                  y = parseInt($("#ami-servers-list").attr('data-total-calls'));
              series.addPoint([x, y], true, true);
            }, 1000);
          }
        },
        renderTo: 'callschart',
        type: 'area',
        height: 100
      },
      title: {
        text: 'Last 10 min calls history',
        style: { "color": "#333333", "fontSize": "11px" }
      },
      subtitle: { enabled: false },
      legend: { enabled: false },
      credits: { enabled: false },
      xAxis: { visible: false, type: 'datetime' },
      yAxis: { title: null },
      plotOptions: {
        area:{
          lineWidth: 1,
          marker: { radius: 1 },
          states: { hover: { lineWidth: 1 } },
        }
      },
      series: [{
        name: 'Calls',
        data: (function(){
          var data = [], i = 0, time = (new Date()).getTime();
          for (i = -600; i <= 0; i += 1) {
            data.push([ (time + i * 1000), 0 ]);
          }
          return data;
        }())
      }]
    });
  }

  function createMapChart() {
    return new Highcharts.mapChart('callsmap',{
      chart: {
        map: 'custom/world',
      },
      title: null,
      subtitle: { enabled: false },
      legend: { enabled: false },
      credits: { enabled: false },

      mapNavigation: {
        enabled: true,
        buttonOptions: {
          verticalAlign: 'bottom'
        }
      },

      colorAxis: {
        min: 0
      },

      series: [{
        name: 'Calls destination by country',
        data: mapdata
      }]
    });
  }

  return Visual;
})();
