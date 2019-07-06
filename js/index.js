let Terminal = require('../node_modules/terminal.js/dist/terminal.js');
let cp437_to_unicode = require('./cp437.js');

window.login = function() {
  const data = {
    username: $('#username').val(),
    password: $('#password').val()
  };
  $.post("/login", data).done((data) => {
    $('#login_error').text("");
    $('#login_form').hide();
    main(data.username, data.token);
  }).fail(() => {
    $('#login_error').text("Login failed");
    console.log("login failure");
  });
};

function main(username, login_token) {
  let div = document.createElement('pre');
  div.setAttribute('style', 'display: inline-block');
  div.setAttribute('data-columns', '80');
  div.setAttribute('data-rows', '25');
  document.getElementById('terminal').appendChild(div);
  
  let url = window.location.origin;

  if (url.startsWith('http://')) {
    url = url.substr(7);
    url = 'ws://' + url + '/tty';
  } else if (url.startsWith('https://')) {
    url = url.substr(8);
    url = 'wss://' + url + '/tty';
  }
  url += '?token=' + login_token;
  var sock = new WebSocket(url);
  sock.addEventListener('close', function() {
    console.log('websocket closing');
  });
  sock.addEventListener('open', function() {
    console.log("Connected to websocket");
    let term = new Terminal({columns: 80, rows: 25});

    let keypress = function(event) {
      let key = event.key;
      if (key === 'Enter') {
        term.write("\r\n");
        let arr = new Uint8Array(1);
        arr[0] = 10;
        sock.send(arr);
      } else if (key.length === 1) {
        term.write(key);
        let arr = new Uint8Array(1);
        arr[0] = key.charCodeAt(0);
        sock.send(arr);
      } else if (key === 'Backspace') {
        term.write("\x08");
        let arr = new Uint8Array(1);
        arr[0] = 127;
        sock.send(arr);
      } else if (key === 'ArrowLeft') {
        term.write("w\r\n");
        let arr = new Uint8Array(1);
        arr[0] = 119;
        sock.send(arr);
        arr[0] = 10;
        sock.send(arr);
      } else if (key === 'ArrowRight') {
        term.write("e\r\n");
        let arr = new Uint8Array(1);
        arr[0] = 101;
        sock.send(arr);
        arr[0] = 10;
        sock.send(arr);
      } else if (key === 'ArrowUp') {
        term.write("n\r\n");
        let arr = new Uint8Array(1);
        arr[0] = 110;
        sock.send(arr);
        arr[0] = 10;
        sock.send(arr);
      } else if (key === 'ArrowDown') {
        term.write("s\r\n");
        let arr = new Uint8Array(1);
        arr[0] = 115;
        sock.send(arr);
        arr[0] = 10;
        sock.send(arr);
      } else {
        console.log("Ignoring key", key);
      }
      return false;
    };
    div.addEventListener('keydown', keypress);
    term.dom(div);
    $(div).focus();
    let events = [];
    sock.addEventListener('message', function (event) {
      events.push(event.data);
    });

    let check = function() {
      let capture = events;
      events = [];
      if (capture.length === 0) {
        setTimeout(check, 10);
        return;
      }

      let reader = new FileReader();
      let i = 0;
      let processData = function() {
        let data = new Uint8Array(reader.result);
        let s = cp437_to_unicode(data);
        s = s.split("\n").join("\r\n");
        term.write(s, null, function() {
          i++;
          if (i < capture.length) {
            reader = new FileReader();
            reader.addEventListener('loadend', processData);
            reader.readAsArrayBuffer(capture[i]);
          } else {
            setTimeout(check, 500);
          }
        });
      }
      reader.addEventListener("loadend", processData);
      reader.readAsArrayBuffer(capture[i]);
    }
    setTimeout(check, 10);
    window.sock = sock;
  });
}

function refresh_stats() {
  $.get("/stats", function(data) {
    $('#users_on').text('' + data.users_on + ' / ' + data.max_users + ' users logged in');
  });
}

$(function() {
  $('#password').on('keyup', function(e) {
    if (e.which === 13) {
      window.login();
      return false;
    }            
  });

  setInterval(refresh_stats, 10000);
  refresh_stats();
});


