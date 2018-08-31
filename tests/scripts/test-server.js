(function () {
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

    start : function (id, offset) {
      return util.format(prefix, 'speech.startDetected', id , { Offset: offset }) ;
    },

    hypothesis : function (id, offset) {
      return util.format(prefix, 'speech.hypothesis', id, {
        Text: "this is a speech hypothesis",
        Offset: offset,
        Duration: offset+23600000
      });
    },

    hypothesis_unicode : function (id, offset) {
      return util.format(prefix, 'speech.hypothesis', id, {
        Text: "Найдётся всё. Например, когда появился интернет",
        Offset: offset,
        Duration: 26300000
      });
    },

    phrase : function (id, offset) {
      return util.format(prefix, 'speech.phrase', id, {
        RecognitionStatus: "Success",
        DisplayText: "Remind me to buy 5 iPhones.",
        Offset: offset,
        Duration: offset+12300000
      });
    },

    end : function (id, offset) {
      return util.format(prefix, 'speech.endDetected', id, { Offset: offset });
    },
  };


  var turn = {

    start : function (id, offset) {
      return util.format(prefix, 'turn.start', id, {
        context: {
          serviceTag: '7B33613B91714B32817815DC89633AFA'
        }
      });
    },

    end : function (id, offset) {
      return util.format('Path:turn.end\r\nX-RequestId:%s\r\n\r\n', id);
    },
  };

  var replies = [turn.start, speech.start, speech.hypothesis, speech.hypothesis_unicode, speech.end, speech.phrase, turn.end];

  var requests = new Set();

  var offset = 0;

  wss.on('connection', function connection(ws, req) {
      console.log('Incoming connection for url '+ req.url + '\n HEADERS:\n' + JSON.stringify(req.headers));

      var sub_key = req.headers['ocp-apim-subscription-key'];
      if (sub_key == 'invalid_key') {
        ws.close();
        return;
      }

      var max_timeout = 100;

      try {
          var piggyback = JSON.parse(sub_key);
          max_timeout = piggyback['max_timeout']
      } catch (e) {
        // oh, well, sub key is not a json, just go with it.
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
          offset+=100;
          (function reply(i, id, connection, offset) {
            if (i == replies.length) {
              // https://developer.mozilla.org/en-US/docs/Web/API/CloseEvent
              connection.close(4999, JSON.stringify({error:'Client must close the connection'}));
              return;
            }
            if (replies[i] == speech.hypothesis_unicode && rnd(2) == 0) i--;
            var response = replies[i++](id, offset++);
            var timeout = (i != replies.length) ? rnd(max_timeout) : 10000;
            console.log('replying with: %s\n', response);
            connection.send(response, {}, function() {
              setTimeout(function(){reply(i, id, connection, offset)}, timeout) ;
            });
          })(0, id, ws, offset);
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
}());
