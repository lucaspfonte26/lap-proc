#include <pgmspace.h>

const char HTML_INTERFACE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Controle de Servo por PWM - ESP32</title>
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
        .subtitle { color: var(--muted); font-size: 13px; margin-bottom: 24px; }

        /* Mostrador (gauge) com ponteiro */
        .dial {
            position: relative; width: 220px; height: 110px; margin: 8px auto 0;
            border-radius: 110px 110px 0 0;
            background: #f4f7f9; border: 2px solid #d4dce3; border-bottom: none;
        }
        .needle {
            position: absolute; left: 50%; bottom: 0; width: 4px; height: 96px;
            background: var(--poli-blue); border-radius: 4px;
            transform-origin: bottom center; transform: translateX(-50%) rotate(0deg);
            transition: transform .15s ease-out;
        }
        .pivot {
            position: absolute; left: 50%; bottom: -9px; width: 18px; height: 18px;
            background: var(--poli-blue); border-radius: 50%; transform: translateX(-50%);
        }
        .dial-labels {
            display: flex; justify-content: space-between; width: 220px;
            margin: 6px auto 0; font-size: 12px; color: var(--muted);
        }

        .valor { font-family: monospace; font-size: 40px; font-weight: bold; color: var(--poli-blue); margin: 14px 0 2px; }
        .valor small { font-size: 20px; }

        input[type=range] {
            -webkit-appearance: none; width: 100%; height: 14px; border-radius: 8px;
            background: linear-gradient(90deg, var(--poli-blue), var(--poli-yellow));
            outline: none; margin: 8px 0 4px;
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

        .presets { display: flex; gap: 10px; margin-top: 22px; }
        .preset-btn {
            flex: 1; padding: 14px 8px; font-size: 16px; font-weight: bold;
            cursor: pointer; background: #fff; color: var(--poli-blue);
            border: 2px solid var(--poli-blue); border-radius: 10px; transition: all .2s;
        }
        .preset-btn:hover { background: var(--poli-yellow); border-color: var(--poli-yellow); }

        .hint {
            margin-top: 20px; font-size: 13px; color: var(--muted); font-style: italic;
            border-top: 1px dashed #ccc; padding-top: 14px;
        }
        .hint b { color: var(--poli-blue); }
    </style>
</head>
<body>
    <div class="container">
        <h2>Controle de Posição do Servo</h2>
        <div class="subtitle">PWM por hardware (LEDC) &middot; 50 Hz &middot; ESP32-C3 &middot; GPIO4</div>

        <div class="dial">
            <div class="needle" id="needle"></div>
            <div class="pivot"></div>
        </div>
        <div class="dial-labels"><span>0°</span><span>90°</span><span>180°</span></div>

        <div class="valor"><span id="valAng">90</span><small>°</small></div>

        <input type="range" id="angulo" min="0" max="180" value="90">
        <div class="range-ends"><span>0°</span><span>180°</span></div>

        <div class="presets">
            <button class="preset-btn" onclick="setAng(0)">0°</button>
            <button class="preset-btn" onclick="setAng(90)">90°</button>
            <button class="preset-btn" onclick="setAng(180)">180°</button>
        </div>

        <div class="hint">
            Pulso de <b>1,0 ms</b> → 0°, <b>1,5 ms</b> → 90°, <b>2,0 ms</b> → 180°.
            Teste os <b>extremos (0° e 180°)</b> e confira se o servo atinge a
            posição sem engasgar.
        </div>
    </div>

    <script>
        const slider   = document.getElementById('angulo');
        const valLabel = document.getElementById('valAng');
        const needle   = document.getElementById('needle');

        function atualizarVisual(ang) {
            valLabel.innerText = ang;
            needle.style.transform = 'translateX(-50%) rotate(' + (Number(ang) - 90) + 'deg)';
        }

        // Evita acumular requisicoes durante o arrasto do slider
        let inflight = false, pendente = null;
        function enviar(ang) {
            atualizarVisual(ang);
            if (inflight) { pendente = ang; return; }
            inflight = true;
            fetch('/servo?ang=' + ang).finally(() => {
                inflight = false;
                if (pendente !== null) { const p = pendente; pendente = null; enviar(p); }
            });
        }

        slider.addEventListener('input', () => enviar(slider.value));

        function setAng(a) { slider.value = a; enviar(a); }

        atualizarVisual(slider.value);   // estado inicial (90°)
    </script>
</body>
</html>
)rawliteral";
