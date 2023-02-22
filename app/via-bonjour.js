const bonjour = require('bonjour-hap')()

// Advertise an HAP bridge instance
bonjour.publish(vpsHomebridge())

// Advertise a homebridge API server
// bonjour.publish(localHomebridgeDNSRecord())

function vpsHomebridge() {
  const bridgeConfig = {
    configVersion: '4',        // c#
    statusFlags:   '0',        // sf
    setupHash:     '321uIQ==', // sh
    homebridgeID:  '0E:5F:63:56:A3:BA'
  }

  const uniquePart = bridgeConfig.homebridgeID.split(separator=':').slice(-2).join('')
  const serverConfig = {
    name: `Homebridge ${uniquePart} 0494`,
    host: 'myvps.ddns.net',
    port: 51226
  }
  
  return getTXTRecord(serverConfig, bridgeConfig)
}

function getTXTRecord(serverConfig, bridgeConfig) {
  return {
    name: serverConfig.name,
    host: serverConfig.host,
    type: 'hap',
    port: serverConfig.port,
    txt: {
      pv:   '1.1',
      ff:   '0',
      's#': '1',
      md:   'homebridge',
      'c#': bridgeConfig.configVersion,
      id:   bridgeConfig.homebridgeID,
      sf:   bridgeConfig.statusFlags,
      ci:   '2',
      sh:   bridgeConfig.setupHash
    }
  }
}

// Browse for all available HAP services
bonjour.find({ type: 'hap' }, function (service) {
  console.log('Found an HAP service:', service)
})
 
// Unannaunce all services when CTRL+C is pressed
process.on('SIGINT', signal => {
  console.log('\nBonjour mDNS Annauncer has been interrupted')
  bonjour.unpublishAll(() => {
    console.log("All services were unannaunced");
    process.exit(0);
  });
})
