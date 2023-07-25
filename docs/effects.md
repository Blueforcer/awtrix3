# Effects

with v0.71 AWTRIX light can show effects wherever you want
- Notification and CustomApps. This will show the effect as the first layer, so you can stil draw text over it.  
- Backgroundlayer. This will show the effect behind everything and in each app. You can add it via Hidden features.

Just call the name for your favorite effect.

<table>
  <tr>
    <th>Name</th>
    <th>Effect</th>
    <th>Name</th>
    <th>Effect</th>
  </tr>
  <tr>
    <td>BrickBreaker</td>
    <td><img src="https://raw.githubusercontent.com/Blueforcer/awtrix-light/main/docs/assets/BrickBreaker.gif" style="max-height:100px;"></td>
    <td>Checkerboard</td>
    <td><img src="https://raw.githubusercontent.com/Blueforcer/awtrix-light/main/docs/assets/Checkerboard.gif" style="max-height:100px;"></td>
  </tr>
  <tr>
    <td>Fireworks</td>
    <td><img src="https://raw.githubusercontent.com/Blueforcer/awtrix-light/main/docs/assets/Fireworks.gif" style="max-height:100px;"></td>
     <td>PingPong</td>
    <td><img src="https://raw.githubusercontent.com/Blueforcer/awtrix-light/main/docs/assets/PingPong.gif" style="max-height:100px;"></td>
  </tr>
  </tr>
    <tr>
    <td>Radar</td>
    <td><img src="https://raw.githubusercontent.com/Blueforcer/awtrix-light/main/docs/assets/radar.gif" style="max-height:100px;"></td>
     <td>Ripple</td>
    <td><img src="https://raw.githubusercontent.com/Blueforcer/awtrix-light/main/docs/assets/Ripple.gif" style="max-height:100px;"></td>
  </tr>
   <tr>
    <td>Snake</td>
    <td><img src="https://raw.githubusercontent.com/Blueforcer/awtrix-light/main/docs/assets/Snake.gif" style="max-height:100px;"></td>
     <td>TwinklingStars</td>
    <td><img src="https://raw.githubusercontent.com/Blueforcer/awtrix-light/main/docs/assets/TwinklingStars.gif" style="max-height:100px;"></td>
  </tr>
   <tr>
    <td>TheaterChase</td>
    <td><img src="https://raw.githubusercontent.com/Blueforcer/awtrix-light/main/docs/assets/TheaterChase.gif" style="max-height:100px;"></td>
      <td>ColorWaves</td>
    <td><img src="https://raw.githubusercontent.com/Blueforcer/awtrix-light/main/docs/assets/ColorWaves.gif" style="max-height:100px;"></td>
  </tr>
     <tr>
    <td>SwirlOut</td>
    <td><img src="https://raw.githubusercontent.com/Blueforcer/awtrix-light/main/docs/assets/SwirlOut.gif" style="max-height:100px;"></td>
    <td>SwirlIn</td>
    <td><img src="https://raw.githubusercontent.com/Blueforcer/awtrix-light/main/docs/assets/SwirlIn.gif" style="max-height:100px;"></td>
  </tr>
<tr>
    <td>LookingEyes</td>
    <td><img src="https://raw.githubusercontent.com/Blueforcer/awtrix-light/main/docs/assets/LookingEyes.gif" style="max-height:100px;"></td>
     <td>Matrix</td>
    <td><img src="https://raw.githubusercontent.com/Blueforcer/awtrix-light/main/docs/assets/Matrix.gif" style="max-height:100px;"></td>
  </tr>
           <tr>
    <td>Pacifica</td>
    <td><img src="https://raw.githubusercontent.com/Blueforcer/awtrix-light/main/docs/assets/Pacifica.gif" style="max-height:100px;"></td>
     <td>Plasma</td>
    <td><img src="https://raw.githubusercontent.com/Blueforcer/awtrix-light/main/docs/assets/Plasma.gif" style="max-height:100px;"></td>
  </tr>
               <tr>
    <td>PlasmaCloud</td>
    <td><img src="https://raw.githubusercontent.com/Blueforcer/awtrix-light/main/docs/assets/PlasmaCloud.gif" style="max-height:100px;"></td>
  </tr>
</table>

# Artnet (DMX)
  
Awtrix light supports Artnet out of the box.  
For [Jinx!](http://www.live-leds.de/) you can <a href="awtrix_light.jnx" download>download this template</a>. Just change the IP of both universes to your awtrix IP and youre ready to go.

**For any Other Arnet controller:**    
Create 2 universes with 384 channels each. Also add a new matrix layout with 8 strings á 32 Strands and top left starting position. When you start to send data, AWTRIX will stop its normal operation and shows your data. 1s after you stop sending data, AWTRIX will return to normal operation.
  
