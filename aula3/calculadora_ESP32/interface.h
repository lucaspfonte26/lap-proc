#include <pgmspace.h>

const char HTML_INTERFACE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Calculadora Binária ESP32</title>
    <style>
        body { font-family: Arial, sans-serif; text-align: center; margin-top: 50px; background-color: #f4f4f9; }
        .container { background: white; padding: 20px; border-radius: 10px; display: inline-block; box-shadow: 0px 0px 10px rgba(0,0,0,0.1); }
        input { padding: 10px; font-size: 16px; margin: 10px; width: 100px; text-align: center; }
        button { padding: 10px 20px; font-size: 16px; cursor: pointer; margin: 5px; background-color: #007bff; color: white; border: none; border-radius: 5px; }
        button:hover { background-color: #0056b3; }
        .alert { color: white; background-color: #dc3545; padding: 10px; border-radius: 5px; margin-top: 15px; font-weight: bold; display: none; }
        .success { color: white; background-color: #28a745; padding: 10px; border-radius: 5px; margin-top: 15px; font-weight: bold; }
    </style>
</head>
<body>
    <div class="container">
        <h2>Calculadora 4-Bits</h2>
        <input type="text" id="opA" maxlength="4" placeholder="Ex: 0110">
        <input type="text" id="opB" maxlength="4" placeholder="Ex: 0010">
        <br>
        <button onclick="calcular('add')">SOMA</button>
        <button onclick="calcular('sub')">SUBTRAÇÃO</button>
        
        <div id="resultadoBox" class="success" style="display:none;">
            Resultado (Decimal): <span id="resDec"></span> <br>
            Bits: <span id="resBin"></span>
        </div>
        <div id="alertaOverflow" class="alert">Overflow!</div>
    </div>

    <script>
        function calcular(operacao) {
            let a = document.getElementById('opA').value;
            let b = document.getElementById('opB').value;

            // Validação básica
            if(!/^[01]{4}$/.test(a) || !/^[01]{4}$/.test(b)) {
                alert("Por favor, insira exatamente 4 bits (0s e 1s).");
                return;
            }

            // Requisição GET dinâmica
            fetch(`/calc?a=${a}&b=${b}&op=${operacao}`)
                .then(response => response.json())
                .then(data => {
                    let divAlerta = document.getElementById('alertaOverflow');
                    let divRes = document.getElementById('resultadoBox');
                    
                    divRes.style.display = 'block';
                    document.getElementById('resDec').innerText = data.decimal;
                    document.getElementById('resBin').innerText = data.binario;

                    // Injeta alerta se houver overflow
                    if(data.overflow) {
                        divAlerta.style.display = 'block';
                        divRes.style.display = 'none';
                    } else {
                        divAlerta.style.display = 'none';
                    }
                })
                .catch(error => console.error('Erro na requisição:', error));
        }
    </script>
</body>
</html>
)rawliteral";