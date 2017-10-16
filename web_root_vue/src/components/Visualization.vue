<template>
  <div class="panel panel-default">

    <div class="panel-heading">
      <h3 class="panel-title">{{ title }}</h3>
    </div>
    <div class="panel-body">
      <chart-calls-total :calls="callsPerServer"></chart-calls-total>
      <chart-calls-timeline :totalcalls="callsPerServer"></chart-calls-timeline>
      <chart-calls-map :country="newCallCountry" ref="chartcallsmap"></chart-calls-map>
    </div>

  </div>
</template>

<script>
import { mapGetters } from 'vuex';
import ChartCallsTotal from './charts/CallsTotal';
import ChartCallsTimeline from './charts/CallsTimeline';
import ChartCallsMap from './charts/CallsMap';

export default {
  name: 'visualization',
  data() {
    return {
      title: 'Visualization',
    };
  },
  computed: mapGetters({ callsPerServer: 'getCallsPerServer', newCallCountry: 'getCallCountry' }),
  components: {
    ChartCallsTotal,
    ChartCallsTimeline,
    ChartCallsMap,
  },
};
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style scoped>
</style>
