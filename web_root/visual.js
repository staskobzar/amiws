/**
 * Data visualization with charts
 */
var Visual = (function(){
  var total_calls;
  var map;

  function Visual(){
    total_calls = createTotalCalls();
    map = createMapChart();
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

      series: [{
        name: 'Countries',
        color: '#E0E0E0',
        enableMouseTracking: false
      }, {
        type: 'mapbubble',
        name: 'By country',
        joinBy: ['iso-a2', 'code'],
        minSize: 4,
        maxSize: '12%',
        tooltip: {
          pointFormat: '{point.code}: {point.z} calls'
        }
      }]
    });
  }

  return Visual;
})();
