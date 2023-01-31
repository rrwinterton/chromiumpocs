async function initialize() {
  const response = await fetch('main.wasm');
  const bytes = await response.arrayBuffer();
  const {instance} = await WebAssembly.instantiate(bytes, { });
  console.log(instance.exports.myAdd(1,2));
  console.log(instance.exports.pocHwy(0));
}

initialize();