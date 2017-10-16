<template>
  <div class="calls-gauge col-xs-3"></div>
</template>

<script>
import { mapGetters } from 'vuex';
import HC from 'highcharts';

export default {
  name: 'chart-server-gauge',
  data() {
    return {
      chart: null,
    };
  },
  props: ['calls'],
  watch: {
    calls(value) {
      const total = this.getAmiServers()
        .map(s => s.calls)
        .reduce((t, c) => t + c);

      this.chart.series[0].setData([total - value, value]);
    },
  },
  methods: mapGetters(['getAmiServers']),
  mounted() {
    this.chart = HC.chart(this.$el, {
      chart: {
        plotBackgroundColor: '#337ab7',
        type: 'pie',
        margin: [0, 0, 0, 0],
        spacingTop: 0,
        spacingBottom: 0,
        spacingLeft: 0,
        spacingRight: 0,
        width: 100,
        height: 50,
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
          center: ['50%', '200%'],
        },
      },
      series: [{
        name: 'Calls',
        data: [0, 1],
      }],
    });
  },
};
</script>

<style scoped>
.calls-distribution-chart {width:200px; height:200px; margin: auto}
</style>
