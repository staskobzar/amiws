export default {
  getAmiServers(state) {
    return state.servers;
  },
  getActiveCalls(state) {
    return state.calls;
  },
  getCallsPerServer(state) {
    return state.servers.map(s => s.calls);
  },
  getCallCountry(state) {
    return state.newCallCountry;
  },
};

