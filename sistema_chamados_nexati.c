/*
 * PIM IV - Sistema de Gerenciamento de Chamados de TI
 * Projeto ficticio: NexaTI
 * Linguagem: C (compativel com C11)
 *
 * Os registros sao armazenados localmente no arquivo chamados.dat.
 * Este programa e independente do banco SQL Server desenvolvido na parte
 * de banco de dados do PIM IV.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define MAX_CHAMADOS 1000
#define TAM_TITULO 100
#define TAM_DESCRICAO 300
#define TAM_NOME 80
#define TAM_CATEGORIA 50
#define TAM_OPCAO 20
#define ARQUIVO_DADOS "chamados.dat"

typedef struct {
    int id;
    char solicitante[TAM_NOME];
    char equipamento[TAM_NOME];
    char titulo[TAM_TITULO];
    char descricao[TAM_DESCRICAO];
    char categoria[TAM_CATEGORIA];
    char prioridade[20];
    char status[30];
} Chamado;

static Chamado chamados[MAX_CHAMADOS];
static int totalChamados = 0;
static int proximoId = 1;

/* Le uma linha completa e remove o caractere de nova linha. */
void lerTexto(const char *mensagem, char *destino, size_t tamanho) {
    printf("%s", mensagem);

    if (fgets(destino, (int)tamanho, stdin) == NULL) {
        destino[0] = '\0';
        return;
    }

    size_t comprimento = strlen(destino);
    if (comprimento > 0 && destino[comprimento - 1] == '\n') {
        destino[comprimento - 1] = '\0';
    } else {
        int caractere;
        while ((caractere = getchar()) != '\n' && caractere != EOF) {
            /* Descarta o restante da linha. */
        }
    }
}

/* Le um numero inteiro, evitando problemas com entradas invalidas. */
int lerInteiro(const char *mensagem) {
    char entrada[TAM_OPCAO];
    char *fim;
    long valor;

    while (1) {
        lerTexto(mensagem, entrada, sizeof(entrada));
        errno = 0;
        valor = strtol(entrada, &fim, 10);

        if (entrada[0] != '\0' && *fim == '\0' && errno == 0) {
            return (int)valor;
        }

        printf("Entrada invalida. Digite um numero inteiro.\n");
    }
}

void pausar(void) {
    char entrada[4];
    lerTexto("\nPressione ENTER para continuar...", entrada, sizeof(entrada));
}

void carregarChamados(void) {
    FILE *arquivo = fopen(ARQUIVO_DADOS, "rb");

    if (arquivo == NULL) {
        return; /* Na primeira execucao, o arquivo ainda nao existe. */
    }

    totalChamados = (int)fread(chamados, sizeof(Chamado),
                               MAX_CHAMADOS, arquivo);
    fclose(arquivo);

    for (int i = 0; i < totalChamados; i++) {
        if (chamados[i].id >= proximoId) {
            proximoId = chamados[i].id + 1;
        }
    }
}

int salvarChamados(void) {
    FILE *arquivo = fopen(ARQUIVO_DADOS, "wb");

    if (arquivo == NULL) {
        printf("Erro: nao foi possivel salvar os dados.\n");
        return 0;
    }

    size_t gravados = fwrite(chamados, sizeof(Chamado),
                             (size_t)totalChamados, arquivo);
    fclose(arquivo);

    if (gravados != (size_t)totalChamados) {
        printf("Erro ao gravar os chamados no arquivo.\n");
        return 0;
    }

    return 1;
}

const char *nomePrioridade(int opcao) {
    switch (opcao) {
        case 1: return "Baixa";
        case 2: return "Media";
        case 3: return "Alta";
        case 4: return "Urgente";
        default: return "";
    }
}

const char *nomeStatus(int opcao) {
    switch (opcao) {
        case 1: return "Aberto";
        case 2: return "Em atendimento";
        case 3: return "Aguardando";
        case 4: return "Concluido";
        default: return "";
    }
}

int buscarIndicePorId(int id) {
    for (int i = 0; i < totalChamados; i++) {
        if (chamados[i].id == id) {
            return i;
        }
    }
    return -1;
}

void exibirChamado(const Chamado *chamado) {
    printf("\n----------------------------------------\n");
    printf("Chamado: %d\n", chamado->id);
    printf("Solicitante: %s\n", chamado->solicitante);
    printf("Equipamento: %s\n", chamado->equipamento);
    printf("Titulo: %s\n", chamado->titulo);
    printf("Descricao: %s\n", chamado->descricao);
    printf("Categoria: %s\n", chamado->categoria);
    printf("Prioridade: %s\n", chamado->prioridade);
    printf("Status: %s\n", chamado->status);
    printf("----------------------------------------\n");
}

void cadastrarChamado(void) {
    if (totalChamados >= MAX_CHAMADOS) {
        printf("Limite maximo de chamados atingido.\n");
        return;
    }

    Chamado novo = {0};
    novo.id = proximoId++;

    printf("\n========== CADASTRO DE CHAMADO ==========\n");
    lerTexto("Nome do solicitante: ", novo.solicitante,
             sizeof(novo.solicitante));
    lerTexto("Equipamento: ", novo.equipamento,
             sizeof(novo.equipamento));
    lerTexto("Titulo do chamado: ", novo.titulo,
             sizeof(novo.titulo));
    lerTexto("Descricao: ", novo.descricao,
             sizeof(novo.descricao));
    lerTexto("Categoria (ex.: Hardware, Software, Rede): ",
             novo.categoria, sizeof(novo.categoria));

    printf("\nPrioridade:\n1 - Baixa\n2 - Media\n3 - Alta\n4 - Urgente\n");
    int opcaoPrioridade;
    do {
        opcaoPrioridade = lerInteiro("Escolha a prioridade: ");
        if (opcaoPrioridade < 1 || opcaoPrioridade > 4) {
            printf("Opcao invalida.\n");
        }
    } while (opcaoPrioridade < 1 || opcaoPrioridade > 4);
    snprintf(novo.prioridade, sizeof(novo.prioridade), "%s",
             nomePrioridade(opcaoPrioridade));

    snprintf(novo.status, sizeof(novo.status), "%s", "Aberto");

    chamados[totalChamados++] = novo;

    if (salvarChamados()) {
        printf("\nChamado %d cadastrado e salvo com sucesso!\n", novo.id);
    }
}

void listarChamados(void) {
    printf("\n========== LISTA DE CHAMADOS ==========\n");

    if (totalChamados == 0) {
        printf("Nenhum chamado cadastrado.\n");
        return;
    }

    for (int i = 0; i < totalChamados; i++) {
        printf("\nID: %d | Solicitante: %s | Prioridade: %s | Status: %s\n",
               chamados[i].id, chamados[i].solicitante,
               chamados[i].prioridade, chamados[i].status);
        printf("Titulo: %s\n", chamados[i].titulo);
    }
}

void pesquisarChamado(void) {
    printf("\n========== PESQUISA DE CHAMADO ==========\n");
    int id = lerInteiro("Digite o numero do chamado: ");
    int indice = buscarIndicePorId(id);

    if (indice == -1) {
        printf("Chamado nao encontrado.\n");
    } else {
        exibirChamado(&chamados[indice]);
    }
}

void atualizarChamado(void) {
    printf("\n========== ATUALIZACAO DE CHAMADO ==========\n");
    int id = lerInteiro("Digite o numero do chamado: ");
    int indice = buscarIndicePorId(id);

    if (indice == -1) {
        printf("Chamado nao encontrado.\n");
        return;
    }

    printf("\nStatus atual: %s\n", chamados[indice].status);
    printf("Novo status:\n");
    printf("1 - Aberto\n2 - Em atendimento\n3 - Aguardando\n4 - Concluido\n");

    int opcao;
    do {
        opcao = lerInteiro("Escolha o novo status: ");
        if (opcao < 1 || opcao > 4) {
            printf("Opcao invalida.\n");
        }
    } while (opcao < 1 || opcao > 4);

    snprintf(chamados[indice].status, sizeof(chamados[indice].status),
             "%s", nomeStatus(opcao));

    if (salvarChamados()) {
        printf("Status do chamado %d atualizado com sucesso!\n", id);
    }
}

void relatorioChamados(void) {
    int porStatus[4] = {0, 0, 0, 0};
    int porPrioridade[4] = {0, 0, 0, 0};

    for (int i = 0; i < totalChamados; i++) {
        for (int j = 1; j <= 4; j++) {
            if (strcmp(chamados[i].status, nomeStatus(j)) == 0) {
                porStatus[j - 1]++;
            }
            if (strcmp(chamados[i].prioridade, nomePrioridade(j)) == 0) {
                porPrioridade[j - 1]++;
            }
        }
    }

    printf("\n========== RELATORIO DE CHAMADOS ==========\n");
    printf("Total de chamados: %d\n", totalChamados);

    printf("\nPor status:\n");
    for (int i = 1; i <= 4; i++) {
        printf("%-18s: %d\n", nomeStatus(i), porStatus[i - 1]);
    }

    printf("\nPor prioridade:\n");
    for (int i = 1; i <= 4; i++) {
        printf("%-18s: %d\n", nomePrioridade(i), porPrioridade[i - 1]);
    }
}

void exibirMenu(void) {
    printf("\n============================================\n");
    printf("                 NEXATI\n");
    printf("       SISTEMA DE CHAMADOS DE TI\n");
    printf("============================================\n");
    printf("1 - Cadastrar chamado\n");
    printf("2 - Listar chamados\n");
    printf("3 - Pesquisar chamado\n");
    printf("4 - Atualizar status do chamado\n");
    printf("5 - Relatorio de chamados\n");
    printf("0 - Sair\n");
    printf("============================================\n");
}

int main(void) {
    int opcao;

    carregarChamados();

    printf("Sistema NexaTI iniciado.\n");
    printf("Chamados carregados: %d\n", totalChamados);

    do {
        exibirMenu();
        opcao = lerInteiro("Escolha uma opcao: ");

        switch (opcao) {
            case 1:
                cadastrarChamado();
                pausar();
                break;
            case 2:
                listarChamados();
                pausar();
                break;
            case 3:
                pesquisarChamado();
                pausar();
                break;
            case 4:
                atualizarChamado();
                pausar();
                break;
            case 5:
                relatorioChamados();
                pausar();
                break;
            case 0:
                printf("\nEncerrando o sistema NexaTI...\n");
                break;
            default:
                printf("Opcao invalida. Tente novamente.\n");
                pausar();
        }
    } while (opcao != 0);

    return 0;
}
