#ifndef WEBSERVER_CORE_H
#define WEBSERVER_CORE_H

#include <pgmspace.h>

const char HTML_INTERFACE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Sinalização e Monitoramento - Poli</title>
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
        
        /* Bloco de destaque para o sensor analógico */
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

        /* Caixas de Feedback herdadas da estrutura original */
        .feedback-box {
            padding: 20px; 
            border-radius: 8px; 
            margin-top: 20px; 
            font-size: 18px;
            animation: fadeIn 0.3s ease-in-out;
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
            display: none; /* Controlado dinamicamente pelo JS */
            animation: pisca 1s infinite alternate, fadeIn 0.3s ease-in-out;
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
        <h2>Painel de Telemetria</h2>
        <p style="color: var(--time-color); font-style: italic; margin-top: -5px; font-size: 14px;">
            PCS3732 Laboratório de Processadores | Escola Politécnica
        </p>
        
        <div class="sensor-box">
            <div class="sensor-titulo">Sensor de Luminosidade (LDR)</div>
            <span id="ldrValor" class="result-highlight">0000</span>
            <span style="font-size: 14px; color: var(--time-color); display: block; margin-top: 5px;">Resolução ADC: 12-bit (0 a 4095)</span>
        </div>
        
        <div id="boxNormal" class="feedback-box success-box">
            ✔️ Sistema Operando Regularmente
        </div>
        
        <div id="boxSOS" class="feedback-box alert-box">
            ⚠️ ALERTA: BOTÃO SOS ATIVADO!
            <span class="execution-time" style="color:#b71c1c; font-weight: bold;">
                [Interrupção Externa - Prioridade Máxima]
            </span>
        </div>

        <span class="execution-time">Frequência de atualização: 1 Hz (1s)</span>
    </div>

    <script>
        // Função que faz o pedido em segundo plano para o ESP32
        function buscarTelemetria() {
            fetch('/dados')
                .then(response => response.json())
                .then(data => {
                    // 1. Atualiza o valor numérico do LDR na tela
                    document.getElementById('ldrValor').innerText = data.ldr;
                    
                    const boxNormal = document.getElementById('boxNormal');
                    const boxSOS = document.getElementById('boxSOS');

                    // 2. Altera a Interface dinamicamente baseado no estado do Botão SOS
                    if (data.sos) {
                        boxSOS.style.display = 'block';
                        boxNormal.style.display = 'none';
                        document.body.style.backgroundColor = '#f8d7da'; // Deixa o fundo levemente avermelhado em SOS
                    } else {
                        boxSOS.style.display = 'none';
                        boxNormal.style.display = 'block';
                        document.body.style.backgroundColor = 'var(--bg-color)'; // Volta pro cinza padrão
                    }
                })
                .catch(error => {
                    console.error('Erro de comunicação com o ESP32:', error);
                });
        }

        // Configura o cronômetro para rodar a função a cada 1000 milissegundos (1Hz fixo)
        setInterval(buscarTelemetria, 1000);
        
        // Executa uma primeira vez assim que a página abre para não esperar 1 segundo em branco
        buscarTelemetria();
    </script>
</body>
</html>
)rawliteral";

#endif