#include <Keypad.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>

#define CUSTOM_SDA_PIN 5
#define CUSTOM_SCL_PIN 6
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const byte ROWS = 3;
bool tour = false;
const byte COLS = 3;
char keys[ROWS][COLS] = {
  {'U', 'L', 'D'},
  {'R', 'S', 'T'},
  {'A', 'B', ' '}
};

int plateauJeu[8][8] = {
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
};

const char *ssid = "PoleDeVinci_DVIC";
const char *password = "8PfURsp!dvic";
const char *serverIP = "192.168.4.1";  // Adresse IP du serveur ESP32
const int serverPort = 80;
byte rowPins[ROWS] = {20, 10, 0};
byte colPins[COLS] = {7, 8, 9};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

WiFiClient client;

int selectorX = 0;
int selectorY = 0;

bool grid[8][8] = {false};

unsigned long startTime = 0;
const unsigned long displayTime = 2000; // 2 secondes

void setup() {
  Serial.begin(9600);
  Wire.begin(CUSTOM_SDA_PIN, CUSTOM_SCL_PIN);

  delay(1000);
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("Allocation SSD1306 échouée"));
    for (;;)
      ;
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20, 20);
  display.print("Othello");
  display.display();
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20, 20);
  display.print("Connexion au réseau WiFi...");
  display.display();
  Serial.println("Connexion au réseau WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connexion en cours...");
  }

  Serial.println("Connecté au réseau WiFi");
}

void loop() {
  if (!client.connected()) {
    if (client.connect(serverIP, serverPort)) {
      Serial.println("Connecté au serveur ESP32");
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(20, 20);
      display.print("Connecté à l'ESP32");
      display.display();

      // Attendre pendant une courte période avant d'envoyer une autre commande
    } else {
      Serial.println("La connexion au serveur a échoué. Réessayer dans 5 secondes...");
      return;
    }
  }
  
  else 
  { 
      String recues = client.readString();
      recues = client.readString();
      char x_serveur = recues[0];
      char y_serveur = recues[1];
      plateauJeu[x_serveur][y_serveur] = 1;
      Serial.println(recues);
      int recues2 = recues.toInt(); 
      updateDisplay2(x_serveur,y_serveur);
      Serial.println("Oui2");
      if (recues2 >= 00 && recues2 <= 88)
      {
        tour = true;
      }
      char key = keypad.getKey();
    if (key and tour) {
      switch (key) {
      case 'U':
        selectorY = max(0, selectorY - 1);
        updateDisplay();
        break;
      case 'D':
        selectorY = min(7, selectorY + 1);
        updateDisplay();
        break;
      case 'L':
        selectorX = max(0, selectorX - 1);
        updateDisplay();
        break;
      case 'R':
        selectorX = min(7, selectorX + 1);
        updateDisplay();
        break;
      case 'A':
        grid[selectorX][selectorY] = true;
        plateauJeu[selectorX][selectorY] = 1;
        updateDisplay();
        String positionMessage = String(selectorX) + String(selectorY);
        client.println(positionMessage);
        Serial.println(positionMessage);
        break;
      }
    }
  }
}

void updateDisplay() {
  display.clearDisplay();
  for (int x = 0; x < 8; x++) {
    for (int y = 0; y < 8; y++) {
      display.drawRect(x * (SCREEN_WIDTH / 8), y * (SCREEN_HEIGHT / 8), SCREEN_WIDTH / 8, SCREEN_HEIGHT / 8, SSD1306_WHITE);
      if (grid[x][y]) {
        display.fillCircle(x * (SCREEN_WIDTH / 8) + (SCREEN_WIDTH / 16), y * (SCREEN_HEIGHT / 8) + (SCREEN_HEIGHT / 16), 2, SSD1306_WHITE);
      }
    }
  }
  display.drawRect(selectorX * (SCREEN_WIDTH / 8), selectorY * (SCREEN_HEIGHT / 8), SCREEN_WIDTH / 8, SCREEN_HEIGHT / 8, SSD1306_INVERSE);
  display.display();
}

void updateDisplay2(int clientx, int clienty) {
  display.clearDisplay();
  
  // Dessiner le plateau de jeu
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      display.drawRect(i * (SCREEN_WIDTH / 8), j * (SCREEN_HEIGHT / 8), SCREEN_WIDTH / 8, SCREEN_HEIGHT / 8, SSD1306_WHITE);
      if (grid[i][j]) {
        display.fillCircle(i * (SCREEN_WIDTH / 8) + (SCREEN_WIDTH / 16), j * (SCREEN_HEIGHT / 8) + (SCREEN_HEIGHT / 16), 2, SSD1306_WHITE);
      }
    }
  }

  // Dessiner un cercle à la position x, y
  display.fillCircle(clientx * (SCREEN_WIDTH / 8) + (SCREEN_WIDTH / 16), clienty * (SCREEN_HEIGHT / 8) + (SCREEN_HEIGHT / 16), 2, SSD1306_INVERSE);
  
  // Mettre à jour l'affichage
  display.display();
}
