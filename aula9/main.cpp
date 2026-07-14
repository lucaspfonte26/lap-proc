#include "metronomo.h"
#include <csignal>
#include <atomic>
#include <cstdio>

static std::atomic<bool> g_rodando{true};

static void onSigint(int) { g_rodando.store(false); }

int main() {
    if (!initMetronomo()) return 1;

    std::signal(SIGINT, onSigint);   // sem isso, Ctrl+C deixa buzzer e servo ligados
    std::printf("Metronomo iniciado (BPM=%d). Botoes ajustam o BPM. Ctrl+C para sair.\n",
                obterBPM());

    while (g_rodando.load()) {
        passoMetronomo();
    }

    encerrarMetronomo();
    std::printf("\nMetronomo encerrado.\n");
    return 0;
}
