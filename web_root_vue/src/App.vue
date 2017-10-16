<template>
  <b-container fluid id="app">
    <b-navbar toggleable="md" type="inverse" variant="inverse">
      <b-navbar-brand href="#">{{ title }}</b-navbar-brand>
    </b-navbar>
    <b-row>
      <b-col lg="2">
        <ami-servers-list></ami-servers-list>
      </b-col>

      <main class="col-lg-8">
        <active-calls></active-calls>
      </main>

      <b-col lg="2" class="visualization">
        <visualization></visualization>
      </b-col>
    </b-row>
  </b-container>
</template>

<script>
import AmiServersList from './components/AMIServersList';
import Visualization from './components/Visualization';
import ActiveCalls from './components/ActiveCalls';

export default {
  name: 'app',
  data() {
    return {
      title: 'AMI Calls Dashboard',
    };
  },
  components: {
    AmiServersList,
    Visualization,
    ActiveCalls,
  },
};
</script>

<style>
body {padding: 10px 40px;}
#app {}
</style>
