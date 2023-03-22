svgLogo = '<svg width=64px height=64px viewBox="0 0 28 28"><path d="M12.926 19.324a7.6 7.6 0 0 0-2.983-6.754 7.44 7.44 0 0 0-3.828-1.554.697.697 0 0 1-.606-.731.674.674 0 0 1 .743-.617 8.97 8.97 0 0 1 8 9.805 7.828 7.828 0 0 1-.298 1.542l1.989.56a11.039 11.039 0 0 0 1.714-.651 12.159 12.159 0 0 0 .217-2.343A12.57 12.57 0 0 0 7.212 6.171a5.53 5.53 0 0 0-2 0 4.354 4.354 0 0 0-2.16 1.337 4.274 4.274 0 0 0 1.909 6.856 9.896 9.896 0 0 0 1.074.195 4.011 4.011 0 0 1 3.337 3.954 3.965 3.965 0 0 1-.64 2.16l1.371.88a10.182 10.182 0 0 0 2.057.342 7.52 7.52 0 0 0 .754-2.628m.16 4.73A13.073 13.073 0 0 1 .001 10.983 12.982 12.982 0 0 1 3.83 1.737l.743.697a12.067 12.067 0 0 0 0 17.141 12.067 12.067 0 0 0 17.141 0l.697.697a12.97 12.97 0 0 1-9.336 3.726M24 10.993A10.993 10.993 0 0 0 12.949 0l-1.143.057-.252.732a18.912 18.912 0 0 1 11.588 11.576l.731-.263c0-.366.069-.732.069-1.143m-1.269 5.165A17.53 17.53 0 0 0 7.818 1.27a11.119 11.119 0 0 0-2.457 1.77v1.635A13.919 13.919 0 0 1 19.268 18.57h1.634a11.713 11.713 0 0 0 1.771-2.446M7.92 17.884a1.691 1.691 0 1 1-1.69-1.691 1.691 1.691 0 0 1 1.69 1.691" /></svg>';
const svgMenu = '<path d="M3,6H21V8H3V6M3,11H21V13H3V11M3,16H21V18H3V16Z"/>';
const svgLock = '<svg height="16pt" viewBox="0 0 512 512"><path d="m336 512h-288c-26.453125 0-48-21.523438-48-48v-224c0-26.476562 21.546875-48 48-48h288c26.453125 0 48 21.523438 48 48v224c0 26.476562-21.546875 48-48 48zm-288-288c-8.8125 0-16 7.167969-16 16v224c0 8.832031 7.1875 16 16 16h288c8.8125 0 16-7.167969 16-16v-224c0-8.832031-7.1875-16-16-16zm0 0"/><path d="m304 224c-8.832031 0-16-7.167969-16-16v-80c0-52.929688-43.070312-96-96-96s-96 43.070312-96 96v80c0 8.832031-7.167969 16-16 16s-16-7.167969-16-16v-80c0-70.59375 57.40625-128 128-128s128 57.40625 128 128v80c0 8.832031-7.167969 16-16 16zm0 0"/></svg>';
const svgUnlock = '<svg height="16pt" viewBox="0 0 512 512"><path d="m336 512h-288c-26.453125 0-48-21.523438-48-48v-224c0-26.476562 21.546875-48 48-48h288c26.453125 0 48 21.523438 48 48v224c0 26.476562-21.546875 48-48 48zm-288-288c-8.8125 0-16 7.167969-16 16v224c0 8.832031 7.1875 16 16 16h288c8.8125 0 16-7.167969 16-16v-224c0-8.832031-7.1875-16-16-16zm0 0"/><path d="m80 224c-8.832031 0-16-7.167969-16-16v-80c0-70.59375 57.40625-128 128-128s128 57.40625 128 128c0 8.832031-7.167969 16-16 16s-16-7.167969-16-16c0-52.929688-43.070312-96-96-96s-96 43.070312-96 96v80c0 8.832031-7.167969 16-16 16zm0 0"/></svg>';
const svgScan = '<path d="M12 12C9.97 12 8.1 12.67 6.6 13.8L4.8 11.4C6.81 9.89 9.3 9 12 9S17.19 9.89 19.2 11.4L18.74 12C18.66 12 18.58 12 18.5 12C17.43 12 16.42 12.26 15.53 12.72C14.45 12.26 13.26 12 12 12M21 9L22.8 6.6C19.79 4.34 16.05 3 12 3S4.21 4.34 1.2 6.6L3 9C5.5 7.12 8.62 6 12 6S18.5 7.12 21 9M12 15C10.65 15 9.4 15.45 8.4 16.2L12 21L12.34 20.54C12.13 19.9 12 19.22 12 18.5C12 17.24 12.36 16.08 13 15.08C12.66 15.03 12.33 15 12 15M18 14.5C15.79 14.5 14 16.29 14 18.5S15.79 22.5 18 22.5C19.68 22.5 21.12 21.47 21.71 20H20C19.54 20.61 18.82 21 18 21C16.62 21 15.5 19.88 15.5 18.5S16.62 16 18 16C18.69 16 19.32 16.28 19.77 16.73L18 18.5H22V14.5L20.83 15.67C20.11 14.95 19.11 14.5 18 14.5Z" />';
const svgConnect = '<path d="M12 6C8.62 6 5.5 7.12 3 9L1.2 6.6C4.21 4.34 7.95 3 12 3S19.79 4.34 22.8 6.6L21 9C18.5 7.12 15.38 6 12 6M16.84 13.41C17.18 13.27 17.55 13.17 17.92 13.1L19.2 11.4C17.19 9.89 14.7 9 12 9S6.81 9.89 4.8 11.4L6.6 13.8C8.1 12.67 9.97 12 12 12C13.78 12 15.44 12.5 16.84 13.41M12 15C10.65 15 9.4 15.45 8.4 16.2L12 21L13.04 19.61C13 19.41 13 19.21 13 19C13 17.66 13.44 16.43 14.19 15.43C13.5 15.16 12.77 15 12 15M23 19L20 16V18H16V20H20V22L23 19Z" />';
const svgSave = '<path d="M15,9H5V5H15M12,19A3,3 0 0,1 9,16A3,3 0 0,1 12,13A3,3 0 0,1 15,16A3,3 0 0,1 12,19M17,3H5C3.89,3 3,3.9 3,5V19A2,2 0 0,0 5,21H19A2,2 0 0,0 21,19V7L17,3Z" />';
const svgRestart = '<path d="M12,4C14.1,4 16.1,4.8 17.6,6.3C20.7,9.4 20.7,14.5 17.6,17.6C15.8,19.5 13.3,20.2 10.9,19.9L11.4,17.9C13.1,18.1 14.9,17.5 16.2,16.2C18.5,13.9 18.5,10.1 16.2,7.7C15.1,6.6 13.5,6 12,6V10.6L7,5.6L12,0.6V4M6.3,17.6C3.7,15 3.3,11 5.1,7.9L6.6,9.4C5.5,11.6 5.9,14.4 7.8,16.2C8.3,16.7 8.9,17.1 9.6,17.4L9,19.4C8,19 7.1,18.4 6.3,17.6Z" />';
const svgEye = '<path d="M12 6.5c2.76 0 5 2.24 5 5 0 .51-.1 1-.24 1.46l3.06 3.06c1.39-1.23 2.49-2.77 3.18-4.53C21.27 7.11 17 4 12 4c-1.27 0-2.49.2-3.64.57l2.17 2.17c.47-.14.96-.24 1.47-.24zM2.71 3.16c-.39.39-.39 1.02 0 1.41l1.97 1.97C3.06 7.83 1.77 9.53 1 11.5 2.73 15.89 7 19 12 19c1.52 0 2.97-.3 4.31-.82l2.72 2.72c.39.39 1.02.39 1.41 0 .39-.39.39-1.02 0-1.41L4.13 3.16c-.39-.39-1.03-.39-1.42 0zM12 16.5c-2.76 0-5-2.24-5-5 0-.77.18-1.5.49-2.14l1.57 1.57c-.03.18-.06.37-.06.57 0 1.66 1.34 3 3 3 .2 0 .38-.03.57-.07L14.14 16c-.65.32-1.37.5-2.14.5zm2.97-5.33c-.15-1.4-1.25-2.49-2.64-2.64l2.64 2.64z" />';
const svgNoEye = '<path d="M0 0h24v24H0V0z" fill="none"/><path d="M12 4.5C7 4.5 2.73 7.61 1 12c1.73 4.39 6 7.5 11 7.5s9.27-3.11 11-7.5c-1.73-4.39-6-7.5-11-7.5zM12 17c-2.76 0-5-2.24-5-5s2.24-5 5-5 5 2.24 5 5-2.24 5-5 5zm0-8c-1.66 0-3 1.34-3 3s1.34 3 3 3 3-1.34 3-3-1.34-3-3-3z"/>';
const svgCloseModal = '<path fill-rule="evenodd" clip-rule="evenodd" d="M13.7071 1.70711C14.0976 1.31658 14.0976 0.683417 13.7071 0.292893C13.3166 -0.0976311 12.6834 -0.0976311 12.2929 0.292893L7 5.58579L1.70711 0.292893C1.31658 -0.0976311 0.683417 -0.0976311 0.292893 0.292893C-0.0976311 0.683417 -0.0976311 1.31658 0.292893 1.70711L5.58579 7L0.292893 12.2929C-0.0976311 12.6834 -0.0976311 13.3166 0.292893 13.7071C0.683417 14.0976 1.31658 14.0976 1.70711 13.7071L7 8.41421L12.2929 13.7071C12.6834 14.0976 13.3166 14.0976 13.7071 13.7071C14.0976 13.3166 14.0976 12.6834 13.7071 12.2929L8.41421 7L13.7071 1.70711Z" fill="black" />';

var options = {};

// Simple JQuery-like selector
var $ = function (el) {
  return document.getElementById(el);
};


function showHidePassword() {
  var inp = $("password");
  openModalMessage('Restart!', '<br>ESP restarted. Please wait a little and then reload this page.<br>');
  if (inp.type === "password") {
    inp.type = "text";
    $('show-pass').classList.remove("w--current");
    $('hide-pass').classList.add("w--current");
  }
  else {
    inp.type = "password";
    $('show-pass').classList.add("w--current");
    $('hide-pass').classList.remove("w--current");
  }
}

/**
* Read some data from database
*/
function getWiFiList() {
  $('loader').classList.remove('hide');
  var url = new URL("http://" + `${window.location.hostname}` + "/scan");
  fetch(url)
    .then(response => response.json())
    .then(data => {
      listWifiNetworks(data);
      $('loader').classList.add('hide');
    });
}


function selectWifi(row) {
  try {
    $('select-' + row.target.parentNode.id).checked = true;
  }
  catch (err) {
    $(row.target.id).checked = true;
  }
  $('ssid').value = this.cells[1].innerHTML;
  $('ssid-name').innerHTML = this.cells[1].innerHTML;
  $('password').focus();
}


function listWifiNetworks(elems) {
  const list = document.querySelector('#wifi-list');
  list.innerHTML = "";
  elems.forEach((elem, idx) => {

    // Create a single row with all columns
    var row = document.createElement('tr');
    var id = 'wifi-' + idx;
    row.id = id;
    row.addEventListener('click', selectWifi);
    row.innerHTML = `<td><input type="radio" name="select" id="select-${id}"></td>`;
    row.innerHTML += `<td id="ssid-${id}">${elem.ssid}</td>`;
    row.innerHTML += '<td class="hide-tiny">' + elem.strength + ' dBm</td>';
    if (elem.security)
      row.innerHTML += '<td>' + svgLock + '</td>';
    else
      row.innerHTML += '<td>' + svgUnlock + '</td>';

    // Add row to list
    list.appendChild(row);
  });

  $("wifi-table").classList.remove("hide");
}


function getParameters() {
  $('loader').classList.remove('hide');
  var url = new URL("http://" + `${window.location.hostname}` + "/config.json");
  fetch(url)
    .then(response => response.json())
    .then(data => {
      Object.keys(data).forEach(function (key) {
        if (key.startsWith('logo-name')) {
          $('name-logo').innerHTML = data[key];
          delete data[key];
        }
        if (key.startsWith('logo-svg')) {
          $('svg-logo').innerHTML = data[key];
          delete data[key];
        }
        $('loader').classList.add('hide');
      });

      options = data;
      listParameters(options);

      // Read logo from file if present, otherwise use inline svg;
      if (options['logo-file-hidden']) {
        svgLogo = '';
        $('svg-logo').innerHTML = '<img class=logo src="' + options['logo-file-hidden'] + '"/>';
        $('svg-logo').setAttribute('title', '');
        $('logo-file').setAttribute('type', 'number');
      }
    });
}


function createNewBox(cont, lbl) {
  var el = document.createElement('div');
  el.setAttribute('id', 'option-box' + cont);
  el.classList.add('ctn', 'opt-box', 'hide');

  var h = document.createElement('h2');
  h.classList.add('heading-2');
  h.innerHTML = lbl;
  el.appendChild(h);

  var form = document.createElement('form');
  form.classList.add('form');

  el.appendChild(form);
  $('main-box').appendChild(el);

  // Add new voice in menu and relatvie listener
  var lnk = document.createElement('a');
  lnk.setAttribute('id', 'set-opt' + cont);
  lnk.setAttribute('data-box', 'option-box' + cont);
  lnk.classList.add('a-link');
  lnk.innerHTML = lbl;
  lnk.addEventListener('click', switchPage);

  $('nav-link').appendChild(lnk);
  return form;
}

function listParameters(params) {
  var el, pBox;
  if (!Object.keys(params)[0].startsWith('param-box')) {
    params = { 'param-box1': 'Options', ...params };
    options = params;
  }

  Object.entries(params).forEach(([key, value], i) => {

    if (key.startsWith('name-logo')) {
      $('name-logo').innerHTML = value;
      return;
    }

    else if (key.startsWith('logo-file')) {
      return;
    }

    else if (key.startsWith('param-box')) {
      pBox = createNewBox(i, value);
      return;
    }

    else if (key.startsWith('raw-css')) {
      var css = document.createElement("style");
      css.innerHTML = value.trim();
      document.body.appendChild(css);
      return;
    }

    else if (key.startsWith('raw-javascript')) {
      var js = document.createElement("script");
      js.innerHTML = value.trim();
      document.body.appendChild(js);
      return;
    }

    else if (key.startsWith('raw-html')) {
      html = value.trim();
      el = document.createElement('div');
      el.setAttribute('id', 'row' + i)
      el.style.width = '100%';
      el.innerHTML = html;
      pBox.appendChild(el);
    }
    else {
      let val = value;
      let lbl = document.createElement('label');
      el = document.createElement('input');
      el.setAttribute('id', key);
      el.setAttribute('type', 'text');

      // Set input property (id, type and value). Check first if is boolean
      if (typeof (val) === "boolean") {
        el.setAttribute('type', 'checkbox');
        el.classList.add('t-check', 'opt-input');
        el.checked = val;
        lbl.classList.add('input-label', 'toggle');
        let dv = document.createElement('div');
        dv.classList.add('toggle-switch');
        let sp = document.createElement('span');
        sp.classList.add('toggle-label');
        sp.textContent = key;
        lbl.appendChild(el);
        lbl.appendChild(dv);
        lbl.appendChild(sp);
        pBox.appendChild(lbl);
        addInputListener(el);
      }
      else {
        el.value = val;
        el.classList.add('opt-input');
        lbl.setAttribute('label-for', key);
        lbl.classList.add('input-label');
        lbl.textContent = key;
        if (typeof (val) === "number")
          el.setAttribute('type', 'number');

        if (typeof (val) === "object") {
          // This is a select/option
          if (val.values) {
            el = document.createElement('select');
            el.setAttribute('id', key);
            val.values.forEach((a) => {
              var opt = document.createElement('option');
              opt.textContent = a;
              opt.value = a;
              el.appendChild(opt);
            })
            el.value = val.selected;
            pBox.appendChild(el);
          }

          // This is a float value
          else {
            var num = Math.round(val.value * (1 / val.step)) / (1 / val.step);
            el.setAttribute('type', 'number');
            el.setAttribute('step', val.step);
            el.setAttribute('min', val.min);
            el.setAttribute('max', val.max);
            el.value = Number(num).toFixed(3);
          }
        }
        addInputListener(el);
        var d = document.createElement('div');
        d.classList.add('tf-wrapper');
        d.appendChild(lbl);
        d.appendChild(el);
        pBox.appendChild(d);
      }
    }

    if (key.endsWith('-hidden')) {
      el.classList.add('hide');
    }
  });
}

function addInputListener(item) {
  // Add event listener to option inputs

  if (item.type === "number") {
    item.addEventListener('change', () => {
      if (item.getAttribute("step")) {
        var obj = {};
        obj.value = Math.round(item.value * (1 / item.step)) / (1 / item.step);
        obj.step = item.getAttribute("step");
        obj.min = item.getAttribute("min");
        obj.max = item.getAttribute("max");
        options[item.id] = obj;
      }
      else
        options[item.id] = parseInt(item.value);
    });
    return;
  }

  if (item.type === "text") {
    item.addEventListener('change', () => {
      options[item.id] = item.value;
    });
    return;
  }

  if (item.type === "checkbox") {
    item.addEventListener('change', () => {
      options[item.id] = item.checked;
    });
    return;
  }

  if (item.type === 'select-one') {
    item.addEventListener('change', (e) => {
      options[e.target.id].selected = e.target.value;
    });
    return;
  }
}


function saveParameters() {
  var myblob = new Blob([JSON.stringify(options, null, 2)], {
    type: 'application/json'
  });
  var formData = new FormData();
  formData.append("data", myblob, '/config.json');

  // POST data using the Fetch API
  fetch('/edit', {
    method: 'POST',
    body: formData
  })
    // Handle the server response
    .then(response => response.text())
    .then(text => {
      openModalMessage('Save options', 'Settings saved successfully on flash memory!<br><br>');
    });

  fetch('/save')
    .then(response => response.text())
    .then(text => {

    })
    .catch(error => {

    });
}


function doConnection() {
  var formdata = new FormData();
  formdata.append("ssid", $('ssid').value);
  formdata.append("password", $('password').value);
  formdata.append("persistent", $('persistent').checked);
  var requestOptions = {
    method: 'POST',
    body: formdata,
    redirect: 'follow'
  };

  $('loader').classList.remove('hide');
  fetch('/connect', requestOptions)
    .then(function (response) {
      httpCode = response.status;
      return response.text();
    })
    .then(function (text) {
      if (httpCode === 200) {
        openModalMessage('Connected', 'AWTRTIX will restart now <br> Your new IP is ' + text);
      }
      else {
        openModalMessage('Error!', '<br>Error on connection: <b>' + text + '</b><br><br>');
      }
      $('loader').classList.add('hide');
    });
}


function switchPage(el) {
  $('top-nav').classList.remove('responsive');

  // Menu items
  document.querySelectorAll("a").forEach(item => {
    item.classList.remove('active');
  });
  el.target.classList.add('active');

  // Box items
  document.querySelectorAll(".opt-box").forEach(e => {
    e.classList.add('hide');
  });
  $(el.target.getAttribute("data-box")).classList.remove('hide');

  if (el.target.id != 'set-wifi') {
    var fragment = document.createDocumentFragment();
    fragment.appendChild($('btn-box'));
    $(el.target.getAttribute("data-box")).appendChild(fragment);
    $('btn-box').classList.remove('hide');
  }
  else
    $('btn-box').classList.add('hide');
}


function showMenu() {
  $('top-nav').classList.add('responsive');
}

var closeCallback = function () { ; };

function openModalMessage(title, msg, fn) {
  $('message-title').innerHTML = title;
  $('message-body').innerHTML = msg;
  $('modal-message').open = true;
  $('main-box').style.filter = "blur(3px)";
  if (typeof fn != 'undefined') {
    closeCallback = fn;
    $('ok-modal').classList.remove('hide');
  }
  else
    $('ok-modal').classList.add('hide');
}

function closeModalMessage(do_cb) {
  $('modal-message').open = false;
  $('main-box').style.filter = "";
  if (typeof closeCallback != 'undefined' && do_cb)
    closeCallback();
}


function restartESP() {
  var url = new URL("http://" + `${window.location.hostname}` + "/restart");
  fetch(url)
    .then(response => response.text())
    .then(data => {
      closeModalMessage();
      openModalMessage('Restart!', '<br>ESP restarted. Please wait a little and then reload this page.<br>');
    });
}

function uploadLogo() {
  var file = $('logo-file').files[0];
  var reader = new FileReader();

  reader.onload = async () => {
    const formData = new FormData();
    formData.append('image', file);
    try {
      const response = await fetch('/edit', {
        method: 'POST',
        body: formData,
      });
      options = { ...options, 'logo-file-hidden': file.name };
      saveParameters();
    }
    catch (error) {
      console.error(error);
    }
  };
  reader.readAsDataURL(file);
}

// Initializes the app.
$('svg-menu').innerHTML = svgMenu;
$('svg-eye').innerHTML = svgEye;
$('svg-no-eye').innerHTML = svgNoEye;
$('svg-scan').innerHTML = svgScan;
$('svg-connect').innerHTML = svgConnect;
$('svg-save').innerHTML = svgSave;
$('svg-restart').innerHTML = svgRestart;
$('close-modal').innerHTML = svgCloseModal;
$('svg-logo').innerHTML = svgLogo;
$('svg-logo').setAttribute('title', 'Click to upload your logo file');

$('hum-btn').addEventListener('click', showMenu);
$('scan-wifi').addEventListener('click', getWiFiList);
$('connect-wifi').addEventListener('click', doConnection);
$('save-params').addEventListener('click', saveParameters);
$('show-hide-password').addEventListener('click', showHidePassword);
$('set-wifi').addEventListener('click', switchPage);
// $('set-opt1').addEventListener('click', switchPage);
$('about').addEventListener('click', switchPage);
$('restart').addEventListener('click', restartESP);

window.addEventListener('load', getParameters);

// Enable wifi-connect btn only if password inserted
$('connect-wifi').disabled = true;
$('password').addEventListener('input', (event) => {
  if ($('password').value.length === 0)
    $('connect-wifi').disabled = true;
  else
    $('connect-wifi').disabled = false;
});