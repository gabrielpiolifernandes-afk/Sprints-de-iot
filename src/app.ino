const int PINO_TRIG = 19;
const int PINO_ECHO = 18;

const float DISTANCIA_ATIVACAO = 20.0;
const float DISTANCIA_REARME = 30.0;

const int LEITURAS_PARA_ATIVAR = 3;

const unsigned long INTERVALO_LEITURA = 200;
const unsigned long TIMEOUT_ECHO = 30000;

int leiturasProximas = 0;
bool assistenteAtivado = false;

unsigned long ultimaLeitura = 0;

float medirDistancia() {
  // Garante que o TRIG comece em nível baixo.
  digitalWrite(PINO_TRIG, LOW);
  delayMicroseconds(2);

  // Envia um pulso de 10 microssegundos.
  digitalWrite(PINO_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PINO_TRIG, LOW);

  // Mede a duração do sinal recebido no ECHO.
  unsigned long duracao = pulseIn(
    PINO_ECHO,
    HIGH,
    TIMEOUT_ECHO
  );

  // Sem retorno válido do sensor.
  if (duracao == 0) {
    return -1.0;
  }

  // Distância = tempo x velocidade do som / 2.
  // Dividimos por 2 porque o som vai e volta.
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
  Serial.println(" MOTIVA - ATIVACAO POR PROXIMIDADE");
  Serial.println("==================================");
  Serial.println("Distancia de ativacao: 20 cm");
  Serial.println("Confirmacao: 3 leituras consecutivas");
  Serial.println("Aguardando aproximacao...");
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

  Serial.print("Distancia: ");
  Serial.print(distancia, 1);
  Serial.print(" cm | ");

  // Detecta aproximação dentro do limite configurado.
  if (distancia <= DISTANCIA_ATIVACAO) {
    if (leiturasProximas < LEITURAS_PARA_ATIVAR) {
      leiturasProximas++;
    }

    Serial.print("Leituras proximas: ");
    Serial.print(leiturasProximas);
    Serial.print("/");
    Serial.println(LEITURAS_PARA_ATIVAR);

    // Ativa somente após três leituras consecutivas.
    if (
      leiturasProximas >= LEITURAS_PARA_ATIVAR &&
      !assistenteAtivado
    ) {
      assistenteAtivado = true;

      Serial.println();
      Serial.println("**********************************");
      Serial.println("       ASSISTENTE ATIVADO!");
      Serial.println(" Aproximacao confirmada pela Motiva");
      Serial.println("**********************************");
      Serial.println();
    }
  } else {
    leiturasProximas = 0;
    Serial.println("Aguardando aproximacao...");

    // Rearma apenas quando a distância superar 30 cm.
    if (distancia >= DISTANCIA_REARME) {
      if (assistenteAtivado) {
        Serial.println("Usuario se afastou.");
        Serial.println("Sistema pronto para nova ativacao.");
      }

      assistenteAtivado = false;
    }
  }
}