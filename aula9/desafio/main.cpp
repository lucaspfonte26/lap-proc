#include "metronomo.h"
#include <csignal>
#include <atomic>
#include <cstdio>

static std::atomic<bool> g_rodando{true};

static void onSigint(int) { g_rodando.store(false); }

int main() {
    if (!initMetronomo()) return 1;

    std::signal(SIGINT, onSigint);   // sem isso, Ctrl+C deixa buzzer e servo ligados
    std::printf("Metronomo (desafio) iniciado. BPM = %d, som %s.\n",
                obterBPM(), somLigado() ? "ligado" : "desligado");
    std::printf("GPIO %d: +%d BPM | GPIO %d: -%d BPM | GPIO %d: reset | GPIO %d: som\n",
                PIN_BOTAO_MAIS, BPM_PASSO, PIN_BOTAO_MENOS, BPM_PASSO,
                PIN_BOTAO_RESET, PIN_BOTAO_SOM);
    std::printf("Ctrl+C para sair.\n");

    while (g_rodando.load()) {
        passoMetronomo();
    }

    encerrarMetronomo();
    std::printf("\nMetronomo encerrado.\n");
    return 0;
}
