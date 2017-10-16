import Vue from 'vue';
import Vuex from 'vuex';

import mutations from './mutations';
import actions from './actions';
import getters from './getters';

Vue.use(Vuex);

const state = {
  servers: [],
  calls: [],
  newCallCountry: '',
};

export default new Vuex.Store({
  state,
  actions,
  mutations,
  getters,
});
