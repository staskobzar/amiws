import 'babel-polyfill';
import Vue from 'vue';
import BootstrapVue from 'bootstrap-vue';
import ActiveCalls from '@/components/ActiveCalls';
import * as mtype from '@/store/mutation-types';
import store from '@/store';

Vue.use(BootstrapVue);

describe('Active Calls', () => {
  afterEach(() => {
    store.commit(mtype.CLEAR_CALLS_LIST);
  });

  it('new call initiated and up', () => {
    const messages = [
      '{ "type": 3, "server_id": 1, "server_name": "asterisk01.local", "ssl": false, "data": {"Event": "Newchannel","Privilege": "call,all","Channel": "SIP/sipp-0000009a","ChannelState": "0","ChannelStateDesc": "Down","CallerIDNum": "15078897695","CallerIDName": "Alice","ConnectedLineNum": "<unknown>","ConnectedLineName": "<unknown>","Language": "en","AccountCode": "","Context": "from-sip","Exten": "16725558473","Priority": "1","Uniqueid": "1507761845.308","Linkedid": "1507761845.308"}}',
      '{ "type": 3, "server_id": 1, "server_name": "asterisk01.local", "ssl": false, "data": {"Event": "Newstate","Privilege": "call,all","Channel": "SIP/sipp-0000009a","ChannelState": "4","ChannelStateDesc": "Ring","CallerIDNum": "15078897695","CallerIDName": "Alice","ConnectedLineNum": "<unknown>","ConnectedLineName": "<unknown>","Language": "en","AccountCode": "","Context": "from-sip","Exten": "16725558473","Priority": "1","Uniqueid": "1507761845.308","Linkedid": "1507761845.308"}}',
    ];
    messages.forEach(msg => store.dispatch('newMessage', msg));
    const Constructor = Vue.extend(ActiveCalls);
    const vm = new Constructor({ store }).$mount();
    expect(vm.$el.querySelectorAll('div.active-calls table tbody tr').length)
      .to.equal(1);
  });

  it('one call up and new call ringing', () => {
    const messages = [
      '{ "type": 3, "server_id": 1, "server_name": "asterisk01.local", "ssl": false, "data": {"Event": "Newchannel","Privilege": "call,all","Channel": "SIP/sipp-0000009a","ChannelState": "0","ChannelStateDesc": "Down","CallerIDNum": "15078897695","CallerIDName": "Alice","ConnectedLineNum": "<unknown>","ConnectedLineName": "<unknown>","Language": "en","AccountCode": "","Context": "from-sip","Exten": "16725558473","Priority": "1","Uniqueid": "1507761845.308","Linkedid": "1507761845.308"}}',
      '{ "type": 3, "server_id": 1, "server_name": "asterisk01.local", "ssl": false, "data": {"Event": "Newstate","Privilege": "call,all","Channel": "SIP/sipp-0000009a","ChannelState": "4","ChannelStateDesc": "Ring","CallerIDNum": "15078897695","CallerIDName": "Alice","ConnectedLineNum": "<unknown>","ConnectedLineName": "<unknown>","Language": "en","AccountCode": "","Context": "from-sip","Exten": "16725558473","Priority": "1","Uniqueid": "1507761845.308","Linkedid": "1507761845.308"}}',
      '{ "type": 3, "server_id": 2, "server_name": "pbx07.local", "ssl": false, "data": {"Event": "Newchannel","Privilege": "call,all","Channel": "SIP/sipp-00000093","ChannelState": "0","ChannelStateDesc": "Down","CallerIDNum": "15078897695","CallerIDName": "Alice","ConnectedLineNum": "<unknown>","ConnectedLineName": "<unknown>","Language": "en","AccountCode": "","Context": "from-sip","Exten": "16725558473","Priority": "1","Uniqueid": "1507765205.294","Linkedid": "1507765205.294"}}',
      '{ "type": 3, "server_id": 2, "server_name": "pbx07.local", "ssl": false, "data": {"Event": "Newstate","Privilege": "call,all","Channel": "SIP/sipp-00000093","ChannelState": "4","ChannelStateDesc": "Ring","CallerIDNum": "15078897695","CallerIDName": "Alice","ConnectedLineNum": "<unknown>","ConnectedLineName": "<unknown>","Language": "en","AccountCode": "","Context": "from-sip","Exten": "16725558473","Priority": "1","Uniqueid": "1507765205.294","Linkedid": "1507765205.294"}}',
      '{ "type": 4, "server_id": 2, "server_name": "pbx07.local", "ssl": false, "data": {"Event": "Newstate","Privilege": "call,all","Channel": "SIP/sipp-00000093","ChannelState": "6","ChannelStateDesc": "Up","CallerIDNum": "15078897695","CallerIDName": "Alice","ConnectedLineNum": "<unknown>","ConnectedLineName": "<unknown>","Language": "en","AccountCode": "","Context": "from-sip","Exten": "16725558473","Priority": "4","Uniqueid": "1507765205.294","Linkedid": "1507765205.294"}}',
    ];
    messages.forEach(msg => store.dispatch('newMessage', msg));
    const Constructor = Vue.extend(ActiveCalls);
    const vm = new Constructor({ store }).$mount();
    expect(vm.$el.querySelectorAll('div.active-calls table tbody tr').length)
      .to.equal(2);
  });

  it('hang up call with two active calls', () => {
    const messages = [
      '{ "type": 3, "server_id": 1, "server_name": "asterisk01.local", "ssl": false, "data": {"Event": "Newchannel","Privilege": "call,all","Channel": "SIP/sipp-0000009a","ChannelState": "0","ChannelStateDesc": "Down","CallerIDNum": "15078897695","CallerIDName": "Alice","ConnectedLineNum": "<unknown>","ConnectedLineName": "<unknown>","Language": "en","AccountCode": "","Context": "from-sip","Exten": "16725558473","Priority": "1","Uniqueid": "1507761845.308","Linkedid": "1507761845.308"}}',
      '{ "type": 3, "server_id": 1, "server_name": "asterisk01.local", "ssl": false, "data": {"Event": "Newstate","Privilege": "call,all","Channel": "SIP/sipp-0000009a","ChannelState": "4","ChannelStateDesc": "Ring","CallerIDNum": "15078897695","CallerIDName": "Alice","ConnectedLineNum": "<unknown>","ConnectedLineName": "<unknown>","Language": "en","AccountCode": "","Context": "from-sip","Exten": "16725558473","Priority": "1","Uniqueid": "1507761845.308","Linkedid": "1507761845.308"}}',
      '{ "type": 3, "server_id": 1, "server_name": "asterisk01.local", "ssl": false, "data": {"Event": "Newstate","Privilege": "call,all","Channel": "SIP/sipp-0000009a","ChannelState": "6","ChannelStateDesc": "Up","CallerIDNum": "15078897695","CallerIDName": "Alice","ConnectedLineNum": "<unknown>","ConnectedLineName": "<unknown>","Language": "en","AccountCode": "","Context": "from-sip","Exten": "16725558473","Priority": "4","Uniqueid": "1507761845.308","Linkedid": "1507761845.308"}}',
      '{ "type": 3, "server_id": 2, "server_name": "pbx07.local", "ssl": false, "data": {"Event": "Newchannel","Privilege": "call,all","Channel": "SIP/sipp-00000093","ChannelState": "0","ChannelStateDesc": "Down","CallerIDNum": "15078897695","CallerIDName": "Alice","ConnectedLineNum": "<unknown>","ConnectedLineName": "<unknown>","Language": "en","AccountCode": "","Context": "from-sip","Exten": "16725558473","Priority": "1","Uniqueid": "1507765205.294","Linkedid": "1507765205.294"}}',
      '{ "type": 3, "server_id": 2, "server_name": "pbx07.local", "ssl": false, "data": {"Event": "Newstate","Privilege": "call,all","Channel": "SIP/sipp-00000093","ChannelState": "4","ChannelStateDesc": "Ring","CallerIDNum": "15078897695","CallerIDName": "Alice","ConnectedLineNum": "<unknown>","ConnectedLineName": "<unknown>","Language": "en","AccountCode": "","Context": "from-sip","Exten": "16725558473","Priority": "1","Uniqueid": "1507765205.294","Linkedid": "1507765205.294"}}',
      '{ "type": 3, "server_id": 2, "server_name": "pbx07.local", "ssl": false, "data": {"Event": "Newstate","Privilege": "call,all","Channel": "SIP/sipp-00000093","ChannelState": "6","ChannelStateDesc": "Up","CallerIDNum": "15078897695","CallerIDName": "Alice","ConnectedLineNum": "<unknown>","ConnectedLineName": "<unknown>","Language": "en","AccountCode": "","Context": "from-sip","Exten": "16725558473","Priority": "4","Uniqueid": "1507765205.294","Linkedid": "1507765205.294"}}',
      '{ "type": 3, "server_id": 1, "server_name": "asterisk01.local", "ssl": false, "data": {"Event": "Hangup","Privilege": "call,all","Channel": "SIP/sipp-0000009a","ChannelState": "6","ChannelStateDesc": "Up","CallerIDNum": "15078897695","CallerIDName": "Alice","ConnectedLineNum": "<unknown>","ConnectedLineName": "<unknown>","Language": "en","AccountCode": "","Context": "from-sip","Exten": "16725558473","Priority": "8","Uniqueid": "1507761845.308","Linkedid": "1507761845.308","Cause": "0","Cause-txt": "Unknown"}}',
    ];
    messages.forEach(msg => store.dispatch('newMessage', msg));
    const Constructor = Vue.extend(ActiveCalls);
    const vm = new Constructor({ store }).$mount();
    expect(vm.$el.querySelectorAll('div.active-calls table tbody tr').length)
      .to.equal(1);
  });

  it('update duration on active calls', () => {
    const messages = [
      '{ "type": 3, "server_id": 1, "server_name": "asterisk01.local", "ssl": false, "data": {"Event": "Newchannel","Privilege": "call,all","Channel": "SIP/sipp-0000009a","ChannelState": "0","ChannelStateDesc": "Down","CallerIDNum": "15078897695","CallerIDName": "Alice","ConnectedLineNum": "<unknown>","ConnectedLineName": "<unknown>","Language": "en","AccountCode": "","Context": "from-sip","Exten": "16725558473","Priority": "1","Uniqueid": "1507761845.308","Linkedid": "1507761845.308"}}',
      '{ "type": 3, "server_id": 1, "server_name": "asterisk01.local", "ssl": false, "data": {"Event": "Newstate","Privilege": "call,all","Channel": "SIP/sipp-0000009a","ChannelState": "4","ChannelStateDesc": "Ring","CallerIDNum": "15078897695","CallerIDName": "Alice","ConnectedLineNum": "<unknown>","ConnectedLineName": "<unknown>","Language": "en","AccountCode": "","Context": "from-sip","Exten": "16725558473","Priority": "1","Uniqueid": "1507761845.308","Linkedid": "1507761845.308"}}',
      '{ "type": 3, "server_id": 1, "server_name": "asterisk01.local", "ssl": false, "data": {"Event": "Newstate","Privilege": "call,all","Channel": "SIP/sipp-0000009a","ChannelState": "6","ChannelStateDesc": "Up","CallerIDNum": "15078897695","CallerIDName": "Alice","ConnectedLineNum": "<unknown>","ConnectedLineName": "<unknown>","Language": "en","AccountCode": "","Context": "from-sip","Exten": "16725558473","Priority": "4","Uniqueid": "1507761845.308","Linkedid": "1507761845.308"}}',
    ];
    messages.forEach(msg => store.dispatch('newMessage', msg));

    for (let i = 0; i < 89; i++) {
      store.commit(mtype.UPDATE_CALLS_DUR);
    }
    const Constructor = Vue.extend(ActiveCalls);
    const vm = new Constructor({ store }).$mount();
    expect(vm.$el.querySelectorAll('div.active-calls table tbody tr').length)
      .to.equal(1);
    expect(vm.$el.querySelector('div.active-calls table tbody tr td.duration').textContent.trim())
      .to.equal('00:01:29');
    expect(vm.$el.querySelector('div.active-calls table tbody tr td.server-name').textContent.trim())
      .to.equal('asterisk01.local');
  });

  it('new calls on CoreShowChannel event', () => {
    const messages = [
      '{ "type": 3, "server_id": 1, "server_name": "asterisk01.local", "ssl": false, "data": {"Event": "CoreShowChannel","Channel": "SIP/sipp-000000c3","ChannelState": "6","ChannelStateDesc": "Up","CallerIDNum": "18439184039","CallerIDName": "Alice","ConnectedLineNum": "<unknown>","ConnectedLineName": "<unknown>","Language": "en","AccountCode": "","Context": "from-sip","Exten": "15417285104","Priority": "7","Uniqueid": "1508082097.387","Linkedid": "1508082097.387","Application": "MusicOnHold","ApplicationData": "default,10","Duration": "00:00:04","BridgeId": ""}}',
      '{ "type": 3, "server_id": 1, "server_name": "asterisk01.local", "ssl": false, "data": {"Event": "CoreShowChannel","Channel": "SIP/sipp-000000c2","ChannelState": "6","ChannelStateDesc": "Up","CallerIDNum": "13105256793","CallerIDName": "Alice","ConnectedLineNum": "<unknown>","ConnectedLineName": "<unknown>","Language": "en","AccountCode": "","Context": "from-sip","Exten": "50324502244","Priority": "7","Uniqueid": "1508082095.386","Linkedid": "1508082095.386","Application": "MusicOnHold","ApplicationData": "default,11","Duration": "00:00:07","BridgeId": ""}}',
      '{ "type": 3, "server_id": 1, "server_name": "asterisk01.local", "ssl": false, "data": {"Event": "CoreShowChannel","Channel": "SIP/sipp-000000c1","ChannelState": "6","ChannelStateDesc": "Up","CallerIDNum": "52159727017","CallerIDName": "Alice","ConnectedLineNum": "<unknown>","ConnectedLineName": "<unknown>","Language": "en","AccountCode": "","Context": "from-sip","Exten": "17169427531","Priority": "7","Uniqueid": "1508082087.383","Linkedid": "1508082087.383","Application": "MusicOnHold","ApplicationData": "default,17","Duration": "00:00:14","BridgeId": ""}}',
    ];
    messages.forEach(msg => store.dispatch('newMessage', msg));
    const Constructor = Vue.extend(ActiveCalls);
    const vm = new Constructor({ store }).$mount();
    expect(vm.$el.querySelectorAll('div.active-calls table tbody tr').length)
      .to.equal(3);
  });

  it('new calls on CoreShowChannel event', () => {
    const messages = [
      '{ "type": 3, "server_id": 1, "server_name": "asterisk01.local", "ssl": false, "data": {"Event": "CoreShowChannel","Channel": "SIP/sipp-000000c3","ChannelState": "6","ChannelStateDesc": "Up","CallerIDNum": "18439184039","CallerIDName": "Alice","ConnectedLineNum": "<unknown>","ConnectedLineName": "<unknown>","Language": "en","AccountCode": "","Context": "from-sip","Exten": "15417285104","Priority": "7","Uniqueid": "1508082097.387","Linkedid": "1508082097.387","Application": "MusicOnHold","ApplicationData": "default,10","Duration": "00:01:04","BridgeId": ""}}',
    ];
    messages.forEach(msg => store.dispatch('newMessage', msg));
    const Constructor = Vue.extend(ActiveCalls);
    const vm = new Constructor({ store }).$mount();
    expect(vm.$el.querySelectorAll('div.active-calls table tbody tr').length)
      .to.equal(1);
    expect(vm.$el.querySelector('div.active-calls table tbody tr td.duration').textContent.trim())
      .to.equal('00:01:04');
  });

  it('on CoreShowChannel update existing call duration', () => {
    const messages = [
      '{ "type": 3, "server_id": 1, "server_name": "asterisk01.local", "ssl": false, "data": {"Event": "Newchannel","Privilege": "call,all","Channel": "SIP/sipp-0000009a","ChannelState": "0","ChannelStateDesc": "Down","CallerIDNum": "15078897695","CallerIDName": "Alice","ConnectedLineNum": "<unknown>","ConnectedLineName": "<unknown>","Language": "en","AccountCode": "","Context": "from-sip","Exten": "16725558473","Priority": "1","Uniqueid": "1507761845.308","Linkedid": "1507761845.308"}}',
      '{ "type": 3, "server_id": 1, "server_name": "asterisk01.local", "ssl": false, "data": {"Event": "Newstate","Privilege": "call,all","Channel": "SIP/sipp-0000009a","ChannelState": "4","ChannelStateDesc": "Ring","CallerIDNum": "15078897695","CallerIDName": "Alice","ConnectedLineNum": "<unknown>","ConnectedLineName": "<unknown>","Language": "en","AccountCode": "","Context": "from-sip","Exten": "16725558473","Priority": "1","Uniqueid": "1507761845.308","Linkedid": "1507761845.308"}}',
      '{ "type": 3, "server_id": 1, "server_name": "asterisk01.local", "ssl": false, "data": {"Event": "Newstate","Privilege": "call,all","Channel": "SIP/sipp-0000009a","ChannelState": "6","ChannelStateDesc": "Up","CallerIDNum": "15078897695","CallerIDName": "Alice","ConnectedLineNum": "<unknown>","ConnectedLineName": "<unknown>","Language": "en","AccountCode": "","Context": "from-sip","Exten": "16725558473","Priority": "4","Uniqueid": "1507761845.308","Linkedid": "1507761845.308"}}',
      '{ "type": 3, "server_id": 1, "server_name": "asterisk01.local", "ssl": false, "data": {"Event": "CoreShowChannel","Channel": "SIP/sipp-0000009a","ChannelState": "6","ChannelStateDesc": "Up","CallerIDNum": "15078897695","CallerIDName": "Alice","ConnectedLineNum": "<unknown>","ConnectedLineName": "<unknown>","Language": "en","AccountCode": "","Context": "from-sip","Exten": "16725558473","Priority": "7","Uniqueid": "1507761845.308","Linkedid": "1507761845.308","Application": "MusicOnHold","ApplicationData": "default,10","Duration": "02:14:08","BridgeId": ""}}',
    ];
    messages.forEach(msg => store.dispatch('newMessage', msg));
    const Constructor = Vue.extend(ActiveCalls);
    const vm = new Constructor({ store }).$mount();
    expect(vm.$el.querySelectorAll('div.active-calls table tbody tr').length)
      .to.equal(1);
    expect(vm.$el.querySelector('div.active-calls table tbody tr td.duration').textContent.trim())
      .to.equal('02:14:08');
  });
});
