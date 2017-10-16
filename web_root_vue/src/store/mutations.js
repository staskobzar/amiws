import * as phonenum from 'libphonenumber-js';
import log from '../logger';
import * as mtype from './mutation-types';

function msgToAmiServer(message) {
  if (!message.server_id || !message.data.CoreCurrentCalls) {
    return null;
  }
  const reloaded = new Date(`${message.data.CoreReloadDate} ${message.data.CoreReloadTime}`);
  const started = new Date(`${message.data.CoreStartupDate} ${message.data.CoreStartupTime}`);
  return {
    id: message.server_id,
    name: message.server_name,
    ssl: message.ssl,
    calls: 0, // +(message.data.CoreCurrentCalls),
    reload: reloaded,
    startup: started,
    lp: 0,
    rlp: 0,
    rtt: 0,
    rxjitter: 0,
    txjitter: 0,
  };
}

function phoneNumber(value) {
  const num = phonenum.parse(`+${value}`);
  const iso = num.country ? num.country : process.env.DEFAULT_COUNTRY;
  const phoneNum = num.phone ?
        phonenum.format(num.phone, num.country, 'International') :
        phonenum.format(value, process.env.DEFAULT_COUNTRY, 'International');
  return { number: phoneNum, countryISO: iso.toLowerCase() };
}

function parseDuration(val) {
  if (!val) return 0;
  const t = val.split(':').map(n => +n);
  return (t[0] * 60 * 60) + (t[1] * 60) + t[2];
}

function msgToActiveCall(message) {
  const data = message.data;
  const call = {
    cid: data.Channel,        // channel id
    uid: data.Uniqueid,       // unique id
    sid: message.server_id,   // server id
    state: data.ChannelState,
    stateStr: data.ChannelStateDesc,
    startTime: new Date(),
    source: phoneNumber(data.CallerIDNum),
    dest: phoneNumber(data.Exten),
    duration: parseDuration(data.Duration),
    server: message.server_name,
  };

  return call;
}

export default {
  [mtype.ADD_AMI_SERVER](state, message) {
    const data = msgToAmiServer(message);
    log.dbg(`::ADD_AMI_SERVER:: ${data.id}`);
    if (data) {
      state.servers.push(data);
    }
  },

  [mtype.ADD_QOS_STATS](state, payload) {
    const qos = payload.qos;
    const srv = state.servers.find(s => s.id === payload.sid);
    if (!srv) return;
    const val = (a, b) => (a > 0 ? (a + b) / 2 : b);
    srv.lp += qos.lp;
    srv.rlp += qos.rlp;
    srv.rtt = val(srv.rtt, qos.rtt);
    srv.rxjitter = val(srv.rxjitter, qos.rxjitter);
    srv.txjitter = val(srv.txjitter, qos.txjitter);
  },

  [mtype.CLEAR_AMISRV_LIST](state) {
    state.servers.splice(0);
  },

  [mtype.CLEAR_CALLS_LIST](state) {
    state.calls.splice(0);
  },

  [mtype.ADD_ACTIVE_CALL](state, message) {
    const data = msgToActiveCall(message);
    const srv = state.servers.find(s => s.id === message.server_id);
    const call = state.calls.find(c => (c.cid === data.cid &&
                                        c.uid === data.uid &&
                                        c.sid === data.sid));
    if (data) {
      if (call) {
        call.duration = data.duration;
        call.state = data.state;
        call.stateStr = data.stateStr;
      } else {
        state.calls.push(data);
        if (srv) srv.calls += 1;
        state.newCallCountry = data.dest.countryISO;
      }
    }
  },

  [mtype.HUP_ACTIVE_CALL](state, message) {
    const data = message.data;
    const call = state.calls.find(c => (c.cid === data.Channel &&
                                        c.uid === data.Uniqueid &&
                                        c.sid === message.server_id));
    const srv = state.servers.find(s => s.id === message.server_id);
    if (call) {
      call.stateStr = 'Down';
      state.calls.splice(state.calls.indexOf(call), 1);
      if (srv) srv.calls -= 1;
    }
  },

  [mtype.UPDATE_CALLS_DUR](state) {
    state.calls.forEach(call => call.duration++);
  },

  [mtype.NEW_CALL_COUNTRY](state, countryCode) {
    state.newCallCountry = countryCode;
  },
};

