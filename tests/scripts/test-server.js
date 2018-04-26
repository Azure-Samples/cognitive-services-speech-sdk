'use strict';

WebSocket = require('ws');
httpHeaders = require('http-headers');
util = require('util');
wss = new WebSocket.Server({ port: 8080});

prefix = 'Path:%s\r\nContent-Type: application/json; charset=utf-8\r\nX-RequestId:%s\r\n\r\n%j';


function rnd(max) {
  return Math.floor(Math.random() * Math.floor(max));
}

speech = {

  start : util.format(prefix, 'speech.startDetected', '%s', { Offset: 0 }),

  hypothesis : util.format(prefix, 'speech.hypothesis', '%s', {
      Text: "this is a speech hypothesis",
      Offset: 0,
      Duration: 23600000
    }),

  phrase : util.format(prefix, 'speech.phrase', '%s', { 
      RecognitionStatus: "Success", 
      DisplayText: "Remind me to buy 5 iPhones.", 
      Offset: 0,
      Duration: 12300000
    }),

  end : util.format(prefix, 'speech.endDetected', '%s', { Offset: 0 }),
};


turn = {

  start : util.format(prefix, 'turn.start', '%s', { 
    context: { 
      serviceTag: '7B33613B91714B32817815DC89633AFA' 
    } 
  }),

  end : 'Path:turn.end\r\nX-RequestId:%s\r\n\r\n',
};

replies = [turn.start, speech.start, speech.hypothesis, speech.end, speech.phrase, turn.end];

wss.on('connection', function connection(ws, req) {
    console.log('Incoming connection for url '+ req.url + '\n HEADERS:\n' + JSON.stringify(req.headers));

    requests = new Set();

    ws.on('message', function incoming(message) {

      if (typeof message === 'string') {  
       console.log('received: %s', message);
      } else {
        console.log('received binary message');
      }

      headers = httpHeaders(message);
      id = headers['x-requestid']
      if (!(requests.has(id))) {

        console.log('Incoming request with id: %s', id); 

        requests.add(id);
        var i = 0;
        (function reply() {
          if (i == replies.length) {
            // https://developer.mozilla.org/en-US/docs/Web/API/CloseEvent
            ws.close(4999, JSON.stringify({error:'Client must close the connection'}));
            return;
          }
          if (replies[i] == speech.hypothesis && rnd(2) == 0) i--;
          response = util.format(replies[i++], id);
          timeout = (i != replies.length) ? rnd(100) : 1000;
          console.log('replying with: %s', response);
          ws.send(response, {}, function() {
            setTimeout(reply, timeout) ;
          });
        })();
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