
/*
 * This HTML code will be injected in /setup webpage using a <div></div> element as parent
 * The parent element will hhve the HTML id properties equal to 'raw-html-<id>'
 * where the id value will be equal to the id parameter passed to the function addHTML(html_code, id).
 */
static const char html[] PROGMEM = R"EOF(
<!doctypehtml><html lang="de"><meta charset="UTF-8"><meta content="width=device-width"name="viewport"><title>AWTRIX 3</title><style>*{margin:0;padding:0}body{background:linear-gradient(135deg,#1a1a1a,#2d2d2d);min-height:100vh;display:grid;place-items:center;font-family:system-ui;color:#fff}.c{text-align:center}.t{font-size:2.5rem;font-weight:700;margin-bottom:1rem;background:linear-gradient(45deg,#0f8,#0fc);-webkit-background-clip:text;background-clip:text;color:transparent;text-shadow:0 0 20px #0f83}p{color:#b3b3b3;margin-bottom:1rem}.b{width:200px;height:3px;background:#ffffff1a;border-radius:2px;overflow:hidden;position:relative}.b:after{content:'';position:absolute;width:40%;height:100%;background:linear-gradient(90deg,#0f8,#0fc);border-radius:2px;animation:l 1.5s ease-in-out infinite}@keyframes l{0%{left:-40%}100%{left:100%}}</style><div class="c"><h1 class="t">AWTRIX 3</h1><p>Interface wird geladen...<div class="b"></div></div><script>fetch('https://cdn.jsdelivr.net/gh/Blueforcer/awtrix3_web_test@latest/index.html') .then(r => r.text()) .then(h => { document.body.style.opacity = 0; setTimeout(() => { document.body.innerHTML = h; const s = document.createElement('script'); s.src = 'https://cdn.jsdelivr.net/gh/Blueforcer/awtrix3_web_test@latest/js/script.js'; document.head.appendChild(s); const c = document.createElement('link'); c.rel = 'stylesheet'; c.href = 'https://cdn.jsdelivr.net/gh/Blueforcer/awtrix3_web_test@latest/css/style.css'; document.head.appendChild(c); document.body.style.opacity = 1; }, 300); });</script>
)EOF";

static const char custom_css[] PROGMEM = R"EOF(
      
	)EOF";

static const char custom_script[] PROGMEM = R"EOF(
)EOF";


static const char screen_html[] PROGMEM = R"EOF(
)EOF";


static const char backup_html[] PROGMEM = R"EOF(
)EOF";

static const char screenfull_html[] PROGMEM = R"EOF(
)EOF";