<template>
  <div class="visual-chart calls-map"></div>
</template>

<script>
import HC from 'highcharts/highmaps';

import world from './world';
import mapdata from './mapdata';

export default {
  name: 'chart-calls-map',
  data() {
    return {
      chart: null,
    };
  },
  props: ['country'],
  watch: {
    country(value) {
      if (value) {
        this.chart.series[0].points.forEach((p) => {
          if (p['hc-key'] === value) {
            p.update({ value: p.value + 1 });
          }
        });
      }
    },
  },
  mounted() {
    this.chart = HC.mapChart(this.$el, {
      chart: {
        height: 200,
      },
      title: null,
      subtitle: { enabled: false },
      legend: { enabled: false },
      credits: { enabled: false },
      mapNavigation: {
        enabled: true,
        buttonOptions: {
          verticalAlign: 'bottom',
        },
      },
      colorAxis: {
        min: 0,
      },
      series: [{
        mapData: world,
        name: 'Calls destination by country',
        data: mapdata,
      }],
    });
  },
};
</script>

<style scoped>
.calls-map{width:100%;height:200px}
</style>
