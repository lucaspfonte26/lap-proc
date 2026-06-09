#include <pgmspace.h>

const char HTML_INTERFACE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Painel de Controle ESP32 - LED + Servo</title>
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
            text-align: center; margin: 0; padding: 32px 16px;
            background-color: var(--bg-color); color: var(--text-main);
        }
        .wrap { max-width: 460px; margin: 0 auto; }
        h1 { color: var(--poli-blue); font-size: 26px; margin: 0 0 4px; }
        .page-sub { color: var(--muted); font-size: 13px; margin-bottom: 24px; }

        .card {
            background: var(--card-bg); padding: 26px 24px; border-radius: 16px;
            box-shadow: 0 10px 25px rgba(0,58,112,0.12); margin-bottom: 22px;
        }
        .card-title {
            font-size: 18px; font-weight: bold; color: var(--poli-blue);
            text-align: left; margin-bottom: 4px;
        }
        .card-sub { text-align: left; color: var(--muted); font-size: 12px; margin-bottom: 18px; }

        .label-row {
            display: flex; justify-content: space-between; align-items: baseline;
            margin-bottom: 8px; font-size: 15px;
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
        .range-ends { display: flex; justify-content: space-between; color: var(--muted); font-size: 12px; }

        /* LED: seletor de frequencia */
        .freq-grid { display: flex; flex-wrap: wrap; gap: 8px; margin-top: 14px; }
        .freq-btn {
            flex: 1 1 64px; padding: 11px 6px; font-size: 14px; font-weight: bold;
            cursor: pointer; background: #fff; color: var(--poli-blue);
            border: 2px solid var(--poli-blue); border-radius: 9px; transition: all .2s;
        }
        .freq-btn:hover { background: var(--poli-yellow); border-color: var(--poli-yellow); }
        .freq-btn.active { background: var(--poli-blue); color: #fff; }
        .custom-row { display: flex; gap: 8px; margin-top: 10px; }
        .custom-row input {
            flex: 1; padding: 11px; font-size: 14px; text-align: center;
            border: 2px solid #ccc; border-radius: 9px; font-family: monospace;
        }
        .custom-row button {
            padding: 11px 14px; font-weight: bold; cursor: pointer; border-radius: 9px;
            background: var(--poli-blue); color: #fff; border: 2px solid var(--poli-blue);
        }

        /* Servo: mostrador */
        .dial {
            position: relative; width: 200px; height: 100px; margin: 4px auto 0;
            border-radius: 100px 100px 0 0;
            background: #f4f7f9; border: 2px solid #d4dce3; border-bottom: none;
        }
        .needle {
            position: absolute; left: 50%; bottom: 0; width: 4px; height: 88px;
            background: var(--poli-blue); border-radius: 4px;
            transform-origin: bottom center; transform: translateX(-50%) rotate(0deg);
            transition: transform .15s ease-out;
        }
        .pivot {
            position: absolute; left: 50%; bottom: -8px; width: 16px; height: 16px;
            background: var(--poli-blue); border-radius: 50%; transform: translateX(-50%);
        }
        .dial-labels {
            display: flex; justify-content: space-between; width: 200px;
            margin: 6px auto 14px; font-size: 12px; color: var(--muted);
        }
        .presets { display: flex; gap: 10px; margin-top: 16px; }
        .preset-btn {
            flex: 1; padding: 13px 8px; font-size: 15px; font-weight: bold;
            cursor: pointer; background: #fff; color: var(--poli-blue);
            border: 2px solid var(--poli-blue); border-radius: 10px; transition: all .2s;
        }
        .preset-btn:hover { background: var(--poli-yellow); border-color: var(--poli-yellow); }

        .hint {
            margin-top: 16px; font-size: 12px; color: var(--muted); font-style: italic;
            border-top: 1px dashed #ccc; padding-top: 12px; text-align: left;
        }
        .hint b { color: var(--poli-blue); }
    </style>
</head>
<body>
    <div class="wrap">
        <h1>Painel de Controle ESP32</h1>
        <div class="page-sub">LED + Servo &middot; PWM por hardware (LEDC) &middot; ESP32-C3</div>

        <!-- ============ LED ============ -->
        <div class="card">
            <div class="card-title">Intensidade do LED</div>
            <div class="card-sub">GPIO3 &middot; alta frequência (kHz)</div>

            <div class="label-row">
                <span>Brilho</span>
                <span class="valor"><span id="valBrilho">0</span>%</span>
            </div>
            <input type="range" id="brilho" min="0" max="100" value="0">
            <div class="range-ends"><span>Min</span><span>Max</span></div>

            <div style="text-align:left; margin-top:16px; font-size:14px;">
                Frequência: <b id="freqAtual" style="color:var(--poli-blue)">5 kHz</b>
            </div>
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

            <div class="hint">
                Abaixe a frequência (ex.: <b>60-100 Hz</b>) para ver a
                <b>cintilação</b>; em <b>kHz</b> o brilho parece contínuo.
            </div>
        </div>

        <!-- ============ SERVO ============ -->
        <div class="card">
            <div class="card-title">Posição do Servo</div>
            <div class="card-sub">GPIO4 &middot; 50 Hz (pulso 1,0-2,0 ms)</div>

            <div class="dial">
                <div class="needle" id="needle"></div>
                <div class="pivot"></div>
            </div>
            <div class="dial-labels"><span>0°</span><span>90°</span><span>180°</span></div>

            <div class="label-row">
                <span>Ângulo</span>
                <span class="valor"><span id="valAng">90</span>°</span>
            </div>
            <input type="range" id="angulo" min="0" max="180" value="90">
            <div class="range-ends"><span>0°</span><span>180°</span></div>

            <div class="presets">
                <button class="preset-btn" onclick="setAng(0)">0°</button>
                <button class="preset-btn" onclick="setAng(90)">90°</button>
                <button class="preset-btn" onclick="setAng(180)">180°</button>
            </div>

            <div class="hint">
                Teste os <b>extremos (0° e 180°)</b> e confira se o servo atinge a
                posição sem engasgar.
            </div>
        </div>
    </div>

    <script>
        /* -------- LED -------- */
        const slBrilho = document.getElementById('brilho');
        const lblBrilho = document.getElementById('valBrilho');
        let inflightLed = false, pendenteLed = null;

        function enviarBrilho(v) {
            lblBrilho.innerText = v;
            if (inflightLed) { pendenteLed = v; return; }
            inflightLed = true;
            fetch('/led?val=' + v).finally(() => {
                inflightLed = false;
                if (pendenteLed !== null) { const p = pendenteLed; pendenteLed = null; enviarBrilho(p); }
            });
        }
        slBrilho.addEventListener('input', () => enviarBrilho(slBrilho.value));

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

        /* -------- SERVO -------- */
        const slAng = document.getElementById('angulo');
        const lblAng = document.getElementById('valAng');
        const needle = document.getElementById('needle');
        let inflightServo = false, pendenteServo = null;

        function atualizarVisual(ang) {
            lblAng.innerText = ang;
            needle.style.transform = 'translateX(-50%) rotate(' + (Number(ang) - 90) + 'deg)';
        }
        function enviarServo(ang) {
            atualizarVisual(ang);
            if (inflightServo) { pendenteServo = ang; return; }
            inflightServo = true;
            fetch('/servo?ang=' + ang).finally(() => {
                inflightServo = false;
                if (pendenteServo !== null) { const p = pendenteServo; pendenteServo = null; enviarServo(p); }
            });
        }
        slAng.addEventListener('input', () => enviarServo(slAng.value));
        function setAng(a) { slAng.value = a; enviarServo(a); }

        atualizarVisual(slAng.value);   // estado inicial (90°)
    </script>
</body>
</html>
)rawliteral";
