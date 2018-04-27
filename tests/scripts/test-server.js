'use strict';

var WebSocket = require('ws');
var httpHeaders = require('http-headers');
var util = require('util');
var wss = new WebSocket.Server({ port: 8080});

var prefix = 'Path:%s\r\nContent-Type: application/json; charset=utf-8\r\nX-RequestId:%s\r\n\r\n%j';


function rnd(max) {
  return Math.floor(Math.random() * Math.floor(max));
}

var speech = {

  start : util.format(prefix, 'speech.startDetected', '%s', { Offset: 0 }),

  hypothesis : util.format(prefix, 'speech.hypothesis', '%s', {
      Text: "this is a speech hypothesis",
      Offset: 0,
      Duration: 23600000
    }),

  hypothesis_unicode : util.format(prefix, 'speech.hypothesis', '%s', {
      Text: "Найдётся всё. Например, когда появился интернет",
      Offset: 0,
      Duration: 26300000
    }),

  phrase : util.format(prefix, 'speech.phrase', '%s', { 
      RecognitionStatus: "Success", 
      DisplayText: "Remind me to buy 5 iPhones.", 
      Offset: 0,
      Duration: 12300000
    }),

  end : util.format(prefix, 'speech.endDetected', '%s', { Offset: 0 }),
};


var turn = {

  start : util.format(prefix, 'turn.start', '%s', { 
    context: { 
      serviceTag: '7B33613B91714B32817815DC89633AFA' 
    } 
  }),

  end : 'Path:turn.end\r\nX-RequestId:%s\r\n\r\n',
};

var replies = [turn.start, speech.start, speech.hypothesis, speech.hypothesis_unicode, speech.end, speech.phrase, turn.end];

var requests = new Set();

wss.on('connection', function connection(ws, req) {
    console.log('Incoming connection for url '+ req.url + '\n HEADERS:\n' + JSON.stringify(req.headers));

    if (req.headers['ocp-apim-subscription-key'] == 'invalid_key') {
      ws.close();
      return;
    }

    ws.on('message', function incoming(message) {

      if (typeof message === 'string') {  
       console.log('received message: %s', message);
      } else {
        console.log('received binary message');
      }

      var headers = httpHeaders(message);
      var id = headers['x-requestid'].substring(0,32)

      if (!(requests.has(id))) {

        console.log('Incoming request with id: %s', id); 

        requests.add(id);
        var i = 0;
        (function reply(id) {
          if (i == replies.length) {
            // https://developer.mozilla.org/en-US/docs/Web/API/CloseEvent
            ws.close(4999, JSON.stringify({error:'Client must close the connection'}));
            return;
          }
          if (replies[i] == speech.hypothesis && rnd(2) == 0) i--;
          var response = util.format(replies[i++], id);
          var timeout = (i != replies.length) ? rnd(100) : 10000;
          console.log('replying with: %s\n', response);
          ws.send(response, {}, function() {
            setTimeout(function(){reply(id)}, timeout) ;
          });
        })(id);
      }

    });

    ws.on('close', function() {
      console.log('Connection closed!');
    });

    ws.on('error', function(e) {
      console.log('ERROR:' + e);
      ws.close(4000, 'Keep clear, we\'re going under...');
      process.exit(1);
    });

});
