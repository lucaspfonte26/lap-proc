#include <pgmspace.h>

const char HTML_INTERFACE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Controle de LED por PWM - ESP32</title>
    <style>
        :root {
            --bg-color: #eef2f5;
            --card-bg: #ffffff;
            --poli-blue: #003A70;
            --poli-yellow: #FFB81C;
            --text-main: #333333;
            --muted: #6c757d;
        }
        * { box-sizing: border-box; }
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            text-align: center; margin: 0; padding: 40px 16px;
            background-color: var(--bg-color); color: var(--text-main);
        }
        .container {
            background: var(--card-bg); padding: 32px 28px; border-radius: 16px;
            display: inline-block; box-shadow: 0 10px 25px rgba(0,58,112,0.15);
            max-width: 460px; width: 100%;
        }
        h2 { margin: 0 0 4px; color: var(--poli-blue); font-size: 24px; }
        .subtitle { color: var(--muted); font-size: 13px; margin-bottom: 28px; }

        .label-row {
            display: flex; justify-content: space-between; align-items: baseline;
            margin-bottom: 10px; font-size: 15px;
        }
        .valor { font-family: monospace; font-size: 26px; font-weight: bold; color: var(--poli-blue); }

        input[type=range] {
            -webkit-appearance: none; width: 100%; height: 14px; border-radius: 8px;
            background: linear-gradient(90deg, var(--poli-blue), var(--poli-yellow));
            outline: none; margin: 6px 0 4px;
        }
        input[type=range]::-webkit-slider-thumb {
            -webkit-appearance: none; width: 34px; height: 34px; border-radius: 50%;
            background: var(--poli-blue); border: 3px solid #fff;
            box-shadow: 0 2px 6px rgba(0,0,0,0.3); cursor: pointer;
        }
        input[type=range]::-moz-range-thumb {
            width: 34px; height: 34px; border-radius: 50%; background: var(--poli-blue);
            border: 3px solid #fff; cursor: pointer;
        }

        .section { margin-top: 30px; text-align: left; }
        .section > span { font-size: 15px; }
        .freq-grid { display: flex; flex-wrap: wrap; gap: 10px; margin-top: 12px; }
        .freq-btn {
            flex: 1 1 70px; padding: 12px 8px; font-size: 15px; font-weight: bold;
            cursor: pointer; background: #fff; color: var(--poli-blue);
            border: 2px solid var(--poli-blue); border-radius: 10px; transition: all .2s;
        }
        .freq-btn:hover { background: var(--poli-yellow); border-color: var(--poli-yellow); }
        .freq-btn.active { background: var(--poli-blue); color: #fff; }

        .custom-row { display: flex; gap: 10px; margin-top: 12px; }
        .custom-row input {
            flex: 1; padding: 12px; font-size: 15px; text-align: center;
            border: 2px solid #ccc; border-radius: 10px; font-family: monospace;
        }
        .custom-row button {
            padding: 12px 16px; font-weight: bold; cursor: pointer; border-radius: 10px;
            background: var(--poli-blue); color: #fff; border: 2px solid var(--poli-blue);
        }
        .hint {
            margin-top: 18px; font-size: 13px; color: var(--muted); font-style: italic;
            border-top: 1px dashed #ccc; padding-top: 14px;
        }
        .hint b { color: var(--poli-blue); }
    </style>
</head>
<body>
    <div class="container">
        <h2>Controle de Intensidade do LED</h2>
        <div class="subtitle">PWM por hardware (LEDC) &middot; ESP32-C3 &middot; GPIO3</div>

        <div class="label-row">
            <span>Brilho</span>
            <span class="valor"><span id="valBrilho">0</span>%</span>
        </div>
        <input type="range" id="brilho" min="0" max="100" value="0">
        <div class="label-row" style="margin-top:2px; color:var(--muted); font-size:12px;">
            <span>Min</span><span>Max</span>
        </div>

        <div class="section">
            <span>Frequência do PWM: <b id="freqAtual" style="color:var(--poli-blue)">5 kHz</b></span>
            <div class="freq-grid">
                <button class="freq-btn" onclick="setFreq(60,this)">60 Hz</button>
                <button class="freq-btn" onclick="setFreq(100,this)">100 Hz</button>
                <button class="freq-btn" onclick="setFreq(200,this)">200 Hz</button>
                <button class="freq-btn" onclick="setFreq(1000,this)">1 kHz</button>
                <button class="freq-btn active" onclick="setFreq(5000,this)">5 kHz</button>
            </div>
            <div class="custom-row">
                <input type="number" id="freqCustom" min="10" max="8000" placeholder="custom (Hz)">
                <button onclick="setFreqCustom()">OK</button>
            </div>
        </div>

        <div class="hint">
            Abaixe a frequência (ex.: <b>60-100 Hz</b>) para enxergar a
            <b>cintilação (flicker)</b>; em <b>kHz</b> o olho vê brilho contínuo.
        </div>
    </div>

    <script>
        const slider   = document.getElementById('brilho');
        const valLabel = document.getElementById('valBrilho');

        // Evita acumular requisicoes durante o arrasto do slider
        let inflight = false, pendente = null;
        function enviarBrilho(v) {
            if (inflight) { pendente = v; return; }
            inflight = true;
            fetch('/led?val=' + v).finally(() => {
                inflight = false;
                if (pendente !== null) { const p = pendente; pendente = null; enviarBrilho(p); }
            });
        }
        slider.addEventListener('input', () => {
            valLabel.innerText = slider.value;
            enviarBrilho(slider.value);
        });

        function rotuloFreq(hz) { return hz >= 1000 ? (hz / 1000) + ' kHz' : hz + ' Hz'; }

        function setFreq(hz, btn) {
            fetch('/freq?hz=' + hz);
            document.querySelectorAll('.freq-btn').forEach(b => b.classList.remove('active'));
            if (btn) btn.classList.add('active');
            document.getElementById('freqAtual').innerText = rotuloFreq(hz);
        }

        function setFreqCustom() {
            let hz = parseInt(document.getElementById('freqCustom').value);
            if (isNaN(hz) || hz < 10 || hz > 8000) { alert('Frequência válida: 10 a 8000 Hz.'); return; }
            setFreq(hz, null);
            document.querySelectorAll('.freq-btn').forEach(b => b.classList.remove('active'));
        }
    </script>
</body>
</html>
)rawliteral";
