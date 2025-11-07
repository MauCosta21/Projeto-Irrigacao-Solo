#define PINO_UMIDADE A0
#define PINO_TEMPERATURA A1
#define PINO_SALINIDADE A2
#define PINO_LED 7

struct DadosSolo {
  float temperatura;
  float umidade;
  float salinidade;
  DadosSolo* proximo;
};

DadosSolo* inicio = NULL;
unsigned long ultimaLeitura = 0;
unsigned long tempoIrrigacao = 0;
bool irrigacaoAtiva = false;

const unsigned long INTERVALO = 900000;
const int LEITURAS_POR_CICLO = 16;
const unsigned long TEMPO_IRRIGACAO = 60000;

int contadorLeituras = 0;

void setup() {
  Serial.begin(9600);
  pinMode(PINO_LED, OUTPUT);
  digitalWrite(PINO_LED, LOW);
  Serial.println("Sistema Iniciado!");
  Serial.print("Coleta: 15 minutos | Processamento: 4 horas | Irrigacao: 1 minuto");
}

void loop() {
  unsigned long tempoAtual = millis();
  
  if (irrigacaoAtiva && (tempoAtual - tempoIrrigacao >= TEMPO_IRRIGACAO)) {
    digitalWrite(PINO_LED, LOW);
    irrigacaoAtiva = false;
  }
  
  if (tempoAtual - ultimaLeitura >= INTERVALO) {
    coletarDados();
    ultimaLeitura = tempoAtual;
    contadorLeituras++;
    
    if (contadorLeituras >= LEITURAS_POR_CICLO) {
      processarDados();
      contadorLeituras = 0;
    }
  }
  delay(100);
}

void coletarDados() {
  float leituraTempAnalog = analogRead(PINO_TEMPERATURA);
  float leituraUmiAnalog = analogRead(PINO_UMIDADE);
  float leituraSalAnalog = analogRead(PINO_SALINIDADE);
  
  float temperatura = (leituraTempAnalog * 5.0 / 1024.0 - 0.5) * 100;
  float umidade = map(leituraUmiAnalog, 0, 1023, 100, 0);
  float salinidade = map(leituraSalAnalog, 0, 1023, 0, 100);
  
  DadosSolo* novo = new DadosSolo;
  novo->temperatura = temperatura;
  novo->umidade = umidade;
  novo->salinidade = salinidade;
  novo->proximo = NULL;
  
  if (inicio == NULL || novo->umidade <= inicio->umidade) {
    novo->proximo = inicio;
    inicio = novo;
  } else {
    DadosSolo* atual = inicio;
    while (atual->proximo != NULL && atual->proximo->umidade < novo->umidade) {
      atual = atual->proximo;
    }
    novo->proximo = atual->proximo;
    atual->proximo = novo;
  }
  
  Serial.print("\nLeitura "); Serial.print(contadorLeituras + 1); Serial.print("/"); Serial.print(LEITURAS_POR_CICLO);
  Serial.print(": Umi="); Serial.print(umidade, 2);
  Serial.print("%, Temp="); Serial.print(temperatura, 2);
  Serial.print("C, Sal="); Serial.print(salinidade, 2); Serial.println("%");
}

void processarDados() {
  Serial.println("\n=== PROCESSANDO CICLO DE DADOS ===");
  
  if (inicio == NULL || inicio->proximo == NULL) {
    Serial.println("ERRO: Dados insuficientes para processamento.");
    return;
  }
  
  DadosSolo* menor = inicio;
  inicio = inicio->proximo;
  Serial.print("Removido Menor Umidade: "); Serial.print(menor->umidade, 2); Serial.println("%");
  delete menor;
  
  DadosSolo* atual = inicio;
  DadosSolo* anterior = NULL;
  
  while (atual->proximo != NULL) {
    anterior = atual;
    atual = atual->proximo;
  }
  
  if (anterior != NULL) {
    anterior->proximo = NULL;
    Serial.print("Removido Maior Umidade: "); Serial.print(atual->umidade, 2); Serial.println("%");
    delete atual;
  } else {
    Serial.print("Removido Maior Umidade: "); Serial.print(atual->umidade, 2); Serial.println("%");
    delete atual;
    inicio = NULL;
  }
  
  float somaTemp = 0, somaUmi = 0, somaSal = 0;
  int count = 0;
  atual = inicio;
  
  while (atual != NULL) {
    somaTemp += atual->temperatura;
    somaUmi += atual->umidade;
    somaSal += atual->salinidade;
    count++;
    atual = atual->proximo;
  }
  
  if (count == 0) {
    Serial.println("A lista ficou vazia após a remoção dos extremos.");
    return;
  }
  
  float mediaTemp = somaTemp / count;
  float mediaUmi = somaUmi / count;
  float mediaSal = somaSal / count;
  
  bool irrigar = (mediaUmi < 40);
  
  if (irrigar) {
    digitalWrite(PINO_LED, HIGH);
    irrigacaoAtiva = true;
    tempoIrrigacao = millis();
  } else {
    digitalWrite(PINO_LED, LOW);
    irrigacaoAtiva = false;
  }
  
  Serial.println("\n--- RESULTADOS ---");
  Serial.print("Médias - Umi: "); Serial.print(mediaUmi, 2);
  Serial.print("%, Temp: "); Serial.print(mediaTemp, 2);
  Serial.print("C, Sal: "); Serial.print(mediaSal, 2); Serial.println("%");
  Serial.print("Decisão de Irrigação: "); Serial.println(irrigar ? "LIGADA" : "DESLIGADA");
  
  Serial.println("\n--- REGISTRO DE ARQUIVO ---");
  Serial.print(millis());
  Serial.print(";"); Serial.print(mediaTemp, 2);
  Serial.print(";"); Serial.print(mediaUmi, 2);
  Serial.print(";"); Serial.print(mediaSal, 2);
  Serial.print(";"); Serial.println(irrigar ? "LIGADA" : "DESLIGADA");
  Serial.println("---------------------------\n");
  
  while (inicio != NULL) {
    DadosSolo* temp = inicio;
    inicio = inicio->proximo;
    delete temp;
  }
}
