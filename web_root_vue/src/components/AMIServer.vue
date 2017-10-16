<template>
  <div class="amiserver col-lg-12">
    <div class="panel panel-primary">

      <div class="panel-heading">
        <div class="row">

          <div class="col-xs-3">
            <icon name="tasks" scale="2"></icon>
            <icon v-if="server.ssl" name="lock" class="is-ssl"></icon>
            <icon v-else name="unlock"></icon>
          </div>
          <chart-server-gauge :calls="server.calls"></chart-server-gauge>
          <div class="col-xs-6 text-right">
            <div class="calls-number">{{ server.calls }}</div>
            <div>Calls</div>
          </div>

        </div>
      </div>

      <div class="panel-body">
        <div class="row small text-center">
          <div class="col-md-3 lp" data-toggle="tooltip" title="Lost packets" >
            <div class="value">{{ server.lp }}</div><div class="bg-primary">lp </div></div>
          <div class="col-md-3 rlp" data-toggle="tooltip" title="Remote lost packets">
            <div class="value">{{ server.rlp }}</div><div class="bg-primary">rlp </div></div>
          <div class="col-md-6 rtt" data-toggle="tooltip" title="Round trip time/RTT">
            <div class="value">{{ server.rtt | numfrac }}</div><div class="bg-primary">rtt</div></div>
        </div>
        <div class="row small text-center">
          <div class="col-md-6 rxjitter" data-toggle="tooltip" title="Reciever jitter">
            <div class="value">{{ server.rxjitter | numfrac }}</div><div class="bg-primary">rxjitter</div></div>
          <div class="col-md-6 txjitter" data-toggle="tooltip" title="Reported transmitted jitter">
            <div class="value">{{ server.txjitter | numfrac }}</div><div class="bg-primary">txjitter</div></div>
        </div>
      </div>

      <div class="panel-footer">
        <div class="amiserv-details"
          title="server.name"
          >
          <span class="pull-left server-name">{{ server.name }}</span>
          <span class="pull-right">
            <icon name="arrow-circle-right"></icon>
          </span>
          <div class="clearfix"></div>
        </div>
      </div>

    </div>
  </div>
</template>

<script>
import Icon from 'vue-awesome/components/Icon';
import 'vue-awesome/icons/tasks';
import 'vue-awesome/icons/arrow-circle-right';
import 'vue-awesome/icons/lock';
import 'vue-awesome/icons/unlock';
import ChartServerGauge from './charts/ServerGauge';

export default {
  name: 'ami-server',
  props: ['server'],
  components: {
    Icon,
    ChartServerGauge,
  },
  filters: {
    numfrac: value => (+value).toFixed(6).toString(),
  },
};
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style scoped>
</style>
