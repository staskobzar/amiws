<template>
  <div class="visual-chart calls-chart"></div>
</template>

<script>
import HC from 'highcharts';

export default {
  name: 'chart-calls-timeline',
  data() {
    return {
      chart: null,
    };
  },
  props: ['totalcalls'],
  mounted() {
    this.chart = HC.chart(this.$el, {
      chart: {
        zoomType: 'x',
        type: 'area',
        height: 100,
      },
      title: {
        text: 'Last 10 min calls history',
        style: { color: '#333333', fontSize: '11px' },
      },
      subtitle: { enabled: false },
      legend: { enabled: false },
      credits: { enabled: false },
      xAxis: { visible: false, type: 'datetime' },
      yAxis: { title: null },
      plotOptions: {
        area: {
          lineWidth: 1,
          marker: { radius: 1 },
          states: { hover: { lineWidth: 1 } },
        },
      },
      series: [{
        name: 'Calls',
        data: this.initData(),
      }],
    });
    // update chart every second
    setInterval(() => {
      const data = this.totalcalls.length ?
        this.totalcalls.reduce((t, c) => t + c) :
        [];

      this.chart.series[0]
        .addPoint([(new Date()).getTime(), data],
          true, true);
    }, 1000);
  },
  methods: {
    initData() {
      const data = [];
      const time = (new Date()).getTime();
      for (let i = -600; i <= 0; i += 1) {
        data.push([(time + (i * 1000)), 0]);
      }
      return data;
    },
  },
};

</script>
