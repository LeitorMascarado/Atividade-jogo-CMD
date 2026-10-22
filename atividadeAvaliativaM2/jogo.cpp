#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <termios.h>
#include <unistd.h>

using namespace std;

// const permite apenas ler um dado; & usa o dado original, sem criar uma cópia.
// Funções bool indicam se uma ação deu certo; funções void apenas executam uma ação.

// Guarda as informações do personagem controlado pelo jogador.
struct jogador {
    int vida;
    int x;
    int y;
};

// Guarda a posição e as características da caixa que pode ser empurrada.
struct caixa {
    int x;
    int y;
    int vida;
    bool quebravel;
};

// Representa a poção que recupera a vida do jogador.
struct item_cura {
    int x;
    int y;
    int quantidade;
    int timer;
    bool ativo;
};

// Representa a espada que permite atacar o inimigo.
struct item_ataque {
    int x;
    int y;
    int dano;
    int alcance;
    bool ativo;
};

// Guarda a posição e o dano causado pelo inimigo.
struct inimigo {
    int x;
    int y;
    int dano;
    bool proximaHorizontal;
};

// Define uma passagem de um mapa para outro.
struct passagem {
    int x;
    int y;
    
    int mapaDestino;
    int destinoX;
    int destinoY;
};

// Guarda a porta, a chave e se elas já foram usadas.
struct porta {
    int portaX;
    int portaY;
    bool portaAberta;

    int chaveX;
    int chaveY;
    bool chaveColetada;
};

// Guarda os pontos e outras contagens da partida.
struct pontuacao {
    int rotacao;
    int portasAbertas;
    int inimigosDerrotados;
    int distanciaPercorrida;
};

// Função reservada para uma futura troca de mapa.
// É bool porque, no futuro, poderá informar se a troca deu certo.
bool trocarMapa() {
    return false;
};

// Guarda todos os desenhos de mapas que podem ser carregados.
struct listaMapas {

    int mapaFogueira[20][20];

    int mapa1[20][20];

    int mapa2[20][20];

    int mapa3[20][20];

    int mapa4[20][20];

};
// Guarda o número e o desenho do mapa que está em uso agora.
struct mapa {

    int numeroMapa;
    int mapaSelecionado[20][20];

};

// Guarda tudo que pode mudar durante uma jogada.
struct estadoJogo {
    mapa mapaAtual;
    jogador jogadorAtual;
    item_cura curaAtual;
    item_ataque espadaAtual;
    inimigo inimigoAtual;
    caixa caixaAtual;
    porta portaAtual;
    pontuacao pontosAtuais;
};

// Guarda as cinco últimas jogadas para permitir voltar no tempo.
struct historicoJogo {
    estadoJogo jogadas[5];
    int quantidade = 0;
};

// Lê um arquivo CSV e coloca seus números em uma matriz de mapa.
// É bool para avisar se o arquivo foi lido corretamente ou se houve erro.
bool Mapa_Memoria(const string& nomeArquivo, int matriz[20][20]){
    ifstream arquivo(nomeArquivo);
    if (!arquivo) {
        return false;
    }

    for (int linha = 0; linha < 20; linha++) {
        for (int coluna = 0; coluna < 20; coluna++) {
            if (!(arquivo >> matriz[linha][coluna])) {
                return false;
            }

            if (coluna < 19) {
                char separador;
                if (!(arquivo >> separador) || separador != ',') {
                    return false;
                }
            }
        }
    }

    return static_cast<bool>(arquivo);
}

// Salva o mapa atual em um arquivo CSV.
// É bool para avisar se o arquivo foi salvo com sucesso.
bool salvarMapaTemp(const string& nomeArquivo, const mapa& m) {
    ofstream arquivo(nomeArquivo);
    if (!arquivo) {
        cout << "Erro para salvar/abrir arquivo de mapas" << endl;
        return false;
    }

    for (int linha = 0; linha < 20; linha++) {
        for (int coluna = 0; coluna < 20; coluna++) {
            arquivo << m.mapaSelecionado[linha][coluna];
            if (coluna < 19) {
                arquivo << ',';
            }
        }
        arquivo << '\n';
    }

    return true;

}

// Configura o terminal para ler uma tecla por vez, sem pressionar Enter.
// É bool porque pode falhar ao tentar alterar as configurações do terminal.
bool configurar_entrada_terminal(termios& configuracaoOriginal) {
    if (tcgetattr(STDIN_FILENO, &configuracaoOriginal) != 0) {
        return false;
    }

    termios configuracao = configuracaoOriginal;
    configuracao.c_lflag &= ~(ICANON | ECHO);
    configuracao.c_cc[VMIN] = 1;
    configuracao.c_cc[VTIME] = 0;
    return tcsetattr(STDIN_FILENO, TCSANOW, &configuracao) == 0;
}

// Devolve ao terminal a configuração que ele tinha antes do jogo.
// É void porque apenas executa essa ação e não precisa devolver resposta.
void restaurar_entrada_terminal(const termios& configuracaoOriginal) {
    tcsetattr(STDIN_FILENO, TCSANOW, &configuracaoOriginal);
}

// Copia o mapa escolhido para a tela e registra qual mapa foi escolhido.
// É void porque altera o mapa recebido por & e não precisa devolver valor.
void mudarMapa(mapa& m, const listaMapas& mapas, int numeroMapa) {

    switch (numeroMapa)
    {
    case 0:
        for (int linha = 0; linha < 20; linha++) {
            for (int coluna = 0; coluna < 20; coluna++) {
                m.mapaSelecionado[linha][coluna] = mapas.mapaFogueira[linha][coluna];
            }
        }
        break;
    case 1:
        for (int linha = 0; linha < 20; linha++) {
            for (int coluna = 0; coluna < 20; coluna++) {
                m.mapaSelecionado[linha][coluna] = mapas.mapa1[linha][coluna];
            }
        }
        break;
    case 2:
        for (int linha = 0; linha < 20; linha++) {
            for (int coluna = 0; coluna < 20; coluna++) {
                m.mapaSelecionado[linha][coluna] = mapas.mapa2[linha][coluna];
            }
        }
        break;
    case 3:
        for (int linha = 0; linha < 20; linha++) {
            for (int coluna = 0; coluna < 20; coluna++) {
                m.mapaSelecionado[linha][coluna] = mapas.mapa3[linha][coluna];
            }
        }
        break;
    case 4:
        for (int linha = 0; linha < 20; linha++) {
            for (int coluna = 0; coluna < 20; coluna++) {
                m.mapaSelecionado[linha][coluna] = mapas.mapa4[linha][coluna];
            }
        }
        break;
    default:
        break;
    }

        m.numeroMapa = numeroMapa;
}

// Lê a tecla pressionada pelo jogador e coloca o resultado em tecla.
// É bool para avisar se foi possível ler uma tecla.
bool ler_tecla(char& tecla) {
    return read(STDIN_FILENO, &tecla, 1) == 1;
}

// Verifica se uma posição está livre e não fica dentro de uma parede.
// É bool porque a resposta só pode ser sim ou não.
bool posicaoValida(int x, int y, const mapa& m, const jogador& j, const item_cura& cura, const caixa& caixa, const item_ataque& espada, const porta& porta, const inimigo& inim) {
    
    return m.mapaSelecionado[y][x] == 0 &&
           !(x == j.x && y == j.y) &&
           !(x == cura.x && y == cura.y) &&
           !(x == caixa.x && y == caixa.y) &&
           !(x == espada.x && y == espada.y) &&
           !(x == porta.portaX && y == porta.portaY) &&
           !(x == porta.chaveX && y == porta.chaveY) &&
           !(x == inim.x && y == inim.y);
}

// Sorteia uma posição livre para colocar um objeto no mapa.
// É void porque muda x e y recebidos por & diretamente.
void sortearPosicao(int& x, int& y, const mapa& m, const jogador& j, const item_cura& cura, const caixa& caixa, const item_ataque& espada, const porta& porta, const inimigo& inim) {
    
    // Verifica se a posição sorteada é válida, caso contrário, sorteia novamente.
    // 18 * 18 é o total de casas disponiveis
    int totalCasas = 18 * 18;
    int primeiraCasa = rand() % totalCasas;

    // Ele pega uma posição inicial aleatória e percorre todas as casas possíveis, verificando se a posição é válida.
    for (int tentativa = 0; tentativa < totalCasas; tentativa++) {
        int indice = (primeiraCasa + tentativa) % totalCasas;
        int candidatoX = indice % 18 + 1;
        int candidatoY = indice / 18 + 1;

        if (posicaoValida(candidatoX, candidatoY, m, j, cura, caixa,
                          espada, porta, inim)) {
            x = candidatoX;
            y = candidatoY;
            return;
        }
    }
}

// Salva uma jogada e descarta a mais antiga quando já houver cinco.
// É void porque altera o histórico recebido por &.
void salvarJogada(historicoJogo& historico, const estadoJogo& estado) {
    if (historico.quantidade == 5) {
        for (int i = 0; i < 4; i++) {
            historico.jogadas[i] = historico.jogadas[i + 1];
        }
        historico.quantidade--;
    }

    historico.jogadas[historico.quantidade] = estado;
    historico.quantidade++;
}

// Restaura a última jogada salva. Retorna false se não houver histórico.
// É bool para o jogo saber se realmente conseguiu voltar no tempo.
bool voltarNoTempo(historicoJogo& historico, mapa& m, jogador& j,
                   item_cura& cura, item_ataque& espada, inimigo& inim,
                   caixa& caixa, porta& portaRobson, pontuacao& pontos) {
    if (historico.quantidade == 0) {
        return false;
    }

    historico.quantidade--;
    estadoJogo estado = historico.jogadas[historico.quantidade];

    m = estado.mapaAtual;
    j = estado.jogadorAtual;
    cura = estado.curaAtual;
    espada = estado.espadaAtual;
    inim = estado.inimigoAtual;
    caixa = estado.caixaAtual;
    portaRobson = estado.portaAtual;
    pontos = estado.pontosAtuais;
    return true;
}

// Move o inimigo em direção ao jogador e aplica dano quando ele chega perto.
// É bool para informar se o inimigo conseguiu fazer uma ação.
bool moverInimigo(inimigo& inim, jogador& j, const mapa& m, const porta& portaRobson, const caixa& caixa) {
    bool moverHorizontal = inim.proximaHorizontal;

    int novoX = inim.x;
    int novoY = inim.y;

    if (moverHorizontal && inim.x == j.x) {
        moverHorizontal = false;
    } else if (!moverHorizontal && inim.y == j.y) {
        moverHorizontal = true;
    }


    if (moverHorizontal) {
        novoX += (j.x > inim.x) ? 1 : -1;
    } else {
        novoY += (j.y > inim.y) ? 1 : -1;
    }

if (abs(inim.x - j.x) <= 1 && abs(inim.y - j.y) <= 1) {
    j.vida -= inim.dano;

    do {
        inim.x = rand() % 18 + 1;
        inim.y = rand() % 18 + 1;
    } while (inim.x == j.x && inim.y == j.y);

    inim.proximaHorizontal = false;
    return true;
}

if (m.mapaSelecionado[novoY][novoX] == 1 ||
    (novoX == caixa.x && novoY == caixa.y) ||
    ((novoX == portaRobson.portaX &&
      novoY == portaRobson.portaY) &&
     !portaRobson.portaAberta)) {

    inim.proximaHorizontal = !moverHorizontal;
    return false;
}

    inim.x = novoX;
    inim.y = novoY;
    inim.proximaHorizontal = !moverHorizontal;
    return true;
}

// Reinicia os objetos da fase atual em posições livres do mapa.
// É void porque altera os objetos recebidos por & e não precisa retornar valor.
void reiniciarFase(jogador& j, const mapa& m, item_cura& itemCura,
                   caixa& caixa, porta& portaRobson, item_ataque& espada,
                   inimigo& inim) {
    j = {100, 2, 17};
    itemCura = {2, 4, 10, 10, true};
    espada = {8, 8, 20, 2, false};
    inim = {17, 2, 10, false};
    caixa = {3, 3, 20, true};
    portaRobson = {15, 15, false, 17, 17, false};

    int novaPosicaoX;
    int novaPosicaoY;

    sortearPosicao(novaPosicaoX, novaPosicaoY, m, j, itemCura, caixa, espada, portaRobson, inim);
    j.x = novaPosicaoX;
    j.y = novaPosicaoY;

    sortearPosicao(novaPosicaoX, novaPosicaoY, m, j, itemCura, caixa, espada, portaRobson, inim);
    itemCura.x = novaPosicaoX;
    itemCura.y = novaPosicaoY;

    sortearPosicao(novaPosicaoX, novaPosicaoY, m, j, itemCura, caixa, espada, portaRobson, inim);
    espada.x = novaPosicaoX;
    espada.y = novaPosicaoY;

    sortearPosicao(novaPosicaoX, novaPosicaoY, m, j, itemCura, caixa, espada, portaRobson, inim);
    caixa.x = novaPosicaoX;
    caixa.y = novaPosicaoY;

    sortearPosicao(novaPosicaoX, novaPosicaoY, m, j, itemCura, caixa, espada, portaRobson, inim);
    portaRobson.portaX = novaPosicaoX;
    portaRobson.portaY = novaPosicaoY;

    sortearPosicao(novaPosicaoX, novaPosicaoY, m, j, itemCura, caixa, espada, portaRobson, inim);
    portaRobson.chaveX = novaPosicaoX;
    portaRobson.chaveY = novaPosicaoY;

    sortearPosicao(novaPosicaoX, novaPosicaoY, m, j, itemCura, caixa, espada, portaRobson, inim);
    inim.x = novaPosicaoX;
    inim.y = novaPosicaoY;
}

// Move o jogador e trata colisões com objetos, itens e portas.
// É bool para informar ao jogo se a jogada foi válida.
bool mover(jogador& j, mapa& m, const listaMapas& mapas, item_cura& itemCura, caixa& caixa, porta& portaRobson, item_ataque& espada, inimigo& inim, pontuacao& pontos, historicoJogo& historico, char direcao) {
    int novoX = j.x;
    int novoY = j.y;


    switch (direcao) {
        case 'w': --novoY; break;
        case 's': ++novoY; break;
        case 'a': --novoX; break;
        case 'd': ++novoX; break;
        case 'k': pontos.rotacao++; return true;
        case ' ': return true;
        case 't':
        case 'T':
            return voltarNoTempo(historico, m, j, itemCura, espada, inim,
                                 caixa, portaRobson, pontos);
        case 'r':
        case 'R':
            reiniciarFase(j, m, itemCura, caixa, portaRobson, espada, inim);
            return true;
        default: return false;
    }

    if (novoX == caixa.x && novoY == caixa.y && caixa.quebravel) {
        // A caixa anda na mesma direção que o jogador tentou andar.
        int novaCaixaX = caixa.x + (novoX - j.x);
        int novaCaixaY = caixa.y + (novoY - j.y);

        // Não deixa empurrar a caixa contra uma parede ou porta fechada.
        if (m.mapaSelecionado[novaCaixaY][novaCaixaX] == 1 ||
            (novaCaixaX == portaRobson.portaX &&
            novaCaixaY == portaRobson.portaY &&
            !portaRobson.portaAberta)) {
            return false;
        }

        caixa.x = novaCaixaX;
        caixa.y = novaCaixaY;

        // O jogador entra na posição antiga da caixa.
        j.x = novoX;
        j.y = novoY;

        return true;
    }

    if ((novoX == portaRobson.portaX && novoY == portaRobson.portaY) && portaRobson.chaveColetada) {
        portaRobson.portaAberta = true;
    }
    if ((novoX == portaRobson.chaveX && novoY == portaRobson.chaveY) && !portaRobson.chaveColetada) {
        portaRobson.chaveColetada = true;
    }
    if ((novoX == espada.x && novoY == espada.y) && !espada.ativo) {
        espada.ativo = true;
    }
        
    // As bordas estão nas linhas e colunas 0 e 19.
    if (m.mapaSelecionado[novoY][novoX] == 1 || (novoX == caixa.x && novoY == caixa.y) || ((novoX == portaRobson.portaX && novoY == portaRobson.portaY) && (!portaRobson.portaAberta))) {
        return false;
    }

    if (itemCura.ativo && novoX == itemCura.x && novoY == itemCura.y) {
        j.vida += itemCura.quantidade;
        itemCura.ativo = false;  
    }

    j.x = novoX;
    j.y = novoY;

    if (j.x == portaRobson.portaX && j.y == portaRobson.portaY && portaRobson.portaAberta) {
        portaRobson.portaAberta = false;
        int mapaDestino;

        if (m.numeroMapa == 0) {
            pontos.rotacao = 0;
            j = {100, 2, 17};
            itemCura = {2, 4, 10, 10, true};
            espada = {8, 8, 20, 2, false};
            inim = {17, 2, 10, false};
            caixa = {3, 3, 20, true};
            portaRobson = {15, 15, false, 17, 17, false};
            mapaDestino = pontos.portasAbertas % 4 + 1;
        } else {
            pontos.rotacao++;
            pontos.portasAbertas++;

            if (pontos.portasAbertas % 5 == 0) {
                pontos.rotacao = 0;
                j = {100, 2, 17};
                itemCura = {2, 4, 10, 10, true};
                espada = {8, 8, 20, 2, false};
                inim = {17, 2, 10, false};
                caixa = {3, 3, 20, true};
                portaRobson = {15, 15, false, 17, 17, false};
                mapaDestino = 0;
            } else {
                mapaDestino = m.numeroMapa % 4 + 1;
            }
        }

        mudarMapa(m, mapas, mapaDestino);

        int novaPosicaoX;
        int novaPosicaoY;

        sortearPosicao(novaPosicaoX, novaPosicaoY, m, j, itemCura, caixa, espada, portaRobson, inim);
        j.x = novaPosicaoX;
        j.y = novaPosicaoY;

        sortearPosicao(novaPosicaoX, novaPosicaoY, m, j, itemCura, caixa, espada, portaRobson, inim);
        itemCura.x = novaPosicaoX;
        itemCura.y = novaPosicaoY;

        sortearPosicao(novaPosicaoX, novaPosicaoY, m, j, itemCura, caixa, espada, portaRobson, inim);
        caixa.x = novaPosicaoX;
        caixa.y = novaPosicaoY;

        sortearPosicao(novaPosicaoX, novaPosicaoY, m, j, itemCura, caixa, espada, portaRobson, inim);
        espada.x = novaPosicaoX;
        espada.y = novaPosicaoY;

        sortearPosicao(novaPosicaoX, novaPosicaoY, m, j, itemCura, caixa, espada, portaRobson, inim);
        portaRobson.portaX = novaPosicaoX;
        portaRobson.portaY = novaPosicaoY;

        sortearPosicao(novaPosicaoX, novaPosicaoY, m, j, itemCura, caixa, espada, portaRobson, inim);
        portaRobson.chaveX = novaPosicaoX;
        portaRobson.chaveY = novaPosicaoY;
        portaRobson.chaveColetada = false;

        sortearPosicao(novaPosicaoX, novaPosicaoY, m, j, itemCura, caixa, espada, portaRobson, inim);
        inim.x = novaPosicaoX;
        inim.y = novaPosicaoY;
        inim.proximaHorizontal = false;
    }

    return true;
}

// Imprime um símbolo ocupando sempre o mesmo espaço na tela.
// É void porque sua única tarefa é escrever o símbolo no terminal.
void imprimirCelula(const string& simbolo){
    cout << left << setw(2) << simbolo;
}

// Desenha o mapa e todos os objetos na tela.
// É void porque apenas mostra dados; os parâmetros const não são alterados.
void printar_mapa(const mapa& m, const jogador& j, const item_cura& itemCura, const caixa& caixa, const item_ataque& espada, const porta& portaRobson, const inimigo& inim, bool atacando = false) {
    
    for (int linha = 0; linha < 20; linha++) {
        for (int coluna = 0; coluna < 20; coluna++) {
            int dx = coluna - j.x;
            int dy = linha - j.y;
            bool areaAtaque = atacando && dx * dx + dy * dy <= espada.alcance * espada.alcance;

            if (j.x == coluna && j.y == linha) {
                imprimirCelula("@");
            } else if (areaAtaque) {
                imprimirCelula("*");
            } else if (itemCura.x == coluna && itemCura.y == linha) {
                imprimirCelula(itemCura.ativo ? "+ " : "- ");
            } else if (caixa.x == coluna && caixa.y == linha) {
                imprimirCelula("C");
            } else if (portaRobson.portaX == coluna && portaRobson.portaY == linha) {
                imprimirCelula(portaRobson.portaAberta ? "  " : "[]");
            } else if (portaRobson.chaveX == coluna && portaRobson.chaveY == linha) {
                imprimirCelula(portaRobson.chaveColetada ? "  " : "P");
            } else if (!espada.ativo && espada.x == coluna && espada.y == linha) {
                imprimirCelula("Y");
            } else if (inim.x == coluna && inim.y == linha) {
                imprimirCelula("E");
            } else if (m.mapaSelecionado[linha][coluna] == 1) {
                imprimirCelula("#");
            } else {
                imprimirCelula("  ");
            }
        }
        cout << endl;
    }
}

// Mostra o ataque da espada e afasta o inimigo que estiver no alcance.
// É void porque altera o inimigo por & e não precisa devolver resultado.
void atacar(const mapa& m, const jogador& j, const item_cura& itemCura, const caixa& caixa, const item_ataque& espada, const porta& portaRobson, inimigo& inim) {
    if (!espada.ativo) return;

    // Mostra a área circular da espada antes de aplicar o golpe.
    system("clear");
    printar_mapa(m, j, itemCura, caixa, espada, portaRobson, inim, true);
    cout << "Ataque da espada!\n";
    cout.flush();
    usleep(250000);

    int dx = inim.x - j.x;
    int dy = inim.y - j.y;
    if (dx * dx + dy * dy <= espada.alcance * espada.alcance) {
        inim.x = rand() % 18 + 1;
        inim.y = rand() % 18 + 1;
    }

    system("clear");
    printar_mapa(m, j, itemCura, caixa, espada, portaRobson, inim);
}

// Salva os dados atuais da partida no arquivo saveJogo.txt.
// É bool para avisar se o arquivo foi criado corretamente.
bool salvarJogo(const mapa& m, const jogador& j, const item_cura& itemCura, const item_ataque& espada, const inimigo& inim, const caixa& caixa, const porta& portaRobson) {
    ofstream arquivo("saveJogo.txt");
    if (!arquivo) {
        return false;
    }

    arquivo << "JOGADOR " << j.vida << ' ' << j.x << ' ' << j.y << '\n';
    arquivo << "CURA " << itemCura.x << ' ' << itemCura.y << ' '
            << itemCura.quantidade << ' ' << itemCura.timer << ' ' << itemCura.ativo << '\n';
    arquivo << "ATAQUE " << espada.x << ' ' << espada.y << ' ' << espada.dano << ' '
            << espada.alcance << ' ' << espada.ativo << '\n';
    arquivo << "INIMIGO " << inim.x << ' ' << inim.y << ' ' << inim.dano << ' '
            << inim.proximaHorizontal << '\n';
    arquivo << "CAIXA " << caixa.x << ' ' << caixa.y << ' ' << caixa.vida << ' '
            << caixa.quebravel << '\n';
    arquivo << "PORTA " << portaRobson.portaX << ' ' << portaRobson.portaY << ' '
            << portaRobson.portaAberta << ' ' << portaRobson.chaveX << ' '
            << portaRobson.chaveY << ' ' << portaRobson.chaveColetada << '\n';
    arquivo << "MAPA\n";

    for (int linha = 0; linha < 20; linha++) {
        for (int coluna = 0; coluna < 20; coluna++) {
            arquivo << m.mapaSelecionado[linha][coluna] << ' ';
        }
        arquivo << '\n';
    }

    // O static_cast verifica o estado do arquivo e retorna true se o arquivo foi escrito com sucesso, ou false caso contrário.
    return static_cast<bool>(arquivo);
}

// Lê saveJogo.txt e devolve o jogo ao estado que foi salvo.
// É bool para avisar se o save foi encontrado e lido sem erro.
bool carregarJogo(mapa& m, jogador& j, item_cura& itemCura, item_ataque& espada, inimigo& inim, caixa& caixaAtual, porta& portaRobson) {
    
    ifstream arquivo("saveJogo.txt");

    if (!arquivo) {
        return false;
    }

    mapa mapaTemp = m;
    jogador jogadorTemp = j;
    item_cura curaTemp = itemCura;
    item_ataque ataqueTemp = espada;
    inimigo inimigoTemp = inim;
    caixa caixaTemp = caixaAtual;
    porta portaTemp = portaRobson;
    string secao;

    if (!(arquivo >> secao) || secao != "JOGADOR" ||
        !(arquivo >> jogadorTemp.vida >> jogadorTemp.x >> jogadorTemp.y)) {
        return false;
    }
    if (!(arquivo >> secao) || secao != "CURA" ||
        !(arquivo >> curaTemp.x >> curaTemp.y >> curaTemp.quantidade
                  >> curaTemp.timer >> curaTemp.ativo)) {
        return false;
    }
    if (!(arquivo >> secao) || secao != "ATAQUE" ||
        !(arquivo >> ataqueTemp.x >> ataqueTemp.y >> ataqueTemp.dano
                  >> ataqueTemp.alcance >> ataqueTemp.ativo)) {
        return false;
    }
    if (!(arquivo >> secao) || secao != "INIMIGO" ||
        !(arquivo >> inimigoTemp.x >> inimigoTemp.y >> inimigoTemp.dano
                  >> inimigoTemp.proximaHorizontal)) {
        return false;
    }
    if (!(arquivo >> secao) || secao != "CAIXA" ||
        !(arquivo >> caixaTemp.x >> caixaTemp.y >> caixaTemp.vida >> caixaTemp.quebravel)) {
        return false;
    }
    if (!(arquivo >> secao) || secao != "PORTA" ||
        !(arquivo >> portaTemp.portaX >> portaTemp.portaY >> portaTemp.portaAberta
                  >> portaTemp.chaveX >> portaTemp.chaveY >> portaTemp.chaveColetada)) {
        return false;
    }
    if (!(arquivo >> secao) || secao != "MAPA") {
        return false;
    }

    for (int linha = 0; linha < 20; linha++) {
        for (int coluna = 0; coluna < 20; coluna++) {
            if (!(arquivo >> mapaTemp.mapaSelecionado[linha][coluna])) {
                return false;
            }
        }
    }

    m = mapaTemp;
    j = jogadorTemp;
    itemCura = curaTemp;
    espada = ataqueTemp;
    inim = inimigoTemp;
    caixaAtual = caixaTemp;
    portaRobson = portaTemp;
    return true;
}

// Inicia o jogo e mantém o laço principal recebendo os comandos do jogador.
// É int porque o sistema recebe um número no fim: 0 significa que tudo terminou bem.
int main() {
    
    termios configuracaoOriginal{};
    if (!configurar_entrada_terminal(configuracaoOriginal)) {
        cerr << "Execute o jogo em um terminal interativo.\n";
        return 1;
    }

    srand(static_cast<unsigned>(time(nullptr)));

    listaMapas mapas;
    if (!Mapa_Memoria("mapaSalvo0.csv", mapas.mapaFogueira) ||
        !Mapa_Memoria("mapaSalvo1.csv", mapas.mapa1) ||
        !Mapa_Memoria("mapaSalvo2.csv", mapas.mapa2) ||
        !Mapa_Memoria("mapaSalvo3.csv", mapas.mapa3) ||
        !Mapa_Memoria("mapaSalvo4.csv", mapas.mapa4)) {
        cerr << "Nao foi possivel carregar os arquivos CSV dos mapas.\n";
        restaurar_entrada_terminal(configuracaoOriginal);
        return 1;
    }

    mapa mapa{};
    mudarMapa(mapa, mapas, 0);

    jogador jogador {100, 2, 17};
    item_cura pocao{2, 4, 10, 10, true};
    item_ataque espada{8, 8, 20, 2, false};
    inimigo inimigo {17, 2, 10, false};
    caixa caixa {3, 3, 20, true};
    porta portaRobson {15, 15, false, 17, 17, false};
    pontuacao pontos {0, 0, 0, 0};
    historicoJogo historico;

    int novaPosicaoX;
    int novaPosicaoY;

    sortearPosicao(novaPosicaoX, novaPosicaoY, mapa, jogador, pocao, caixa, espada, portaRobson, inimigo);
    pocao.x = novaPosicaoX;
    pocao.y = novaPosicaoY;

    sortearPosicao(novaPosicaoX, novaPosicaoY, mapa, jogador, pocao, caixa, espada, portaRobson, inimigo);
    espada.x = novaPosicaoX;
    espada.y = novaPosicaoY;

    sortearPosicao(novaPosicaoX, novaPosicaoY, mapa, jogador, pocao, caixa, espada, portaRobson, inimigo);
    caixa.x = novaPosicaoX;
    caixa.y = novaPosicaoY;

    sortearPosicao(novaPosicaoX, novaPosicaoY, mapa, jogador, pocao, caixa, espada, portaRobson, inimigo);
    portaRobson.portaX = novaPosicaoX;
    portaRobson.portaY = novaPosicaoY;

    sortearPosicao(novaPosicaoX, novaPosicaoY, mapa, jogador, pocao, caixa, espada, portaRobson, inimigo);
    portaRobson.chaveX = novaPosicaoX;
    portaRobson.chaveY = novaPosicaoY;

    sortearPosicao(novaPosicaoX, novaPosicaoY, mapa, jogador, pocao, caixa, espada, portaRobson, inimigo);
    inimigo.x = novaPosicaoX;
    inimigo.y = novaPosicaoY;
    
    system("clear");
    printar_mapa(mapa, jogador, pocao, caixa, espada, portaRobson, inimigo);
    cout << "Vida: " << jogador.vida << endl;
    cout << "Mover: W/A/S/D | Atacar: Espaco | Voltar: T | Reiniciar: R | Salvar: P | Carregar: L | Sair: Q\n";
    cout.flush();
    
    while (jogador.vida > 0) {

        char direcao;
        if (!ler_tecla(direcao)) {
            break;
        }

        if (direcao == 'q' || direcao == 'Q') {
            break;
        }

        if (direcao == 'p' || direcao == 'P') {
            bool salvo = salvarJogo(mapa, jogador, pocao, espada, inimigo, caixa, portaRobson);
            bool mapaSalvo = salvarMapaTemp("mapaSalvo1.csv", mapa);
            system("clear");
            printar_mapa(mapa, jogador, pocao, caixa, espada, portaRobson, inimigo);
            cout << (salvo && mapaSalvo ? "Jogo salvo em saveJogo.txt e mapaSalvo.csv.\n" : "Nao foi possivel salvar o jogo ou o mapa.\n");
            cout << "Vida: " << jogador.vida << endl;
            cout << "Mover: W/A/S/D | Atacar: Espaco | Voltar: T | Reiniciar: R | Salvar: P | Carregar: L | Sair: Q\n";
            cout.flush();
            continue;
        }

        if (direcao == 'l' || direcao == 'L') {
            bool carregado = carregarJogo(mapa, jogador, pocao, espada, inimigo, caixa, portaRobson);
            system("clear");
            printar_mapa(mapa, jogador, pocao, caixa, espada, portaRobson, inimigo);
            cout << (carregado ? "Jogo carregado.\n" : "Nao foi possivel carregar saveJogo.txt.\n");
            cout << "Vida: " << jogador.vida << endl;
            cout << "Mover: W/A/S/D | Atacar: Espaco | Voltar: T | Reiniciar: R | Salvar: P | Carregar: L | Sair: Q\n";
            cout.flush();
            continue;
        }

        estadoJogo estadoAntes = {mapa, jogador, pocao, espada, inimigo,
                                  caixa, portaRobson, pontos};
        bool movimentoValido = mover(jogador, mapa, mapas, pocao, caixa,
                                     portaRobson, espada, inimigo, pontos,
                                     historico, direcao);
        bool voltouNoTempo = (direcao == 't' || direcao == 'T') && movimentoValido;

        if (movimentoValido && !voltouNoTempo) {
            salvarJogada(historico, estadoAntes);
        }

        if (direcao == ' ') {
            atacar(mapa, jogador, pocao, caixa, espada, portaRobson, inimigo);
            if (espada.ativo) {
                espada.ativo = false; // Desativa a espada após o ataque
                sortearPosicao(espada.x, espada.y, mapa, jogador, pocao, caixa, espada, portaRobson, inimigo);
            }
        }

        if (movimentoValido && !voltouNoTempo) {
            if (moverInimigo(inimigo, jogador, mapa, portaRobson, caixa)) {
                if (jogador.x == inimigo.x && jogador.y == inimigo.y) {
                    jogador.vida -= inimigo.dano;
                }
            }
        }

        system("clear");
        printar_mapa(mapa, jogador, pocao, caixa, espada, portaRobson, inimigo);
        if (!movimentoValido) {
            cout << ((direcao == 't' || direcao == 'T') ?
                         "Nao ha jogada para voltar.\n" : "Movimento invalido.\n");
        }
        cout << "Vida: " << jogador.vida << " | Pontos: " << pontos.portasAbertas << " Rotacao: " << pontos.rotacao << endl;
        cout << "Espada ativa: " << (espada.ativo ? "Ativo" : "Inativo") << endl;
        cout << "Mover: W/A/S/D | Atacar: Espaco | Voltar: T | Reiniciar: R | Salvar: P | Carregar: L | Sair: Q\n";
        cout.flush();
    }

    restaurar_entrada_terminal(configuracaoOriginal);
    return 0;
}
