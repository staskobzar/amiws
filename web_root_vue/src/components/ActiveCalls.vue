<template>
  <div class="panel panel-default">
    <div class="panel-heading">
      <h1 class="panel-title">{{ title }}</h1>
    </div>
    <div class="panel-body ac-panel-body active-calls" >
      <b-table striped bordered small
        :items="activeCalls"
        :sort-by.sync="sortBy"
        :sort-desc.sync="sortDesc"
        :fields="fields"
      >

        <template slot="stateStr" scope="data">
          <icon name="phone-square" :class="getStatusClass(data.value)"></icon>
        </template>

        <template slot="startTime" scope="data">
          {{ data.value | formatDate }}
        </template>

        <template slot="duration" scope="data">
          {{ data.value | formatDuration }}
        </template>

        <template slot="source" scope="data">
          <span :class="countryFlagClass(data.value.countryISO)"></span>
           {{ data.value.number }}
        </template>

        <template slot="dest" scope="data">
          <span :class="countryFlagClass(data.value.countryISO)"></span>
           {{ data.value.number }}
        </template>

      </b-table>
    </div>
  </div>
</template>

<script>
import Icon from 'vue-awesome/components/Icon';
import { mapGetters } from 'vuex';
import moment from 'moment';

import 'flag-icon-css/css/flag-icon.min.css';
import 'vue-awesome/icons/phone-square';

export default {
  name: 'active-calls',
  data() {
    return {
      title: 'Active Calls',
      sortBy: null,
      sortDesc: null,
      fields: [
        { key: 'stateStr', label: ' ', sortable: false, class: 'state' },
        { key: 'startTime', label: 'Start Time', sortable: true, class: 'start-time' },
        { key: 'duration', label: 'Duration', sortable: true, class: 'duration' },
        { key: 'source', label: 'Source', sortable: true },
        { key: 'dest', label: 'Destination', sortable: true },
        { key: 'server', label: 'Server Name', sortable: true, class: 'server-name' },
      ],
    };
  },
  methods: {
    countryFlagClass(value) {
      return `flag-icon flag-icon-${value}`;
    },
    getStatusClass(value) {
      switch (value) {
        case 'Up':
          return 'call-status-up';
        case 'Down':
          return 'call-status-down';
        case 'Ring':
          return 'call-status-ring';
        default:
          return 'call-status-down';
      }
    },
  },
  components: { Icon },
  computed: mapGetters({ activeCalls: 'getActiveCalls' }),
  filters: {
    formatDuration(value) {
      const dur = moment.duration(+value, 'seconds');
      const format = v => (v > 9 ? v : `0${v}`);
      return `${format(dur.hours())}:${format(dur.minutes())}:${format(dur.seconds())}`;
    },
    formatDate(value) {
      return moment(value).format('YYYY-MM-DD hh:mm:ss');
    },
  },
};
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style scoped>
td.server-name {font-weight: bold}
.call-status-up {color: #00cc00}
.call-status-ring {color: #ffcc00}
.call-status-down {color: #cc0000}
</style>
