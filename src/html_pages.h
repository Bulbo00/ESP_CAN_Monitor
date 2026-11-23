#ifndef HTML_PAGES_H
#define HTML_PAGES_H

#include <Arduino.h>

// Gemeinsames CSS und dynamische Navbar mit aktiver Markierung
String getHeader(String title, String currentPath) {
  String h = "<!DOCTYPE html><html><head>";
  h += "<meta charset='utf-8'>";
  h += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  h += "<title>" + title + "</title>";
//  h += "table { width:100%; max-width:700px; margin:0 auto; border-collapse:collapse; font-size:13px; }";
//  h += "th, td { border:1px solid #444; padding:4px 6px; text-align:center; }";
//  h += "th { color:#bb86fc; } td.d { font-family:monospace; }";
  h += "<style>";
  h += "body { font-family: Arial, sans-serif; background-color: #121212; color: #e0e0e0; margin: 0; padding: 10px; box-sizing: border-box; }";
  h += "h1 { color: #03dac6; text-align: center; margin: 8px 0; font-size: 18px; }";
  h += "h2 { color: #03dac6; text-align: center; margin: 5px 0; font-size: 16px; }";
  
  // Navigation (3 Buttons)
  h += ".navbar { display: flex; justify-content: center; gap: 10px; margin-bottom: 12px; }";
  h += ".nav-btn { background: #333; color: #03dac6; padding: 8px 14px; border-radius: 6px; text-decoration: none; font-size: 13px; font-weight: bold; border: 1px solid #03dac6; }";
  h += ".nav-btn.active { background: #03dac6; color: #121212; }";
  
  // Allgemeine Karten (Dashboard)
  h += ".card-container { display: flex; flex-wrap: wrap; gap: 6px; justify-content: center; margin-bottom: 8px; }";
  h += ".card { background: #1e1e1e; padding: 6px; border-radius: 6px; width: 22%; min-width: 70px; text-align: center; box-shadow: 0 2px 4px rgba(0,0,0,0.3); }";
  h += ".card h3 { margin: 0 0 2px 0; color: #bb86fc; font-size: 11px; }";
  h += ".card p { font-size: 13px; font-weight: bold; margin: 0; color: #ffffff; }";
  
  // ==========================================
  // FAHRANSICHT - DEUTLICH VERGRÖßERT (OHNE SCROLLEN)
  // ==========================================
  h += ".drive-cards { display: grid; grid-template-columns: repeat(2, 1fr); gap: 8px; margin-bottom: 10px; }";
  h += ".drive-card { background: #1e1e1e; padding: 10px; border-radius: 8px; text-align: center; border: 1px solid #333; }";
  h += ".drive-card h3 { margin: 0 0 4px 0; color: #bb86fc; font-size: 13px; text-transform: uppercase; }";
  h += ".drive-card p { font-size: 22px; font-weight: bold; margin: 0; color: #ffffff; }";
  
  h += ".section-title { color: #bb86fc; font-size: 13px; margin: 8px 0 6px 0; text-align: center; font-weight: bold; }";
  
  // 10 niedrigste Zellen als größeres Grid (2 Reihen à 5 Zellen)
  h += ".lowest-grid { display: grid; grid-template-columns: repeat(5, 1fr); gap: 6px; max-width: 600px; margin: 0 auto 10px auto; }";
  h += ".low-box { background: #4a1515; border: 2px solid #e74c3c; padding: 8px 4px; border-radius: 6px; text-align: center; }";
  h += ".low-id { color: #f1c40f; font-size: 11px; font-weight: bold; margin-bottom: 2px; }"; // Zellnummer gelb & größer
  h += ".low-val { color: #ffffff; font-size: 15px; font-weight: bold; }"; // Spannungswert groß und weiß

  // SD-Karten Kontroll-Buttons unten in Fahransicht
  h += ".sd-container { display: flex; justify-content: center; gap: 15px; margin-top: 10px; }";
  h += ".sd-btn { padding: 12px 20px; font-size: 14px; font-weight: bold; border: none; border-radius: 6px; cursor: pointer; text-decoration: none; box-shadow: 0 4px 6px rgba(0,0,0,0.3); }";
  h += ".btn-start { background: #27ae60; color: white; }";
  h += ".btn-stop { background: #c0392b; color: white; }";

  // Standard Grid für Voll-Dashboard (196 Zellen)
  h += ".grid-container { display: grid; grid-template-columns: repeat(auto-fill, minmax(65px, 1fr)); gap: 4px; max-width: 900px; margin: 0 auto; }";
  h += ".cell-box { padding: 4px; border-radius: 4px; text-align: center; border: 1px solid #444; }";
  h += ".cell-normal { background: #1b3b2b; color: #2ecc71; }";
  h += ".cell-alert { background: #4a1515; color: #e74c3c; }";
  h += ".cell-id { color: #aaa; font-size: 8px; }";
  h += ".cell-voltage { font-size: 10px; font-weight: bold; }";

  // Formulare (Einstellungen)
  h += "form { max-width: 350px; margin: 0 auto; background: #1e1e1e; padding: 15px; border-radius: 6px; }";
  h += "label { display: block; margin-top: 8px; color: #bb86fc; font-size: 13px; }";
  h += "input { width: 100%; padding: 8px; margin-top: 4px; background: #2c2c2c; border: 1px solid #444; color: #fff; border-radius: 4px; box-sizing: border-box; font-size: 14px; }";
  h += "button[type='submit'] { margin-top: 15px; width: 100%; background: #03dac6; color: #121212; border: none; padding: 10px; font-weight: bold; border-radius: 6px; cursor: pointer; font-size: 14px; }";
  h += "</style></head><body>";
  
  // Navbar generieren
  h += "<div class='navbar'>";
  h += "<a href='/' class='nav-btn " + String(currentPath == "/" ? "active" : "") + "'>Dashboard</a>";
  h += "<a href='/drive' class='nav-btn " + String(currentPath == "/drive" ? "active" : "") + "'>Fahransicht</a>";
  h += "<a href='/settings' class='nav-btn " + String(currentPath == "/settings" ? "active" : "") + "'>Schwellwerte</a>";
  h += "<a href='/can' class='nav-btn " + String(currentPath == "/can" ? "active" : "") + "'>CAN</a>";
  h += "</div>";
  return h;
}



// 1. Hauptseite: Vollständiges Dashboard (196 Zellen)_______________________________________________________________________________________________
String buildDrivePage(bool sd_recording) {
    String html = getHeader("BMS Fahransicht", "/drive");
    html += "<h1>Fahr-Live-Ansicht</h1>";
    html += R"HTML(
  <div class='drive-cards'>
    <div class='drive-card'><h3>SoC</h3><p id='soc'>-</p></div>
    <div class='drive-card'><h3>Akt. Strom</h3><p id='i'>-</p></div>
    <div class='drive-card'><h3>Gesamt</h3><p id='u'>-</p></div>
    <div class='drive-card'><h3>Ø Temperatur</h3><p id='t'>-</p></div>
  </div>
  <div class='section-title'>⚠️ Top 10 Niedrigste Zellspannungen</div>
  <div class='lowest-grid' id='low'></div>
  )HTML";

    html += "<div style='text-align: center; font-size: 12px; margin-bottom: 6px;'>SD-Status: ";
    html += sd_recording ? "<span style='color: #2ecc71; font-weight: bold;'>● AUFNAHME LÄUFT</span>"
                        : "<span style='color: #e74c3c; font-weight: bold;'>Bereit (Gestoppt)</span>";
    html += "</div>";
    html += "<div class='sd-container'>"
            "<a href='/sd_start' class='sd-btn btn-start'>▶ SD Start</a>"
            "<a href='/sd_stop' class='sd-btn btn-stop'>⏹ SD Stopp</a></div>";

    html += R"JS(<script>
  const $=id=>document.getElementById(id);
  async function upd(){
    try{
      const j=await (await fetch('/api/bms')).json();
      $('soc').textContent=j.soc.toFixed(1)+' %';
      $('i').textContent=j.i.toFixed(1)+' A';
      $('u').textContent=j.u.toFixed(1)+' V';
      $('t').textContent=j.tavg.toFixed(1)+' °C';
      const low=j.cells.map((mv,k)=>({n:k+1,mv})).filter(c=>c.mv>0)
        .sort((a,b)=>a.mv-b.mv).slice(0,10);
      $('low').innerHTML=low.map(c=>`<div class="low-box"><div class="low-id">Z. ${c.n}</div><div class="low-val">${(c.mv/1000).toFixed(3)}V</div></div>`).join('');
    }catch(e){}
    finally{ setTimeout(upd,500); }
  }
  upd();
  </script></body></html>)JS";
    return html;
}



// 2. Einstellungsseite_______________________________________________________________________________________________
String buildSettingsPage(float limit_low, float limit_high) {
  String html = getHeader("BMS Einstellungen", "/settings");
  html += "<h1>Schwellwerte</h1>";
  html += "<form action='/settings' method='POST'>";
  html += "<label>Unterer Alarm (Rot in V):</label>";
  html += "<input type='number' step='0.01' name='low' value='" + String(limit_low) + "'>";
  
  html += "<label>Oberer Alarm (Rot in V):</label>";
  html += "<input type='number' step='0.01' name='high' value='" + String(limit_high) + "'>";
  
  html += "<button type='submit'>Speichern</button>";
  html += "</form>";
  html += R"JS(
  <div style='max-width:350px;margin:15px auto;background:#1e1e1e;padding:15px;border-radius:6px;text-align:center'>
    <div style='color:#bb86fc;font-size:13px'>Uhr (RTC)</div>
    <div id='rt' style='font-size:18px;margin:8px 0'>-</div>
    <button id='sync' style='width:100%;background:#03dac6;color:#121212;border:none;padding:10px;font-weight:bold;border-radius:6px;font-size:14px'>Zeit vom Handy übernehmen</button>
  </div>
  <script>
  const $=id=>document.getElementById(id);
  async function show(){
    try{ const j=await (await fetch('/api/time')).json();
      $('rt').textContent=j.t+(j.valid?'':'  ⚠ ungültig'); }catch(e){}
  }
  $('sync').onclick=async()=>{
    const n=new Date();
    const q=`y=${n.getFullYear()}&mo=${n.getMonth()+1}&d=${n.getDate()}&h=${n.getHours()}&mi=${n.getMinutes()}&s=${n.getSeconds()}`;
    await fetch('/settime?'+q); show();
  };
  show(); setInterval(show,1000);
  </script></body></html>)JS";
  
  return html;
}

// 3. SEITE: Fahransicht (Groß, übersichtlich, ohne Scrollen)_______________________________________________________________________________________________
String buildDashboardPage(float limit_low, float limit_high) {
    String html = getHeader("BMS Live Dashboard", "/");
    html += "<h1>196s1p LFP-BMS Live Monitor</h1>";
    html += R"HTML(
  <div id='st' style='text-align:center;font-size:12px;margin-bottom:6px'>verbinde...</div>
  <div class='card-container'>
    <div class='card'><h3>Gesamt</h3><p id='u'>-</p></div>
    <div class='card'><h3>Strom</h3><p id='i'>-</p></div>
    <div class='card'><h3>SoC</h3><p id='soc'>-</p></div>
    <div class='card'><h3>Temp</h3><p id='t'>-</p></div>
  </div>
  <h2 style='font-size: 14px;'>Zellspannungen (1 - 196)</h2>
  <div class='grid-container' id='grid'></div>
  )HTML";

    html += "<script>const LO=" + String(limit_low, 3) + ",HI=" + String(limit_high, 3) + ";";
    html += R"JS(
  const $=id=>document.getElementById(id);
  const g=$('grid');
  for(let n=1;n<=196;n++) g.insertAdjacentHTML('beforeend',
    `<div class="cell-box cell-normal" id="c${n}"><div class="cell-id">Z.${n}</div><div class="cell-voltage">-</div></div>`);
  async function upd(){
    try{
      const j=await (await fetch('/api/bms')).json();
      $('st').textContent=j.online?'BMS online':'Keine BMS-Daten';
      $('u').textContent=j.u.toFixed(1)+' V';
      $('i').textContent=j.i.toFixed(1)+' A';
      $('soc').textContent=j.soc.toFixed(1)+' %';
      $('t').textContent=j.tavg.toFixed(1)+' °C';
      j.cells.forEach((mv,k)=>{
        const e=$('c'+(k+1)); const v=mv/1000;
        e.lastChild.textContent=mv?v.toFixed(3)+'V':'-';
        e.className='cell-box '+((mv&&(v<=LO||v>=HI))?'cell-alert':'cell-normal');
      });
    }catch(e){ $('st').textContent='Keine Verbindung'; }
    finally{ setTimeout(upd,1000); }
  }
  upd();
  </script></body></html>)JS";
    return html;
}


// 4. Seite mit Can Infos_______________________________________________________________________________________________
String buildCanPage() {
  String html = getHeader("CAN Monitor", "/can");
  html += "<h1>CAN-Monitor</h1>";
  html += "<div id='info' style='text-align:center;font-size:12px;margin-bottom:8px'>...</div>";
  html += "<table><thead><tr><th>ID</th><th>DLC</th><th>Daten</th><th>Anzahl</th><th>Alter ms</th></tr></thead><tbody id='rows'></tbody></table>";
  html += "<script>"
          "async function upd(){try{"
          "const j=await (await fetch('/api/can')).json();"
          "document.getElementById('info').textContent='Frames: '+j.total+' | verpasst: '+j.missed+' | Busfehler: '+j.buserr;"
          "j.frames.sort((a,b)=>a.id.localeCompare(b.id));"
          "document.getElementById('rows').innerHTML=j.frames.map(f=>"
          "'<tr><td>'+f.id+'</td><td>'+f.len+'</td><td class=d>'+f.data+'</td><td>'+f.n+'</td><td>'+f.age+'</td></tr>').join('');"
          "}catch(e){}finally{setTimeout(upd,500);}}"
          "upd();</script>";
  html += "</body></html>";
  return html;
}

#endif