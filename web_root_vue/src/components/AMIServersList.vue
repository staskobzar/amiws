<template>
  <div class="ami-servers-list panel panel-default">
    <div class="panel-heading">
      <h3 class="panel-title">{{ title }}</h3>
    </div>
    <div class="panel-body">
      <ami-server v-for="server in amiServers" :server="server" :key="server.id">
      </ami-server>
    </div>
  </div>
</template>

<script>
import { mapGetters } from 'vuex';
import AmiServer from './AMIServer';

export default {
  name: 'ami-servers-list',
  data() {
    return {
      title: 'AMI Servers',
    };
  },
  computed: mapGetters({ amiServers: 'getAmiServers' }),
  components: {
    AmiServer,
  },
};
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style scoped>
</style>
