#ifndef WEBSERVER_CORE_H
#define WEBSERVER_CORE_H

#include <pgmspace.h>

const char HTML_INTERFACE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Semáforo Inteligente - Poli</title>
    <style>
        :root {
            --bg-color: #eef2f5;
            --card-bg: #ffffff;
            --poli-blue: #003A70;
            --poli-yellow: #FFB81C;
            --danger: #dc3545;
            --success: #28a745;
            --text-main: #333333;
            --time-color: #6c757d;
        }
        body { 
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; 
            text-align: center; 
            margin: 0; 
            padding: 50px 20px; 
            background-color: var(--bg-color); 
            color: var(--text-main);
            transition: background-color 0.5s ease;
        }
        .container { 
            background: var(--card-bg); 
            padding: 40px; 
            border-radius: 16px; 
            display: inline-block; 
            box-shadow: 0px 10px 25px rgba(0, 58, 112, 0.15); 
            max-width: 500px; 
            width: 100%;
        }
        h2 { 
            margin-top: 0; 
            color: var(--poli-blue); 
            font-size: 28px; 
        }
        
        .sensor-box {
            background-color: #f8f9fa;
            border: 2px solid #e2e8f0;
            border-radius: 12px;
            padding: 20px;
            margin: 20px 0;
        }
        .sensor-titulo {
            font-size: 14px;
            color: var(--time-color);
            text-transform: uppercase;
            letter-spacing: 1px;
            margin-bottom: 5px;
        }
        .result-highlight { 
            font-family: monospace; 
            font-size: 36px; 
            font-weight: bold; 
            color: var(--poli-blue); 
        }

        .feedback-box {
            padding: 20px; 
            border-radius: 8px; 
            margin-top: 20px; 
            font-size: 18px;
            animation: fadeIn 0.3s ease-in-out;
            display: none; /* Controlado via JS */
        }
        .success-box { 
            background-color: #e8f5e9; 
            border: 2px solid var(--success); 
            color: #1b5e20; 
        }
        .alert-box { 
            background-color: #ffebee; 
            border: 2px solid var(--danger); 
            color: #b71c1c; 
            font-weight: bold;
            animation: pisca 1s infinite alternate, fadeIn 0.3s ease-in-out;
        }
        .warning-box {
            background-color: #fff3cd;
            border: 2px solid var(--poli-yellow);
            color: #856404;
            font-weight: bold;
        }
        
        .execution-time {
            display: block;
            margin-top: 10px;
            font-size: 13px;
            color: var(--time-color);
            font-style: italic;
        }

        @keyframes pisca {
            from { filter: drop-shadow(0 0 2px rgba(220,53,69,0.3)); }
            to { filter: drop-shadow(0 0 12px rgba(220,53,69,0.8)); background-color: #ffcdd2; }
        }
        @keyframes fadeIn {
            from { opacity: 0; transform: translateY(-5px); }
            to { opacity: 1; transform: translateY(0); }
        }
    </style>
</head>
<body>
    <div class="container">
        <h2>Controle Viário Inteligente</h2>
        <p style="color: var(--time-color); font-style: italic; margin-top: -5px; font-size: 14px;">
            PCS3732 Laboratório de Processadores | Escola Politécnica
        </p>
        
        <div class="sensor-box">
            <div class="sensor-titulo">Sensor de Luminosidade (LDR)</div>
            <span id="ldrValor" class="result-highlight">0000</span>
            <span style="font-size: 14px; color: var(--time-color); display: block; margin-top: 5px;">Resolução ADC: 12-bit (0 a 4095)</span>
        </div>
        
        <div id="boxDiaNormal" class="feedback-box success-box">
            ☀️ MODO DIURNO: Tráfego de Veículos Livre
        </div>
        
        <div id="boxDiaPedestre" class="feedback-box alert-box">
            🚶‍♂️ TRAVESSIA SOLICITADA!
            <span class="execution-time" style="color:#b71c1c; font-weight: bold;">
                [Interrupção Ativa - Transição em Andamento]
            </span>
        </div>
        
        <div id="boxNoite" class="feedback-box warning-box">
            🌙 MODO NOTURNO: Alerta Piscante Ativo (1 Hz)
            <span class="execution-time" style="color:#856404;">
                Interrupção de Pedestres Desativada por Segurança
            </span>
        </div>

        <span class="execution-time">Frequência de amostragem: 1 Hz (1s)</span>
    </div>

    <script>
        function buscarTelemetria() {
            fetch('/dados')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('ldrValor').innerText = data.ldr;
                    
                    const boxDiaNormal = document.getElementById('boxDiaNormal');
                    const boxDiaPedestre = document.getElementById('boxDiaPedestre');
                    const boxNoite = document.getElementById('boxNoite');

                    if (data.noite) {
                        // Se for noite: Mostra o painel noturno e oculta os de pedestre
                        boxNoite.style.display = 'block';
                        boxDiaNormal.style.display = 'none';
                        boxDiaPedestre.style.display = 'none';
                        document.body.style.backgroundColor = '#fcf8e3'; // Fundo levemente amarelado (atenção)
                    } else {
                        // Se for dia: Oculta o painel noturno e avalia o botão de pedestre
                        boxNoite.style.display = 'none';
                        
                        if (data.pedestre) {
                            boxDiaPedestre.style.display = 'block';
                            boxDiaNormal.style.display = 'none';
                            document.body.style.backgroundColor = '#f8d7da'; // Fundo avermelhado (parada)
                        } else {
                            boxDiaPedestre.style.display = 'none';
                            boxDiaNormal.style.display = 'block';
                            document.body.style.backgroundColor = 'var(--bg-color)'; // Fundo cinza padrão
                        }
                    }
                })
                .catch(error => {
                    console.error('Erro ao ler dados do ESP32:', error);
                });
        }

        setInterval(buscarTelemetria, 1000);
        buscarTelemetria();
    </script>
</body>
</html>
)rawliteral";

#endif