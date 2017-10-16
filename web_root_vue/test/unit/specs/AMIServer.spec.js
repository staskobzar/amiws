import 'babel-polyfill';
import Vue from 'vue';
import ServersList from '@/components/AMIServersList';
import * as mtype from '@/store/mutation-types';
import store from '@/store';

describe('AMI Server component', () => {
  let vm;
  beforeEach(() => {
    const messages = [
      '{ "type": 4, "server_id": 1, "server_name": "pbx01.ast.local", ' +
      '"ssl": true, "data": {"Response": "Success","CoreStartupDate": ' +
      '"2017-09-03","CoreStartupTime": "15:07:23","CoreReloadDate": ' +
      '"2017-10-18","CoreReloadTime": "09:43:16","CoreCurrentCalls": "125"}}',
    ];
    messages.forEach(msg => store.dispatch('newMessage', msg));
    const Constructor = Vue.extend(ServersList);
    vm = new Constructor({ store }).$mount();
  });

  afterEach(() => {
    store.commit(mtype.CLEAR_AMISRV_LIST);
    store.commit(mtype.CLEAR_CALLS_LIST);
  });

  it('set server current calls parameter to 125', () => {
    expect(vm.$el.querySelector('div.amiserver div.calls-number').textContent)
      .to.equal('0'); // reset to 0 - will be added by chan events
  });

  it('set server name to pbx01.ast.local', () => {
    expect(vm.$el.querySelector('div.amiserver span.server-name').textContent)
      .to.equal('pbx01.ast.local');
  });

  it('server is SSL protected', () => {
    expect(vm.$el.querySelectorAll('div.panel-heading svg.fa-icon').length)
      .to.equal(2);
    expect(vm.$el.querySelectorAll('svg.fa-icon.is-ssl').length)
      .to.equal(1);
  });

  it('lost packets is set to zero', () => {
    expect(vm.$el.querySelector('div.amiserver div.lp .value').textContent)
      .to.equal('0');
    expect(vm.$el.querySelector('div.amiserver div.rlp .value').textContent)
      .to.equal('0');
  });

  it('rrt and jitter is set to zero with fraction', () => {
    expect(vm.$el.querySelector('div.amiserver div.rtt .value').textContent)
      .to.equal('0.000000');
    expect(vm.$el.querySelector('div.amiserver div.rxjitter .value').textContent)
      .to.equal('0.000000');
    expect(vm.$el.querySelector('div.amiserver div.txjitter .value').textContent)
      .to.equal('0.000000');
  });

  it('update audio QoS with RTPAUDIOQOS packet', (done) => {
    const msg = '{ "type": 3, "server_id": 1, "server_name": "asterisk01.local", ' +
    '"ssl": false, "data": {"Event": "VarSet","Privilege": "dialplan,all","Channel": ' +
    '"SIP/sipp-00000098","ChannelState": "6","ChannelStateDesc": "Up","CallerIDNum": ' +
    '"44780558771","CallerIDName": "Alice","ConnectedLineNum": "<unknown>","ConnectedLineName": ' +
    '"<unknown>","Language": "en","AccountCode": "","Context": "from-sip","Exten": ' +
    '"50326674178","Priority": "8","Uniqueid": "1507685034.281","Linkedid": "1507685034.281",' +
    '"Variable": "RTPAUDIOQOS","Value": "ssrc=1982787858;themssrc=0;lp=12;rxjitter=0.000034;' +
    'rxcount=0;txjitter=2.003400;txcount=950;rlp=1;rtt=0.000670"}}';

    store.dispatch('newMessage', msg);

    Vue.nextTick(() => {
      expect(vm.$el.querySelector('div.amiserver div.lp .value').textContent)
        .to.equal('12');
      expect(vm.$el.querySelector('div.amiserver div.rlp .value').textContent)
        .to.equal('1');
      expect(vm.$el.querySelector('div.amiserver div.rtt .value').textContent)
        .to.equal('0.000670');
      expect(vm.$el.querySelector('div.amiserver div.rxjitter .value').textContent)
        .to.equal('0.000034');
      expect(vm.$el.querySelector('div.amiserver div.txjitter .value').textContent)
        .to.equal('2.003400');

      done();
    });
  });

  it('QoS packet stats calculations for several packets', (done) => {
    const messages = ['{ "type": 3, "server_id": 1, "server_name": "asterisk01.local", ' +
    '"ssl": false, "data": {"Event": "VarSet","Privilege": "dialplan,all","Channel": ' +
    '"SIP/sipp-00000098","ChannelState": "6","ChannelStateDesc": "Up","CallerIDNum": ' +
    '"44780558771","CallerIDName": "Alice","ConnectedLineNum": "<unknown>","ConnectedLineName": ' +
    '"<unknown>","Language": "en","AccountCode": "","Context": "from-sip","Exten": ' +
    '"50326674178","Priority": "8","Uniqueid": "1507685034.281","Linkedid": "1507685034.281",' +
    '"Variable": "RTPAUDIOQOS","Value": "ssrc=1982787858;themssrc=0;lp=12;rxjitter=0.000034;' +
    'rxcount=0;txjitter=2.003400;txcount=950;rlp=1;rtt=0.000670"}}',

      '{ "type": 3, "server_id": 1, "server_name": "asterisk01.local", ' +
    '"ssl": false, "data": {"Event": "VarSet","Privilege": "dialplan,all","Channel": ' +
    '"SIP/sipp-00000098","ChannelState": "6","ChannelStateDesc": "Up","CallerIDNum": ' +
    '"44780558771","CallerIDName": "Alice","ConnectedLineNum": "<unknown>","ConnectedLineName": ' +
    '"<unknown>","Language": "en","AccountCode": "","Context": "from-sip","Exten": ' +
    '"50326674178","Priority": "8","Uniqueid": "1507685034.281","Linkedid": "1507685034.281",' +
    '"Variable": "RTPAUDIOQOS","Value": "ssrc=1982787858;themssrc=0;lp=2;rxjitter=0.00300;' +
    'rxcount=0;txjitter=0.000087;txcount=950;rlp=71;rtt=0.067000"}}'];

    messages.forEach(msg => store.dispatch('newMessage', msg));

    Vue.nextTick(() => {
      expect(vm.$el.querySelector('div.amiserver div.lp .value').textContent)
        .to.equal('14');
      expect(vm.$el.querySelector('div.amiserver div.rlp .value').textContent)
        .to.equal('72');
      expect(vm.$el.querySelector('div.amiserver div.rtt .value').textContent)
        .to.equal('0.033835');
      expect(vm.$el.querySelector('div.amiserver div.rxjitter .value').textContent)
        .to.equal('0.001517');
      expect(vm.$el.querySelector('div.amiserver div.txjitter .value').textContent)
        .to.equal('1.001744');
      done();
    });
  });

  it('QoS packet update with matching server id', (done) => {
    const messages = ['{ "type": 3, "server_id": 1, "server_name": "asterisk01.local", ' +
    '"ssl": false, "data": {"Event": "VarSet","Privilege": "dialplan,all","Channel": ' +
    '"SIP/sipp-00000098","ChannelState": "6","ChannelStateDesc": "Up","CallerIDNum": ' +
    '"44780558771","CallerIDName": "Alice","ConnectedLineNum": "<unknown>","ConnectedLineName": ' +
    '"<unknown>","Language": "en","AccountCode": "","Context": "from-sip","Exten": ' +
    '"50326674178","Priority": "8","Uniqueid": "1507685034.281","Linkedid": "1507685034.281",' +
    '"Variable": "RTPAUDIOQOS","Value": "ssrc=1982787858;themssrc=0;lp=12;rxjitter=0.000034;' +
    'rxcount=0;txjitter=2.003400;txcount=950;rlp=1;rtt=0.000670"}}',

      '{ "type": 3, "server_id": 2, "server_name": "asterisk01.local", ' +
    '"ssl": false, "data": {"Event": "VarSet","Privilege": "dialplan,all","Channel": ' +
    '"SIP/sipp-00000098","ChannelState": "6","ChannelStateDesc": "Up","CallerIDNum": ' +
    '"44780558771","CallerIDName": "Alice","ConnectedLineNum": "<unknown>","ConnectedLineName": ' +
    '"<unknown>","Language": "en","AccountCode": "","Context": "from-sip","Exten": ' +
    '"50326674178","Priority": "8","Uniqueid": "1507685034.281","Linkedid": "1507685034.281",' +
    '"Variable": "RTPAUDIOQOS","Value": "ssrc=1982787858;themssrc=0;lp=2;rxjitter=0.00300;' +
    'rxcount=0;txjitter=0.000087;txcount=950;rlp=71;rtt=0.067000"}}'];

    messages.forEach(msg => store.dispatch('newMessage', msg));

    Vue.nextTick(() => {
      expect(vm.$el.querySelector('div.amiserver div.lp .value').textContent)
        .to.equal('12');
      expect(vm.$el.querySelector('div.amiserver div.rlp .value').textContent)
        .to.equal('1');
      expect(vm.$el.querySelector('div.amiserver div.rtt .value').textContent)
        .to.equal('0.000670');
      expect(vm.$el.querySelector('div.amiserver div.rxjitter .value').textContent)
        .to.equal('0.000034');
      expect(vm.$el.querySelector('div.amiserver div.txjitter .value').textContent)
        .to.equal('2.003400');

      done();
    });
  });

  it('update calls when new calls added', (done) => {
    const messages = [
      '{ "type": 3, "server_id": 1, "server_name": "asterisk01.local", "ssl": false, "data": {"Event": "Newchannel","Privilege": "call,all","Channel": "SIP/sipp-0000009a","ChannelState": "0","ChannelStateDesc": "Down","CallerIDNum": "15078897695","CallerIDName": "Alice","ConnectedLineNum": "<unknown>","ConnectedLineName": "<unknown>","Language": "en","AccountCode": "","Context": "from-sip","Exten": "16725558473","Priority": "1","Uniqueid": "1507761845.308","Linkedid": "1507761845.308"}}',
      '{ "type": 3, "server_id": 1, "server_name": "asterisk01.local", "ssl": false, "data": {"Event": "Newstate","Privilege": "call,all","Channel": "SIP/sipp-0000009a","ChannelState": "4","ChannelStateDesc": "Ring","CallerIDNum": "15078897695","CallerIDName": "Alice","ConnectedLineNum": "<unknown>","ConnectedLineName": "<unknown>","Language": "en","AccountCode": "","Context": "from-sip","Exten": "16725558473","Priority": "1","Uniqueid": "1507761845.308","Linkedid": "1507761845.308"}}',
      '{ "type": 3, "server_id": 1, "server_name": "asterisk01.local", "ssl": false, "data": {"Event": "Newstate","Privilege": "call,all","Channel": "SIP/sipp-0000009a","ChannelState": "6","ChannelStateDesc": "Up","CallerIDNum": "15078897695","CallerIDName": "Alice","ConnectedLineNum": "<unknown>","ConnectedLineName": "<unknown>","Language": "en","AccountCode": "","Context": "from-sip","Exten": "16725558473","Priority": "4","Uniqueid": "1507761845.308","Linkedid": "1507761845.308"}}',
      '{ "type": 3, "server_id": 1, "server_name": "asterisk01.local", "ssl": false, "data": {"Event": "Newchannel","Privilege": "call,all","Channel": "SIP/sipp-00000093","ChannelState": "0","ChannelStateDesc": "Down","CallerIDNum": "15078897695","CallerIDName": "Alice","ConnectedLineNum": "<unknown>","ConnectedLineName": "<unknown>","Language": "en","AccountCode": "","Context": "from-sip","Exten": "16725558473","Priority": "1","Uniqueid": "1507765205.294","Linkedid": "1507765205.294"}}',
      '{ "type": 3, "server_id": 1, "server_name": "asterisk01.local", "ssl": false, "data": {"Event": "Newstate","Privilege": "call,all","Channel": "SIP/sipp-00000093","ChannelState": "4","ChannelStateDesc": "Ring","CallerIDNum": "15078897695","CallerIDName": "Alice","ConnectedLineNum": "<unknown>","ConnectedLineName": "<unknown>","Language": "en","AccountCode": "","Context": "from-sip","Exten": "16725558473","Priority": "1","Uniqueid": "1507765205.294","Linkedid": "1507765205.294"}}',
      '{ "type": 3, "server_id": 1, "server_name": "asterisk01.local", "ssl": false, "data": {"Event": "Newstate","Privilege": "call,all","Channel": "SIP/sipp-00000093","ChannelState": "6","ChannelStateDesc": "Up","CallerIDNum": "15078897695","CallerIDName": "Alice","ConnectedLineNum": "<unknown>","ConnectedLineName": "<unknown>","Language": "en","AccountCode": "","Context": "from-sip","Exten": "16725558473","Priority": "4","Uniqueid": "1507765205.294","Linkedid": "1507765205.294"}}',
    ];
    messages.forEach(msg => store.dispatch('newMessage', msg));
    Vue.nextTick(() => {
      expect(vm.$el.querySelector('div.amiserver div.calls-number').textContent)
        .to.equal('2');

      done();
    });
  });

  it('update calls when new calls added for pertinent server', (done) => {
    const messages = [
      '{ "type": 3, "server_id": 1, "server_name": "asterisk01.local", "ssl": false, "data": {"Event": "Newchannel","Privilege": "call,all","Channel": "SIP/sipp-0000009a","ChannelState": "0","ChannelStateDesc": "Down","CallerIDNum": "15078897695","CallerIDName": "Alice","ConnectedLineNum": "<unknown>","ConnectedLineName": "<unknown>","Language": "en","AccountCode": "","Context": "from-sip","Exten": "16725558473","Priority": "1","Uniqueid": "1507761845.308","Linkedid": "1507761845.308"}}',
      '{ "type": 3, "server_id": 1, "server_name": "asterisk01.local", "ssl": false, "data": {"Event": "Newstate","Privilege": "call,all","Channel": "SIP/sipp-0000009a","ChannelState": "4","ChannelStateDesc": "Ring","CallerIDNum": "15078897695","CallerIDName": "Alice","ConnectedLineNum": "<unknown>","ConnectedLineName": "<unknown>","Language": "en","AccountCode": "","Context": "from-sip","Exten": "16725558473","Priority": "1","Uniqueid": "1507761845.308","Linkedid": "1507761845.308"}}',
      '{ "type": 3, "server_id": 1, "server_name": "asterisk01.local", "ssl": false, "data": {"Event": "Newstate","Privilege": "call,all","Channel": "SIP/sipp-0000009a","ChannelState": "6","ChannelStateDesc": "Up","CallerIDNum": "15078897695","CallerIDName": "Alice","ConnectedLineNum": "<unknown>","ConnectedLineName": "<unknown>","Language": "en","AccountCode": "","Context": "from-sip","Exten": "16725558473","Priority": "4","Uniqueid": "1507761845.308","Linkedid": "1507761845.308"}}',
      '{ "type": 3, "server_id": 2, "server_name": "pbx07.local", "ssl": false, "data": {"Event": "Newchannel","Privilege": "call,all","Channel": "SIP/sipp-00000093","ChannelState": "0","ChannelStateDesc": "Down","CallerIDNum": "15078897695","CallerIDName": "Alice","ConnectedLineNum": "<unknown>","ConnectedLineName": "<unknown>","Language": "en","AccountCode": "","Context": "from-sip","Exten": "16725558473","Priority": "1","Uniqueid": "1507765205.294","Linkedid": "1507765205.294"}}',
      '{ "type": 3, "server_id": 2, "server_name": "pbx07.local", "ssl": false, "data": {"Event": "Newstate","Privilege": "call,all","Channel": "SIP/sipp-00000093","ChannelState": "4","ChannelStateDesc": "Ring","CallerIDNum": "15078897695","CallerIDName": "Alice","ConnectedLineNum": "<unknown>","ConnectedLineName": "<unknown>","Language": "en","AccountCode": "","Context": "from-sip","Exten": "16725558473","Priority": "1","Uniqueid": "1507765205.294","Linkedid": "1507765205.294"}}',
      '{ "type": 3, "server_id": 2, "server_name": "pbx07.local", "ssl": false, "data": {"Event": "Newstate","Privilege": "call,all","Channel": "SIP/sipp-00000093","ChannelState": "6","ChannelStateDesc": "Up","CallerIDNum": "15078897695","CallerIDName": "Alice","ConnectedLineNum": "<unknown>","ConnectedLineName": "<unknown>","Language": "en","AccountCode": "","Context": "from-sip","Exten": "16725558473","Priority": "4","Uniqueid": "1507765205.294","Linkedid": "1507765205.294"}}',
    ];
    messages.forEach(msg => store.dispatch('newMessage', msg));
    Vue.nextTick(() => {
      expect(vm.$el.querySelector('div.amiserver div.calls-number').textContent)
        .to.equal('1');

      done();
    });
  });

  it('update calls when call hangup', (done) => {
    const messages = [
      '{ "type": 3, "server_id": 1, "server_name": "asterisk01.local", "ssl": false, "data": {"Event": "Newchannel","Privilege": "call,all","Channel": "SIP/sipp-0000009a","ChannelState": "0","ChannelStateDesc": "Down","CallerIDNum": "15078897695","CallerIDName": "Alice","ConnectedLineNum": "<unknown>","ConnectedLineName": "<unknown>","Language": "en","AccountCode": "","Context": "from-sip","Exten": "16725558473","Priority": "1","Uniqueid": "1507761845.308","Linkedid": "1507761845.308"}}',
      '{ "type": 3, "server_id": 1, "server_name": "asterisk01.local", "ssl": false, "data": {"Event": "Newstate","Privilege": "call,all","Channel": "SIP/sipp-0000009a","ChannelState": "4","ChannelStateDesc": "Ring","CallerIDNum": "15078897695","CallerIDName": "Alice","ConnectedLineNum": "<unknown>","ConnectedLineName": "<unknown>","Language": "en","AccountCode": "","Context": "from-sip","Exten": "16725558473","Priority": "1","Uniqueid": "1507761845.308","Linkedid": "1507761845.308"}}',
      '{ "type": 3, "server_id": 1, "server_name": "asterisk01.local", "ssl": false, "data": {"Event": "Newstate","Privilege": "call,all","Channel": "SIP/sipp-0000009a","ChannelState": "6","ChannelStateDesc": "Up","CallerIDNum": "15078897695","CallerIDName": "Alice","ConnectedLineNum": "<unknown>","ConnectedLineName": "<unknown>","Language": "en","AccountCode": "","Context": "from-sip","Exten": "16725558473","Priority": "4","Uniqueid": "1507761845.308","Linkedid": "1507761845.308"}}',
      '{ "type": 3, "server_id": 1, "server_name": "asterisk01.local", "ssl": false, "data": {"Event": "Newchannel","Privilege": "call,all","Channel": "SIP/sipp-00000093","ChannelState": "0","ChannelStateDesc": "Down","CallerIDNum": "15078897695","CallerIDName": "Alice","ConnectedLineNum": "<unknown>","ConnectedLineName": "<unknown>","Language": "en","AccountCode": "","Context": "from-sip","Exten": "16725558473","Priority": "1","Uniqueid": "1507765205.294","Linkedid": "1507765205.294"}}',
      '{ "type": 3, "server_id": 1, "server_name": "asterisk01.local", "ssl": false, "data": {"Event": "Newstate","Privilege": "call,all","Channel": "SIP/sipp-00000093","ChannelState": "4","ChannelStateDesc": "Ring","CallerIDNum": "15078897695","CallerIDName": "Alice","ConnectedLineNum": "<unknown>","ConnectedLineName": "<unknown>","Language": "en","AccountCode": "","Context": "from-sip","Exten": "16725558473","Priority": "1","Uniqueid": "1507765205.294","Linkedid": "1507765205.294"}}',
      '{ "type": 3, "server_id": 1, "server_name": "asterisk01.local", "ssl": false, "data": {"Event": "Newstate","Privilege": "call,all","Channel": "SIP/sipp-00000093","ChannelState": "6","ChannelStateDesc": "Up","CallerIDNum": "15078897695","CallerIDName": "Alice","ConnectedLineNum": "<unknown>","ConnectedLineName": "<unknown>","Language": "en","AccountCode": "","Context": "from-sip","Exten": "16725558473","Priority": "4","Uniqueid": "1507765205.294","Linkedid": "1507765205.294"}}',
      '{ "type": 3, "server_id": 1, "server_name": "asterisk01.local", "ssl": false, "data": {"Event": "Hangup","Privilege": "call,all","Channel": "SIP/sipp-0000009a","ChannelState": "6","ChannelStateDesc": "Up","CallerIDNum": "15078897695","CallerIDName": "Alice","ConnectedLineNum": "<unknown>","ConnectedLineName": "<unknown>","Language": "en","AccountCode": "","Context": "from-sip","Exten": "16725558473","Priority": "8","Uniqueid": "1507761845.308","Linkedid": "1507761845.308","Cause": "0","Cause-txt": "Unknown"}}',
    ];
    messages.forEach(msg => store.dispatch('newMessage', msg));
    Vue.nextTick(() => {
      expect(vm.$el.querySelector('div.amiserver div.calls-number').textContent)
        .to.equal('1'); // two calls added one call has been terminated
      done();
    });
  });
});
