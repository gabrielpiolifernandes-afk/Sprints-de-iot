
const int PINO_TRIG = 19;
const int PINO_ECHO = 18;

const float DISTANCIA_ATIVACAO = 20.0;
const float DISTANCIA_REARME = 30.0;

const int LEITURAS_PARA_ATIVAR = 3;

const unsigned long INTERVALO_LEITURA = 500;
const unsigned long TIMEOUT_ECHO = 30000;
const unsigned long TEMPO_SEQUENCIA = 5000;

int leiturasProximas = 0;
int etapa = 0;

unsigned long ultimaLeitura = 0;
unsigned long inicioSequencia = 0;

float medirDistancia() {
  digitalWrite(PINO_TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(PINO_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PINO_TRIG, LOW);

  unsigned long duracao = pulseIn(
    PINO_ECHO,
    HIGH,
    TIMEOUT_ECHO
  );

  if (duracao == 0) {
    return -1.0;
  }

  float distancia = duracao * 0.0343 / 2.0;

  return distancia;
}

void setup() {
  Serial.begin(115200);

  pinMode(PINO_TRIG, OUTPUT);
  pinMode(PINO_ECHO, INPUT);

  digitalWrite(PINO_TRIG, LOW);

  Serial.println();
  Serial.println("==================================");
  Serial.println(" MOTIVA - ATIVACAO POR GESTOS");
  Serial.println("==================================");
  Serial.println("1 - Aproximacao");
  Serial.println("2 - Distanciamento");
  Serial.println("3 - Nova aproximacao");
  Serial.println("Tempo limite: 5 segundos");
  Serial.println("Aguardando gesto...");
}

void loop() {
  unsigned long agora = millis();

  // Controla o intervalo entre as leituras.
  if (agora - ultimaLeitura < INTERVALO_LEITURA) {
    return;
  }

  ultimaLeitura = agora;

  float distancia = medirDistancia();

  if (distancia < 0) {
    Serial.println("Leitura invalida. Verifique o sensor.");
    leiturasProximas = 0;
    return;
  }

  // Verifica se o tempo da sequencia expirou.
  if (
    (etapa == 1 || etapa == 2) &&
    agora - inicioSequencia > TEMPO_SEQUENCIA
  ) {
    Serial.println();
    Serial.println("Tempo esgotado!");
    Serial.println("Sequencia reiniciada.");
    Serial.println();

    etapa = 0;
    leiturasProximas = 0;
  }

  Serial.print("Distancia: ");
  Serial.print(distancia, 1);
  Serial.print(" cm | ");

  // ETAPA 0: Primeira aproximacao.
  if (etapa == 0) {

    if (distancia <= DISTANCIA_ATIVACAO) {
      leiturasProximas++;

      Serial.print("Primeira aproximacao: ");
      Serial.print(leiturasProximas);
      Serial.print("/");
      Serial.println(LEITURAS_PARA_ATIVAR);

      if (leiturasProximas >= LEITURAS_PARA_ATIVAR) {
        etapa = 1;
        inicioSequencia = agora;
        leiturasProximas = 0;

        Serial.println();
        Serial.println("Primeira aproximacao confirmada!");
        Serial.println("Agora afaste-se do sensor.");
        Serial.println();
      }

    } else {
      leiturasProximas = 0;
      Serial.println("Aguardando primeira aproximacao...");
    }
  }

  // ETAPA 1: Aguardar o distanciamento.
  else if (etapa == 1) {

    Serial.println("Aguardando distanciamento...");

    if (distancia >= DISTANCIA_REARME) {
      etapa = 2;
      leiturasProximas = 0;

      Serial.println();
      Serial.println("Distanciamento confirmado!");
      Serial.println("Aproxime-se novamente!");
      Serial.println();
    }
  }

  // ETAPA 2: Aguardar a segunda aproximacao.
  else if (etapa == 2) {

    if (distancia <= DISTANCIA_ATIVACAO) {
      leiturasProximas++;

      Serial.print("Segunda aproximacao: ");
      Serial.print(leiturasProximas);
      Serial.print("/");
      Serial.println(LEITURAS_PARA_ATIVAR);

      if (leiturasProximas >= LEITURAS_PARA_ATIVAR) {

        Serial.println();
        Serial.println("**********************************");
        Serial.println("       ASSISTENTE ATIVADO!");
        Serial.println(" GESTO RECONHECIDO PELA MOTIVA");
        Serial.println("**********************************");
        Serial.println();

        etapa = 3;
        leiturasProximas = 0;
      }

    } else {
      leiturasProximas = 0;
      Serial.println("Aguardando segunda aproximacao...");
    }
  }

  // ETAPA 3: Impede ativacoes repetidas.
  else if (etapa == 3) {

    Serial.println("Assistente ativo. Aguardando afastamento.");

    if (distancia >= DISTANCIA_REARME) {
      etapa = 0;

      Serial.println();
      Serial.println("Sistema rearmado!");
      Serial.println("Pronto para novo gesto.");
      Serial.println();
    }
  }
}
