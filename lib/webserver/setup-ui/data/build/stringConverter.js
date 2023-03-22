var stringConverter = {
  convertByte: function (oneByte, bytesPerPixel) {
    var stringByte = '0x' + oneByte.toString(16).padStart(bytesPerPixel * 2, '0');
    return stringByte;
  },
  convert: function (dataLength, bytesPerPixel, multiLine, colNumber, data) {
    var resultString = '';
    for (var i = 0; i < dataLength; i++) {
      var stringByte = '';
      // need to use bigint, so we can use 32bit integers (4byte per pixel)
      let combinedByte = BigInt("0b00000000000000000000000000000000");
      for (let j = 0; j < bytesPerPixel; j++) {
        let pixelByte = BigInt(data[(i * bytesPerPixel) + j]);
        if (j != 0) {
          combinedByte = combinedByte << BigInt(8);
        }
        combinedByte = combinedByte | pixelByte;
      }
      stringByte = this.convertByte(combinedByte, bytesPerPixel) + ', ';
      if (multiLine && ((i + 1) % colNumber == 0)) {
        stringByte += '\n  ';
      }
      resultString += stringByte;
    }
    resultString = resultString.substr(0, resultString.lastIndexOf(',')).trim();
    // add the array definition
    return resultString;
  }
};


module.exports = {

  toString : function(data, colNum) {
    console.log('Converting data to string');

    var dataLength = data.byteLength;
    console.log('actualDataLength: ' + dataLength);

    var resultString = '#define SETUP_HTML_SIZE ';
    resultString += dataLength + '\n'
    resultString += 'const char SETUP_HTML[] PROGMEM = {\n  ';
    resultString += stringConverter.convert(dataLength, 1, true, 16, data);
    resultString += '\n};';
    return resultString;
  }

}
