import log from '../logger';
import * as ami from './ami-types';
import * as mtype from './mutation-types';

function setAudioQoS(data) {
  const qos = {};
  data.Value.split(';').forEach((mem) => {
    const [key, val] = mem.split('=');
    if (['lp', 'rlp', 'rtt', 'txjitter', 'rxjitter'].includes(key)) {
      qos[key] = +val; // these are numeric values
    }
  });
  return qos;
}

function eventRecv(message, commit) {
  const data = message.data;
  switch (message.data.Event) {
    case 'CoreShowChannel':
    case 'Newchannel':
    case 'Newstate':
      commit(mtype.ADD_ACTIVE_CALL, message);
      break;
    case 'Hangup':
      commit(mtype.HUP_ACTIVE_CALL, message);
      break;
    case 'VarSet':
      if (message.data.Variable === 'RTPAUDIOQOS') {
        commit(mtype.ADD_QOS_STATS, { sid: message.server_id, qos: setAudioQoS(message.data) });
      }
      break;
    default:
      log.dbg(`Skip event ${data.Event}`);
      break;
  }
}

export default {
  newMessage({ commit }, msgRaw) {
    const message = JSON.parse(msgRaw);
    switch (message.type) {
      case ami.PROMPT:
        log.dbg('AMI PROMPT');
        break;
      case ami.ACTION:
        log.dbg('AMI ACTION');
        break;
      case ami.EVENT:
        eventRecv(message, commit);
        break;
      case ami.RESPONSE:
        if (message.data.CoreStartupDate) {
          commit(mtype.ADD_AMI_SERVER, message);
        }
        break;
      default:
        break;
    }
  },
};

