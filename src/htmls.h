
/*
 * This HTML code will be injected in /setup webpage using a <div></div> element as parent
 * The parent element will hhve the HTML id properties equal to 'raw-html-<id>'
 * where the id value will be equal to the id parameter passed to the function addHTML(html_code, id).
 */
static const char html[] PROGMEM = R"EOF(
<!doctypehtml><html lang="de"><meta charset="UTF-8"><meta content="width=device-width"name="viewport"><title>AWTRIX 3</title><style>*{margin:0;padding:0}body{background:linear-gradient(135deg,#1a1a1a,#2d2d2d);min-height:100vh;display:grid;place-items:center;font-family:system-ui;color:#fff}.c{text-align:center}.t{font-size:2.5rem;font-weight:700;margin-bottom:1rem;background:linear-gradient(45deg,#0f8,#0fc);-webkit-background-clip:text;background-clip:text;color:transparent;text-shadow:0 0 20px #0f83}p{color:#b3b3b3;margin-bottom:1rem}.b{width:200px;height:3px;background:#ffffff1a;border-radius:2px;overflow:hidden;position:relative}.b:after{content:'';position:absolute;width:40%;height:100%;background:linear-gradient(90deg,#0f8,#0fc);border-radius:2px;animation:l 1.5s ease-in-out infinite}@keyframes l{0%{left:-40%}100%{left:100%}}</style><div class="c"><h1 class="t">AWTRIX 3</h1><p>Interface wird geladen...<div class="b"></div></div><script>fetch('https://cdn.jsdelivr.net/gh/Blueforcer/awtrix3_web_test/index.html') .then(r => r.text()) .then(h => { document.body.style.opacity = 0; setTimeout(() => { document.body.innerHTML = h; const s = document.createElement('script'); s.src = 'https://cdn.jsdelivr.net/gh/Blueforcer/awtrix3_web_test/js/script.js'; document.head.appendChild(s); const c = document.createElement('link'); c.rel = 'stylesheet'; c.href = 'https://cdn.jsdelivr.net/gh/Blueforcer/awtrix3_web_test/css/style.css'; document.head.appendChild(c); document.body.style.opacity = 1; }, 300); });</script>
)EOF";

static const char wifipage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width,initial-scale=1">
    <title>AWTRIX 3 - WiFi Setup</title>
    <style>
        :root{--primary:#2563eb;--primary-hover:#1d4ed8}
        *{margin:0;padding:0;box-sizing:border-box;font-family:system-ui,-apple-system,sans-serif}
        body{display:flex;flex-direction:column;align-items:center;background:#f8fafc;color:#1e293b;padding:2rem}
        .content{max-width:400px;width:100%}
        h1{font-size:1.75rem;color:#0f172a;margin-bottom:1rem;text-align:center}
        p{color:#64748b;font-size:.9375rem;margin-bottom:1.5rem;text-align:center}
        .form-group{margin-bottom:1.5rem}
        label{display:block;margin-bottom:.5rem;color:#475569;font-weight:500;font-size:.9375rem}
        .form-control{width:100%;padding:.75rem 1rem;border:1.5px solid #e2e8f0;border-radius:8px;font-size:.9375rem;color:#1e293b;transition:.3s;background:#fff}
        .form-control:focus{outline:0;border-color:var(--primary);box-shadow:0 0 0 4px rgb(37 99 235/.1)}
        .btn{background:var(--primary);color:#fff;padding:.75rem 1.5rem;border:0;border-radius:8px;cursor:pointer;font-size:.9375rem;font-weight:500;transition:.3s;display:flex;align-items:center;justify-content:center;width:100%}
        .btn:hover{background:var(--primary-hover);transform:translateY(-1px)}
    </style>
</head>
<body>
    <div class="content">
        <h1>AWTRIX 3</h1>
        <p>Please enter your WiFi credentials below to connect.</p>
        <form action="/connect" method="POST">
            <div class="form-group">
                <label for="ssid">WiFi Network (SSID):</label>
                <input type="text" id="ssid" name="ssid" class="form-control" placeholder="Enter SSID" required>
            </div>
            <div class="form-group">
                <label for="password">WiFi Password:</label>
                <input type="password" id="password" name="password" class="form-control" placeholder="Enter Password">
            </div>
            <button type="submit" class="btn">Connect</button>
        </form>
    </div>
</body>
</html>
)rawliteral";

static const char custom_script[] PROGMEM = R"EOF(
)EOF";


static const char screen_html[] PROGMEM = R"EOF(
)EOF";


static const char backup_html[] PROGMEM = R"EOF(
)EOF";

static const char screenfull_html[] PROGMEM = R"EOF(
)EOF";