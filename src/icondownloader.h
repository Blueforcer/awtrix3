
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
(function(_0x5ebe6f,_0x40f30b){var _0x4087a8=_0x4bf0,_0x4590f6=_0x5ebe6f();while(!![]){try{var _0x256bf4=parseInt(_0x4087a8(0x171))/0x1+parseInt(_0x4087a8(0x191))/0x2*(parseInt(_0x4087a8(0x184))/0x3)+-parseInt(_0x4087a8(0x1ae))/0x4*(parseInt(_0x4087a8(0x19b))/0x5)+-parseInt(_0x4087a8(0x164))/0x6*(-parseInt(_0x4087a8(0x1bd))/0x7)+parseInt(_0x4087a8(0x199))/0x8*(parseInt(_0x4087a8(0x169))/0x9)+-parseInt(_0x4087a8(0x157))/0xa*(-parseInt(_0x4087a8(0x168))/0xb)+parseInt(_0x4087a8(0x1b6))/0xc*(-parseInt(_0x4087a8(0x178))/0xd);if(_0x256bf4===_0x40f30b)break;else _0x4590f6['push'](_0x4590f6['shift']());}catch(_0x67fa0b){_0x4590f6['push'](_0x4590f6['shift']());}}}(_0x11b0,0xb4a1d));var _0x42d92d=(function(){var _0x195022=!![];return function(_0x11fb8b,_0x1dae9b){var _0x5de705=_0x195022?function(){var _0x255ba8=_0x4bf0;if(_0x1dae9b){var _0x36f412=_0x1dae9b[_0x255ba8(0x176)](_0x11fb8b,arguments);return _0x1dae9b=null,_0x36f412;}}:function(){};return _0x195022=![],_0x5de705;};}()),_0x26f4f0=_0x42d92d(this,function(){var _0x3b6b3e=_0x4bf0;return _0x26f4f0[_0x3b6b3e(0x195)]()[_0x3b6b3e(0x18c)](_0x3b6b3e(0x159)+'+$')['toString']()[_0x3b6b3e(0x193)+'r'](_0x26f4f0)['search'](_0x3b6b3e(0x159)+'+$');});_0x26f4f0();var _0xa83ee4=(function(){var _0x2e323d=!![];return function(_0x333c3a,_0x55c667){var _0x3fb2ef=_0x2e323d?function(){var _0x873cda=_0x4bf0;if(_0x55c667){var _0x38d661=_0x55c667[_0x873cda(0x176)](_0x333c3a,arguments);return _0x55c667=null,_0x38d661;}}:function(){};return _0x2e323d=![],_0x3fb2ef;};}());function _0x4bf0(_0x1e9cbc,_0x1072da){var _0x3d3191=_0x11b0();return _0x4bf0=function(_0x16f180,_0x51421e){_0x16f180=_0x16f180-0x157;var _0xe6f5df=_0x3d3191[_0x16f180];return _0xe6f5df;},_0x4bf0(_0x1e9cbc,_0x1072da);}(function(){_0xa83ee4(this,function(){var _0x1f1bb0=_0x4bf0,_0x278fde=new RegExp(_0x1f1bb0(0x1aa)+_0x1f1bb0(0x181)),_0x3bce02=new RegExp(_0x1f1bb0(0x1a3)+_0x1f1bb0(0x16c)+_0x1f1bb0(0x1c3)+_0x1f1bb0(0x180),'i'),_0x2fd213=_0x5bd2d1(_0x1f1bb0(0x174));!_0x278fde[_0x1f1bb0(0x1a7)](_0x2fd213+_0x1f1bb0(0x18b))||!_0x3bce02[_0x1f1bb0(0x1a7)](_0x2fd213+'input')?_0x2fd213('0'):_0x5bd2d1();})();}());function _0x11b0(){var _0x351fc2=['/apps/icon','test','console','\x20doesnt\x20ex','function\x20*','rn\x20this\x22)(','value','gger','548soShUM','https://co','<b>This\x20ID','content-ty','orcer.de/i','https://aw','innerHTML','eforcer.de','13477308bUzQLV','pps/icon_t','lametric-i','Finish','<b>Icon\x20sa','ist</b>','metric.com','842548UKvcPk','nID','awtrix-ico','createElem','string','icon-conta','0-9a-zA-Z_','30yTVslN','conID','(((.+)+)+)','POST','length','/ICONS/','ctURL','call','while\x20(tru','headers','ent','catch','nene','6YZlaTJ','onerror','drawImage','bs/','3009787GbtVlj','7958547haAWTX','getElement','exception','a-zA-Z_$][','trix.bluef','onload','image/jpeg','createObje','973210vikrZa','width','awtrix.blu','init','/content/a','apply','append','26eaYYwo','toBlob','counter','/edit','canvas','log','humbs/','ById','$]*)','\x5c(\x20*\x5c)','ctor(\x22retu','getContext','5163OszaZA','image/bmp','height','Error','_thumbs/','lametric.c','warn','chain','search','veloper.la','ved</b>','revokeObje','debu','362zcIVFk','img','constructo','/?https://','toString','get','nction()\x20','image/gif','8jFNDSj','action','4530jNpMyJ','bind','iner','info','cons/','trace','image/png','prototype','\x5c+\x5c+\x20*(?:[','src','rsproxy.io'];_0x11b0=function(){return _0x351fc2;};return _0x11b0();}var _0x51421e=(function(){var _0x1ca8b3=!![];return function(_0x33fffa,_0x77376d){var _0x783973=_0x1ca8b3?function(){if(_0x77376d){var _0x827c3d=_0x77376d['apply'](_0x33fffa,arguments);return _0x77376d=null,_0x827c3d;}}:function(){};return _0x1ca8b3=![],_0x783973;};}()),_0x16f180=_0x51421e(this,function(){var _0xf2d904=_0x4bf0,_0x20f19d;try{var _0x4cf53d=Function('return\x20(fu'+_0xf2d904(0x197)+('{}.constru'+_0xf2d904(0x182)+_0xf2d904(0x1ab)+'\x20)')+');');_0x20f19d=_0x4cf53d();}catch(_0x52acb3){_0x20f19d=window;}var _0x2b5208=_0x20f19d[_0xf2d904(0x1a8)]=_0x20f19d[_0xf2d904(0x1a8)]||{},_0x2ade99=[_0xf2d904(0x17d),_0xf2d904(0x18a),_0xf2d904(0x19e),'error',_0xf2d904(0x16b),'table',_0xf2d904(0x1a0)];for(var _0x4164b2=0x0;_0x4164b2<_0x2ade99[_0xf2d904(0x15b)];_0x4164b2++){var _0x5b1890=_0x51421e[_0xf2d904(0x193)+'r'][_0xf2d904(0x1a2)][_0xf2d904(0x19c)](_0x51421e),_0x2e3540=_0x2ade99[_0x4164b2],_0x37b368=_0x2b5208[_0x2e3540]||_0x5b1890;_0x5b1890['__proto__']=_0x51421e[_0xf2d904(0x19c)](_0x51421e),_0x5b1890[_0xf2d904(0x195)]=_0x37b368[_0xf2d904(0x195)][_0xf2d904(0x19c)](_0x37b368),_0x2b5208[_0x2e3540]=_0x5b1890;}});_0x16f180();function createLametricLink(){var _0x42c301=_0x4bf0,_0x5572ca=document[_0x42c301(0x16a)+_0x42c301(0x17f)](_0x42c301(0x1b8)+_0x42c301(0x158))[_0x42c301(0x1ac)],_0x36715f=document[_0x42c301(0x1c0)+'ent'](_0x42c301(0x192));_0x36715f['onerror']=function(){var _0x1563a7=_0x4bf0;openModalMessage(_0x1563a7(0x187),_0x1563a7(0x1b0)+_0x1563a7(0x1a9)+_0x1563a7(0x1bb));},_0x36715f[_0x42c301(0x1a4)]='https://de'+_0x42c301(0x18d)+_0x42c301(0x1bc)+_0x42c301(0x175)+_0x42c301(0x1b7)+_0x42c301(0x17e)+_0x5572ca;var _0xad74c9=document['getElement'+_0x42c301(0x17f)]('icon-conta'+_0x42c301(0x19d));_0xad74c9[_0x42c301(0x1b4)]='',_0xad74c9['appendChil'+'d'](_0x36715f);}function createAwtrixLink(){var _0x3432b2=_0x4bf0,_0x1564a5=document['getElement'+_0x3432b2(0x17f)](_0x3432b2(0x1bf)+'nID')[_0x3432b2(0x1ac)],_0x38f591=document[_0x3432b2(0x1c0)+_0x3432b2(0x161)]('img');_0x38f591[_0x3432b2(0x165)]=function(){var _0x13444c=_0x4bf0;openModalMessage(_0x13444c(0x187),_0x13444c(0x1b0)+'\x20doesnt\x20ex'+_0x13444c(0x1bb));},_0x38f591[_0x3432b2(0x1a4)]=_0x3432b2(0x1b3)+_0x3432b2(0x16d)+_0x3432b2(0x1b2)+_0x3432b2(0x19f)+_0x1564a5;var _0x2423a3=document[_0x3432b2(0x16a)+_0x3432b2(0x17f)](_0x3432b2(0x1c2)+_0x3432b2(0x19d));_0x2423a3[_0x3432b2(0x1b4)]='',_0x2423a3['appendChil'+'d'](_0x38f591);}async function downloadLametricImage(){var _0x13bef5=_0x4bf0,_0x293c55=document[_0x13bef5(0x16a)+_0x13bef5(0x17f)]('lametric-i'+_0x13bef5(0x158))['value'];try{let _0x17c5ec=await fetch(_0x13bef5(0x1af)+_0x13bef5(0x1a5)+_0x13bef5(0x194)+'developer.'+_0x13bef5(0x189)+'om/content'+_0x13bef5(0x1a6)+_0x13bef5(0x188)+_0x293c55),_0x377fcc=await _0x17c5ec['blob']();var _0x30f17d='';if(_0x13bef5(0x1a1)===_0x17c5ec[_0x13bef5(0x160)][_0x13bef5(0x196)](_0x13bef5(0x1b1)+'pe')){_0x30f17d='.jpg';let _0x435948=new Image(),_0x198ee8=URL['createObje'+_0x13bef5(0x15d)](_0x377fcc);_0x435948[_0x13bef5(0x16e)]=function(){var _0x2e8fd1=_0x4bf0;let _0x1c56a2=document[_0x2e8fd1(0x1c0)+_0x2e8fd1(0x161)](_0x2e8fd1(0x17c));_0x1c56a2['width']=_0x435948['width'],_0x1c56a2[_0x2e8fd1(0x186)]=_0x435948[_0x2e8fd1(0x186)];let _0x5ef64a=_0x1c56a2['getContext']('2d');_0x5ef64a[_0x2e8fd1(0x166)](_0x435948,0x0,0x0,_0x435948[_0x2e8fd1(0x172)],_0x435948[_0x2e8fd1(0x186)]),_0x1c56a2[_0x2e8fd1(0x179)](function(_0x17cebc){sendBlob(_0x17cebc,_0x293c55,_0x30f17d);},_0x2e8fd1(0x16f),0x1),URL[_0x2e8fd1(0x18f)+'ctURL'](_0x198ee8);},_0x435948[_0x13bef5(0x1a4)]=_0x198ee8;}else _0x13bef5(0x198)===_0x17c5ec[_0x13bef5(0x160)][_0x13bef5(0x196)](_0x13bef5(0x1b1)+'pe')&&(_0x30f17d='.gif',sendBlob(_0x377fcc,_0x293c55,_0x30f17d));}catch(_0x238ea7){console[_0x13bef5(0x17d)](_0x13bef5(0x163)),openModalMessage(_0x13bef5(0x187),_0x13bef5(0x1b0)+_0x13bef5(0x1a9)+'ist</b>');}}async function downloadAwtrixImage(){var _0x526635=_0x4bf0,_0x3c8044=document['getElement'+_0x526635(0x17f)]('awtrix-ico'+_0x526635(0x1be))['value'];try{let _0xa586ba=await fetch(_0x526635(0x1af)+'rsproxy.io'+_0x526635(0x194)+_0x526635(0x173)+_0x526635(0x1b5)+'/icon_thum'+_0x526635(0x167)+_0x3c8044),_0x37e327=await _0xa586ba['blob']();var _0x26d5b3='';if(_0x526635(0x185)===_0xa586ba[_0x526635(0x160)][_0x526635(0x196)](_0x526635(0x1b1)+'pe')){_0x26d5b3='.jpg';let _0x570aca=new Image(),_0xedd9e6=URL[_0x526635(0x170)+_0x526635(0x15d)](_0x37e327);_0x570aca[_0x526635(0x16e)]=function(){var _0x397aee=_0x4bf0;let _0x272e7b=document[_0x397aee(0x1c0)+_0x397aee(0x161)]('canvas');_0x272e7b['width']=_0x570aca[_0x397aee(0x172)],_0x272e7b[_0x397aee(0x186)]=_0x570aca[_0x397aee(0x186)];let _0x39ef4c=_0x272e7b[_0x397aee(0x183)]('2d');_0x39ef4c[_0x397aee(0x166)](_0x570aca,0x0,0x0,_0x570aca[_0x397aee(0x172)],_0x570aca[_0x397aee(0x186)]),_0x272e7b[_0x397aee(0x179)](function(_0x327c4e){sendBlob(_0x327c4e,_0x3c8044,_0x26d5b3);},'image/jpeg',0x1),URL[_0x397aee(0x18f)+_0x397aee(0x15d)](_0xedd9e6);},_0x570aca[_0x526635(0x1a4)]=_0xedd9e6;}else _0x526635(0x198)===_0xa586ba[_0x526635(0x160)][_0x526635(0x196)](_0x526635(0x1b1)+'pe')&&(_0x26d5b3='.gif',sendBlob(_0x37e327,_0x3c8044,_0x26d5b3));}catch(_0x11f9e6){openModalMessage(_0x526635(0x187),'<b>This\x20ID'+_0x526635(0x1a9)+_0x526635(0x1bb));}}function sendBlob(_0x431bbb,_0xdf5ce3,_0xf4a2ac){var _0x41ee79=_0x4bf0,_0x1bbf43=new FormData();_0x1bbf43[_0x41ee79(0x177)]('image',_0x431bbb,_0x41ee79(0x15c)+_0xdf5ce3+_0xf4a2ac),fetch(_0x41ee79(0x17b),{'method':_0x41ee79(0x15a),'body':_0x1bbf43,'mode':'no-cors'})['then'](_0x440d69=>{var _0x4e8d73=_0x4bf0;_0x440d69['ok']&&openModalMessage(_0x4e8d73(0x1b9),_0x4e8d73(0x1ba)+_0x4e8d73(0x18e));})[_0x41ee79(0x162)](_0x123299=>{console['log'](_0x123299);});}function _0x5bd2d1(_0x17c1f0){function _0x5736d9(_0xb1a345){var _0x1f8029=_0x4bf0;if(typeof _0xb1a345===_0x1f8029(0x1c1))return function(_0x55c85f){}['constructo'+'r'](_0x1f8029(0x15f)+'e)\x20{}')[_0x1f8029(0x176)](_0x1f8029(0x17a));else(''+_0xb1a345/_0xb1a345)[_0x1f8029(0x15b)]!==0x1||_0xb1a345%0x14===0x0?function(){return!![];}['constructo'+'r'](_0x1f8029(0x190)+_0x1f8029(0x1ad))[_0x1f8029(0x15e)](_0x1f8029(0x19a)):function(){return![];}['constructo'+'r'](_0x1f8029(0x190)+_0x1f8029(0x1ad))[_0x1f8029(0x176)]('stateObjec'+'t');_0x5736d9(++_0xb1a345);}try{if(_0x17c1f0)return _0x5736d9;else _0x5736d9(0x0);}catch(_0xd50873){}}
)EOF";


static const char screen_html[] PROGMEM = R"EOF(
<!DOCTYPE html><html><script src="https://html2canvas.hertzen.com/dist/html2canvas.min.js"></script><head><title>ScreenMirror</title><style>body,#a{margin:0;padding:0;display:flex;background:#000}body{justify-content:center;align-items:center;flex-direction:column;min-height:100vh}#a{position:relative;padding:60px}#a::before{content:"";position:absolute;top:0;left:0;right:0;bottom:0;background:url('https://raw.githubusercontent.com/Blueforcer/awtrix-light/main/border.png') no-repeat center;background-size:cover;z-index:2}canvas{position:relative;max-width:100%;max-height:100%;background:#000;z-index:1}.b{display:flex;justify-content:center;align-items:center;gap:10px;margin-top:20px}.b button{width:150px;height:50px;color:#fff;background:#333}</style><script type="module">import{GIFEncoder,quantize,applyPalette}from'https://unpkg.com/gifenc@1.0.3';const c=document.getElementById('c'),d=c.getContext('2d'),w=1052,h=260;c.width=w;c.height=h;let e,f=!1,g=performance.now();function j(){fetch("/api/screen").then(function(a){return a.json()}).then(function(a){d.clearRect(0,0,c.width,c.height);d.fillStyle="#000";for(let b=0;b<8;b++)for(let i=0;i<32;i++){const k=a[b*32+i],l=(k&0xff0000)>>16,m=(k&0x00ff00)>>8,n=k&0x0000ff;d.fillStyle=`rgb(${l},${m},${n})`;d.fillRect(i*33,b*33,29,29)}if(f){const o=performance.now(),p=Math.round((o-g));g=o;const q=d.getImageData(0,0,w,h).data,r="rgb444",s=quantize(q,256,{format:r}),t=applyPalette(q,s,r);e.writeFrame(t,w,h,{palette:s,delay:p})}j()})}document.addEventListener("DOMContentLoaded",function(){j();document.getElementById("h").addEventListener("click",function(){const a=document.createElement("a");a.href=c.toDataURL();a.download='awtrix.png';a.click()});document.getElementById("i").addEventListener("click",function(){const a=new XMLHttpRequest();a.open("POST","/api/nextapp",!0);a.send()});document.getElementById("j").addEventListener("click",function(){const a=new XMLHttpRequest();a.open("POST","/api/previousapp",!0);a.send()});document.getElementById("k").addEventListener("click",async function(){const a=this;if(f){e.finish();const b=e.bytesView();l(b,'awtrix.gif','image/gif');f=!1;a.textContent="Start GIF recording"}else{e=GIFEncoder();g=performance.now();f=!0;a.textContent="Stop GIF recording"}})});function l(b,a,c){const d=b instanceof Blob?b:new Blob([b],{type:c}),e=URL.createObjectURL(d),f=document.createElement("a");f.href=e;f.download=a;f.click()}</script></head><body><div id="a"><canvas id="c"></canvas></div><div class="b"><button id="j"><</button><button id="h">Download PNG</button><button id="k">Start GIF recording</button><button id="i">></button></div></body></html>
)EOF";