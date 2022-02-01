function loop(a, b, inc = 1, debug = false) {
  while (a < b) {
    if (debug) {
      console.warn(this.name + "()", " LOOP COND ", a, "<", b, "===", a < b);
      console.warn("loop(" + a + ", " + b + ", " + inc + ")");
    }
    const res = this(a, b, inc);
    if (res === undefined) {
      a += inc;
    } else {
      a += res;
    }
  }
  if (debug) {
    console.warn("COND FALSE", a, "<", b, "===", a < b);
  }
  return this;
}
Object.prototype.loop = loop;

function do_work() {
  console.log(arguments[0]);
}
do_work.loop(0, 20, 3, true);
