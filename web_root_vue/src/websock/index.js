/**
 * web-socket connector
 */
import log from '../logger';

export default {

  connect: (onMessage) => {
    log.dbg(`Connecting to ${process.env.WS_URL}`);
    const sock = new WebSocket(process.env.WS_URL);

    sock.onopen = () => {
      log.dbg('On open send status request.');
      sock.send(JSON.stringify({ Action: 'CoreStatus' }));
      sock.send(JSON.stringify({ Action: 'CoreShowChannels' }));
    };

    sock.onmessage = (ev) => {
      log.dbg(`On message: ${ev.data}`);
      onMessage(ev.data);
    };

    sock.onclose = (ev) => {
      log.dbg(`Connection is closed. [${ev}]`);
    };

    sock.onerror = (err) => {
      log.dbg(`Connection is closed: ${err.message}`);
      sock.close();
    };
  },

};
