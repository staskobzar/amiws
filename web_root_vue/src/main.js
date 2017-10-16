// The Vue build version to load with the `import` command
// (runtime-only or standalone) has been set in webpack.base.conf with an alias.
import Vue from 'vue';
import BootstrapVue from 'bootstrap-vue';

import 'bootstrap-vue/dist/bootstrap-vue.css';
import 'bootstrap/dist/css/bootstrap.css';
import { mapActions } from 'vuex';

import App from './App';
import websock from './websock';
import * as mtype from './store/mutation-types';
import store from './store';

Vue.config.productionTip = false;

Vue.use(BootstrapVue);

/* eslint-disable no-new */
new Vue({
  el: '#app',
  template: '<App/>',
  store,
  components: { App },
  created() {
    websock.connect(message => this.newMessage(message));
    setInterval(() => { store.commit(mtype.UPDATE_CALLS_DUR); }, 1000);
  },
  methods: mapActions(['newMessage']),
});
