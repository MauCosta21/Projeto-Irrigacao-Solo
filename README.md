# Sistema de Monitoramento e Irrigação Automática do Solo

## Integrante do Grupo
- Maurício Oliveira da Costa DRE: 125387787

## Objetivo
Desenvolver um sistema que colete dados ambientais do solo e tome decisões automáticas de irrigação baseadas em análise de dados.

## Funcionalidades Implementadas

### Coleta de Dados
- **Sensor de Umidade do Solo** - A0
- **Sensor de Temperatura TMP36** - A1  
- **Sensor de Salinidade (simulado com potenciômetro)** - A2
- **Coleta automática a cada 15 minutos**

### Processamento de Dados
- **Armazenamento em lista encadeada**
- **Ordenação com algoritmo Insertion Sort**
- **Remoção de valores extremos** (maior e menor)
- **Cálculo de médias a cada 4 horas** (16 leituras)

### Controle de Irrigação
- **LED indicador** - Pino 7
- **Lógica de decisão**: `UmidadeMédia < 40%`
- **Tempo de irrigação**: 1 minuto
- **Registro em "arquivo" Serial**

## Tecnologias e Componentes

### Hardware
- Arduino Uno
- Sensor de Umidade do Solo
- Sensor de Temperatura TMP36
- Potenciômetro (simulação de salinidade)
- LED Verde
- Resistor 220Ω

### Software
- Linguagem C++ (Arduino)
- Estrutura de dados: Lista Encadeada
- Algoritmo: Insertion Sort

## 📁 Estrutura do Código

```c
// Estrutura principal
struct DadosSolo {
    float temperatura;
    float umidade;
    float salinidade;
    DadosSolo* proximo;
};

// Funcionalidades principais
- coletarDados()      // Leitura dos sensores
- inserirOrdenado()   // Insertion Sort por umidade
- processarDados()    // Análise a cada 4 horas
- tomarDecisao()      // Controle de irrigação
