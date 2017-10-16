import 'babel-polyfill';
import Vue from 'vue';
import ServersList from '@/components/AMIServersList';
import * as mtype from '@/store/mutation-types';
import store from '@/store';


describe('AMI Servers List', () => {
  afterEach(() => {
    store.commit(mtype.CLEAR_AMISRV_LIST);
  });

  it('AMI servers component loaded', () => {
    const Constructor = Vue.extend(ServersList);
    const vm = new Constructor({ store }).$mount();
    expect(vm.$el.querySelector('h3.panel-title').textContent)
      .to.equal('AMI Servers');
  });

  it('empty AMI Servers list', () => {
    const Constructor = Vue.extend(ServersList);
    const vm = new Constructor({ store }).$mount();
    expect(vm.$el.querySelectorAll('div.amiserver').length)
      .to.equal(0);
  });

  it('list of one server', () => {
    const messages = [
      '{ "type": 4, "server_id": 1, "server_name": "asterisk01.local", ' +
      '"ssl": false, "data": {"Response": "Success","CoreStartupDate": ' +
      '"2017-10-08","CoreStartupTime": "20:28:26","CoreReloadDate": ' +
      '"2017-10-08","CoreReloadTime": "20:28:26","CoreCurrentCalls": "0"}}',
    ];
    messages.forEach(msg => store.dispatch('newMessage', msg));
    const Constructor = Vue.extend(ServersList);
    const vm = new Constructor({ store }).$mount();
    expect(vm.$el.querySelectorAll('div.amiserver').length)
      .to.equal(1);
  });

  it('list of 3 servers', () => {
    const messages = [
      '{ "type": 4, "server_id": 1, "server_name": "asterisk01.local", "ssl": false, "data": {"Response": "Success","CoreStartupDate": "2017-10-08","CoreStartupTime": "20:28:26","CoreReloadDate": "2017-10-08","CoreReloadTime": "20:28:26","CoreCurrentCalls": "0"}}',
      '{ "type": 4, "server_id": 2, "server_name": "pbx07.local", "ssl": false, "data": {"Response": "Success","CoreStartupDate": "2017-10-08","CoreStartupTime": "20:28:20","CoreReloadDate": "2017-10-08","CoreReloadTime": "20:28:20","CoreCurrentCalls": "0"}}',
      '{ "type": 4, "server_id": 3, "server_name": "pbx.bar.local", "ssl": false, "data": {"Response": "Success","CoreStartupDate": "2017-10-08","CoreStartupTime": "20:28:20","CoreReloadDate": "2017-10-08","CoreReloadTime": "20:28:20","CoreCurrentCalls": "0"}}',
    ];
    messages.forEach(msg => store.dispatch('newMessage', msg));
    const Constructor = Vue.extend(ServersList);
    const vm = new Constructor({ store }).$mount();
    expect(vm.$el.querySelectorAll('div.amiserver').length)
      .to.equal(3);
  });
});
