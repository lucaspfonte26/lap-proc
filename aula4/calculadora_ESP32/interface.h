#include <pgmspace.h>

const char HTML_INTERFACE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Calculadora Binária ESP32 - Poli</title>
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
        .input-group { margin-bottom: 25px; }
        input { 
            padding: 15px; 
            font-size: 20px; 
            margin: 10px; 
            width: 180px; 
            text-align: center;
            font-family: monospace;
            border: 2px solid #ccc;
            border-radius: 8px;
            transition: border-color 0.3s, box-shadow 0.3s;
        }
        input:focus { 
            border-color: var(--poli-blue); 
            box-shadow: 0 0 8px rgba(0, 58, 112, 0.3);
            outline: none; 
        }
        .btn-group { display: flex; flex-wrap: wrap; justify-content: center; gap: 15px; margin-bottom: 25px; }
        button { 
            flex: 1 1 120px;
            padding: 15px; 
            font-size: 16px; 
            font-weight: bold;
            cursor: pointer; 
            background-color: var(--poli-blue); 
            color: white; 
            border: 2px solid var(--poli-blue); 
            border-radius: 8px;
            transition: all 0.3s; 
        }
        button:hover:not(:disabled) { 
            background-color: var(--poli-yellow); 
            color: var(--poli-blue); 
            border-color: var(--poli-yellow);
        }
        button:disabled {
            background-color: #6c757d;
            border-color: #6c757d;
            cursor: not-allowed;
            opacity: 0.7;
        }
        
        .feedback-box {
            padding: 20px; 
            border-radius: 8px; 
            margin-top: 20px; 
            font-size: 18px;
            display: none;
            animation: fadeIn 0.3s ease-in-out;
            position: relative; /* Para posicionar o tempo de execução */
        }
        .success-box { background-color: #e8f5e9; border: 2px solid var(--success); color: #1b5e20; }
        .alert-box { background-color: #ffebee; border: 2px solid var(--danger); color: #b71c1c; font-weight: bold;}
        
        .result-highlight { font-family: monospace; font-size: 24px; font-weight: bold; color: var(--poli-blue); }
        
        .execution-time {
            display: block;
            margin-top: 10px;
            font-size: 14px;
            color: var(--time-color);
            font-style: italic;
        }

        /* Estilos do Histórico */
        .history-box {
            margin-top: 25px;
            padding: 15px;
            background-color: #f8f9fa;
            border-radius: 8px;
            border: 1px solid #ddd;
            text-align: left;
            display: none;
            animation: fadeIn 0.3s ease-in-out;
        }
        .history-box h3 {
            margin-top: 0;
            font-size: 16px;
            color: var(--poli-blue);
            border-bottom: 1px solid #ccc;
            padding-bottom: 8px;
            margin-bottom: 10px;
        }
        .history-list {
            list-style-type: none;
            padding: 0;
            margin: 0;
        }
        .history-list li {
            padding: 8px 0;
            border-bottom: 1px dashed #ccc;
            font-family: monospace;
            font-size: 16px;
            color: var(--text-main);
            display: flex;
            justify-content: space-between;
        }
        .history-list li:last-child {
            border-bottom: none;
        }
        .history-time {
            font-size: 12px;
            color: var(--time-color);
            align-self: flex-end;
        }


        @keyframes fadeIn {
            from { opacity: 0; transform: translateY(-5px); }
            to { opacity: 1; transform: translateY(0); }
        }
    </style>
</head>
<body>
    <div class="container">
        <h2>Calculadora Dinâmica</h2>
        
        <div class="input-group">
            <input type="text" id="opA" maxlength="16" placeholder="Op A (Binário)" autocomplete="off">
            <input type="text" id="opB" maxlength="16" placeholder="Op B (Binário)" autocomplete="off">
        </div>
        
        <div class="btn-group">
            <button id="btnSoma" onclick="calcular('add', this)">SOMAR</button>
            <button id="btnSub" onclick="calcular('sub', this)">SUBTRAIR</button>
            <button id="btnMul" onclick="calcular('mul', this)">MULTIPLICAR</button>
            <button id="btnFat" onclick="calcular('fat', this)">FATORIAL (!)</button>
            <button id="btnDiv" onclick="calcular('div', this)">DIVIDIR</button>
        </div>
        
        <div id="resultadoBox" class="feedback-box success-box">
            Resultado (Dec): <span id="resDec" class="result-highlight"></span> <br>
            Resultado (Bin): <span id="resBin" class="result-highlight"></span>
            <span class="execution-time">Tempo de processamento: <span id="resTempo"></span> μs</span>
        </div>
        
        <div id="alertaOverflow" class="feedback-box alert-box">
            ⚠️ OVERFLOW DETECTADO!
            <span class="execution-time" style="color:#b71c1c;">Tempo de processamento: <span id="resTempoOverflow"></span> μs</span>
        </div>

        <div id="historyBox" class="history-box">
            <h3>Histórico de Cálculos</h3>
            <ul id="historyList" class="history-list"></ul>
        </div>
    </div>


    <script>
        // --- 1. Filtro em Tempo Real ---
        function aplicarFiltroBinario(event) {
            this.value = this.value.replace(/[^01]/g, '');
        }
        document.getElementById('opA').addEventListener('input', aplicarFiltroBinario);
        document.getElementById('opB').addEventListener('input', aplicarFiltroBinario);

        // --- Variável global do Histórico ---
        let historicoCalculos = [];

        // --- 2. Função principal de cálculo ---
        function calcular(operacao, botao) {
            const opA = document.getElementById('opA');
            const opB = document.getElementById('opB');
            const a = opA.value;
            const b = opB.value;

            const unaria = (operacao === 'fat'); 
            
            // Validação dinâmica do tamanho de bits (garantindo pelo menos 2 bits e máximo 16)
            if (a.length < 2 || a.length > 16 || (!unaria && a.length !== b.length)) {
                alert("Preencha comprimentos válidos (ex: 4 bits, 8 bits). Ambos os operandos devem ter o mesmo tamanho.");
                return;
            }

            const botoes = document.querySelectorAll('.btn-group button');
            botoes.forEach(btn => btn.disabled = true);
            const txtOrig = botao.innerText;
            botao.innerText = 'Calculando...';

            const restaurar = () => {
                botoes.forEach(btn => btn.disabled = false);
                botao.innerText = txtOrig;
            };

            fetch(`/calc?a=${a}&b=${b}&op=${operacao}`)
                .then(response => response.json())
                .then(data => {
                    restaurar();

                    if (data.erro) {
                        alert("Erro no Microcontrolador: " + data.erro);
                        return;
                    }

                    const divAlerta = document.getElementById('alertaOverflow');
                    const divRes = document.getElementById('resultadoBox');

                    if (data.overflow) {
                        divAlerta.style.display = 'block';
                        divRes.style.display = 'none';
                        document.getElementById('resTempoOverflow').innerText = data.tempo_us;
                    } else {
                        document.getElementById('resDec').innerText = data.decimal;
                        document.getElementById('resBin').innerText = data.binario;
                        document.getElementById('resTempo').innerText = data.tempo_us;
                        divRes.style.display = 'block';
                        divAlerta.style.display = 'none';
                    }

                    adicionarAoHistorico(a, b, operacao, data);

                    // Deixa os valores nos inputs para facilitar re-testes durante coleta de tempos experimentais
                    // opA.value = '';
                    // opB.value = '';
                    opA.focus();
                })
                .catch(error => {
                    restaurar();
                    console.error('Erro de comunicação com o ESP32:', error);
                    alert("Erro ao conectar com o ESP32. Verifique a rede Wi-Fi.");
                });
        }

        // Função auxiliar para montar a lista do histórico
        function adicionarAoHistorico(a, b, operacao, data) {
            let expressao = '';
            
            // Tratamento visual específico implementado pelo Aluno A
            if (operacao === 'add') expressao = `${a} + ${b}`;
            else if (operacao === 'sub') expressao = `${a} - ${b}`;
            else if (operacao === 'mul') expressao = `${a} × ${b}`;
            else if (operacao === 'div') expressao = `${a} ÷ ${b}`;
            else if (operacao === 'fat') expressao = `${a}!`; // Oculta o operando B
            
            let resultadoVisual;

            if (data.overflow) {
                resultadoVisual = '<span style="color: var(--danger);">⚠️ OVERFLOW</span>';
            } else {
                resultadoVisual = `<strong>${data.binario}</strong> (Dec: ${data.decimal})`;
            }

            // Constrói a linha flexível com a expressão à esquerda e o tempo (em us) à direita
            const itemHTML = `<li>
                                <span>${expressao} = ${resultadoVisual}</span>
                                <span class="history-time">[${data.tempo_us} μs]</span>
                              </li>`;
            
            historicoCalculos.unshift(itemHTML);
            
            if (historicoCalculos.length > 5) {
                historicoCalculos.pop();
            }

            document.getElementById('historyBox').style.display = 'block';
            document.getElementById('historyList').innerHTML = historicoCalculos.join('');
        }
    </script>
</body>
</html>
)rawliteral";