#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Estrutura para armazenar dados dos produtos
typedef struct produto
{
    int chave;
    int product_id;
    char category_code[50];
    char brand[50];
    float price;
} Produto;

// Estrutura para armazenar dados dos acessos
typedef struct acesso
{
    int chave;
    int user_id;
    char event_type[20];
    char user_session[50];
    char event_time[50];
} Acesso;

// Estrutura para armazenar índices de produtos e acessos
typedef struct indice
{
    int chave;
    long posicao;
} Indice;

int carregarUltimaChave() {
    FILE *arquivoChave = fopen("chaves.txt", "r");
    int ultimaChave = 0;

    if (arquivoChave == NULL) {
        // Se o arquivo não existir, começamos com a chave 0
        ultimaChave = 0;
    } else {
        fscanf(arquivoChave, "%d", &ultimaChave);
        fclose(arquivoChave);
    }

    return ultimaChave;
}

void salvarUltimaChave(int ultimaChave) {
    FILE *arquivoChave = fopen("chaves.txt", "w");

    if (arquivoChave == NULL) {
        printf("Erro ao abrir o arquivo de chaves para gravação.\n");
        return;
    }

    fprintf(arquivoChave, "%d", ultimaChave);
    fclose(arquivoChave);
}

void processarCSV(char *nomeArquivoCSV) {
    FILE *csv = fopen(nomeArquivoCSV, "r");
    if (csv == NULL) {
        printf("Erro ao abrir o arquivo CSV.\n");
        return;
    }

    // Arquivos binários de saída
    FILE *arquivoProdutos = fopen("produtos.bin", "ab");
    FILE *arquivoAcessos = fopen("acessos.bin", "ab");
    FILE *indiceProdutos = fopen("indice_produtos.idx", "ab");
    FILE *indiceAcessos = fopen("indice_acessos.idx", "ab");

    if (arquivoProdutos == NULL || arquivoAcessos == NULL || indiceProdutos == NULL || indiceAcessos == NULL) {
        printf("Erro ao abrir os arquivos binários ou de índices.\n");
        fclose(csv);
        return;
    }

    // Carregar a última chave salva
    int chaveAtual = carregarUltimaChave();

    char linha[1024];
    // Ignorar a primeira linha (cabeçalho)
    fgets(linha, sizeof(linha), csv);

    int chaveProduto = chaveAtual;
    int chaveAcesso = 0; // Se for necessário para acessos, pode ser ajustado

    long long linhasLidas = 0;  // Contador de linhas

    while (fgets(linha, sizeof(linha), csv)) {
        Produto produto = {0};  // Inicializar com zeros
        Acesso acesso = {0};
        Indice indiceProduto = {0};
        Indice indiceAcesso = {0};

        // Incrementar a chave
        chaveProduto++;
        produto.chave = chaveProduto;
        acesso.chave = chaveAcesso++;

        char *campo = strtok(linha, ",");

        // Campo 1: event_time (pode ser vazio)
        if (campo != NULL && strlen(campo) > 0) {
            strcpy(acesso.event_time, campo);
        } else {
            strcpy(acesso.event_time, "Desconhecido");
        }

        // Campo 2: event_type (pode ser vazio)
        campo = strtok(NULL, ",");
        if (campo != NULL && strlen(campo) > 0) {
            strcpy(acesso.event_type, campo);
        } else {
            strcpy(acesso.event_type, "Desconhecido");
        }

        // Campo 3: product_id (pode ser vazio)
        campo = strtok(NULL, ",");
        if (campo != NULL && strlen(campo) > 0) {
            produto.product_id = atoi(campo);
        } else {
            produto.product_id = -1;
        }

        // Campo 4: category_id (ignorando)
        campo = strtok(NULL, ",");

        // Campo 5: category_code (pode ser vazio)
        campo = strtok(NULL, ",");
        if (campo != NULL && strlen(campo) > 0) {
            strcpy(produto.category_code, campo);
        } else {
            strcpy(produto.category_code, "Desconhecido");
        }

        // Campo 6: brand (pode ser vazio)
        campo = strtok(NULL, ",");
        if (campo != NULL && strlen(campo) > 0) {
            strcpy(produto.brand, campo);
        } else {
            strcpy(produto.brand, "Generica");
        }

        // Campo 7: price (pode ser vazio)
        campo = strtok(NULL, ",");
        if (campo != NULL && strlen(campo) > 0) {
            produto.price = atof(campo);
        } else {
            produto.price = 0.0;
        }

        // Campo 8: user_id (pode ser vazio)
        campo = strtok(NULL, ",");
        if (campo != NULL && strlen(campo) > 0) {
            acesso.user_id = atoi(campo);
        } else {
            acesso.user_id = -1;
        }

        // Campo 9: user_session (pode ser vazio)
        campo = strtok(NULL, ",");
        if (campo != NULL && strlen(campo) > 0) {
            strcpy(acesso.user_session, campo);
        } else {
            strcpy(acesso.user_session, "Desconhecido");
        }

        // Escreve os dados nos arquivos binários
        long long posProduto = ftell(arquivoProdutos);  // Usando long long
        if (fwrite(&produto, sizeof(Produto), 1, arquivoProdutos) != 1) {
            printf("Erro ao escrever no arquivo produtos.bin\n");
        }

        long long posAcesso = ftell(arquivoAcessos);  // Usando long long
        if (fwrite(&acesso, sizeof(Acesso), 1, arquivoAcessos) != 1) {
            printf("Erro ao escrever no arquivo acessos.bin\n");
        }

        // Escreve os índices
        indiceProduto.chave = produto.chave;
        indiceProduto.posicao = posProduto;
        if (fwrite(&indiceProduto, sizeof(Indice), 1, indiceProdutos) != 1) {
            printf("Erro ao escrever no arquivo indice_produtos.idx\n");
        }

        indiceAcesso.chave = acesso.chave;
        indiceAcesso.posicao = posAcesso;
        if (fwrite(&indiceAcesso, sizeof(Indice), 1, indiceAcessos) != 1) {
            printf("Erro ao escrever no arquivo indice_acessos.idx\n");
        }

        linhasLidas++;

        // A cada 10.000 linhas, imprime o progresso
        if (linhasLidas % 10000 == 0) {
            printf("%lld linhas lidas...\n", linhasLidas);
        }
    }

    // Imprime a última chave importada
    printf("Última chave importada: %d\n", chaveProduto);

    // Salva a última chave usada no arquivo
    salvarUltimaChave(chaveProduto);

    fclose(csv);
    fclose(arquivoProdutos);
    fclose(arquivoAcessos);
    fclose(indiceProdutos);
    fclose(indiceAcessos);

    printf("\nProcessamento concluído. Total de linhas lidas: %lld\n", linhasLidas);
}

void procuraProdutos()
{
    FILE *p = fopen("produtos.bin", "rb");

    if (p == NULL)
    {
        printf("Erro ao abrir o arquivo produtos.bin\n");
        return;
    }

    Produto produto;

    while (fread(&produto, sizeof(Produto), 1, p) == 1)
    {
        printf("Chave do Produto: %d\n", produto.chave);
        printf("ID do Produto: %d\n", produto.product_id);
        printf("Categoria: %s\n", produto.category_code);
        printf("Marca: %s\n", produto.brand);
        printf("Preço: %.2f\n", produto.price);
        printf("--------------------------\n");
    }

    fclose(p);
}

void pesquisaBinaria(int chave)
{
    FILE *p = fopen("produtos.bin", "rb");
    if (p == NULL)
    {
        printf("Erro ao abrir o arquivo produtos.bin\n");
        return;
    }

    Produto produto;
    long left = 0;

    fseek(p, 0, SEEK_END);
    long size = ftell(p);
    long right = size / sizeof(Produto) - 1;

    while (left <= right)
    {
        long mid = left + (right - left) / 2;
        fseek(p, mid * sizeof(Produto), SEEK_SET);

        fread(&produto, sizeof(Produto), 1, p);

        if (produto.chave == chave)
        {
            printf("Chave do Produto: %d\n", produto.chave);
            printf("ID do Produto: %d\n", produto.product_id);
            printf("Categoria: %s\n", produto.category_code);
            printf("Marca: %s\n", produto.brand);
            printf("Preço: %.2f\n", produto.price);
            fclose(p);
            return;
        }
        if (produto.chave < chave)
        {
            left = mid + 1;
        }
        else
        {
            right = mid - 1;
        }
    }

    printf("Produto com chave %d não encontrado.\n", chave);
    fclose(p);
}

void faixaPreco(float price)
{
    FILE *p = fopen("produtos.bin", "rb");

    if (p == NULL)
    {
        printf("Erro ao abrir o arquivo produtos.bin\n");
        return;
    }

    Produto produto;

    while (fread(&produto, sizeof(Produto), 1, p) == 1)
    {
        if (produto.price >= price)
        {
            printf("Produto %d\n", produto.product_id);
            printf("Categoria %s\n", produto.category_code);
            printf("Marca %s\n", produto.brand);
            printf("Preço: %.2f\n", produto.price);
            printf("--------------------------\n");
        }
    }

    fclose(p);
}

int adicionarProduto() {
    FILE *arquivoProdutos = fopen("produtos.bin", "ab");
    FILE *indiceProdutos = fopen("indice_produtos.idx", "ab");

    if (arquivoProdutos == NULL || indiceProdutos == NULL) {
        printf("Erro ao abrir arquivos para adicionar produto.\n");
        return -1;
    }

    Produto produto;
    Indice indice;

    // Solicita dados do produto ao usuário
    printf("Digite o ID do produto: ");
    scanf("%d", &produto.product_id);
    printf("Digite o código da categoria: ");
    scanf("%s", produto.category_code);
    printf("Digite a marca: ");
    scanf("%s", produto.brand);
    printf("Digite o preço: ");
    scanf("%f", &produto.price);

    // Carrega a última chave disponível
    int chaveAtual = carregarUltimaChave();
    produto.chave = chaveAtual + 1;  // Incrementa a chave para o novo produto

    // Grava o produto no arquivo
    fseek(arquivoProdutos, 0, SEEK_END);
    long long posProduto = ftell(arquivoProdutos);
    fwrite(&produto, sizeof(Produto), 1, arquivoProdutos);

    // Cria o índice
    indice.chave = produto.chave;
    indice.posicao = posProduto;
    fwrite(&indice, sizeof(Indice), 1, indiceProdutos);

    // Salva a nova chave no arquivo de controle
    salvarUltimaChave(produto.chave);

    fclose(arquivoProdutos);
    fclose(indiceProdutos);

    printf("Produto adicionado com sucesso. Chave: %d\n", produto.chave);
    return produto.chave;
}

void removerProduto(int chave)
{
    FILE *arquivoProdutos = fopen("produtos.bin", "rb+");
    FILE *indiceProdutos = fopen("indice_produtos.idx", "rb+");

    if (arquivoProdutos == NULL || indiceProdutos == NULL)
    {
        printf("Erro ao abrir arquivos para remover produto.\n");
        return;
    }

    Indice indice;
    int encontrado = 0;
    long long posicaoIndice;

    // Procura o produto pelo índice
    while (fread(&indice, sizeof(Indice), 1, indiceProdutos))
    {
        if (indice.chave == chave)
        {
            encontrado = 1;
            posicaoIndice = ftell(indiceProdutos) - sizeof(Indice);
            break;
        }
    }

    if (!encontrado)
    {
        printf("Produto com a chave %d não encontrado.\n", chave);
        fclose(arquivoProdutos);
        fclose(indiceProdutos);
        return;
    }

    // Marca o índice como removido (opção simples, setar chave como -1)
    fseek(indiceProdutos, posicaoIndice, SEEK_SET);
    indice.chave = -1; // Marca como removido
    fwrite(&indice, sizeof(Indice), 1, indiceProdutos);

    fclose(arquivoProdutos);
    fclose(indiceProdutos);

    printf("Produto com chave %d removido.\n", chave);
}

void pesquisaBinariaProduto(int chave)
{
    FILE *indiceProdutos = fopen("indice_produtos.idx", "rb");
    FILE *arquivoProdutos = fopen("produtos.bin", "rb");

    if (indiceProdutos == NULL || arquivoProdutos == NULL)
    {
        printf("Erro ao abrir arquivos de índice ou de produtos.\n");
        return;
    }

    Indice indice;
    long long left = 0, right, mid;

    fseek(indiceProdutos, 0, SEEK_END);
    right = ftell(indiceProdutos) / sizeof(Indice) - 1;

    while (left <= right)
    {
        mid = left + (right - left) / 2;
        fseek(indiceProdutos, mid * sizeof(Indice), SEEK_SET);
        fread(&indice, sizeof(Indice), 1, indiceProdutos);

        if (indice.chave == chave)
        {
            Produto produto;
            fseek(arquivoProdutos, indice.posicao, SEEK_SET);
            fread(&produto, sizeof(Produto), 1, arquivoProdutos);

            printf("Chave do Produto: %d\n", produto.chave);
            printf("ID do Produto: %d\n", produto.product_id);
            printf("Categoria: %s\n", produto.category_code);
            printf("Marca: %s\n", produto.brand);
            printf("Preço: %.2f\n", produto.price);
            fclose(indiceProdutos);
            fclose(arquivoProdutos);
            return;
        }

        if (indice.chave < chave)
        {
            left = mid + 1;
        }
        else
        {
            right = mid - 1;
        }
    }

    printf("Produto com chave %d não encontrado.\n", chave);
    fclose(indiceProdutos);
    fclose(arquivoProdutos);
}

int main()
{
    char nomeArquivoCSV[255];
    int chave;
    float valor;

    int i = -1;
    while (i != 0)
    {
        printf("Digite 1 para mostrar os dados\n");
        printf("Digite 2 para consultar produtos por uma faixa de valor\n");
        printf("Digite 3 para consultar produtos por pesquisa binária\n");
        printf("Digite 4 para adicionar um produto\n");
        printf("Digite 5 para remover um produto\n");
        printf("Digite 6 para consultar produtos por pesquisa binária no arquivo de índices\n");
        printf("Digite 9 para ler um arquivo csv\n");
        printf("Digite 0 para Sair\n");
        scanf("%d", &i);

        switch (i)
        {
        case 1:
            procuraProdutos();
            break;
        case 2:
            printf("Digite um valor: \n");
            scanf("%f", &valor);
            faixaPreco(valor);
            break;
        case 3:
            printf("Digite a chave: \n");
            scanf("%d", &chave);
            pesquisaBinariaProduto(chave);
            break;
        case 4:
            adicionarProduto();
            break;
        case 5:
            printf("Digite a chave do produto a remover: ");
            scanf("%d", &chave);
            removerProduto(chave);
            break;
        case 6:
            printf("Digite a chave do produto a pesquisar: ");
            scanf("%d", &chave);
            pesquisaBinariaProduto(chave);
            break;
        case 9:
            printf("Digite o nome do arquivo CSV: \n");
            scanf("%s", nomeArquivoCSV);
            processarCSV(nomeArquivoCSV);
            break;

        default:
            break;
        }
    }

    printf("-----------------------------");

    return 0;
}