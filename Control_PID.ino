const int pwmPin = 2;
const int frequency = 100000;  // Frecuencia deseada en Hz
const int dutyCycle = 128;     // Ciclo de trabajo deseado al 50%

long int sum1 = 0;
long int temp1;
int VC2;
double Vc2 = 0;

// controlador PI de corriente de voltaje
double K1_v = -2; 
double K2_v = -13;     
double K3_v = -1.0027;     
double K4_v = 0.7;    
double K5_v = 0.317;
double Vc2ref;
float time_1v;
float time_2v;
float Tsv;

double er_v;
double erm1_v;
double erm2_v;

double Un;
double Un1;
double Un2;

// Estas son las variables para enviar datos a excel
int analogSensor1;
int digitalSensor1;

// Esta parte del código va a correr al iniciar el micro
void setup() {
  Serial.begin(115200);

  pinMode(pwmPin, OUTPUT);

  // Para modificar el setpoint Vc2ref=15
  Vc2ref = 10;

  // Configurar el pin 2 para generar una señal PWM de alta frecuencia
  TCCR3A = _BV(COM3A0) | _BV(WGM30);
  TCCR3B = _BV(WGM32) | _BV(CS30);
  OCR3A = (F_CPU / frequency) / 2 - 1;

  // Establecer el ciclo de trabajo inicial
  analogWrite(pwmPin, dutyCycle);
}

// Esta es la parte que corre en tiempo real en tu micro
void loop() {
  if (Serial.available() > 0) {
    // Leer el comando recibido por Serial
    String command = Serial.readStringUntil('\n');
    command.trim(); // Eliminar espacios en blanco al inicio y al final

    // Verificar si el comando es para actualizar Vc2ref
    if (command.startsWith("Vc2ref=")) {
      // Obtener el valor después del igual
      String valueString = command.substring(7);
      // Convertir el valor a un número decimal
      Vc2ref = valueString.toDouble();
    }
  }

  sum1 = 0;
  for (int i = 0; i < 80; i++) {
    temp1 = analogRead(A7);
    sum1 += temp1;
  }

  VC2 = (sum1 / 80);
  //Vc2 = (double) VC2 * ((3.3 * 11.35) / 1023.0);
  Vc2 = (double) VC2 * ((5 * 11.05) / 1023.0);
  //Serial.print(Vc2);
  //Serial.println(' ');

  time_2v = millis();
  Tsv = time_2v - time_1v;

  if (Tsv >= 1) {
    er_v = Vc2ref - Vc2;

    Un = er_v * K1_v + erm1_v * K2_v + erm2_v * K3_v + K4_v * Un1 + K5_v * Un2;
    //Serial.print(Un);
    //Serial.println(',');
    if (Un < 10) {
      Un = 10;
    }

    if (Un > 250) {
      Un = 250;
    }

    Un2 = Un1;
    Un1 = Un;

    erm2_v = erm1_v;
    erm1_v = er_v;
  }
  analogWrite(pwmPin, Un);//un accion control
  Serial.print(millis());
  Serial.print(",");
  Serial.print(Un);
  Serial.print(",");
  Serial.print(Vc2);
  Serial.print(",");
  Serial.print(er_v);
  Serial.print(",");
  Serial.print(Vc2ref);
  Serial.print(",");
  Serial.println();
}

