
/*
 * This HTML code will be injected in /setup webpage using a <div></div> element as parent
 * The parent element will hhve the HTML id properties equal to 'raw-html-<id>'
 * where the id value will be equal to the id parameter passed to the function addHTML(html_code, id).
 */
static const char custom_html[] PROGMEM = R"EOF(
  <div id="iconcontent">
        <div id="form-con">
            <form>
                <label for="lametric-iconID">Icon ID</label><br>
                <input type="text" id="lametric-iconID" name="lametric-iconID"><br>
                <div class="button-row">
                    <input class="btn" type="button" value="Preview" onclick="createLametricLink()">
                    <input class="btn" type="button" value="Download" onclick="downloadLametricImage()">
                </div>
            </form>
        </div>
        <br>
        <br>
        <div id="icon-container">
        </div>
    </div>
)EOF";

static const char custom_css[] PROGMEM = R"EOF(
        .iconcontent {
            width: 50%;
            justify-content: center;
        }
        #form-con {
            width: 50%;
            margin: 0 auto;
            min-width: 200px;
        }
        .button-row input {
            width: 50%;
            margin: 0 5px;
        }
        .button-row {
            display: flex;
            justify-content: space-evenly;
            margin: 0 -5px;
            margin-top: 5px;
        }
        #icon-container {
            margin: 0 auto;
            max-width: 150px;
            max-height: 150px;
            width: 150px;
            background-color: black;
            height: 150px;
            margin: 0 auto;
        }
        #icon-container img {
            image-rendering: pixelated;
            max-width: 150px;
            max-height: 150px;
            width: 150px;
            background-color: black;
            height: 150px;
        }
	)EOF";

static const char custom_script[] PROGMEM = R"EOF(
function createLametricLink(){const e=document.getElementById("lametric-iconID").value,t=document.createElement("img");t.onerror=function(){openModalMessage("Error","<b>This ID doesnt exist</b>")},t.src="https://developer.lametric.com/content/apps/icon_thumbs/"+e;const n=document.getElementById("icon-container");n.innerHTML="",n.appendChild(t)}async function downloadLametricImage(){const e=document.getElementById("lametric-iconID").value;try{let n=await fetch("https://developer.lametric.com/content/apps/icon_thumbs/"+e),o=await n.blob();var t="";const c=n.headers.get("content-type");if("image/jpeg"===c||"image/png"===c){t=".jpg";let n=new Image,c=URL.createObjectURL(o);n.onload=function(){let o=document.createElement("canvas");o.width=n.width,o.height=n.height,o.getContext("2d").drawImage(n,0,0,n.width,n.height),o.toBlob((function(n){sendBlob(n,e,t)}),"image/jpeg",1),URL.revokeObjectURL(c)},n.src=c}else"image/gif"===n.headers.get("content-type")&&sendBlob(o,e,t=".gif")}catch(e){console.log("Error"),openModalMessage("Error","<b>This ID doesnt exist</b>")}}function sendBlob(e,t,n){const o=new FormData;o.append("image",e,"ICONS/"+t+n),fetch("/edit",{method:"POST",body:o,mode:"no-cors"}).then((e=>{e.ok&&openModalMessage("Finish","<b>Icon saved</b>")})).catch((e=>{console.log(e)}))}
)EOF";


static const char screen_html[] PROGMEM = R"EOF(
<!DOCTYPE html><html><script src="https://html2canvas.hertzen.com/dist/html2canvas.min.js"></script><head><title>LiveView</title><style>body,#a{margin:0;padding:0;display:flex;background:#000}body{justify-content:center;align-items:center;flex-direction:column;min-height:100vh}#a{position:relative;padding:60px}#a::before{content:"";position:absolute;top:0;left:0;right:0;bottom:0;background:url('https://raw.githubusercontent.com/Blueforcer/awtrix3/main/border.png') no-repeat center;background-size:cover;z-index:2}canvas{position:relative;max-width:100%;max-height:100%;background:#000;z-index:1}.b{display:flex;justify-content:center;align-items:center;gap:10px;margin-top:20px}.b button{width:150px;height:50px;color:#fff;background:#333}</style><script type="module">import{GIFEncoder,quantize,applyPalette}from'https://unpkg.com/gifenc@1.0.3';const c=document.getElementById('c'),d=c.getContext('2d'),w=1052,h=260;c.width=w;c.height=h;let e,f=!1,g=performance.now();function j(){fetch("/api/screen").then(function(a){return a.json()}).then(function(a){d.clearRect(0,0,c.width,c.height);d.fillStyle="#000";for(let b=0;b<8;b++)for(let i=0;i<32;i++){const k=a[b*32+i],l=(k&0xff0000)>>16,m=(k&0x00ff00)>>8,n=k&0x0000ff;d.fillStyle=`rgb(${l},${m},${n})`;d.fillRect(i*33,b*33,29,29)}if(f){const o=performance.now(),p=Math.round((o-g));g=o;const q=d.getImageData(0,0,w,h).data,r="rgb444",s=quantize(q,256,{format:r}),t=applyPalette(q,s,r);e.writeFrame(t,w,h,{palette:s,delay:p})}j()})}document.addEventListener("DOMContentLoaded",function(){j();document.getElementById("h").addEventListener("click",function(){const a=document.createElement("a");a.href=c.toDataURL();a.download='awtrix.png';a.click()});document.getElementById("i").addEventListener("click",function(){const a=new XMLHttpRequest();a.open("POST","/api/nextapp",!0);a.send()});document.getElementById("j").addEventListener("click",function(){const a=new XMLHttpRequest();a.open("POST","/api/previousapp",!0);a.send()});document.getElementById("k").addEventListener("click",async function(){const a=this;if(f){e.finish();const b=e.bytesView();l(b,'awtrix.gif','image/gif');f=!1;a.textContent="Start GIF recording"}else{e=GIFEncoder();g=performance.now();f=!0;a.textContent="Stop GIF recording"}})});function l(b,a,c){const d=b instanceof Blob?b:new Blob([b],{type:c}),e=URL.createObjectURL(d),f=document.createElement("a");f.href=e;f.download=a;f.click()}</script></head><body><div id="a"><canvas id="c"></canvas></div><div class="b"><button id="j"><</button><button id="h">Download PNG</button><button id="k">Start GIF recording</button><button id="i">></button></div></body></html>
)EOF";


static const char backup_html[] PROGMEM = R"EOF(
<!DOCTYPE html><html lang="en"><head> <meta charset="UTF-8"> <meta name="viewport" content="width=device-width, initial-scale=1.0"> <title>Backup & Restore </title> <link href="https://maxcdn.bootstrapcdn.com/bootstrap/4.5.2/css/bootstrap.min.css" rel="stylesheet"> <script src="https://cdnjs.cloudflare.com/ajax/libs/jszip/3.1.5/jszip.min.js"></script></head><body class="bg-light d-flex justify-content-center align-items-center vh-100"> <div class="container bg-white p-5 rounded shadow-lg text-center"> <h2 class="mb-5">AWTRIX 3 Backup & Restore</h2> <button class="btn btn-primary btn-lg mb-3" id="backupButton"> <i class="fas fa-download mr-2"></i> Download Backup </button> <br><input type="file" id="fileInput" style="display: none;"> <button class="btn btn-secondary btn-lg" id="restoreButton"> <i class="fas fa-upload mr-2"></i> Upload for Restore </button> </div><script>function trimLeadingSlash(path){return path.startsWith('/') ? path.slice(1) : path;}function joinPaths(...paths){return paths.join('/').replace(/\/+/g, '/');}async function getFilesFromDirectory(dir, zip){const response=await fetch(`/list?dir=${dir}`); const file_list=await response.json(); for (let file_info of file_list){if (file_info['type']==='file'){const filename=file_info['name']; const source_file_url=`${dir}${filename}`; const fileResponse=await fetch(source_file_url); if (fileResponse.status===200){const fileContent=await fileResponse.blob(); zip.file(trimLeadingSlash(joinPaths(dir, filename)), fileContent);}else{alert(`Failed to download file ${filename}`);}}else if (file_info['type']==='dir'){await getFilesFromDirectory(`${dir}${file_info['name']}/`, zip);}}}document.getElementById('backupButton').addEventListener('click', async function (){let zip=new JSZip(); let btn=document.getElementById('backupButton'); btn.textContent='Downloading...'; await getFilesFromDirectory('/', zip); zip.generateAsync({type: "blob"}).then(function (blob){const url=window.URL.createObjectURL(blob); const a=document.createElement('a'); a.style.display='none'; a.href=url; a.download='backup.zip'; document.body.appendChild(a); a.click(); window.URL.revokeObjectURL(url);}); btn.textContent='Download Backup';}); document.getElementById('restoreButton').addEventListener('click', function (){document.getElementById('fileInput').click();}); document.getElementById('fileInput').addEventListener('change', async function (){const file=this.files[0]; const zip=new JSZip(); let btn=document.getElementById('restoreButton'); btn.textContent='Restoring. Please wait...'; setTimeout(async ()=>{try{const contents=await zip.loadAsync(file); const uploadPromises=[]; for (let filename in contents.files){if (!contents.files[filename].dir){const fileContent=await contents.files[filename].async("blob"); const target_file_url=`/edit?filename=${filename}`; const formData=new FormData(); formData.append('file', fileContent, filename); const uploadPromise=fetch(target_file_url,{method: 'POST', body: formData}); uploadPromises.push(uploadPromise);}}const responses=await Promise.all(uploadPromises); if (responses.every(response=> response.ok)){btn.textContent='Upload for Restore'; fetch('/api/reboot',{method: 'POST'}); alert(`Backup successfully restored. Rebooting..`);}else{alert(`Failed to restore backup.`); btn.textContent='Upload for Restore';}}catch (error){alert(`An error occurred: ${error}`); btn.textContent='Upload for Restore';}}, 10);}); </script></body>
)EOF";

static const char screenfull_html[] PROGMEM = R"EOF(
<!doctype html><html> <head> <title>LiveView</title> <style>body{display: flex; justify-content: center; align-items: center; min-height: 100vh; margin: 0; overflow: hidden; background: #000;}canvas{display: block; width: 100vw; background: #000; z-index: 1;}</style> </head> <body><canvas id=c></canvas></body> <script>const c=document.getElementById("c"), d=c.getContext("2d");const urlParams=new URLSearchParams(window.location.search);const queriedFPS=parseInt(urlParams.get('fps'));let fps=%%FPS%%;function scd(){const t=window.innerWidth; c.width=t, c.height=t / 4;}function j(){fetch("/api/screen").then(t=> t.json()).then(t=>{d.clearRect(0, 0, c.width, c.height); d.fillStyle="#000"; for (let e=0; e < 8; e++) for (let n=0; n < 32; n++){const i=t[32 * e + n], o=(16711680 & i) >> 16, s=(65280 & i) >> 8, h=255 & i; d.fillStyle=`rgb(${o},${s},${h})`; d.fillRect(n * (c.width / 32), e * (c.height / 8), c.width / 32 - 4, c.height / 8 - 4);}setTimeout(j, 1000 / fps);});}scd();document.addEventListener("DOMContentLoaded", j);window.addEventListener("resize", scd); </script></html>
)EOF";