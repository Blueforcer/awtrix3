
/*
 * This HTML code will be injected in /setup webpage using a <div></div> element as parent
 * The parent element will hhve the HTML id properties equal to 'raw-html-<id>'
 * where the id value will be equal to the id parameter passed to the function addHTML(html_code, id).
 */
static const char html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>AWTRIX 3</title>
    <style>
        html, body {
            margin: 0;
            padding: 0;
            height: 100%;
            overflow: hidden; 
        }
        iframe {
            width: 100%;
            height: 100%;
            border: none;
            display: block;
        }
    </style>
</head>
<body>

<iframe id="mainFrame" src="{{IFRAME_URL}}"></iframe>

<script>
        window.addEventListener('message', async function(event) {
            try {
                const localUrl = event.data.url.replace(/^http:\/\/[^/]+/, '');
                const fetchOptions = {
                    method: event.data.method || 'GET',
                    headers: event.data.headers || {}
                };
                if (event.data.body) {
                    if (event.data.body.isFile) {
                        // File Upload Handling
                        const formData = new FormData();
                        const fetchResponse = await fetch(event.data.body.data);
                        const blob = await fetchResponse.blob();
                        formData.append('file', blob, event.data.body.path || event.data.body.name);
                        fetchOptions.body = formData;
                    } else if (event.data.method === 'PUT' || event.data.method === 'DELETE') {
                        // PUT/DELETE mit URL-encoded Daten
                        fetchOptions.body = event.data.body;
                    } else if (typeof event.data.body === 'string') {
                        // String body (z.B. für URL-encoded)
                        fetchOptions.body = event.data.body;
                    } else {
                        // JSON body für andere Fälle
                        fetchOptions.body = JSON.stringify(event.data.body);
                    }
                }
                
                const response = await fetch(localUrl, fetchOptions);
                let data;

                if (event.data.isImage || localUrl.match(/\.(gif|jpe?g|png)$/i)) {
                    // Bild zu base64
                    const blob = await response.blob();
                    data = await new Promise((resolve) => {
                        const reader = new FileReader();
                        reader.onload = () => resolve(reader.result);
                        reader.readAsDataURL(blob);
                    });
                } else if (event.data.method === 'POST' || event.data.method === 'PUT' || event.data.method === 'DELETE') {
                    data = { success: response.ok };
                } else {
                    try {
                        data = await response.json();
                    } catch (e) {
                        console.log('Response is not JSON:', e);
                        data = await response.text();
                    }
                }
            
                event.source.postMessage({
                    id: event.data.id,
                    success: response.ok,
                    data
                }, event.origin);
                
            } catch (error) {
                console.error('Error processing request:', error);
                event.source.postMessage({
                    id: event.data.id,
                    success: false,
                    error: error.message
                }, event.origin);
            }
        });
</script>
</body>
</html>
)rawliteral";

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