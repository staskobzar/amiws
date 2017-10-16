<template>
  <div class="visual-chart calls-distribution-chart"></div>
</template>

<script>
import HC from 'highcharts';

export default {
  name: 'chart-calls-total',
  data() {
    return {
      chart: null,
    };
  },
  props: {
    calls: {
      type: Array,
      required: true,
    },
  },
  watch: {
    calls(val) {
      const total = val.reduce((t, n) => t + n);
      const title = `<span style="font-size:32px;font-weight:bold">
                    ${total}</span><br/>total calls`;
      this.chart.series[0].setData(val);
      this.chart.setTitle({ text: title });
    },
  },
  mounted() {
    this.chart = HC.chart(this.$el, {
      chart: {
        type: 'pie',
      },
      title: {
        text: '',
        align: 'center',
        verticalAlign: 'middle',
        style: { color: '#333333', fontSize: '12px' },
        y: 5,
      },
      credits: { enabled: false },
      plotOptions: {
        pie: {
          dataLabels: { enabled: false },
        },
      },
      series: [{
        name: 'Calls',
        innerSize: '60%',
      }],
    });
  },
};
</script>

<style scoped>
.calls-distribution-chart {width:200px; height:200px; margin: auto}
</style>
