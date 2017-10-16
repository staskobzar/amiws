export default {
  dbg(msg) {
    if (process.env.NODE_ENV === 'development') {
      window.console.log(msg);
    }
  },
};
