const ciao    = require("@homebridge/ciao");
const bonjour = require('bonjour-hap')()

const responder = ciao.getResponder();

// create a service defining an HAP
const bridge = responder.createService({
    name: 'Homebridge A5BA 0494',
    host: 'myvps.ddns.net',
    type: 'hap',
    port: 51226,
    txt: vpsHomebridge()
});

bridge.advertise()

function vpsHomebridge() {
  const bridgeConfig = {
    configVersion: '4',        // c#
    statusFlags:   '0',        // sf
    setupHash:     '3E1uIQ==', // sh
    homebridgeID:  '0E:5F:63:56:A3:BA'
  }
  
  return getTXTRecord(
    bridgeConfig
  )
}

bridge.advertise()

function getTXTRecord(bridgeConfig) {
  return {
    'c#': bridgeConfig.configVersion,
    id:   bridgeConfig.homebridgeID,
    sf:   bridgeConfig.statusFlags,
    sh:   bridgeConfig.setupHash,
    md: 'homebridge',
    ci: '2',
    pv: '1.1',
    ff: '0',
    's#': '1',
  }
}

process.on('SIGTERM', signal => {
  console.log(`Process ${process.pid} received a SIGTERM signal`)
  process.exit(0)
})

process.on('SIGINT', signal => {
  console.log(`\nCiao mDNS Anauncer has been interrupted`)
  bridge.end().then(() => {
    console.log("Homebridge Service was unanaunced");
    process.exit(0);
  });
})


// browse for all available HAP services
bonjour.find({ type: 'hap' }, function (service) {
  console.log('Found an HAP service:', service)
})
